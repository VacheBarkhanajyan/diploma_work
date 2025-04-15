#ifndef CACHE_INVALIDATION_CHECK_H
#define CACHE_INVALIDATION_CHECK_H

#include <systemc>
#include "base_test.h"
#include "cache_parameters.h"

class cache_invalidation_check : public base_test {
    private:
        std::string data_write;
        std::string data_read;
        static bool registered;

    public:
        cache_invalidation_check(sc_core::sc_module_name name, const cache_params& params);

        SC_HAS_PROCESS(cache_invalidation_check);

        void execute();
        void write_request();
        void invalidate_request();
        void read_after_invalidate();

        static test_factory* create_instance(sc_core::sc_module_name name, const cache_params& params);
        ~cache_invalidation_check();
};

#endif //!CACHE_INVALIDATION_CHECK_H
