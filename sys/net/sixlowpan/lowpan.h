/*
 * 6LoWPAN constants, data structs, and prototypes
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @defgroup	net_sixlowpan 6LoWPAN
 * @ingroup 	net
 * @brief		Riots 6LowPAN implementation
 * @{
 * @file    sixlowpan.h
 * @brief   6lowpan header
 * @author  Stephan Zeisberg <zeisberg@mi.fu-berlin.de>
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 * @author  Eric Engel <eric.engel@fu-berlin.de>
 */

#ifndef _SIXLOWPAN_LOWPAN_H
#define _SIXLOWPAN_LOWPAN_H

#include "mutex.h"
#include "vtimer.h"

#include "sixlowpan/lowpan.h"

typedef struct {
    uint8_t num;
    ipv6_addr_t prefix;
    uint8_t length;
    uint8_t comp;
    uint16_t lifetime;
} lowpan_context_t;

extern uint16_t local_address;
extern mutex_t lowpan_context_mutex;

void lowpan_read(uint8_t *data, uint8_t length,
                 ieee_802154_long_t *s_laddr,
                 ieee_802154_long_t *d_laddr);
uint8_t lowpan_context_len(void);
lowpan_context_t *lowpan_context_update(uint8_t num,
                                        const ipv6_addr_t *prefix,
                                        uint8_t length, uint8_t comp,
                                        uint16_t lifetime);
lowpan_context_t *lowpan_context_get(void);
lowpan_context_t *lowpan_context_num_lookup(uint8_t num);

/**
 * @}
 */
#endif  /* _SIXLOWPAN_LOWPAN_H */
