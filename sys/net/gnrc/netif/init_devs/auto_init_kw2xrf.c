/*
 * SPDX-FileCopyrightText: 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2016 PHYTEC Messtechnik GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
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

#include "kernel_defines.h"
#include "log.h"
#include "board.h"
#include "net/gnrc.h"
#include "net/gnrc/netif/ieee802154.h"
#include "net/gnrc/netif/ieee802154_mac.h"
#include "include/init_devs.h"
#include "net/netdev/ieee802154_submac.h"
#include "bhp/event.h"

#include "kw2xrf.h"
#include "kw2xrf_params.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define KW2XRF_MAC_STACKSIZE     (IEEE802154_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF)
#ifndef KW2XRF_MAC_PRIO
#define KW2XRF_MAC_PRIO          (GNRC_NETIF_PRIO)
#endif

#define KW2XRF_NUM ARRAY_SIZE(kw2xrf_params)
/** @} */

static kw2xrf_t kw2xrf_devs[KW2XRF_NUM];
static char _kw2xrf_stacks[KW2XRF_NUM][KW2XRF_MAC_STACKSIZE];
static gnrc_netif_t _netif[KW2XRF_NUM];
static bhp_event_t kw2xrf_bhp[KW2XRF_NUM];

#if IS_USED(MODULE_GNRC_NETIF_IEEE802154_MAC)
static gnrc_netif_ieee802154_mac_dev_t kw2xrf_netdev[KW2XRF_NUM];

static int _radio_init_cb(ieee802154_dev_t *radio, ieee802154_dev_type_t dev_type,
                          unsigned idx, void *arg)
{
    (void)dev_type;
    (void)arg;
    if (idx >= KW2XRF_NUM) {
        return -EINVAL;
    }

    const kw2xrf_params_t *p = &kw2xrf_params[idx];
    bhp_event_init(&kw2xrf_bhp[idx], &_netif[idx].evq[GNRC_NETIF_EVQ_INDEX_PRIO_HIGH],
                   &kw2xrf_radio_hal_irq_handler, radio);
    return kw2xrf_init(&kw2xrf_devs[idx], p, radio, bhp_event_isr_cb,
                       &kw2xrf_bhp[idx]);
}
#else
static netdev_ieee802154_submac_t kw2xrf_netdev[KW2XRF_NUM];
#endif

void auto_init_kw2xrf(void)
{
#if IS_USED(MODULE_GNRC_NETIF_IEEE802154_MAC)
    gnrc_netif_ieee802154_mac_set_dev_type(IEEE802154_DEV_TYPE_KW2XRF);
    gnrc_netif_ieee802154_mac_set_radio_init_cb(_radio_init_cb, NULL);
#endif

    for (unsigned i = 0; i < KW2XRF_NUM; i++) {
        LOG_DEBUG("[auto_init_netif] initializing kw2xrf #%u\n", i);

#if IS_USED(MODULE_GNRC_NETIF_IEEE802154_MAC)
        gnrc_netif_ieee802154_mac_create(&_netif[i], _kw2xrf_stacks[i], KW2XRF_MAC_STACKSIZE,
                                         KW2XRF_MAC_PRIO, "kw2xrf",
                                         &kw2xrf_netdev[i]);
#else
        const kw2xrf_params_t *p = &kw2xrf_params[i];

        /* Init Bottom Half Processor (with events module) and radio */
        bhp_event_init(&kw2xrf_bhp[i], &_netif[i].evq[GNRC_NETIF_EVQ_INDEX_PRIO_HIGH],
                       &kw2xrf_radio_hal_irq_handler, &kw2xrf_netdev[i].submac.dev);
        kw2xrf_init(&kw2xrf_devs[i], p, &kw2xrf_netdev[i].submac.dev,
                    bhp_event_isr_cb, &kw2xrf_bhp[i]);

        netdev_register(&kw2xrf_netdev[i].dev.netdev, NETDEV_KW2XRF, i);
        netdev_ieee802154_submac_init(&kw2xrf_netdev[i]);

        gnrc_netif_ieee802154_create(&_netif[i], _kw2xrf_stacks[i], KW2XRF_MAC_STACKSIZE,
                                     KW2XRF_MAC_PRIO, "kw2xrf",
                                     &kw2xrf_netdev[i].dev.netdev);
#endif
    }
}
/** @} */
