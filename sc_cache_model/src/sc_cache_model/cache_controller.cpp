#include "cache_controller.h"

void cache_controller::load_block_from_memory(uint64_t address, unsigned char* block_data) {
    tlm::tlm_generic_payload *gp = new tlm::tlm_generic_payload();
    sc_core::sc_time delay = sc_core::sc_time(cache_mem.get_memory_read_delay(), sc_core::SC_NS);

    unsigned char* data = block_data;
    uint64_t block_start_address = 
        (address / cache_mem.get_cache_line_size()) * cache_mem.get_cache_line_size();

    gp->set_address(block_start_address);
    gp->set_data_length(cache_mem.get_cache_line_size());
    gp->set_data_ptr(data);

    memory_read_socket_i->b_transport(*gp, delay);

    delete gp;
}

void cache_controller::store_block_to_memory(uint64_t address, unsigned char* block_data, size_t size) {
    tlm::tlm_generic_payload *gp = new tlm::tlm_generic_payload();
    sc_core::sc_time delay = sc_core::sc_time(cache_mem.get_memory_write_delay(), sc_core::SC_NS);

    unsigned char* data = block_data;

    gp->set_address(address);
    gp->set_data_length(size);
    gp->set_data_ptr(data);

    memory_write_socket_i->b_transport(*gp, delay);

    delete gp;
}


void cache_controller::handle_invalidate_request(addr_range_t& address_range) {
    
    sc_core::wait(cache_mem.get_cache_invalidate_delay(), sc_core::SC_NS);

    uint64_t start_block_address = 
        (address_range.first / cache_mem.get_cache_line_size()) * cache_mem.get_cache_line_size();

    uint64_t end_block_address = 
        ((address_range.second + cache_mem.get_cache_line_size() - 1) / cache_mem.get_cache_line_size()) * 
        cache_mem.get_cache_line_size();

    for (uint64_t block_address = start_block_address; block_address <= end_block_address;  block_address += cache_mem.get_cache_line_size()) {
            
        uint32_t set_index = 
            (block_address / cache_mem.get_cache_line_size()) % cache_mem.get_cache_sets_count();
        uint64_t tag = 
            block_address / (cache_mem.get_cache_line_size() * cache_mem.get_cache_sets_count());

        for (int way_index = 0; way_index < cache_mem.get_cache_set_assoc_count(); ++way_index) {

            if (cache_mem.get_cache_line_tag(set_index, way_index) == tag) {
                cache_mem.set_cache_line_valid_bit(set_index, way_index, false);

                ongoing_request_map_t& request_map = ongoing_requests[set_index];
                if (request_map.find(tag) != request_map.end()) {
                    request_map[tag] = false;
                }

                break;
            }
        }
    }

}


void cache_controller::handle_flush_request() {
    
    sc_core::wait(cache_mem.get_cache_flush_delay(), sc_core::SC_NS);

    for (uint32_t set_index = 0; set_index < cache_mem.get_cache_sets_count(); ++set_index) {
        for (int way_index = 0; way_index < cache_mem.get_cache_set_assoc_count(); ++way_index) {
            cache_mem.set_cache_line_valid_bit(set_index, way_index, false);

        }
    }

}


void cache_controller::handle_write_request(tlm::tlm_generic_payload* gp) {
    

    uint64_t address = gp->get_address();    
    

    uint64_t block_address = (address / cache_mem.get_cache_line_size()) * cache_mem.get_cache_line_size();
    uint32_t index = (block_address / cache_mem.get_cache_line_size()) % cache_mem.get_cache_sets_count();
    uint64_t tag = block_address / (cache_mem.get_cache_line_size() * cache_mem.get_cache_sets_count());
    uint32_t word_offset = (address % cache_mem.get_cache_line_size());


    cache_optional_fields_extension* extension;
    gp->get_extension(extension);
    

    cache_action_params op = {
        .set_index = index,
        .way_index = -1, // Placeholder, will be determined later in method calls.
        .tag = tag,
        .address = address,
        .word_offset = word_offset,
        .data = gp->get_data_ptr(),
        .size = gp->get_data_length(),
        .op_type = WRITE,
        .write_policy = get_write_policy(extension->cache),
        .line_alloc = get_write_request_line_alloc(extension->cache)
    };


    ongoing_requests[index][tag] = true;


    SC_REPORT_INFO_VERB(
            "CACHE_CTRL/WRITE",
            ("@ " + sc_core::sc_time_stamp().to_string() + " : "      +
             "\n\twrite_policy: "   + std::to_string(op.write_policy) +
             "\n\tline_alloc: "     + std::to_string(op.line_alloc)

            ).c_str(),
            sc_core::SC_MEDIUM);

    if (op.write_policy == INTERNAL) {
        store_block_to_memory(op.address, op.data, op.size);

        ongoing_requests[index].erase(tag);

        return;
    }

    if (try_write_to_cache(op)) {

        SC_REPORT_INFO_VERB(
            "CACHE_CTRL/WRITE",
            "cache hit",
            sc_core::SC_MEDIUM);
    }
    else if (op.write_policy == WRITE_THROUGH) {

        update_write_through_cache(op);
        SC_REPORT_INFO_VERB(
            "CACHE_CTRL/WRITE",
            "cache miss, write_policy == WRITE_THROUGH",
            sc_core::SC_MEDIUM);
    }
    else if (try_load_into_cache(op)) {
        SC_REPORT_INFO_VERB(
            "CACHE_CTRL/WRITE",
            "cache miss, write_policy == WRITE_BACK loading to empty line",
            sc_core::SC_MEDIUM);
    }
    else {
        handle_cache_eviction(op);
        SC_REPORT_INFO_VERB(
            "CACHE_CTRL/WRITE",
            "cache miss, write_policy == WRITE_BACK loading to replaced line",
            sc_core::SC_MEDIUM);
    }

    ongoing_requests[index].erase(tag);

}


void cache_controller::handle_read_request(tlm::tlm_generic_payload* gp) {

    uint64_t address = gp->get_address();   

    uint64_t block_address = (address / cache_mem.get_cache_line_size()) * cache_mem.get_cache_line_size();
    uint32_t index = (block_address / cache_mem.get_cache_line_size()) % cache_mem.get_cache_sets_count();
    uint64_t tag = block_address / (cache_mem.get_cache_line_size() * cache_mem.get_cache_sets_count());
    uint32_t word_offset = (address % cache_mem.get_cache_line_size()) / cache_mem.get_cache_word_size();

    cache_optional_fields_extension* extension;
    gp->get_extension(extension);

    cache_action_params op = {
        .set_index = index,
        .way_index = -1, // Placeholder, will be determined later in method calls.
        .tag = tag,
        .address = address,
        .word_offset = word_offset,
        .data = gp->get_data_ptr(),
        .size = gp->get_data_length(),
        .op_type = READ,
        .write_policy = get_write_policy(extension->cache),
        .line_alloc = get_read_request_line_alloc(extension->cache)
    };

    ongoing_requests[index][tag] = true;

    SC_REPORT_INFO_VERB(
            "CACHE_CTRL/READ",
            ("@ " + sc_core::sc_time_stamp().to_string() + " : "      +
             "\n\twrite_policy: "   + std::to_string(op.write_policy) +
             "\n\tline_alloc: "     + std::to_string(op.line_alloc)

            ).c_str(),
            sc_core::SC_MEDIUM);


    if (op.write_policy == INTERNAL) {
        std::vector<uint8_t> block_data(cache_mem.get_cache_line_size());
        load_block_from_memory(op.address, block_data.data());

        std::memcpy(
            op.data, 
            block_data.data(), 
            op.size
        );
        
        ongoing_requests[index].erase(tag);

        return;
    }

    if (try_read_from_cache(op)) {
        SC_REPORT_INFO_VERB(
            "CACHE_CTRL/READ",
            "cache hit",
            sc_core::SC_MEDIUM);
        
    }
    else if (try_load_into_cache(op)) {
        
        SC_REPORT_INFO_VERB(
            "CACHE_CTRL/READ",
            "cache miss, loading to empty line",
            sc_core::SC_MEDIUM);
    }
    else {
        handle_cache_eviction(op);
        SC_REPORT_INFO_VERB(
            "CACHE_CTRL/READ",
            "cache miss, loading to replaced line",
            sc_core::SC_MEDIUM);
    }

    ongoing_requests[index].erase(tag);
}


bool cache_controller::try_write_to_cache(cache_action_params &op) {

    for (int i = 0; i < cache_mem.get_cache_set_assoc_count(); ++i) {

        if ( cache_mem.get_cache_line_valid_bit(op.set_index, i) && 
             cache_mem.get_cache_line_tag(op.set_index, i) == op.tag ) {

            op.way_index = i;
            cache_mem.write_word(op);

            if (op.write_policy == WRITE_BACK) {
                cache_mem.set_cache_line_dirty_bit(op.set_index, op.way_index, true);
            } else {
                store_block_to_memory(op.address, op.data, op.size);
            }

            update_lru(op.set_index, op.way_index);

            return true;
        }
    }

    return false;
}

bool cache_controller::try_read_from_cache(cache_action_params &op) {

    for (int i = 0; i < cache_mem.get_cache_set_assoc_count(); ++i) {

        if ( cache_mem.get_cache_line_valid_bit(op.set_index, i) && 
             cache_mem.get_cache_line_tag(op.set_index, i) == op.tag ) {

            op.way_index = i;
            cache_mem.read_word(op);

            update_lru(op.set_index, op.way_index);

            return true;
        }
    }

    return false;
}


void cache_controller::update_write_through_cache(cache_action_params& op) {
    store_block_to_memory(op.address, op.data, op.size);

    if (op.line_alloc == ALLOCATE) {

        for (int i = 0; i < cache_mem.get_cache_set_assoc_count(); ++i) {
            if (!cache_mem.get_cache_line_valid_bit(op.set_index, i)) {
                op.way_index = i;
                if ( ongoing_requests[op.set_index][op.tag] == true ) {
                    cache_mem.set_cache_line_valid_bit(op.set_index, op.way_index, true);
                }
                cache_mem.set_cache_line_tag(op.set_index, op.way_index, op.tag);

                cache_mem.write_word(op);
                update_lru(op.set_index, op.way_index);

                return;
            }
        }

        op.way_index = get_lru_index(op.set_index);

        if ( ongoing_requests[op.set_index][op.tag] == true ) {
            cache_mem.set_cache_line_valid_bit(op.set_index, op.way_index, true);
        }
        
        cache_mem.set_cache_line_tag(op.set_index, op.way_index, op.tag);
        cache_mem.write_word(op);
        update_lru(op.set_index, op.way_index);

    }

    return;

}


bool cache_controller::try_load_into_cache(cache_action_params &op) {

    for (int i = 0; i < cache_mem.get_cache_set_assoc_count(); ++i) {
        if (!cache_mem.get_cache_line_valid_bit(op.set_index, i)) {
            op.way_index = i;
            load_block(op);

            return true;
        }
    }

    return false;
}

void cache_controller::handle_cache_eviction(cache_action_params &op) {

    if (op.line_alloc == ALLOCATE) {
        op.way_index = get_lru_index(op.set_index);

        if (cache_mem.get_cache_line_valid_bit(op.set_index, op.way_index) && 
            cache_mem.get_cache_line_dirty_bit(op.set_index, op.way_index) && 
            op.write_policy == WRITE_BACK) 
        {

            uint64_t block_addr = 
                (static_cast<uint64_t>(cache_mem.get_cache_line_tag(op.set_index, op.way_index)) * 
                cache_mem.get_cache_sets_count() + 
                op.set_index) * 
                cache_mem.get_cache_line_size();

            store_block_to_memory(
                block_addr, 
                cache_mem.get_cache_line_data_ptr(op.set_index, op.way_index), 
                cache_mem.get_cache_line_size()
            );

            cache_mem.set_cache_line_dirty_bit(op.set_index, op.way_index, false);
        }

    }

    load_block(op);
}

void cache_controller::load_block(cache_action_params &op) {


    if (op.line_alloc == ALLOCATE) {

        std::vector<uint8_t> block_data(cache_mem.get_cache_line_size());
        
        load_block_from_memory(op.address, block_data.data());

        if ( ongoing_requests[op.set_index][op.tag] == true ) {
            cache_mem.set_cache_line_valid_bit(op.set_index, op.way_index, true);
        }

        cache_mem.set_cache_line_tag(op.set_index, op.way_index, op.tag);
        std::memcpy(
            cache_mem.get_cache_line_data_ptr(op.set_index, op.way_index), 
            block_data.data(), 
            cache_mem.get_cache_line_size()
        );

        if ( op.op_type == READ ) {
            cache_mem.set_cache_line_dirty_bit(op.set_index, op.way_index, false);
            cache_mem.read_word(op);
        }
        else {
            cache_mem.set_cache_line_dirty_bit(op.set_index, op.way_index, (op.write_policy == WRITE_BACK));
            cache_mem.write_word(op);
        }

        update_lru(op.set_index, op.way_index);

    }
    else {
        if (op.op_type == WRITE && op.write_policy == WRITE_BACK) {
            store_block_to_memory(op.address, op.data, op.size);
        }
        else {
            std::vector<uint8_t> block_data(cache_mem.get_cache_line_size());
            load_block_from_memory(op.address, block_data.data());

            std::memcpy(
                op.data, 
                block_data.data(), 
                op.size
            );
        }
    }
}

int cache_controller::get_lru_index(int set_index) {
    return lru_lists[set_index].back();
}

void cache_controller::update_lru(int set_index, int way_index) {
    lru_lists[set_index].remove(way_index);
    lru_lists[set_index].push_front(way_index);
}


WRITE_POLICY cache_controller::get_write_policy(int value) {

    int last_two_bits = value & 0b11;

    int first_two_bits = (value >> 2) & 0b11;


    if (first_two_bits == 0b00) {
        return INTERNAL;
    }

    switch (last_two_bits) {
        case 0b10:
            return WRITE_THROUGH;
        case 0b11:
            return WRITE_BACK;
        default:
            return INTERNAL;
    }

}


LINE_ALLOC cache_controller::get_read_request_line_alloc(int value) {

    int first_two_bits = (value >> 2) & 0b11;

    switch (first_two_bits) {
        case 0b01:
        case 0b11:
            return ALLOCATE;
        default:
            return NO_ALLOCATE;
    }
    
}


LINE_ALLOC cache_controller::get_write_request_line_alloc(int value) {

    int first_two_bits = (value >> 2) & 0b11;

    switch (first_two_bits) {
        case 0b10:
        case 0b11:
            return ALLOCATE;
        default:
            return NO_ALLOCATE;
    }

}

cache_controller::cache_controller(sc_core::sc_module_name name, const cache_params& params)
                                  : sc_core::sc_module(name), 
                                    cache_mem("CacheMemory", params),
                                    memory_read_socket_i("memory_read_socket_i"),
                                    memory_write_socket_i("memory_write_socket_i")
{

    lru_lists.resize(cache_mem.get_cache_sets_count());
    for (int i = 0; i < cache_mem.get_cache_sets_count(); ++i) {
        for (int j = 0; j < cache_mem.get_cache_set_assoc_count(); ++j) {
            lru_lists[i].push_back(j);
        }
    }

    ongoing_requests.resize(cache_mem.get_cache_sets_count());

    SC_THREAD(read);
    SC_THREAD(write);
    SC_THREAD(invalidate);
    SC_THREAD(flush);

}

void cache_controller::read() {
    
    while (true) {
        tlm::tlm_generic_payload* gp = read_req_fifo.get();

        invalidate_mutex.lock();
        invalidate_mutex.unlock();

        flush_mutex.lock();
        flush_mutex.unlock();

        handle_read_request(gp);
        print_cache(gp->get_address());
        read_rsp_fifo.put(gp);
    }
}


void cache_controller::write() {

    while (true) {
        tlm::tlm_generic_payload* gp = write_req_fifo.get();

        invalidate_mutex.lock();
        invalidate_mutex.unlock();

        flush_mutex.lock();
        flush_mutex.unlock();

        

        handle_write_request(gp);
    
        print_cache(gp->get_address());
        write_rsp_fifo.put(gp);
        

    }

}


void cache_controller::invalidate() {

    while (true) {
        addr_range_t address_range = invalidate_req_fifo.get();
        
        invalidate_mutex.lock();

        handle_invalidate_request(address_range);

        invalidate_rsp_fifo.put(address_range);

        invalidate_mutex.unlock();
    }

}



void cache_controller::flush() {

    while (true) {
        tlm::tlm_generic_payload* gp = flush_req_fifo.get();
        
        flush_mutex.lock();

        bool requests_done = false;
        while (!requests_done) {
            requests_done = true;

            for (const auto& request_map : ongoing_requests) {
                for (const auto& request : request_map) {
                    if (request.second) {
                        requests_done = false;
                        break;
                    }
                }

                if (!requests_done) {
                    break;
                }
            }
        }

        handle_flush_request();
        flush_rsp_fifo.put(gp);

        flush_mutex.unlock();
    }

}


void cache_controller::print_cache(uint64_t address) {

    cache_mem.print_cache(address);
}


