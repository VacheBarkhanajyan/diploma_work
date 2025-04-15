#include "cache_invalidation_check.h"

cache_invalidation_check::cache_invalidation_check(sc_core::sc_module_name name, const cache_params& params)
    : base_test(name, params) {
    
    data_write = "InvalidateMe";
    data_read.resize(data_write.size());

    SC_THREAD(execute);
}

void cache_invalidation_check::write_request() {
    tlm::tlm_generic_payload* gp = new tlm::tlm_generic_payload();
    gp->set_data_ptr(reinterpret_cast<uint8_t*>(&data_write[0]));
    gp->set_address(0x50);
    gp->set_data_length(data_write.size());

    cache_optional_fields_extension* write_extension = new cache_optional_fields_extension();
    write_extension->cache = 0b1011;
    gp->set_extension(write_extension);

    tb.cache_ctrl.write_req_fifo.put(gp);
    tb.cache_ctrl.write_rsp_fifo.get();
}

void cache_invalidation_check::invalidate_request() {
    addr_range_t addr_range = std::make_pair(0x50, 0x50);
    tb.cache_ctrl.invalidate_req_fifo.put(addr_range);
    tb.cache_ctrl.invalidate_rsp_fifo.get();
}

void cache_invalidation_check::read_after_invalidate() {
    tlm::tlm_generic_payload* gp = new tlm::tlm_generic_payload();
    gp->set_data_ptr(reinterpret_cast<uint8_t*>(&data_read[0]));
    gp->set_address(0x50);
    gp->set_data_length(data_read.size());

    cache_optional_fields_extension* read_extension = new cache_optional_fields_extension();
    read_extension->cache = 0b1011;
    gp->set_extension(read_extension);


    tb.cache_ctrl.read_req_fifo.put(gp);
    tb.cache_ctrl.read_rsp_fifo.get();
}

void cache_invalidation_check::execute() {
    write_request();
    invalidate_request();
    read_after_invalidate();

    if (data_read == data_write) {
        SC_REPORT_ERROR("CACHE_INVALIDATION_TEST", "Cache did not invalidate properly!");
    } else {
        SC_REPORT_INFO("CACHE_INVALIDATION_TEST", "Test Passed.");
    }

    sc_core::sc_stop();
}

test_factory* cache_invalidation_check::create_instance(sc_core::sc_module_name name, const cache_params& params) {
    return new cache_invalidation_check(name, params);
}

bool cache_invalidation_check::registered = test_factory::register_test(
    "cache_invalidation_check", cache_invalidation_check::create_instance
);

cache_invalidation_check::~cache_invalidation_check() {}
