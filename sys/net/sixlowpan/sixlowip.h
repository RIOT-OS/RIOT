/**
 * IPv6 constants, data structs, and prototypes
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup sixlowpan
 * @{
 * @file    sixlowip.h
 * @brief   6lowpan IP layer header
 * @author  Stephan Zeisberg <zeisberg@mi.fu-berlin.de>
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author  Eric Engel <eric.engel@fu-berlin.de>
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 * @}
 */

/* 6LoWPAN IP header file */

#ifndef SIXLOWIP_H
#define SIXLOWIP_H

#include <stdint.h>
#include <timex.h>
#include <mutex.h>

/* set maximum transmission unit */
#define MTU                         256
/* IPv6 field values */
#define IPV6_VER                    0x60
#define PROTO_NUM_ICMPV6            58
#define PROTO_NUM_NONE              59
#define ND_HOPLIMIT                 0xFF
#define SIXLOWPAN_IPV6_LL_ADDR_LEN  8
/* size of global buffer */
#define BUFFER_SIZE (LL_HDR_LEN + MTU)

#define MULTIHOP_HOPLIMIT           64

#define IP_PKT_RECV_BUF_SIZE        64
#define DEBUGLINE printf("%s:%d\n",__FILE__,__LINE__)

/* extern variables */
extern uint8_t ipv6_ext_hdr_len;
extern uint8_t opt_hdr_len;
extern uint16_t packet_length;
extern uint8_t packet_dispatch;
extern uint8_t iface_addr_list_count;
extern mutex_t buf_mutex;

extern double start;

/* base header lengths */
#define LL_HDR_LEN                  0x4
#define ICMPV6_HDR_LEN              0x4
#define IPV6_HDR_LEN                0x28
#define LLHDR_IPV6HDR_LEN           (LL_HDR_LEN + IPV6_HDR_LEN)
#define LLHDR_ICMPV6HDR_LEN         (LL_HDR_LEN + IPV6_HDR_LEN + ICMPV6_HDR_LEN)
#define IPV6HDR_ICMPV6HDR_LEN       (IPV6_HDR_LEN + ipv6_ext_hdr_len + ICMPV6_HDR_LEN)

#define IFACE_ADDR_LIST_LEN         10 // maybe to much
/* rfc 4862 section 2. address states */
#define ADDR_STATE_TENTATIVE        0
#define ADDR_STATE_PREFERRED        1
#define ADDR_STATE_DEPRECATED       2
/* addresses with this state are always permitted */
#define ADDR_STATE_ANY              4
/* how the address is configured */
#define ADDR_CONFIGURED_AUTO        1
#define ADDR_CONFIGURED_MANUAL      2
/* address types */
#define ADDR_TYPE_NONE              0
#define ADDR_TYPE_UNICAST           1
#define ADDR_TYPE_MULTICAST         2
#define ADDR_TYPE_ANYCAST           3
#define ADDR_TYPE_SOL_NODE_MCAST    4
#define ADDR_TYPE_LOOPBACK          5
#define ADDR_TYPE_LINK_LOCAL        6
#define ADDR_TYPE_GLOBAL            7
/* dispatch types */
#define DISPATCH_TYPE_IPV6          0x41
#define DISPATCH_TYPE_LOWPAN_HC1    0x42
/* compression types */
#define COMPRESSION_TYPE_NONE

/* buffer */
extern uint8_t buffer[BUFFER_SIZE];


/* ipv6 extension header length */

typedef union __attribute__((packed)) {
    uint8_t uint8[16];
    uint16_t uint16[8];
    uint32_t uint32[4];
} ipv6_addr_t;

struct __attribute__((packed)) icmpv6_hdr_t {
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
};

typedef struct __attribute__((packed)) {
    uint8_t version_trafficclass;
    uint8_t trafficclass_flowlabel;
    uint16_t flowlabel;
    uint16_t length;
    uint8_t nextheader;
    uint8_t hoplimit;
    ipv6_addr_t srcaddr;
    ipv6_addr_t destaddr;
} ipv6_hdr_t;

/* link layer addressing */
typedef union __attribute__((packed)) {
    uint8_t uint8[8];
    uint16_t uint16[4];
} ieee_802154_long_t;

typedef union __attribute__((packed)) {
    uint8_t uint8[2];
    uint16_t uint16[1];
} ieee_802154_short_t;

typedef struct __attribute__((packed)) {
    uint8_t state;
    timex_t val_ltime;
    timex_t pref_ltime;
    uint8_t type;
    ipv6_addr_t addr;
} addr_list_t;

typedef struct __attribute__((packed)) {
    ieee_802154_short_t saddr;
    ieee_802154_long_t laddr;
    addr_list_t addr_list[IFACE_ADDR_LIST_LEN];
    uint8_t adv_cur_hop_limit;
    uint32_t adv_reachable_time;
    uint32_t adv_retrans_timer;
} iface_t;

extern iface_t iface;

/* function prototypes */
struct icmpv6_hdr_t *get_icmpv6_buf(uint8_t ext_len);
struct ipv6_hdr_t *get_ipv6_buf(void);
uint8_t *get_payload_buf(uint8_t ext_len);
uint8_t *get_payload_buf_send(uint8_t ext_len);

int icmpv6_demultiplex(const struct icmpv6_hdr_t *hdr);
void ipv6_init_iface_as_router(void);
uint8_t ipv6_is_router(void);
void ipv6_set_ll_prefix(ipv6_addr_t *ipaddr);
void ipv6_set_all_rtrs_mcast_addr(ipv6_addr_t *ipaddr);
void ipv6_set_all_nds_mcast_addr(ipv6_addr_t *ipaddr);
void ipv6_set_loaddr(ipv6_addr_t *ipaddr);
void ipv6_set_sol_node_mcast_addr(ipv6_addr_t *addr_in, ipv6_addr_t *addr_out);
void sixlowpan_bootstrapping(void);
void sixlowpan_send(ipv6_addr_t *addr, uint8_t *payload, uint16_t p_len, uint8_t next_header);
void ipv6_print_addr(ipv6_addr_t *ipaddr);
void ipv6_process(void);
void ipv6_get_saddr(ipv6_addr_t *src, ipv6_addr_t *dst);
uint8_t ipv6_get_addr_match(ipv6_addr_t *src, ipv6_addr_t *dst);
uint8_t ipv6_prefix_mcast_match(ipv6_addr_t *addr);
uint8_t ipv6_prefix_ll_match(ipv6_addr_t *addr);
void ipv6_iface_add_addr(ipv6_addr_t *addr, uint8_t state, uint32_t val_ltime,
                         uint32_t pref_ltime, uint8_t type);
addr_list_t *ipv6_iface_addr_prefix_eq(ipv6_addr_t *addr);
addr_list_t *ipv6_iface_addr_match(ipv6_addr_t *addr);
void ipv6_iface_print_addrs(void);
void ipv6_init_addr_prefix(ipv6_addr_t *inout, ipv6_addr_t *prefix);
void ipv6_init_address(ipv6_addr_t *addr, uint16_t addr0, uint16_t addr1,
                       uint16_t addr2, uint16_t addr3, uint16_t addr4,
                       uint16_t addr5, uint16_t addr6, uint16_t addr7);
uint32_t get_remaining_time(timex_t *t);
void set_remaining_time(timex_t *t, uint32_t time);
void ipv6_set_prefix(ipv6_addr_t *inout, ipv6_addr_t *prefix);
uint8_t ipv6_addr_unspec_match(ipv6_addr_t *addr);
uint8_t ipv6_addr_sol_node_mcast_match(ipv6_addr_t *addr);
uint8_t ipv6_next_hdr_unrec(uint8_t next_hdr);
void set_tcp_packet_handler_pid(int pid);
void set_udp_packet_handler_pid(int pid);
void set_rpl_process_pid(int pid);
#endif /* SIXLOWIP_H*/
