#ifndef CACHE_CONSISTENCY_CHECK_H
#define CACHE_CONSISTENCY_CHECK_H

#include <systemc>
#include "base_test.h"
#include "cache_parameters.h"

class cache_consistency_check : public base_test {
    private:
        tlm::tlm_generic_payload *gp_write;
        tlm::tlm_generic_payload *gp_read;

        std::string data_write;
        std::string data_read;

        static bool registered;
        WRITE_POLICY write_policy;

        void validate_cache_parameters();
    
    public:
        cache_consistency_check(sc_module_name name, const cache_params& params);

        SC_HAS_PROCESS(cache_consistency_check);

        void execute();
        void write_request();
        void read_request();

        static test_factory* create_instance(sc_core::sc_module_name name, const cache_params& params);
        ~cache_consistency_check();
};

#endif //!CACHE_CONSISTENCY_CHECK_H
