#include "testbench.h"


testbench::testbench(sc_core::sc_module_name name, const cache_params& params) 
                    : sc_core::sc_module(name),
                      cache_ctrl("cache_ctrl", params),
                      memory("memory")
{
    cache_ctrl.memory_read_socket_i.bind(memory.read_socket);
    cache_ctrl.memory_write_socket_i.bind(memory.write_socket);
}

