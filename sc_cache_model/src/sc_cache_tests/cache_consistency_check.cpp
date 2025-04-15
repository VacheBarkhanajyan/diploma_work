#include "cache_consistency_check.h"

cache_consistency_check::cache_consistency_check(sc_core::sc_module_name name, const cache_params& params)
    : base_test(name, params), write_policy(params.write_policy) 
{
    

    gp_write = new tlm::tlm_generic_payload();
    gp_read = new tlm::tlm_generic_payload();

    data_write = "TestStringCache";
    data_read.resize(data_write.size());

    gp_write->set_data_ptr(reinterpret_cast<uint8_t*>(&data_write[0]));
    gp_read->set_data_ptr(reinterpret_cast<uint8_t*>(&data_read[0]));

    gp_write->set_address(0x20);
    gp_read->set_address(0x20);

    gp_write->set_data_length(data_write.size());
    gp_read->set_data_length(data_read.size());

    cache_optional_fields_extension* write_extension = new cache_optional_fields_extension();
    write_extension->cache = 0b1011;
    
    cache_optional_fields_extension* read_extension = new cache_optional_fields_extension();
    read_extension->cache = 0b1011;
    
    gp_write->set_extension(write_extension);
    gp_read->set_extension(read_extension);

    SC_THREAD(execute);
}

void cache_consistency_check::write_request() {

    tb.cache_ctrl.write_req_fifo.put(gp_write);

    gp_write = tb.cache_ctrl.write_rsp_fifo.get();

}

void cache_consistency_check::read_request() {
    tb.cache_ctrl.read_req_fifo.put(gp_read);
    gp_read = tb.cache_ctrl.read_rsp_fifo.get();
}

void cache_consistency_check::execute() {

    write_request();

    read_request();

    printf("data_read -> %s\n", data_read.c_str());
    printf("data_write -> %s\n", data_write.c_str());

    if (data_read != data_write) {
        SC_REPORT_ERROR("CACHE_CONSISTENCY_CHECK", "Data mismatch after read!");
    } else {
        SC_REPORT_INFO("CACHE_CONSISTENCY_CHECK", "Test Passed.");
    }

    sc_core::sc_stop();
}

test_factory* cache_consistency_check::create_instance(sc_core::sc_module_name name, const cache_params& params) {
    return new cache_consistency_check(name, params);
}

bool cache_consistency_check::registered = test_factory::register_test(
    "cache_consistency_check", cache_consistency_check::create_instance
);

cache_consistency_check::~cache_consistency_check() {
    delete gp_write;
    delete gp_read;
}
