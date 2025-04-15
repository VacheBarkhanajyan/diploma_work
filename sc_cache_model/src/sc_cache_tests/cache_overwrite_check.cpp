#include "cache_overwrite_check.h"

cache_overwrite_check::cache_overwrite_check(sc_core::sc_module_name name, const cache_params& params)
    : base_test(name, params) {
    
    initial_data = "InitialData";
    new_data = "UpdatedData";
    read_data.resize(new_data.size());

    SC_THREAD(execute);
}

void cache_overwrite_check::write_initial() {
    tlm::tlm_generic_payload* gp = new tlm::tlm_generic_payload();
    gp->set_data_ptr(reinterpret_cast<uint8_t*>(&initial_data[0]));
    gp->set_address(0x40);
    gp->set_data_length(initial_data.size());

    cache_optional_fields_extension* write_extension = new cache_optional_fields_extension();
    write_extension->cache = 0b1011;
    gp->set_extension(write_extension);

    tb.cache_ctrl.write_req_fifo.put(gp);
    tb.cache_ctrl.write_rsp_fifo.get();
}

void cache_overwrite_check::overwrite() {
    tlm::tlm_generic_payload* gp = new tlm::tlm_generic_payload();
    gp->set_data_ptr(reinterpret_cast<uint8_t*>(&new_data[0]));
    gp->set_address(0x40);
    gp->set_data_length(new_data.size());

    cache_optional_fields_extension* overwrite_extension = new cache_optional_fields_extension();
    overwrite_extension->cache = 0b1011;
    gp->set_extension(overwrite_extension);

    tb.cache_ctrl.write_req_fifo.put(gp);
    tb.cache_ctrl.write_rsp_fifo.get();
}

void cache_overwrite_check::read_final() {
    tlm::tlm_generic_payload* gp = new tlm::tlm_generic_payload();
    gp->set_data_ptr(reinterpret_cast<uint8_t*>(&read_data[0]));
    gp->set_address(0x40);
    gp->set_data_length(read_data.size());

    cache_optional_fields_extension* read_final_extension = new cache_optional_fields_extension();
    read_final_extension->cache = 0b1011;
    gp->set_extension(read_final_extension);

    tb.cache_ctrl.read_req_fifo.put(gp);
    tb.cache_ctrl.read_rsp_fifo.get();
}

void cache_overwrite_check::execute() {
    write_initial();
    overwrite();
    read_final();

    if (read_data != new_data) {
        SC_REPORT_ERROR("CACHE_OVERWRITE_TEST", "Cache overwrite failed!");
    } else {
        SC_REPORT_INFO("CACHE_OVERWRITE_TEST", "Test Passed.");
    }

    sc_core::sc_stop();
}

test_factory* cache_overwrite_check::create_instance(sc_core::sc_module_name name, const cache_params& params) {
    return new cache_overwrite_check(name, params);
}

bool cache_overwrite_check::registered = test_factory::register_test(
    "cache_overwrite_check", cache_overwrite_check::create_instance
);

cache_overwrite_check::~cache_overwrite_check() {}
