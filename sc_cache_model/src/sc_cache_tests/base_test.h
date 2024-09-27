/*
----------------------------------------
 Copyright (C) 2024 Grovf Inc. All Rights Reserved.

 Contributors:
 D.Margaryan david.m@grovf.com
-----------------------------------------
*/


#ifndef BASE_TEST_H
#define BASE_TEST_H

#include <systemc>
#include "test_factory.h"
#include "testbench.h"
#include "cache_parameters.h"


class base_test : public test_factory {


    public:

        testbench tb;

        base_test(sc_module_name name, const cache_params& params);

        SC_HAS_PROCESS(base_test);

        virtual ~base_test() = default;


};

#endif //!BASE_TEST_H
