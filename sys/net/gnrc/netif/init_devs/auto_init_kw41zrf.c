/*
 * Copyright (C) 2017 SKF AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup sys_auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for kw41zrf network interfaces
 *
 * @author  Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author  Thomas Stilwell <stilwellt@openlabs.co>
 */

#include "board.h"
#include "net/gnrc.h"
#include "net/gnrc/netif/ieee802154.h"
#include "include/init_devs.h"
#include "debug.h"
#include "kw41zrf.h"
#include "net/netdev.h"
#include "net/netdev/ieee802154_submac.h"

/**
 * @name    Stack parameters for the MAC layer thread
 * @{
 */
#ifndef KW41ZRF_NETIF_STACKSIZE
#define KW41ZRF_NETIF_STACKSIZE     (IEEE802154_STACKSIZE_DEFAULT)
#endif
#ifndef KW41ZRF_NETIF_PRIO
#define KW41ZRF_NETIF_PRIO          (GNRC_NETIF_PRIO)
#endif
/** @} */

/* There is only one memory mapped transceiver in the supported SoCs, the driver
 * does not try to take into account multiple instances of the hardware module */
#define KW41ZRF_NUMOF 1

static char _kw41zrf_stacks[KW41ZRF_NUMOF][KW41ZRF_NETIF_STACKSIZE];
static gnrc_netif_t _netif[KW41ZRF_NUMOF];
static netdev_ieee802154_submac_t kw41zrf_netdev[KW41ZRF_NUMOF];

void auto_init_kw41zrf(void)
{
    for (unsigned i = 0; i < KW41ZRF_NUMOF; i++) {
        DEBUG("[auto_init_netif] initializing kw41zrf #%u\n", i);

        netdev_register(&kw41zrf_netdev[i].dev.netdev, NETDEV_KW41ZRF, i);
        kw41zrf_init();
        netdev_ieee802154_submac_init(&kw41zrf_netdev[i]);
        kw41zrf_hal_setup(&kw41zrf_netdev[i].submac.dev);

        gnrc_netif_ieee802154_create(&_netif[i], _kw41zrf_stacks[i], KW41ZRF_NETIF_STACKSIZE,
                                     KW41ZRF_NETIF_PRIO, "kw41zrf",
                                     &kw41zrf_netdev[i].dev.netdev);
    }
}
/** @} */
