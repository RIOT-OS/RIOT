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
#include "net/gnrc.h"
#include "include/init_devs.h"

#include "at86rf2xx.h"
#include "at86rf2xx_params.h"

#include "net/netdev/ieee802154_submac.h"
#include "event/thread.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define AT86RF2XX_MAC_STACKSIZE     (IEEE802154_STACKSIZE_DEFAULT)
#ifndef AT86RF2XX_MAC_PRIO
#define AT86RF2XX_MAC_PRIO          (GNRC_NETIF_PRIO)
#endif

#define AT86RF2XX_NUM ARRAY_SIZE(at86rf2xx_params)

static at86rf2xx_bhp_ev_t at86rf2xx_bhp[AT86RF2XX_NUM];
static netdev_ieee802154_submac_t at86rf2xx_netdev[AT86RF2XX_NUM];
static gnrc_netif_t _netif[AT86RF2XX_NUM];
static char _at86rf2xx_stacks[AT86RF2XX_NUM][AT86RF2XX_MAC_STACKSIZE];

void auto_init_at86rf2xx(void)
{
    for (unsigned i = 0; i < AT86RF2XX_NUM; i++) {
        LOG_DEBUG("[auto_init_netif] initializing at86rf2xx #%u\n", i);

        at86rf2xx_init_event(&at86rf2xx_bhp[i], &at86rf2xx_params[i], &at86rf2xx_netdev[i].submac.dev, EVENT_PRIO_HIGHEST);
        netdev_register(&at86rf2xx_netdev[i].dev.netdev, NETDEV_AT86RF2XX, i);
        netdev_ieee802154_submac_init(&at86rf2xx_netdev[i]);
#if defined(MODULE_GNRC_GOMACH)
        gnrc_netif_gomach_create(&_netif[i], _at86rf2xx_stacks[i],
                                 AT86RF2XX_MAC_STACKSIZE,
                                 AT86RF2XX_MAC_PRIO, "at86rf2xx-gomach",
                                 &at86rf2xx_netdev[i].dev.netdev);
#elif defined(MODULE_GNRC_LWMAC)
        gnrc_netif_lwmac_create(&_netif[i], _at86rf2xx_stacks[i],
                                AT86RF2XX_MAC_STACKSIZE,
                                AT86RF2XX_MAC_PRIO, "at86rf2xx-lwmac",
                                &at86rf2xx_netdev[i].dev.netdev);
#else
        gnrc_netif_ieee802154_create(&_netif[i], _at86rf2xx_stacks[i],
                                     AT86RF2XX_MAC_STACKSIZE,
                                     AT86RF2XX_MAC_PRIO, "at86rf2xx",
                                     &at86rf2xx_netdev[i].dev.netdev);
#endif
    }
}
/** @} */
