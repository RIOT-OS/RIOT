/*
 * 6lowpan constants, data structs, and prototypes
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup sixlowpan
 * @{
 * @file    sixlowpan.h
 * @brief   6lowpan header 
 * @author  Stephan Zeisberg <zeisberg@mi.fu-berlin.de>
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 * @author  Eric Engel <eric.engel@fu-berlin.de>
 * @}
 */

#ifndef SIXLOWPAN_H
#define SIXLOWPAN_H

#define IP_PROCESS_STACKSIZE            3072
#define NC_STACKSIZE                    512
#define CON_STACKSIZE                   512
#define LOWPAN_TRANSFER_BUF_STACKSIZE   512

/* fragment size in bytes*/
#define FRAG_PART_ONE_HDR_LEN   4
#define FRAG_PART_N_HDR_LEN     5

#define LOWPAN_IPHC_DISPATCH    0x60
#define LOWPAN_IPHC_FL_C        0x10
#define LOWPAN_IPHC_TC_C        0x08
#define LOWPAN_IPHC_CID         0x80
#define LOWPAN_IPHC_SAC         0x40
#define LOWPAN_IPHC_SAM         0x30
#define LOWPAN_IPHC_DAC         0x04
#define LOWPAN_IPHC_DAM         0x03
#define LOWPAN_IPHC_M           0x08
#define LOWPAN_IPHC_NH          0x04
#define LOWPAN_IPV6_DISPATCH    0x41
#define LOWPAN_CONTEXT_MAX      16

#define LOWPAN_REAS_BUF_TIMEOUT 15 * 1000 * 1000 /* TODO: Set back to 3 * 1000 * 1000 */

#include "transceiver.h"
#include "sixlowip.h"
#include <vtimer.h>
#include <mutex.h>
#include <time.h>

extern mutex_t lowpan_context_mutex;
extern uint8_t static_route;
extern uint16_t local_address;

typedef struct lowpan_context_t {
    uint8_t num;
    ipv6_addr_t prefix;
    uint8_t length;
    uint8_t comp;
    uint16_t lifetime;
} lowpan_context_t;

typedef struct lowpan_interval_list_t {
    uint8_t                         start;
    uint8_t                         end;
    struct lowpan_interval_list_t   *next;
} lowpan_interval_list_t;

typedef struct lowpan_reas_buf_t {
    /* Source Address */
    ieee_802154_long_t       s_laddr;
    /* Destination Address */
    ieee_802154_long_t       d_laddr;
    /* Identification Number */
    uint16_t                 ident_no;
    /* Timestamp of last packet fragment */
    long                     timestamp;
    /* Size of reassembled packet with possible IPHC header */
    uint16_t                 packet_size;
    /* Additive size of currently already received fragments */
    uint16_t                 current_packet_size;
    /* Pointer to allocated memory for reassembled packet + 6LoWPAN Dispatch Byte */
    uint8_t                  *packet;
    /* Pointer to list of intervals of received packet fragments (if any) */
    lowpan_interval_list_t   *interval_list_head;
    /* Pointer to next reassembly buffer (if any) */
    struct lowpan_reas_buf_t *next;
} lowpan_reas_buf_t;

extern lowpan_reas_buf_t *head;

void sixlowpan_init(transceiver_type_t trans, uint8_t r_addr, int as_border);
void sixlowpan_adhoc_init(transceiver_type_t trans, ipv6_addr_t *prefix, 
                          uint8_t r_addr);
void lowpan_init(ieee_802154_long_t *addr, uint8_t *data);
void lowpan_read(uint8_t *data, uint8_t length, ieee_802154_long_t *s_laddr,
                 ieee_802154_long_t *d_laddr);
void lowpan_iphc_encoding(ieee_802154_long_t *dest, ipv6_hdr_t *ipv6_buf_extra, uint8_t *ptr);
void lowpan_iphc_decoding(uint8_t *data, uint8_t length,
                          ieee_802154_long_t *s_laddr,
                          ieee_802154_long_t *d_laddr);
uint8_t lowpan_context_len(void);
void add_fifo_packet(lowpan_reas_buf_t *current_packet);
lowpan_context_t *lowpan_context_update(
    uint8_t num, const ipv6_addr_t *prefix,
    uint8_t length, uint8_t comp,
    uint16_t lifetime);
lowpan_context_t *lowpan_context_get(void);
lowpan_context_t *lowpan_context_lookup(ipv6_addr_t *addr);
lowpan_context_t *lowpan_context_num_lookup(uint8_t num);
lowpan_reas_buf_t *collect_garbage_fifo(lowpan_reas_buf_t *current_buf);
lowpan_reas_buf_t *collect_garbage(lowpan_reas_buf_t *current_buf);
void check_timeout(void);
void lowpan_ipv6_set_dispatch(uint8_t *data);
void init_reas_bufs(lowpan_reas_buf_t *buf);
void printReasBuffers(void);
void printFIFOBuffers(void);
#endif
