#include "write_read_check.h"

write_read_check::write_read_check(sc_core::sc_module_name name, const cache_params& params)
                                  : base_test(name, params)
{

    gp_write = new tlm::tlm_generic_payload();
    gp_read  = new tlm::tlm_generic_payload();

    data_write = new uint8_t[params.word_size]{1, 2, 3, 4};
    data_read  = new uint8_t[params.word_size]{0};

    gp_write->set_data_ptr(data_write);
    gp_read->set_data_ptr(data_read);

    gp_write->set_address(0x10);
    gp_read->set_address(0x10);

    gp_write->set_data_length(params.word_size);
    gp_read->set_data_length(params.word_size);

    SC_THREAD(execute);
}


void write_read_check::execute() {
    
    tb.cache_ctrl.write_req_fifo.put(gp_write);
    gp_write = tb.cache_ctrl.write_rsp_fifo.get();

    tb.cache_ctrl.read_req_fifo.put(gp_read);
    gp_read = tb.cache_ctrl.read_rsp_fifo.get();

    for (int i = 0; i < gp_write->get_data_length(); ++i) {
        if (data_read[i] != data_write[i]) {
            SC_REPORT_ERROR("WRITE_READ_CHECK", "Cache miss on read");
            sc_core::sc_stop();
        }

    }

    SC_REPORT_INFO_VERB(
        "WRITE_READ_CHECK",
        "TEST passed",
        sc_core::SC_LOW);

    sc_core::sc_stop();
}


test_factory* write_read_check::create_instance(sc_core::sc_module_name name, const cache_params& params) {
    return new write_read_check(name, params);
}


bool write_read_check::registered = test_factory::register_test(
                                                "write_read_check", 
                                                write_read_check::create_instance
                                                );

write_read_check::~write_read_check() {

    delete[] data_write;
    delete[] data_read;

    delete gp_write;
    delete gp_read;

    SC_REPORT_INFO_VERB(
        "WRITE_READ_CHECK",
        "destructor call",
        sc_core::SC_HIGH);

}
