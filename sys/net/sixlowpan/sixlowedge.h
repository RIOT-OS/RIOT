/* 6LoWPAN Edge Router header file */

#ifndef SIXLOWEDGE_H
#define SIXLOWEDGE_H

#include <stdint.h>
#include <mutex.h>
#include "sixlowip.h"
#include "sixlowpan.h"
#include "sixlownd.h"

#define EDGE_HEADER_LENGTH      2

/* packet types of uart-packets */
#define EDGE_PACKET_RAW_TYPE    0

#define EDGE_PACKET_ACK_TYPE    1

#define EDGE_PACKET_CONF_TYPE   2

#define EDGE_PACKET_L3_TYPE     3

/* ethertypes for L3 packets */
#define EDGE_ETHERTYPE_IPV6     0x86DD

extern uint16_t abro_version;

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

#define EDGE_BUFFER_SIZE (sizeof (edge_l3_header_t) + MTU)

uint8_t edge_initialize(transceiver_type_t trans,ipv6_addr_t *edge_router_addr);
void edge_send_ipv6_over_uart(struct ipv6_hdr_t *packet);
void edge_process_uart(void);
void edge_send_ipv6_over_lowpan(struct ipv6_hdr_t *packet);
void edge_process_lowpan(void);

abr_cache_t *get_edge_router_info();

lowpan_context_t *edge_define_context(uint8_t cid, ipv6_addr_t *prefix, uint8_t prefix_len, uint16_t lifetime);
lowpan_context_t *edge_alloc_context(ipv6_addr_t *prefix, uint8_t prefix_len, uint16_t lifetime);

#endif /* SIXLOWEDGE_H*/
