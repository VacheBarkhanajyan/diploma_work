#include "main_memory.h"

main_memory::main_memory(sc_core::sc_module_name name)
    : sc_core::sc_module(name),
      read_socket("read_socket"),
      write_socket("write_socket") 
{

    read_socket.register_b_transport(this, &main_memory::read_b_transport);
    write_socket.register_b_transport(this, &main_memory::write_b_transport);
}

void main_memory::tie_off() {
    if (!read_socket.size()) {
		tieoff_sk = new tlm_utils::simple_initiator_socket<main_memory>();
		tieoff_sk->bind(read_socket);
	}
}


void main_memory::read(uint64_t address, unsigned char* data, size_t size) const {
    size_t page_number = address / PAGE_SIZE;
    size_t offset = address % PAGE_SIZE;

    if (page_table.find(page_number) != page_table.end()) {
        const auto& page = page_table.at(page_number);
        for (size_t i = 0; i < size; ++i) {
            data[i] = page->data[offset + i];
        }
    } else {
        for (size_t i = 0; i < size; ++i) {
            data[i] = 0;
        }
    }
}

void main_memory::write(uint64_t address, const unsigned char* data, size_t size) {
    size_t page_number = address / PAGE_SIZE;
    size_t offset = address % PAGE_SIZE;

    if (page_table.find(page_number) == page_table.end()) {
        loadPage(page_number);
    }

    auto& page = page_table[page_number];
    for (size_t i = 0; i < size; ++i) {
        page->data[offset + i] = data[i];
    }
}

void main_memory::loadPage(size_t page_number) {
    page_table[page_number] = std::make_shared<Page>();
}

void main_memory::read_b_transport(tlm::tlm_generic_payload &gp, sc_core::sc_time &t) {
    sc_core::wait(t);
    unsigned char* data = gp.get_data_ptr();

    read(gp.get_address(), data, gp.get_data_length());
}

void main_memory::write_b_transport(tlm::tlm_generic_payload &gp, sc_core::sc_time &t) {
    sc_core::wait(t);
    unsigned char* data = gp.get_data_ptr();

    write(gp.get_address(), data, gp.get_data_length());
}

