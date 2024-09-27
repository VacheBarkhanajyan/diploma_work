/*
----------------------------------------
 Copyright (C) 2024 Grovf Inc. All Rights Reserved.

 Contributors:
 D.Margaryan david.m@grovf.com
-----------------------------------------
*/

#ifndef WRITE_READ_CHECK_H
#define WRITE_READ_CHECK_H

#include <systemc>
#include "base_test.h"
#include "cache_parameters.h"


class write_read_check : public base_test {

    private:
        tlm::tlm_generic_payload *gp_write;
        tlm::tlm_generic_payload *gp_read;

        uint8_t *data_write;
        uint8_t *data_read;

        static bool registered;

    public:

        write_read_check(sc_module_name name, const cache_params& params);

        SC_HAS_PROCESS(write_read_check);

        void execute();

        static test_factory* create_instance(sc_core::sc_module_name name, const cache_params& params);

        ~write_read_check();

};

#endif //!WRITE_READ_CHECK_H
