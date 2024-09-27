/*
----------------------------------------
 Copyright (C) 2024 Grovf Inc. All Rights Reserved.

 Contributors:
 D.Margaryan david.m@grovf.com
-----------------------------------------
*/

#ifndef CACHEPARAMETERS_H
#define CACHEPARAMETERS_H

#include <utility>
#include <unordered_map>


typedef std::pair<uint32_t, uint32_t> addr_range_t;

// TODO: this shall be run-time setting. I.e. in real device this could change
//       during normal operation
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


typedef std::unordered_map<uint32_t, bool> ongoing_request_map_t;

#endif //!CACHEPARAMETERS_H