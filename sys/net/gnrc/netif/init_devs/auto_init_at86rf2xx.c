/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief   Auto initialization for at86rf2xx network interfaces
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */

#include "log.h"
#include "board.h"
#include "net/gnrc/netif/ieee802154.h"
#ifdef MODULE_GNRC_LWMAC
#include "net/gnrc/lwmac/lwmac.h"
#endif
#ifdef MODULE_GNRC_GOMACH
#include "net/gnrc/gomach/gomach.h"
#endif
#include "net/gnrc.h"

#include "event.h"

#include "net/ieee802154/radio.h"
#include "net/netdev/ieee802154_submac.h"

#include "at86rf2xx.h"
#include "at86rf2xx_params.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define AT86RF2XX_MAC_STACKSIZE     (THREAD_STACKSIZE_DEFAULT)
#ifndef AT86RF2XX_MAC_PRIO
#define AT86RF2XX_MAC_PRIO          (GNRC_NETIF_PRIO)
#endif

#define AT86RF2XX_NUM ARRAY_SIZE(at86rf2xx_params)

static gnrc_netif_t _netif[AT86RF2XX_NUM];
static char _at86rf2xx_stacks[AT86RF2XX_NUM][AT86RF2XX_MAC_STACKSIZE];

#if IS_USED(MODULE_IEEE802154_RADIO_HAL)
typedef struct {
    event_t event;
    at86rf2xx_t dev;
} at86rf2xx_bh_t;

static at86rf2xx_bh_t at86rf2xx_devs[AT86RF2XX_NUM];
static netdev_ieee802154_submac_t at86rf2xx_submac[AT86RF2XX_NUM];

void at86rf2xx_irq_handler(void *dev);

static void _isr_at86rf2xx(void *arg)
{
    at86rf2xx_bh_t *bh = container_of(arg, at86rf2xx_bh_t, dev);
    int index = bh - at86rf2xx_devs;
    event_post(&_netif[index].evq, &bh->event);
}

/* This is our Bottom-Half processor */
static void _irq_event_handler(event_t *event)
{
    at86rf2xx_bh_t *bh = (at86rf2xx_bh_t*) event;
    at86rf2xx_irq_handler(&bh->dev);
}
#else
static at86rf2xx_t at86rf2xx_devs[AT86RF2XX_NUM];
#endif

void auto_init_at86rf2xx(void)
{
    for (unsigned i = 0; i < AT86RF2XX_NUM; i++) {
        LOG_DEBUG("[auto_init_netif] initializing at86rf2xx #%u\n", i);

        netdev_t *netdev;
#if IS_USED(MODULE_IEEE802154_RADIO_HAL)
        netdev_ieee802154_submac_init(&at86rf2xx_submac[i], &(at86rf2xx_devs[i].dev.dev));
        at86rf2xx_init(&at86rf2xx_devs[i].dev, &at86rf2xx_params[i], _isr_at86rf2xx);
        at86rf2xx_devs[i].event.handler = _irq_event_handler;
        netdev = (netdev_t*) &at86rf2xx_submac[i];
#else
        at86rf2xx_setup(&at86rf2xx_devs[i], &at86rf2xx_params[i], i);
        netdev = (netdev_t*) at86rf2xx_devs[i];
#endif

#if defined(MODULE_GNRC_GOMACH)
        gnrc_netif_gomach_create(&_netif[i], _at86rf2xx_stacks[i],
                                 AT86RF2XX_MAC_STACKSIZE,
                                 AT86RF2XX_MAC_PRIO, "at86rf2xx-gomach",
                                 netdev);
#elif defined(MODULE_GNRC_LWMAC)
        gnrc_netif_lwmac_create(&_netif[i], _at86rf2xx_stacks[i],
                                AT86RF2XX_MAC_STACKSIZE,
                                AT86RF2XX_MAC_PRIO, "at86rf2xx-lwmac",
                                netdev);
#else
        gnrc_netif_ieee802154_create(&_netif[i], _at86rf2xx_stacks[i],
                                     AT86RF2XX_MAC_STACKSIZE,
                                     AT86RF2XX_MAC_PRIO, "at86rf2xx",
                                     netdev);
#endif
    }
}
/** @} */
