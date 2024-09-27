#include <systemc>
#include <tlm>
#include "cache_parameters.h"
#include "parse_verbosity_level.h"
#include "test_factory.h"

#include "base_test.h"

int sc_main(int argc, char* argv[]) {
    
    if (argc != 14) {
        SC_REPORT_ERROR("TOP", "arg count is not equal to 14");
        return 1;
    }

    cache_params params;

    std::string verbosity_level_str = argv[1];
    std::string test_name           = argv[2];
    params.cache_size               = std::stoi(argv[3]);
    params.line_size                = std::stoi(argv[4]);
    params.set_assoc_count          = std::stoi(argv[5]);
    params.word_size                = std::stoi(argv[6]);
    params.write_policy             = (std::string(argv[7]) == "WRITE_THROUGH") 
                                        ? WRITE_THROUGH 
                                        : WRITE_BACK;

    params.memory_read_delay        = std::stoi(argv[8]);
    params.memory_write_delay       = std::stoi(argv[9]);
    params.cache_read_delay         = std::stoi(argv[10]);
    params.cache_write_delay        = std::stoi(argv[11]);
    params.cache_invalidate_delay   = std::stoi(argv[12]);



    sc_core::sc_report_handler::set_verbosity_level(parse_verbosity_level(verbosity_level_str));

    SC_REPORT_INFO_VERB(
            "TOP",
            ("@ " + sc_core::sc_time_stamp().to_string() + " : "                                +
             "\n\ttest_name: "                  + test_name                                     +
             "\n\tcache_size: "                 + std::to_string(params.cache_size)             +
             "\n\tblock_size: "                 + std::to_string(params.line_size)              +
             "\n\tset_assoc_count: "            + std::to_string(params.set_assoc_count)        +
             "\n\tword_size: "                  + std::to_string(params.word_size)              +
             "\n\twrite_policy: "               + argv[7]                                       +
             "\n\tmemory_read_delay: "          + std::to_string(params.memory_read_delay)      +
             "\n\tmemory_write_delay: "         + std::to_string(params.memory_write_delay)     +
             "\n\tcache_read_delay: "           + std::to_string(params.cache_read_delay)       +
             "\n\tcache_write_delay: "          + std::to_string(params.cache_write_delay)      +
             "\n\tcache_invalidate_delay: "     + std::to_string(params.cache_invalidate_delay) 
            ).c_str(),
            sc_core::SC_MEDIUM);



    test_factory* test = test_factory::create(test_name, params);
    if (!test) {
        SC_REPORT_ERROR("TOP", ("Unknown test name : " + test_name).c_str());
        return 1;
    }
    
    sc_core::sc_start();
    
    delete test;

    return 0;
}
