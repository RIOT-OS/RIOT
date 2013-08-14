/**
 * sixlowpan/ndp.h  - 6LoWPAN constants, data structs, and prototypes
 *                    related to NDP
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup sixlowpan
 * @{
 * @file
 * @brief   6LoWPAN NDP related header
 * @author  Stephan Zeisberg <zeisberg@mi.fu-berlin.de>
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author  Eric Engel <eric.engel@fu-berlin.de>
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 */

#ifndef SIXLOWPAN_NDP_H
#define SIXLOWPAN_NDP_H

#include <stdint.h>

#include "timex.h"
#include "sixlowpan/types.h"

#define NDP_6LOWPAN_CONTEXT_MAX         (16)

#define NDP_OPT_SLLAO_TYPE                 	(1)
#define NDP_OPT_TLLAO_TYPE                 	(2)
#define NDP_OPT_PI_VLIFETIME_INFINITE      	(0xffffffff)
#define NDP_OPT_ARO_STATE_SUCCESS          	(0)
#define NDP_OPT_ARO_STATE_DUP_ADDR         	(1)
#define NDP_OPT_ARO_STATE_NBR_CACHE_FULL   	(2)

typedef struct __attribute__((packed)) {
    uint8_t inuse;
    uint8_t adv;
    ipv6_addr_t addr;
    uint8_t length;
    uint8_t l_a_reserved1;
    uint32_t val_ltime;
    uint32_t pref_ltime;
    uint8_t infinite;
} ndp_prefix_list_t;

/* default router list - rfc4861 5.1. */
typedef struct __attribute__((packed)) {
    ipv6_addr_t addr;
    timex_t inval_time;
} ndp_default_router_list_t;

/* neighbor cache - rfc4861 5.1. */
typedef struct __attribute__((packed)) {
    uint8_t type;
    uint8_t state;
    uint8_t isrouter;
    ipv6_addr_t addr;
    ieee_802154_long_t laddr;
    ieee_802154_short_t saddr;
    timex_t ltime;
} ndp_neighbor_cache_t;

/* authoritive border router cache - rfc6775 */
typedef struct __attribute__((packed)) {
    uint16_t version;
    ipv6_addr_t abr_addr;
    uint8_t cids[NDP_6LOWPAN_CONTEXT_MAX];
} ndp_a6br_cache_t;

ndp_default_router_list_t *ndp_default_router_list_search(ipv6_addr_t *ipaddr);
ndp_neighbor_cache_t *ndp_neighbor_cache_search(ipv6_addr_t *ipaddr);
/*TODO: to implement*/
uint8_t ndp_prefix_list_search(ipv6_addr_t *addr);
ndp_a6br_cache_t *ndp_a6br_cache_get_most_current(void);
ndp_a6br_cache_t *ndp_a6br_cache_get_oldest(void);

/**
 * @}
 */
#endif /* SIXLOWPAN_NDP_H */
