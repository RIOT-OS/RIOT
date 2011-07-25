#ifndef FLOWCONTROL_H
#define FLOWCONTROL_H

#include <stdint.h>
#include <vtimer.h>

#include "semaphore.h"
#include "sixlowip.h"
#include "bordermultiplex.h"

/* packet types for flowcontrol */
#define BORDER_PACKET_ACK_TYPE    1

/* configuration types for flowcontrol */
#define BORDER_CONF_SYN           0
#define BORDER_CONF_SYNACK        1

#define BORDER_SWS                8
#define BORDER_RWS                8
#define BORDER_SL_TIMEOUT         500 // microseconds, maybe smaller

typedef struct flowcontrol_stat_t {
    /* Sender state */
    uint8_t last_ack;
    uint8_t last_frame;
    sem_t send_win_not_full;
    struct send_slot {
        vtimer_t timeout; 
        uint8_t frame[BORDER_BUFFER_SIZE];
        size_t frame_len;
    } send_win[BORDER_SWS];
    
    /* Receiver state */
    uint8_t next_exp;
    struct recv_slot {
        int8_t received;
        uint8_t frame[BORDER_BUFFER_SIZE];
        size_t frame_len;
    } recv_win[BORDER_RWS];
} flowcontrol_stat_t;

typedef struct __attribute__ ((packed)) border_syn_packet_t {
    uint8_t reserved;
    uint8_t type;
    uint8_t next_seq_num;
    uint8_t conftype;
    uint8_t next_exp;
    ipv6_addr_t addr;
} border_syn_packet_t;

ipv6_addr_t flowcontrol_init();
void flowcontrol_send_over_uart(border_packet_t *packet, int len);
void flowcontrol_deliver_from_uart(border_packet_t *packet, int len);

#endif /* FLOWCONTROL_H*/
