
#include "cache_line.h"


cache_line::cache_line(sc_core::sc_module_name name, const cache_params& params) 
                      : sc_core::sc_module(name), 
                        valid(false), 
                        dirty(false), 
                        tag(0),
                        params(params),
                        data(params.line_size) 
{

}


void cache_line::read_word(uint32_t word_offset, uint8_t* word_data, int word_size, int delay) const {
        sc_core::wait(delay, sc_core::SC_NS);
        std::memcpy(word_data, &data[word_offset * params.word_size], word_size);

}

void cache_line::write_word(uint32_t word_offset, const uint8_t* word_data, int word_size, int delay) {
        sc_core::wait(delay, sc_core::SC_NS);
        uint32_t row = word_offset / params.word_size;
        uint32_t col = word_offset % params.word_size;

        std::memcpy(&data[row * params.word_size + col], word_data, word_size);
        
}
