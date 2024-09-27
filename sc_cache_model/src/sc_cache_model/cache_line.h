/*
----------------------------------------
 Copyright (C) 2024 Grovf Inc. All Rights Reserved.

 Contributors:
 D.Margaryan david.m@grovf.com
-----------------------------------------
*/

#ifndef CACHELINE_H
#define CACHELINE_H

#include <vector>
#include <cstdint>
#include <cstring>
#include <systemc>

#include "cache_parameters.h"


class cache_line : public sc_core::sc_module {
    
    public:
        bool     valid;
        bool     dirty;
        uint64_t tag;

        cache_params params;
        
        std::vector<uint8_t> data;

        cache_line(sc_core::sc_module_name name, const cache_params& params);

        SC_HAS_PROCESS(cache_line);

        void read_word(uint32_t word_offset, uint8_t* word_data, int word_size, int delay) const;
        void write_word(uint32_t word_offset, const uint8_t* word_data, int word_size, int delay);

};

#endif // !CACHELINE_H
