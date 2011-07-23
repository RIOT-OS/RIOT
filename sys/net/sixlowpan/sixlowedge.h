/* 6LoWPAN Border Router header file */

#ifndef SIXLOWBORDER_H
#define SIXLOWBORDER_H

#include <stdint.h>
#include <mutex.h>
#include <vtimer.h>
#include "sixlowip.h"
#include "sixlowpan.h"
#include "sixlownd.h"
#include "semaphore.h"

#define BORDER_SWS                8
#define BORDER_RWS                8
#define BORDER_SL_TIMEOUT         500 // microseconds, maybe smaller

#define BORDER_HEADER_LENGTH      2

/* packet types of uart-packets */
#define BORDER_PACKET_RAW_TYPE    0
#define BORDER_PACKET_ACK_TYPE    1
#define BORDER_PACKET_CONF_TYPE   2
#define BORDER_PACKET_L3_TYPE     3

/* configuration types */
#define BORDER_CONF_SYN           0
#define BORDER_CONF_SYNACK        1
#define BORDER_CONF_CONTEXT       2
#define BORDER_CONF_IPADDR        3

/* ethertypes for L3 packets */
#define BORDER_ETHERTYPE_IPV6     0x86DD

extern uint16_t abro_version;
extern int tun_fd;

typedef struct __attribute__ ((packed)) border_packet_t {
    uint8_t reserved;
    uint8_t type;
    uint8_t seq_num;
} border_packet_t;

typedef struct __attribute__ ((packed)) border_l3_header_t {
    uint8_t reserved;
    uint8_t type;
    uint8_t seq_num;
    uint16_t ethertype;
} border_l3_header_t;

typedef struct __attribute__ ((packed)) border_conf_header_t {
    uint8_t reserved;
    uint8_t type;
    uint8_t seq_num;
    uint8_t conftype;
} border_conf_header_t;

typedef struct __attribute__ ((packed)) border_syn_packet_t {
    uint8_t reserved;
    uint8_t type;
    uint8_t next_seq_num;
    uint8_t conftype;
    uint8_t next_exp;
    ipv6_addr_t addr;
} border_syn_packet_t;

typedef struct __attribute__ ((packed)) border_addr_packet_t {
    uint8_t reserved;
    uint8_t type;
    uint8_t seq_num;
    uint8_t conftype;
    ipv6_addr_t addr;
} border_addr_packet_t;

typedef struct __attribute__ ((packed)) border_context_packet_t {
    uint8_t reserved;
    uint8_t type;
    uint8_t seq_num;
    uint8_t conftype;
    struct border_context_t {
        uint16_t abro_version;
        uint8_t num;
        ipv6_addr_t prefix;
        uint8_t length;
        uint8_t comp;
        uint16_t lifetime;
    } context;
} border_context_packet_t;

#define BORDER_BUFFER_SIZE (sizeof (border_l3_header_t) + MTU)

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

uint8_t border_initialize(transceiver_type_t trans,ipv6_addr_t *border_router_addr);
void multiplex_send_ipv6_over_uart(struct ipv6_hdr_t *packet);
void flowcontrol_send_over_uart(border_packet_t *packet, int len);
void border_send_ipv6_over_lowpan(struct ipv6_hdr_t *packet, uint8_t aro_flag, uint8_t sixco_flag);
void border_process_lowpan(void);
// to be removed
void border_process_uart(void);

void slwin_init();

abr_cache_t *get_border_router_info();

lowpan_context_t *border_define_context(uint8_t cid, ipv6_addr_t *prefix, uint8_t prefix_len, uint16_t lifetime);
lowpan_context_t *border_alloc_context(ipv6_addr_t *prefix, uint8_t prefix_len, uint16_t lifetime);

#endif /* SIXLOWBORDER_H*/
