#ifndef CACHEPARAMETERS_H
#define CACHEPARAMETERS_H

#include <utility>
#include <unordered_map>

#include "argparse.hpp"

typedef std::pair<uint32_t, uint32_t> addr_range_t;

enum WRITE_POLICY {
    WRITE_THROUGH,
    WRITE_BACK,
    INTERNAL
};

enum LINE_ALLOC {
    ALLOCATE,
    NO_ALLOCATE
};

enum OPERATION_TYPE {
    READ,
    WRITE
};

struct cache_action_params {

    uint32_t       set_index;
    int            way_index;
    uint64_t       tag;
    uint64_t       address;
    uint32_t       word_offset;
    uint8_t*       data;
    size_t         size;

    OPERATION_TYPE  op_type;
    WRITE_POLICY    write_policy;
    LINE_ALLOC      line_alloc;
};


// TODO: Think about re-arrange parameters, maybe split into
//       elaboration-time parameters i.e. static for simulation.
//       i.e. parameters that mimic hardware and those that in runtime
struct cache_params {
  
    int cache_size;
    int line_size;
    int set_assoc_count;
    int word_size;

    unsigned int   memory_read_delay;
    unsigned int   memory_write_delay;
    unsigned int   cache_read_delay;
    unsigned int   cache_write_delay;
    unsigned int   cache_invalidate_delay;
    unsigned int   cache_flush_delay;

    WRITE_POLICY write_policy;

};


struct test_args : public argparse::Args {

    std::string &verbosity_level          = kwarg("verbosity_level", "parameter for choosing saving logs");
    std::string &test_name                = kwarg("test_name", "parameter for choosing test to execute");
    std::string &cache_size               = kwarg("cache_size", "parameter for choosing cache memory size");
    std::string &line_size                = kwarg("line_size", "parameter for choosing cache memory line size");
    std::string &set_assoc_count          = kwarg("set_assoc_count", "parameter for choosing cache memory set associative count");
    std::string &word_size                = kwarg("word_size", "parameter for choosing cache memory line word size");

    std::string &memory_read_delay        = kwarg("memory_read_delay", "parameter for choosing main memory(RAM) read delay");
    std::string &memory_write_delay       = kwarg("memory_write_delay", "parameter for choosing main memory(RAM) write delay");
    std::string &cache_read_delay         = kwarg("cache_read_delay", "parameter for choosing cache memory read delay");
    std::string &cache_write_delay        = kwarg("cache_write_delay", "parameter for choosing cache memory write delay");
    std::string &cache_invalidate_delay   = kwarg("cache_invalidate_delay", "parameter for choosing cache memory invalidate delay");

};


typedef std::unordered_map<uint32_t, bool> ongoing_request_map_t;

#endif //!CACHEPARAMETERS_H