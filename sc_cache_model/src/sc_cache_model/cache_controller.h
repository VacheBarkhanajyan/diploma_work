/*
----------------------------------------
 Copyright (C) 2024 Grovf Inc. All Rights Reserved.

 Contributors:
 D.Margaryan david.m@grovf.com
-----------------------------------------
*/

#ifndef CACHECONTROLLER_H
#define CACHECONTROLLER_H

#include <systemc>
#include <tlm>
#include <vector>
#include <list>
#include <tlm>

#include "simple_target_socket.h"
#include "simple_initiator_socket.h"

#include "cache_memory.h"
#include "main_memory.h"

#include "cache_optional_fields_extension.h"


class cache_controller : public sc_core::sc_module {

    private:
        // TODO: mutex is a 'Mutual Exclusion' and architecture requires single-ended
        //       blocking here not mutual. Maybe it is simpler for both implementation
        //       and understanding if we just will put commands into some execution
        //       queue (fifo) and merely cease fetching them during invalidation?
        //       But if there is really mutual exclusion, it is bettter to put explanaitory
        //       comment
        sc_core::sc_mutex invalidate_mutex;
        sc_core::sc_mutex flush_mutex;

        cache_memory cache_mem;

        std::vector<ongoing_request_map_t> ongoing_requests;

        std::vector<std::list<int>> lru_lists;

        int get_lru_index(int set_index);
        void update_lru(int set_index, int way_index);

        void load_block_from_memory(uint64_t address, unsigned char* block_data);
        void store_block_to_memory(uint64_t address, unsigned char* block_data, size_t size);

        bool try_load_into_cache(cache_action_params &op);
        void load_block(cache_action_params &op);
        void handle_cache_eviction(cache_action_params &op);

        bool try_read_from_cache(cache_action_params &op);
        bool try_write_to_cache(cache_action_params &op);


        void handle_read_request(tlm::tlm_generic_payload* gp);
        void handle_write_request(tlm::tlm_generic_payload* gp);
        void handle_invalidate_request(addr_range_t& address_range);
        void handle_flush_request();

        void update_write_through_cache(cache_action_params& op);

        WRITE_POLICY get_write_policy(int value);
        
        LINE_ALLOC get_read_request_line_alloc(int value);
        LINE_ALLOC get_write_request_line_alloc(int value);

    public:

        tlm::tlm_fifo<addr_range_t> invalidate_req_fifo;
        tlm::tlm_fifo<addr_range_t> invalidate_rsp_fifo;

        tlm::tlm_fifo<tlm::tlm_generic_payload*> read_req_fifo;
        tlm::tlm_fifo<tlm::tlm_generic_payload*> read_rsp_fifo;

        tlm::tlm_fifo<tlm::tlm_generic_payload*> write_req_fifo;
        tlm::tlm_fifo<tlm::tlm_generic_payload*> write_rsp_fifo;

        tlm::tlm_fifo<tlm::tlm_generic_payload*> flush_req_fifo;
        tlm::tlm_fifo<tlm::tlm_generic_payload*> flush_rsp_fifo;

        tlm_utils::simple_initiator_socket <cache_memory> memory_read_socket_i;
        tlm_utils::simple_initiator_socket <cache_memory> memory_write_socket_i;

        cache_controller(sc_core::sc_module_name name, const cache_params& params);

        void print_cache(uint64_t address);

        SC_HAS_PROCESS(cache_controller);

        void read();
        void write();
        void invalidate();
        void flush();

};

#endif //!CACHECONTROLLER_H
