#ifndef OHM_PROTOCOL_H_
#define OHM_PROTOCOL_H_

#include <stdint.h>
#include <time.h>
#include <radio/radio.h>

#define OHM_PROTOCOL_NR 6
#define MAX_HOP_LIST    (11)

typedef enum {
    OHM_HELLO,
    OHM_CHAT,
    OHM_DATA
} packet_types;

typedef struct {
    uint16_t seq_nr;
    uint8_t src;
    uint8_t type;
} ohm_packet_header_t; 

typedef struct __attribute__ ((packed)) {
    ohm_packet_header_t header;
} ohm_hello_pkt_t;

typedef struct {
    ohm_packet_header_t header;
    uint8_t len;
    char mesg[40];
} ohm_chat_pkt_t;

typedef struct __attribute__ ((packed)) {
    ohm_packet_header_t header;
    time_t timestamp;
    double temperature;
    double relhum;
    double relhum_temp;
    double energy;
    uint8_t hop_counter;
    uint8_t hops[MAX_HOP_LIST];
} ohm_data_pkt_t;

#endif
