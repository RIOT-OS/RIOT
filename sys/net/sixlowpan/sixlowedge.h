/* 6LoWPAN Edge Router header file */

#ifndef SIXLOWEDGE_H
#define SIXLOWEDGE_H

#include <stdint.h>
#include <mutex.h>
#include <vtimer.h>
#include "sixlowip.h"
#include "sixlowpan.h"
#include "sixlownd.h"
#include "semaphore.h"

#define EDGE_SWS                8
#define EDGE_RWS                8
#define EDGE_SL_TIMEOUT         500 // microseconds, maybe smaller

#define EDGE_HEADER_LENGTH      2

/* packet types of uart-packets */
#define EDGE_PACKET_RAW_TYPE    0
#define EDGE_PACKET_ACK_TYPE    1
#define EDGE_PACKET_CONF_TYPE   2
#define EDGE_PACKET_L3_TYPE     3

/* configuration types */
#define EDGE_CONF_SYN           0
#define EDGE_CONF_SYNACK        1
#define EDGE_CONF_CONTEXT       2
#define EDGE_CONF_IPADDR        3

/* ethertypes for L3 packets */
#define EDGE_ETHERTYPE_IPV6     0x86DD

extern uint16_t abro_version;
extern int tun_fd;

typedef struct __attribute__ ((packed)) edge_packet_t {
    uint8_t reserved;
    uint8_t type;
    uint8_t seq_num;
} edge_packet_t;

typedef struct __attribute__ ((packed)) edge_l3_header_t {
    uint8_t reserved;
    uint8_t type;
    uint8_t seq_num;
    uint16_t ethertype;
} edge_l3_header_t;

typedef struct __attribute__ ((packed)) edge_conf_header_t {
    uint8_t reserved;
    uint8_t type;
    uint8_t seq_num;
    uint8_t conftype;
} edge_conf_header_t;

typedef struct __attribute__ ((packed)) edge_syn_packet_t {
    uint8_t reserved;
    uint8_t type;
    uint8_t next_seq_num;
    uint8_t conftype;
    uint8_t next_exp;
    ipv6_addr_t addr;
} edge_syn_packet_t;

typedef struct __attribute__ ((packed)) edge_addr_packet_t {
    uint8_t reserved;
    uint8_t type;
    uint8_t seq_num;
    uint8_t conftype;
    ipv6_addr_t addr;
} edge_addr_packet_t;

typedef struct __attribute__ ((packed)) edge_context_packet_t {
    uint8_t reserved;
    uint8_t type;
    uint8_t seq_num;
    uint8_t conftype;
    struct edge_context_t {
        uint16_t abro_version;
        uint8_t num;
        ipv6_addr_t prefix;
        uint8_t length;
        uint8_t comp;
        uint16_t lifetime;
    } context;
} edge_context_packet_t;

#define EDGE_BUFFER_SIZE (sizeof (edge_l3_header_t) + MTU)

typedef struct slwin_stat_t {
    /* Sender state */
    uint8_t last_ack;
    uint8_t last_frame;
    sem_t send_win_not_full;
    struct send_slot {
        vtimer_t timeout; 
        uint8_t frame[EDGE_BUFFER_SIZE];
        size_t frame_len;
    } send_win[EDGE_SWS];
    
    /* Receiver state */
    uint8_t next_exp;
    struct recv_slot {
        int8_t received;
        uint8_t frame[EDGE_BUFFER_SIZE];
        size_t frame_len;
    } recv_win[EDGE_RWS];
} slwin_stat_t;

uint8_t edge_initialize(transceiver_type_t trans,ipv6_addr_t *edge_router_addr);
void edge_send_ipv6_over_uart(struct ipv6_hdr_t *packet);
void edge_send_over_uart(edge_packet_t *packet, int len);
void edge_send_ipv6_over_lowpan(struct ipv6_hdr_t *packet, uint8_t aro_flag, uint8_t sixco_flag);
void edge_process_lowpan(void);
// to be removed
void edge_process_uart(void);

void slwin_init();

abr_cache_t *get_edge_router_info();

lowpan_context_t *edge_define_context(uint8_t cid, ipv6_addr_t *prefix, uint8_t prefix_len, uint16_t lifetime);
lowpan_context_t *edge_alloc_context(ipv6_addr_t *prefix, uint8_t prefix_len, uint16_t lifetime);

#endif /* SIXLOWEDGE_H*/
