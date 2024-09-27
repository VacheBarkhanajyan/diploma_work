#include "cache_memory.h"

cache_memory::cache_memory(sc_core::sc_module_name name, const cache_params& params)
                          : sc_core::sc_module(name), 
                            params(params)
{
    sets = params.cache_size / (params.line_size * params.set_assoc_count);
    words_per_block = params.line_size / params.word_size;

    cache.resize(sets);
    for (int i = 0; i < sets; ++i) {
        cache[i].reserve(params.set_assoc_count);
        for (int j = 0; j < params.set_assoc_count; ++j) {

            std::string cache_line_name = "cache_line_set_" + 
                                          std::to_string(i) + 
                                          "_way_"           +
                                          std::to_string(j);

            cache[i].emplace_back(std::make_unique<cache_line>(cache_line_name.c_str(), params));
        }
    }

}


void cache_memory::print_cache(uint64_t address) {
    uint64_t block_address = (address / get_cache_line_size()) * get_cache_line_size();
    uint32_t index = (block_address / get_cache_line_size()) % get_cache_sets_count();
    uint64_t tag = block_address / (get_cache_line_size() * get_cache_sets_count());
    uint32_t word_offset = (address % get_cache_line_size()) / get_cache_word_size();

    std::string report = "@ " + sc_core::sc_time_stamp().to_string() + " :";
    report += "\n  block_address: " + std::to_string(block_address);
    report += "\n  word_offset: " + std::to_string(word_offset);
    report += "\nCache Memory Contents:";

    if (index >= get_cache_sets_count()) {
        report += "\nError: Index out of range!";
        SC_REPORT_INFO_VERB("CACHE_MEMORY", report.c_str(), sc_core::SC_MEDIUM);
        return;
    }

    report += "\nSet " + std::to_string(index) + ":";
    bool found = false;

    for (int j = 0; j < get_cache_set_assoc_count(); ++j) {
        const auto& line = cache[index][j];

        if (line->tag == tag) {
            report += "\n  Way " + std::to_string(j) + ":";
            report += "\n    Valid: " + std::to_string(line->valid);
            report += "\n    Dirty: " + std::to_string(line->dirty);
            report += "\n    Tag: " + std::to_string(line->tag);
            report += "\n    Data: ";

            for (size_t k = 0; k < line->data.size(); ++k) {
                char buffer[3];
                snprintf(buffer, sizeof(buffer), "%02x", static_cast<int>(line->data[k]));
                report += std::string(buffer) + " ";
            }

            found = true;
            break;
        }
    }

    if (!found) {
        report += "\n  Cache line with tag " + std::to_string(tag) + " not found in set " + std::to_string(index) + ".";
    }

    SC_REPORT_INFO_VERB("CACHE_MEMORY", report.c_str(), sc_core::SC_MEDIUM);
}



void cache_memory::read_word(cache_action_params &op) const {
    cache[op.set_index][op.way_index]->read_word(op.word_offset, 
                                                 op.data, 
                                                 op.size,
                                                 params.cache_read_delay
                                                );
}


void cache_memory::write_word(cache_action_params &op) {
    cache[op.set_index][op.way_index]->write_word(op.word_offset,
                                                  op.data, 
                                                  op.size,
                                                  params.cache_write_delay
                                                 );
}


int cache_memory::get_cache_line_size() const {
    return params.line_size;
}

int cache_memory::get_cache_set_assoc_count() const {
    return params.set_assoc_count;
}

int cache_memory::get_cache_word_size() const {
    return params.word_size;
}


int cache_memory::get_cache_sets_count() const {
    return sets;
}


int cache_memory::get_cache_line_valid_bit(uint32_t set_index, int way_index) const {
    return cache[set_index][way_index]->valid;
}

int cache_memory::get_cache_line_dirty_bit(uint32_t set_index, int way_index) const {
    return cache[set_index][way_index]->dirty;
}

uint64_t cache_memory::get_cache_line_tag(uint32_t set_index, int way_index) const {
    return cache[set_index][way_index]->tag;
}

unsigned char* cache_memory::get_cache_line_data_ptr(uint32_t set_index, int way_index) const {
    return cache[set_index][way_index]->data.data();
}


int cache_memory::get_memory_read_delay() const {
    return params.memory_read_delay;
}


int cache_memory::get_memory_write_delay() const {
    return params.memory_write_delay;
}

int cache_memory::get_cache_invalidate_delay() const {
    return params.cache_invalidate_delay;
}

int cache_memory::get_cache_flush_delay() const {
    return params.cache_flush_delay;
}

void cache_memory::set_cache_line_dirty_bit(uint32_t set_index, int way_index, bool value) {
    cache[set_index][way_index]->dirty = value;
}

void cache_memory::set_cache_line_valid_bit(uint32_t set_index, int way_index, bool value) {
    cache[set_index][way_index]->valid = value;
}

void cache_memory::set_cache_line_tag(uint32_t set_index, int way_index, uint64_t tag) {
    cache[set_index][way_index]->tag = tag;
}

