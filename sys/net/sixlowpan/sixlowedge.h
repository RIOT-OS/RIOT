/* 6LoWPAN Edge Router header file */

#ifndef SIXLOWEDGE_H
#define SIXLOWEDGE_H

#include <stdint.h>
#include "sixlowip.h"
#include "sixlowpan.h"
#include "sixlownd.h"

/* constants for conjestion control [rfc2581 section 1] */
#define EDGE_CC_SMSS            1500
#define EDGE_CC_RMSS            1500
#define EDGE_CC_IW              3000 // 2*SMSS
#define EDGE_CC_LW              1500 // 1*SMSS

/* packet types of tty-packets */
#define EDGE_PACKET_TYPE_CONF   0
#define EDGE_PACKET_TYPE_L3     3

/* ethertypes for L3 packets */
#define EDGE_ETHERTYPE_IPV6     0x86DD

extern uint16_t abro_version;

typedef struct edge_packet_t {
    uint8_t type;
    uint8_t seq_num;
} edge_packet_t;

typedef struct edge_l3_header_t {
    uint8_t type;
    uint8_t seq_num;
    uint16_t ethertype;
} edge_l3_header_t;

uint8_t edge_initialize(transceiver_type_t trans,ipv6_addr_t *edge_router_addr);
void edge_send_ipv6_over_tty(uint8_t *packet);
void edge_process_tty(void);
void edge_send_ipv6_over_lowpan(uint8_t *packet);
void edge_process_lowpan(void);

abr_cache_t *get_edge_router_info();

lowpan_context_t *edge_define_context(uint8_t cid, ipv6_addr_t *prefix, uint8_t prefix_len, uint16_t lifetime);
lowpan_context_t *edge_alloc_context(ipv6_addr_t *prefix, uint8_t prefix_len, uint16_t lifetime);

#endif /* SIXLOWEDGE_H*/
