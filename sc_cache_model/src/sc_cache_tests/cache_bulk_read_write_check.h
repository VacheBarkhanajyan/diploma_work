#ifndef CACHE_BULK_READ_WRITE_CHECK_H
#define CACHE_BULK_READ_WRITE_CHECK_H

#include <systemc>
#include "base_test.h"
#include "cache_parameters.h"

class cache_bulk_read_write_check : public base_test {
    private:
        std::vector<std::string> test_data;
        std::vector<std::string> read_data;
        static bool registered;

    public:
        cache_bulk_read_write_check(sc_core::sc_module_name name, const cache_params& params);

        SC_HAS_PROCESS(cache_bulk_read_write_check);

        void execute();
        void write_bulk();
        void read_bulk();

        static test_factory* create_instance(sc_core::sc_module_name name, const cache_params& params);
        ~cache_bulk_read_write_check();
};

#endif //!CACHE_BULK_READ_WRITE_CHECK_H
