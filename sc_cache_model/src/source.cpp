#include <systemc>
#include <tlm>
#include "cache_parameters.h"
#include "parse_verbosity_level.h"
#include "test_factory.h"

#include "base_test.h"

int sc_main(int argc, char* argv[]) {

    cache_params params;
    auto args = argparse::parse<test_args>(sc_core::sc_argc(), sc_core::sc_argv());


    std::string verbosity_level_str = args.verbosity_level;
    std::string test_name           = args.test_name;
    params.cache_size               = std::stoi(args.cache_size);
    params.line_size                = std::stoi(args.line_size);
    params.set_assoc_count          = std::stoi(args.set_assoc_count);
    params.word_size                = std::stoi(args.word_size);


    params.memory_read_delay        = std::stoi(args.memory_read_delay);
    params.memory_write_delay       = std::stoi(args.memory_write_delay);
    params.cache_read_delay         = std::stoi(args.cache_read_delay);
    params.cache_write_delay        = std::stoi(args.cache_write_delay);
    params.cache_invalidate_delay   = std::stoi(args.cache_invalidate_delay);


    sc_core::sc_report_handler::set_verbosity_level(parse_verbosity_level(verbosity_level_str));

    SC_REPORT_INFO_VERB(
            "TOP",
            ("@ " + sc_core::sc_time_stamp().to_string() + " : "                                +
             "\n\ttest_name: "                  + test_name                                     +
             "\n\tcache_size: "                 + std::to_string(params.cache_size)             +
             "\n\tblock_size: "                 + std::to_string(params.line_size)              +
             "\n\tset_assoc_count: "            + std::to_string(params.set_assoc_count)        +
             "\n\tword_size: "                  + std::to_string(params.word_size)              +
             "\n\tmemory_read_delay: "          + std::to_string(params.memory_read_delay)      +
             "\n\tmemory_write_delay: "         + std::to_string(params.memory_write_delay)     +
             "\n\tcache_read_delay: "           + std::to_string(params.cache_read_delay)       +
             "\n\tcache_write_delay: "          + std::to_string(params.cache_write_delay)      +
             "\n\tcache_invalidate_delay: "     + std::to_string(params.cache_invalidate_delay) 
            ).c_str(),
            sc_core::SC_MEDIUM);


    printf("debug 1\n");
    test_factory* test = test_factory::create(test_name, params);
    printf("debug 2\n");

    if (!test) {
        SC_REPORT_ERROR("TOP", ("Unknown test name : " + test_name).c_str());
        return 1;
    }
    printf("debug 3\n");

    sc_core::sc_start();
    
    delete test;

    return 0;
}
