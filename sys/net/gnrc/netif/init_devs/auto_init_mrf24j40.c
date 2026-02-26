/*
 * Copyright (C) 2017 Neo Nenaco <neo@nenaco.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/*
 * @ingroup sys_auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for mrf24j40 network interfaces
 *
 * @author  Neo Nenaco <neo@nenaco.de>
 */

#include "log.h"
#include "board.h"
#include "net/gnrc/netif/ieee802154.h"
#include "net/gnrc.h"
#include "include/init_devs.h"
#include "net/netdev/ieee802154_submac.h"
#include "bhp/event.h"

#include "mrf24j40.h"
#include "mrf24j40_params.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define MRF24J40_MAC_STACKSIZE     (IEEE802154_STACKSIZE_DEFAULT)
#ifndef MRF24J40_MAC_PRIO
#define MRF24J40_MAC_PRIO          (GNRC_NETIF_PRIO)
#endif

#define MRF24J40_NUM ARRAY_SIZE(mrf24j40_params)

static mrf24j40_t mrf24j40_devs[MRF24J40_NUM];
static netdev_ieee802154_submac_t mrf24j40_netdev[MRF24J40_NUM];
static char _mrf24j40_stacks[MRF24J40_NUM][MRF24J40_MAC_STACKSIZE];

static gnrc_netif_t _netif[MRF24J40_NUM];
static bhp_event_t mrf24j40_bhp[MRF24J40_NUM];

void auto_init_mrf24j40(void)
{
    for (unsigned i = 0; i < MRF24J40_NUM; i++) {
        const mrf24j40_params_t *p = &mrf24j40_params[i];
        LOG_DEBUG("[auto_init_netif] initializing mrf24j40 #%u\n", i);

        /* Init Bottom Half Processor (with events module) and radio */
        bhp_event_init(&mrf24j40_bhp[i], &_netif[i].evq[GNRC_NETIF_EVQ_INDEX_PRIO_HIGH], &mrf24j40_radio_irq_handler, &mrf24j40_netdev[i].submac.dev);
        mrf24j40_init(&mrf24j40_devs[i], (mrf24j40_params_t*) p,&mrf24j40_netdev[i].submac.dev,
                        bhp_event_isr_cb, &mrf24j40_bhp[i]);

        netdev_register(&mrf24j40_netdev[i].dev.netdev, NETDEV_MRF24J40, i);
        netdev_ieee802154_submac_init(&mrf24j40_netdev[i], NULL);

        gnrc_netif_ieee802154_create(&_netif[i], _mrf24j40_stacks[i],
                                     MRF24J40_MAC_STACKSIZE, MRF24J40_MAC_PRIO,
                                     "mrf24j40",
                                     &mrf24j40_netdev[i].dev.netdev);
    }
}
/** @} */
