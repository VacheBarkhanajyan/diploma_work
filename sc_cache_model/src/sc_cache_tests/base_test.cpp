#include "base_test.h"

base_test::base_test(sc_core::sc_module_name name, const cache_params& params)
                                  : test_factory(name),
                                    tb("testbench", params)
{

}

