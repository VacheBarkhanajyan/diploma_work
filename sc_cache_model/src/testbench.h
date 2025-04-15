#ifndef TESTBENCH_H
#define TESTBENCH_H

#include <systemc>
#include "cache_controller.h"
#include "main_memory.h"

class testbench : public sc_core::sc_module {

  public:
    main_memory       memory;
    cache_controller  cache_ctrl;

    testbench(sc_core::sc_module_name name, const cache_params& params);

    SC_HAS_PROCESS(testbench);

};

#endif //!TESTBENCH_H
