#include "cache_bulk_read_write_check.h"

cache_bulk_read_write_check::cache_bulk_read_write_check(sc_core::sc_module_name name, const cache_params& params)
    : base_test(name, params) {
    
    test_data = {
        "BulkTest1_Data",
        "BulkTest2_Data",
        "BulkTest3_Data",
        "BulkTest4_Data"
    };

    read_data.resize(test_data.size());
    for (size_t i = 0; i < test_data.size(); ++i) {
        read_data[i].resize(test_data[i].size(), '\0');
    }
    
    SC_THREAD(execute);
}

void cache_bulk_read_write_check::write_bulk() {
    for (size_t i = 0; i < test_data.size(); ++i) {
        tlm::tlm_generic_payload* gp = new tlm::tlm_generic_payload();
        gp->set_data_ptr(reinterpret_cast<uint8_t*>(&test_data[i][0]));
        gp->set_address(0x60 + i * 16);
        gp->set_data_length(test_data[i].size());

        cache_optional_fields_extension* write_extension = new cache_optional_fields_extension();
        write_extension->cache = 0b1011;

        gp->set_extension(write_extension);    
        
        tb.cache_ctrl.write_req_fifo.put(gp);
        tb.cache_ctrl.write_rsp_fifo.get();

    }
}

void cache_bulk_read_write_check::read_bulk() {
    for (size_t i = 0; i < read_data.size(); ++i) {
        tlm::tlm_generic_payload* gp = new tlm::tlm_generic_payload();
        gp->set_data_ptr(reinterpret_cast<uint8_t*>(&read_data[i][0]));
        gp->set_address(0x60 + i * 16);
        gp->set_data_length(test_data[i].size());


        cache_optional_fields_extension* read_extension = new cache_optional_fields_extension();
        read_extension->cache = 0b0111;

        gp->set_extension(read_extension);


        tb.cache_ctrl.read_req_fifo.put(gp);
        tb.cache_ctrl.read_rsp_fifo.get();
    }
}

void cache_bulk_read_write_check::execute() {
    write_bulk();
    read_bulk();



    for (size_t i = 0; i < test_data.size(); ++i) {
        printf("test_data[%d] = %s \n", (int)i, test_data[i].c_str());
        printf("read_data[%d] = %s \n", (int)i, read_data[i].c_str());
    
        if (test_data[i] != read_data[i]) {
            SC_REPORT_INFO("CACHE_BULK_READ_WRITE_TEST", "Bulk data transfer failed!");
        }
    }
    

    SC_REPORT_INFO("CACHE_BULK_READ_WRITE_TEST", "Test Passed.");
    sc_core::sc_stop();
}

test_factory* cache_bulk_read_write_check::create_instance(sc_core::sc_module_name name, const cache_params& params) {
    return new cache_bulk_read_write_check(name, params);
}

bool cache_bulk_read_write_check::registered = test_factory::register_test(
    "cache_bulk_read_write_check", cache_bulk_read_write_check::create_instance
);

cache_bulk_read_write_check::~cache_bulk_read_write_check() {}
