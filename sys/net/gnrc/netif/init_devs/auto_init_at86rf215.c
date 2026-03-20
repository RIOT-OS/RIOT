/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
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
 * @brief   Auto initialization for at86rf215 network interfaces
 *
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#define USED_BANDS (IS_USED(MODULE_AT86RF215_SUBGHZ) + IS_USED(MODULE_AT86RF215_24GHZ))

#include "board.h"
#include "ztimer.h"
#include "event/thread.h"
#include "include/init_devs.h"

#include "at86rf215.h"
#include "at86rf215_params.h"

#include "net/ieee802154/radio.h"
#include "net/gnrc/netif/ieee802154.h"
#include "net/netdev/ieee802154_submac.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define AT86RF215_MAC_STACKSIZE     (IEEE802154_STACKSIZE_DEFAULT)
#ifndef AT86RF215_MAC_PRIO
#define AT86RF215_MAC_PRIO          (GNRC_NETIF_PRIO)
#endif
#ifndef AT86RF215_MAC_PRIO_SUBGHZ
#define AT86RF215_MAC_PRIO_SUBGHZ   (AT86RF215_MAC_PRIO)
#endif
/** @} */

#define AT86RF215_NUM ARRAY_SIZE(at86rf215_params)

static at86rf215_t at86rf215_devs[AT86RF215_NUM * USED_BANDS];
static netdev_ieee802154_submac_t at86rf215_netdev[AT86RF215_NUM * USED_BANDS];
static gnrc_netif_t _netif[AT86RF215_NUM * USED_BANDS];
static char _at86rf215_stacks[AT86RF215_NUM * USED_BANDS][AT86RF215_MAC_STACKSIZE];
static at86rf215_bhp_ev_t at86rf215_bhp[AT86RF215_NUM];

static inline void _setup_netif(gnrc_netif_t *netif, netdev_ieee802154_submac_t *netdev,
                                void *stack, int prio, const char *name)
{
    if (netif == NULL || netdev == NULL) {
        return;
    }

    gnrc_netif_ieee802154_create(netif, stack, AT86RF215_MAC_STACKSIZE, prio, name,
                                 &netdev->dev.netdev);
}

void auto_init_at86rf215(void)
{
    DEBUG("at86rf215: auto init started\n");

    unsigned i = 0;

    for (unsigned j = 0; j < AT86RF215_NUM; ++j) {
        unsigned idx_09 = 0;
        unsigned idx_24 = 0;

        void *stack_09 = NULL;
        void *stack_24 = NULL;

        at86rf215_t *dev_09 = NULL;
        at86rf215_t *dev_24 = NULL;

        gnrc_netif_t *netif_09 = NULL;
        gnrc_netif_t *netif_24 = NULL;

        ieee802154_dev_t *hal_09 = NULL;
        ieee802154_dev_t *hal_24 = NULL;

        netdev_ieee802154_submac_t *netdev_09 = NULL;
        netdev_ieee802154_submac_t *netdev_24 = NULL;

        if (IS_USED(MODULE_AT86RF215_SUBGHZ)) {
            idx_09    = i;
            dev_09    = &at86rf215_devs[i];
            netdev_09 = &at86rf215_netdev[i];
            hal_09    = &at86rf215_netdev[i].submac.dev;
            stack_09  = &_at86rf215_stacks[i];
            netif_09  = &_netif[i];
            ++i;
        }

        if (IS_USED(MODULE_AT86RF215_24GHZ)) {
            idx_24    = i;
            dev_24    = &at86rf215_devs[i];
            netdev_24 = &at86rf215_netdev[i];
            hal_24    = &at86rf215_netdev[i].submac.dev;
            stack_24  = &_at86rf215_stacks[i];
            netif_24  = &_netif[i];
            ++i;
        }

        at86rf215_init_event(&at86rf215_bhp[j], hal_09, hal_24, EVENT_PRIO_HIGHEST);
        at86rf215_init(dev_09, dev_24, hal_09, hal_24, &at86rf215_params[j], &at86rf215_bhp[j]);

        if (IS_USED(MODULE_AT86RF215_SUBGHZ)) {
            netdev_register(&netdev_09->dev.netdev, NETDEV_AT86RF215, idx_09);
            netdev_ieee802154_submac_init(netdev_09);
        }

        if (IS_USED(MODULE_AT86RF215_24GHZ)) {
            netdev_register(&netdev_24->dev.netdev, NETDEV_AT86RF215, idx_24);
            netdev_ieee802154_submac_init(netdev_24);
        }

        /* setup sub-GHz interface */
        _setup_netif(netif_09, netdev_09, stack_09, AT86RF215_MAC_PRIO_SUBGHZ,
                     "at86rf215 [sub GHz]");

        /* setup 2.4-GHz interface */
        _setup_netif(netif_24, netdev_24, stack_24, AT86RF215_MAC_PRIO,
                     "at86rf215 [2.4 GHz]");
    }

    DEBUG("at86rf215: auto init finished\n");
}
/** @} */
