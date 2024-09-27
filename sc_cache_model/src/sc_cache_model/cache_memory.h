/*
----------------------------------------
 Copyright (C) 2024 Grovf Inc. All Rights Reserved.

 Contributors:
 D.Margaryan david.m@grovf.com
-----------------------------------------
*/

#ifndef CACHEMEMORY_H
#define CACHEMEMORY_H

#include <systemc>
#include <vector>
#include <cstring>
#include <tlm>
#include <iomanip>

#include "cache_line.h"
#include "cache_parameters.h"

#include "simple_target_socket.h"
#include "simple_initiator_socket.h"

class cache_memory : public sc_core::sc_module {

    private:
        int sets;
        int words_per_block;
        
        cache_params params;

        std::vector<std::vector<std::unique_ptr<cache_line>>> cache;

    public:    

        cache_memory(sc_core::sc_module_name name, const cache_params& params);

        void print_cache(uint64_t address); 
        void read_word(cache_action_params &op) const;
        void write_word(cache_action_params &op);

        int get_cache_line_size() const;
        int get_cache_set_assoc_count() const;
        int get_cache_word_size() const;
        int get_cache_sets_count() const;
        int get_memory_read_delay() const;
        int get_memory_write_delay() const;
        int get_cache_invalidate_delay() const;
        int get_cache_flush_delay() const;
        int get_cache_line_valid_bit(uint32_t set_index, int way_index) const;
        int get_cache_line_dirty_bit(uint32_t set_index, int way_index) const;
        uint64_t get_cache_line_tag(uint32_t set_index, int way_index) const;
        unsigned char* get_cache_line_data_ptr(uint32_t set_index, int way_index) const;


        void set_cache_line_dirty_bit(uint32_t set_index, int way_index, bool value);
        void set_cache_line_valid_bit(uint32_t set_index, int way_index, bool value);
        void set_cache_line_tag(uint32_t set_index, int way_index, uint64_t tag);
};

#endif //!CACHEMEMORY_H
