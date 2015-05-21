/*
 * IPv6 constants, data structs, and prototypes
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup sixlowpan
 * @{
 * @file
 * @brief   6lowpan IP layer header
 * @author  Stephan Zeisberg <zeisberg@mi.fu-berlin.de>
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author  Eric Engel <eric.engel@fu-berlin.de>
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 * @}
 */

/* 6LoWPAN IP header file */

#ifndef _SIXLOWPAN_IP_H
#define _SIXLOWPAN_IP_H

#include <stdint.h>

#include "kernel.h"
#include "thread.h"
#include "timex.h"
#include "mutex.h"
#include "net_if.h"

#include "sixlowpan/ip.h"
#include "sixlowpan/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* IPv6 field values */
#define IPV6_VER                    (0x60)
/* size of global buffer */
#define BUFFER_SIZE (LL_HDR_LEN + IPV6_MTU)

#define MULTIHOP_HOPLIMIT           (64)

#define SIXLOWIP_MAX_REGISTERED     (4)
#define IP_PROCESS_STACKSIZE        (THREAD_STACKSIZE_MAIN)

/* extern variables */
extern uint8_t ipv6_ext_hdr_len;
extern kernel_pid_t ip_process_pid;

#ifdef MODULE_RPL
extern int srh_handler_pid;
#endif

/* base header lengths */
#define LL_HDR_LEN                  (0x4)
#define ICMPV6_HDR_LEN              (0x4)
#define IPV6_HDR_LEN                (0x28)

#define IPV6_NET_IF_ADDR_LIST_LEN   (10)    // maybe to much

/* buffer */
extern uint8_t sixlowpan_buffer[BUFFER_SIZE];
extern char ip_process_buf[IP_PROCESS_STACKSIZE];

extern kernel_pid_t sixlowip_reg[SIXLOWIP_MAX_REGISTERED];

typedef struct __attribute__((packed))
{
    struct net_if_addr_t *addr_next;
    struct net_if_addr_t *addr_prev;
    net_if_l3p_t addr_protocol;
    ipv6_addr_t *addr_data;
    uint8_t addr_len;
    ndp_addr_state_t ndp_state;
    timex_t valid_lifetime;
    timex_t preferred_lifetime;
    uint8_t is_anycast;
}
ipv6_net_if_addr_t;

typedef struct __attribute__((packed))
{
    ipv6_net_if_addr_t *addr;
    int if_id;
}
ipv6_net_if_hit_t;

typedef struct __attribute__((packed))
{
    uint8_t prefix;             ///< prefix length of the sub-net
    uint8_t adv_cur_hop_limit;
    uint32_t adv_reachable_time;
    uint32_t adv_retrans_timer;
}
ipv6_net_if_ext_t;

/* function prototypes */
ipv6_net_if_ext_t *ipv6_net_if_get_ext(int if_id);

icmpv6_hdr_t *get_icmpv6_buf(uint8_t ext_len);
uint8_t *get_payload_buf(uint8_t ext_len);
uint8_t *get_payload_buf_send(uint8_t ext_len);

int icmpv6_demultiplex(const icmpv6_hdr_t *hdr);
int ipv6_init_as_router(void);
void *ipv6_process(void *);
ipv6_net_if_hit_t *ipv6_net_if_addr_prefix_eq(ipv6_net_if_hit_t *hit, ipv6_addr_t *addr);
ipv6_net_if_hit_t *ipv6_net_if_addr_match(ipv6_net_if_hit_t *hit, const ipv6_addr_t *addr);
uint32_t get_remaining_time(timex_t *t);
void set_remaining_time(timex_t *t, uint32_t time);

#ifdef __cplusplus
}
#endif

#endif /* _SIXLOWPAN_IP_H*/
