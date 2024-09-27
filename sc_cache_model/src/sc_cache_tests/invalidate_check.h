/*
----------------------------------------
 Copyright (C) 2024 Grovf Inc. All Rights Reserved.

 Contributors:
 D.Margaryan david.m@grovf.com
-----------------------------------------
*/

#ifndef INVALIDATE_CHECK_H
#define INVALIDATE_CHECK_H

#include <systemc>
#include "base_test.h"
#include "cache_parameters.h"


class invalidate_check : public base_test {

    private:
        tlm::tlm_generic_payload *gp_write;
        tlm::tlm_generic_payload *gp_read;
        
        addr_range_t invalidate_addr_pair;

        uint8_t *data_write;
        uint8_t *data_read;

        static bool registered;

        sc_core::sc_event write_done;
        sc_core::sc_event read_done;
        sc_core::sc_event invalidate_trigger;

        WRITE_POLICY write_policy;
    
    public:

        invalidate_check(sc_module_name name, const cache_params& params);
        
        SC_HAS_PROCESS(invalidate_check);

        void execute();

        void read_request();

        void write_request();

        void invalidate_request();

        static test_factory* create_instance(sc_core::sc_module_name name, const cache_params& params);

        ~invalidate_check();

};

#endif //!INVALIDATE_CHECK_H
