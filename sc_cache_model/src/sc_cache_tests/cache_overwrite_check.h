#ifndef CACHE_OVERWRITE_CHECK_H
#define CACHE_OVERWRITE_CHECK_H

#include <systemc>
#include "base_test.h"
#include "cache_parameters.h"

class cache_overwrite_check : public base_test {
    private:
        std::string initial_data;
        std::string new_data;
        std::string read_data;
        static bool registered;

    public:
        cache_overwrite_check(sc_core::sc_module_name name, const cache_params& params);

        SC_HAS_PROCESS(cache_overwrite_check);

        void execute();
        void write_initial();
        void overwrite();
        void read_final();

        static test_factory* create_instance(sc_core::sc_module_name name, const cache_params& params);
        ~cache_overwrite_check();
};

#endif //!CACHE_OVERWRITE_CHECK_H
