#ifndef CACHE_EVICTION_CHECK_H
#define CACHE_EVICTION_CHECK_H

#include <systemc>
#include "base_test.h"
#include "cache_parameters.h"

class cache_eviction_check : public base_test {
    private:
        std::vector<std::string> test_data;
        std::vector<std::string> read_data;
        static bool registered;

    public:
        cache_eviction_check(sc_core::sc_module_name name, const cache_params& params);

        SC_HAS_PROCESS(cache_eviction_check);

        void execute();
        void write_requests();
        void read_requests();

        static test_factory* create_instance(sc_core::sc_module_name name, const cache_params& params);
        ~cache_eviction_check();
};

#endif //!CACHE_EVICTION_CHECK_H
