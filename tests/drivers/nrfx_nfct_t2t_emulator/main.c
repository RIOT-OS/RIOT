#include "nrfx_nfct_t2t_emulator.h"


int main(void) {
        nfct_type_2_tag_t tag;
    uint8_t tag_memory[] = {
        '\x01', '\x02', '\x03', '\xFF', // Internal
        '\x11', '\x22', '\x33', '\x44', // Serial Number
        '\x55', '\x66', '\x00', '\x00', // Internal / Lock
        '\xE1', '\x10', '\x06', '\x00', // Capability Container
        '\x03', '\x0C', '\xD1', '\x01', 
        '\x08',                         // TLV for NDEF message (Type, Length, Payload)
        '\x54', '\x02', 'e', 'n',
        'H',    'e',    'l', 'l', 
        'o',    '\xFE', '\x00', '\x00', 
        '\x00', '\x00', '\x00', '\x00', // Data
        '\x00', '\x00', '\x00', '\x00',  // Data
        '\x00', '\x00', '\x00', '\x00',  // Data
        '\x00', '\x00', '\x00', '\x00',  // Data
        '\x00', '\x00', '\x00', '\x00',  // Data
        '\x00', '\x00', '\x00', '\x00',  // Data
        '\x00', '\x00', '\x00', '\x00',  // Data
        '\x00', '\x00', '\x00'
    };
    tag.memory = tag_memory;
    tag.memory_size = sizeof(tag_memory);

    assert(tag.memory_size == 64);

    uint8_t nfcid[] = {'\x01', '\x02', '\x03', '\xFF'};
    
    initialize_t2t(&tag, true, nfcid, sizeof(nfcid));

    enable_t2t();

    start_event_loop();

}