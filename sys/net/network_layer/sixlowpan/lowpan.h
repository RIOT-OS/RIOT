/*
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file        network_layer/sixlowpan/lowpan.h
 * @brief       6lowpan header
 *
 * @author      Stephan Zeisberg <zeisberg@mi.fu-berlin.de>
 * @author      Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author      Oliver Gesch <oliver.gesch@googlemail.com>
 * @author      Eric Engel <eric.engel@fu-berlin.de>
 */

#ifndef _SIXLOWPAN_LOWPAN_H
#define _SIXLOWPAN_LOWPAN_H

#include "mutex.h"
#include "net_if.h"
#include "vtimer.h"

#include "sixlowpan/lowpan.h"

#define IEEE802154_TRANSCEIVER      (TRANSCEIVER_AT86RF231 | TRANSCEIVER_CC2420 | TRANSCEIVER_MC1322X)

typedef struct {
    uint8_t num;
    ipv6_addr_t prefix;
    uint8_t length;
    uint8_t comp;
    uint16_t lifetime;
} lowpan_context_t;

extern uint16_t local_address;
extern mutex_t lowpan_context_mutex;
extern kernel_pid_t lowpan_aux_pid;

typedef struct {
    ipv6_addr_t *dest;
    void *data;
    uint16_t data_len;
} ns_fallback_args_t;

void lowpan_read(uint8_t *data, uint8_t length, net_if_eui64_t *s_addr,
                 net_if_eui64_t *d_addr);
uint8_t lowpan_context_len(void);
lowpan_context_t *lowpan_context_update(uint8_t num,
                                        const ipv6_addr_t *prefix,
                                        uint8_t length, uint8_t comp,
                                        uint16_t lifetime);
lowpan_context_t *lowpan_context_get(void);
lowpan_context_t *lowpan_context_num_lookup(uint8_t num);

#endif  /* _SIXLOWPAN_LOWPAN_H */
