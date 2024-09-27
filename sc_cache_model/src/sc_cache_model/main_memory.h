/*
----------------------------------------
 Copyright (C) 2024 Grovf Inc. All Rights Reserved.

 Contributors:
 D.Margaryan david.m@grovf.com
-----------------------------------------
*/



#ifndef MAIN_MEMORY_H
#define MAIN_MEMORY_H

#include <systemc.h>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <memory>
#include <tlm>
#include "simple_target_socket.h"
#include "simple_initiator_socket.h"

const size_t PAGE_SIZE = 4096;

class Page {
public:
    Page() : data(PAGE_SIZE, 0) {}
    std::vector<uint8_t> data;
};

class main_memory : public sc_core::sc_module {

public:
    tlm_utils::simple_target_socket<main_memory> read_socket;
    tlm_utils::simple_target_socket<main_memory> write_socket;

    main_memory(sc_core::sc_module_name name);

    void read_b_transport(tlm::tlm_generic_payload &gp, sc_core::sc_time &t);
    void write_b_transport(tlm::tlm_generic_payload &gp, sc_core::sc_time &t);

    void tie_off();

private:
    std::unordered_map<size_t, std::shared_ptr<Page>> page_table;

	tlm_utils::simple_initiator_socket<main_memory> *tieoff_sk;

    void read(uint64_t address, unsigned char* data, size_t size) const;
    void write(uint64_t address, const unsigned char* data, size_t size);
    void loadPage(size_t page_number);
};

#endif // MAIN_MEMORY_H
