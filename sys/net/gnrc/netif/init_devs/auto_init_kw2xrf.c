/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2016 PHYTEC Messtechnik GmbH
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
 * @brief   Auto initialization for kw2xrf network interfaces
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 * @author  Jonas Remmert <j.remmert@phytec.de>
 * @author  Sebastian Meiling <s@mlng.net>
 */

#include "log.h"
#include "board.h"
#include "net/gnrc/netif/ieee802154.h"
#include "net/gnrc.h"
#include "include/init_devs.h"
#include "net/netdev/ieee802154_submac.h"
#include "bhp/event.h"

#include "kw2xrf.h"
#include "kw2xrf_params.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define KW2XRF_MAC_STACKSIZE     (IEEE802154_STACKSIZE_DEFAULT)
#ifndef KW2XRF_MAC_PRIO
#define KW2XRF_MAC_PRIO          (GNRC_NETIF_PRIO)
#endif

#define KW2XRF_NUM ARRAY_SIZE(kw2xrf_params)

static kw2xrf_t kw2xrf_devs[KW2XRF_NUM];
static netdev_ieee802154_submac_t kw2xrf_netdev[KW2XRF_NUM];
static char _kw2xrf_stacks[KW2XRF_NUM][KW2XRF_MAC_STACKSIZE];
static gnrc_netif_t _netif[KW2XRF_NUM];
static bhp_event_t kw2xrf_bhp[KW2XRF_NUM];

void auto_init_kw2xrf(void)
{
    for (unsigned i = 0; i < KW2XRF_NUM; i++) {
        const kw2xrf_params_t *p = &kw2xrf_params[i];

        LOG_DEBUG("[auto_init_netif] initializing kw2xrf #%u\n", i);

        /* Init Bottom Half Processor (with events module) and radio */
        bhp_event_init(&kw2xrf_bhp[i], &_netif[i].evq[GNRC_NETIF_EVQ_INDEX_PRIO_HIGH],
                       &kw2xrf_radio_hal_irq_handler, &kw2xrf_netdev[i].submac.dev);
        kw2xrf_init(&kw2xrf_devs[i], p, &kw2xrf_netdev[i].submac.dev,
                        bhp_event_isr_cb, &kw2xrf_bhp[i]);

        netdev_register(&kw2xrf_netdev[i].dev.netdev, NETDEV_KW2XRF, i);
        netdev_ieee802154_submac_init(&kw2xrf_netdev[i], NULL);

        gnrc_netif_ieee802154_create(&_netif[i], _kw2xrf_stacks[i], KW2XRF_MAC_STACKSIZE,
                                     KW2XRF_MAC_PRIO, "kw2xrf",
                                     &kw2xrf_netdev[i].dev.netdev);

    }
}
/** @} */
