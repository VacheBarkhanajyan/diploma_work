/*
----------------------------------------
 Copyright (C) 2024 Grovf Inc. All Rights Reserved.

 Contributors:
 D.Margaryan david.m@grovf.com
-----------------------------------------
*/

#ifndef TEST_INTERFACE_H
#define TEST_INTERFACE_H

#include <iostream>
#include <string>
#include <map>
#include <systemc>
#include "cache_parameters.h"


class test_factory : public sc_core::sc_module {

    public:

        test_factory(sc_core::sc_module_name name);

        SC_HAS_PROCESS(test_factory);

        virtual ~test_factory() = default;
        
        typedef test_factory* (*create_function)(sc_core::sc_module_name name, const cache_params& params);
        
        static test_factory* create(const std::string& test_name, const cache_params& params);
        static bool register_test(const std::string& test_name, create_function create_func);
};

#endif //!TEST_INTERFACE_H
