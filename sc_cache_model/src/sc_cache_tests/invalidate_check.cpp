#include "invalidate_check.h"

invalidate_check::invalidate_check(sc_core::sc_module_name name, const cache_params& params)
                                  : base_test(name, params),
                                    write_policy(params.write_policy)
{

    gp_write       = new tlm::tlm_generic_payload();
    gp_read        = new tlm::tlm_generic_payload();

    data_write = new uint8_t[params.word_size]{1, 2, 3, 4};
    data_read  = new uint8_t[params.word_size]{0};

    invalidate_addr_pair = std::make_pair(0x10, 0x78);

    gp_write->set_data_ptr(data_write);
    gp_read->set_data_ptr(data_read);

    gp_write->set_address(0x10);
    gp_read->set_address(0x10);

    gp_write->set_data_length(params.word_size);
    gp_read->set_data_length(params.word_size);

    SC_THREAD(execute);
    SC_THREAD(write_request);
    SC_THREAD(read_request);
    SC_THREAD(invalidate_request);

}

void invalidate_check::read_request() {

    sc_core::wait(invalidate_trigger);

    tb.cache_ctrl.read_req_fifo.put(gp_read);
    gp_read = tb.cache_ctrl.read_rsp_fifo.get();

    read_done.notify();
}

void invalidate_check::write_request() {

    tb.cache_ctrl.write_req_fifo.put(gp_write);
    gp_write = tb.cache_ctrl.write_rsp_fifo.get();

    write_done.notify();
}

void invalidate_check::invalidate_request() {

    sc_core::wait(write_done);

    tb.cache_ctrl.invalidate_req_fifo.put(invalidate_addr_pair);
    sc_core::wait(1, sc_core::SC_NS);

    invalidate_trigger.notify();
    invalidate_addr_pair = tb.cache_ctrl.invalidate_rsp_fifo.get();
}


void invalidate_check::execute() {

    sc_core::wait(read_done);

    int error_counter = 0;

    for (int i = 0; i < gp_write->get_data_length(); ++i) {
        if (data_read[i] != data_write[i]) {
            error_counter++;
        }

    }

    if (error_counter > 0 && write_policy == WRITE_THROUGH) {
        SC_REPORT_ERROR("INVALIDATE_CHECK", "Cache miss, write_policy == WRITE_THROUGH");
    }
    else if (error_counter == 0 && write_policy == WRITE_BACK) {
        SC_REPORT_ERROR("INVALIDATE_CHECK", "Cache hit, write_policy == WRITE_BACK");
    }
    else {
        SC_REPORT_INFO_VERB(
            "INVALIDATE_CHECK",
            "TEST passed",
            sc_core::SC_LOW);
    }

    sc_core::sc_stop();
}


test_factory* invalidate_check::create_instance(sc_core::sc_module_name name, const cache_params& params) {
    return new invalidate_check(name, params);
}


bool invalidate_check::registered = test_factory::register_test(
                                                "invalidate_check", 
                                                invalidate_check::create_instance
                                                );

invalidate_check::~invalidate_check() {

    delete[] data_write;
    delete[] data_read;

    delete gp_write;
    delete gp_read;

    SC_REPORT_INFO_VERB(
        "WRITE_READ_CHECK",
        "destructor call",
        sc_core::SC_HIGH);

}
