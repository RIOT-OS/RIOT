/**
 * IPv6 constants, data structs, and prototypes
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
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

#ifndef _SIXLOWPAN_IP_H
#define _SIXLOWPAN_IP_H

#include <stdint.h>

#include "timex.h"
#include "mutex.h"

#include "sixlowpan/ip.h"
#include "sixlowpan/types.h"

/* IPv6 field values */
#define IPV6_VER                    (0x60)
/* size of global buffer */
#define BUFFER_SIZE (LL_HDR_LEN + IPV6_MTU)

#define MULTIHOP_HOPLIMIT           (64)

#define SIXLOWIP_MAX_REGISTERED     (4)

/* extern variables */
extern uint8_t ipv6_ext_hdr_len;

/* base header lengths */
#define LL_HDR_LEN                  (0x4)
#define ICMPV6_HDR_LEN              (0x4)
#define IPV6_HDR_LEN                (0x28)

#define IFACE_ADDR_LIST_LEN         (10) // maybe to much

/* buffer */
extern uint8_t buffer[BUFFER_SIZE];

extern int sixlowip_reg[SIXLOWIP_MAX_REGISTERED];

typedef struct __attribute__((packed)) {
    ipv6_addr_t addr;
    ipv6_addr_type_t type;
    ndp_addr_state_t state;
    timex_t val_ltime;
    timex_t pref_ltime;
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
void ipv6_send_bytes(ipv6_hdr_t *bytes);
icmpv6_hdr_t *get_icmpv6_buf(uint8_t ext_len);
uint8_t *get_payload_buf(uint8_t ext_len);
uint8_t *get_payload_buf_send(uint8_t ext_len);

int icmpv6_demultiplex(const icmpv6_hdr_t *hdr);
void ipv6_init_iface_as_router(void);
void ipv6_process(void);
addr_list_t *ipv6_iface_addr_prefix_eq(ipv6_addr_t *addr);
addr_list_t *ipv6_iface_addr_match(const ipv6_addr_t *addr);
uint32_t get_remaining_time(timex_t *t);
void set_remaining_time(timex_t *t, uint32_t time);

#endif /* _SIXLOWPAN_IP_H*/
