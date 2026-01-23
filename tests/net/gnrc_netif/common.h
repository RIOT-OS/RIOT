/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    tests_gnrc_ipv6_nib Common header for GNRC's NIB tests
 * @ingroup     tests
 * @brief       Common definitions for GNRC's NIB tests
 * @{
 *
 * @file
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include "net/gnrc/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NETIF_NUMOF         (4)
#define SPECIAL_DEVS        (2)
#define DEFAULT_DEVS_NUMOF  (NETIF_NUMOF - SPECIAL_DEVS)
#define DEV_ETHERNET   0
#define DEV_IEEE802154 1

#define GP1 (0x20U)
#define GP2 (0x01U)
#define GP3 (0x0dU)
#define GP4 (0xb8U)
#define GP5 (0x00U)
#define GP6 (0x00U)
#define GP7 (0x5aU)
#define GP8 (0x1aU)

#define LP1 (0xfeU)
#define LP2 (0x80U)
#define LP3 (0x00U)
#define LP4 (0x00U)
#define LP5 (0x00U)
#define LP6 (0x00U)
#define LP7 (0x00U)
#define LP8 (0x00U)

#define LA1 (0x3eU)
#define LA2 (0xe6U)
#define LA3 (0xb5U)
#define LA4 (0x0fU)
#define LA5 (0x19U)
#define LA6 (0x22U)
#define LA7 (0xfdU)
#define LA8 (0x0aU)

#define ULA1 (0xfdU)
#define ULA2 (0x01U)
#define ULA3 (0x0dU)
#define ULA4 (0xb8U)
#define ULA5 (0x00U)
#define ULA6 (0x00U)
#define ULA7 (0x5aU)
#define ULA8 (0x1aU)

#define TEST_IEEE802154_MAX_FRAG_SIZE   (102)

#define ETHERNET_SRC        { LA1, LA2, LA3, LA6, LA7, LA8 }
#define ETHERNET_IPV6_LL    { LP1, LP2, LP3, LP4, LP5, LP6, LP7, LP8, \
                              LA1 ^ 0x2, LA2, LA3, 0xff, 0xfe, LA6, LA7, LA8 }
#define ETHERNET_IPV6_G     { GP1, GP2, GP3, GP4, GP5, GP6, GP7, GP8, \
                              LA1 ^ 0x2, LA2, LA3, 0xff, 0xfe, LA6, LA7, LA8 }
#define IEEE802154_LONG_SRC     { LA1, LA2, LA3, LA4, LA5, LA6, LA7, LA8 }
#define IEEE802154_SHORT_SRC    { LA7, LA8 }
#define IEEE802154_IPV6_LL  { LP1, LP2, LP3, LP4, LP5, LP6, LP7, LP8, \
                              LA1 ^ 0x2, LA2, LA3, LA4, LA5, LA6, LA7, LA8 }
#define IEEE802154_IPV6_G   { GP1, GP2, GP3, GP4, GP5, GP6, GP7, GP8, \
                              LA1 ^ 0x2, LA2, LA3, LA4, LA5, LA6, LA7, LA8 }
#define NETIF0_SRC          { LA1, LA2 + 1, LA3, LA4, LA5, LA6, LA7, LA8 }
#define NETIF0_IPV6_LL      { LP1, LP2, LP3, LP4, LP5, LP6, LP7, LP8, \
                              LA1 ^ 0x2, LA2 + 1, LA3, LA4, LA5, LA6, LA7, LA8 }
#define NETIF0_IPV6_ULA     { ULA1, ULA2, ULA3, ULA4, ULA5, ULA6, ULA7, ULA8, \
                              LA1 ^ 0x2, LA2 + 1, LA3, LA4, LA5, LA6, LA7, LA8 }
#define NETIF0_IPV6_G       { GP1, GP2, GP3, GP4, GP5, GP6, GP7, GP8, \
                              LA1 ^ 0x2, LA2 + 1, LA3, LA4, LA5, LA6, LA7, LA8 }
#define GLOBAL_PFX18        { GP1, GP2, GP3 ^ 0x3f, GP4, GP5, GP6, GP7, GP8, \
                              LA1 ^ 0x2, LA2, LA3, LA4, LA5, LA6, LA7, LA8 }
#define GLOBAL_PFX23        { GP1, GP2, GP3 ^ 0x1, GP4, GP5, GP6, GP7, GP8, \
                              LA1 ^ 0x2, LA2, LA3, LA4, LA5, LA6, LA7, LA8 }
#define GLOBAL_PFX64        { GP1, GP2, GP3, GP4, GP5, GP6, GP7, GP8, \
                              LA1 ^ 0x82, LA2, LA3, LA4, LA5, LA6, LA7, LA8 }

extern netdev_t *ethernet_dev;
extern netdev_t *ieee802154_dev;
extern netdev_t *devs[DEFAULT_DEVS_NUMOF];

void _tests_init(void);
void _test_trigger_recv(gnrc_netif_t *netif, const uint8_t *data,
                        size_t data_len);

#ifdef __cplusplus
}
#endif

/** @} */
