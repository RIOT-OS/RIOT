/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/*
 * @ingroup auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for at86rf2xx network interfaces
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifdef MODULE_AT86RF2XX

#include "log.h"
#include "board.h"
#include "net/gnrc/netdev.h"
#include "net/gnrc/netdev/ieee802154.h"
#include "net/gnrc.h"

#include "at86rf2xx.h"
#include "at86rf2xx_params.h"
#include "netif_params.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define AT86RF2XX_MAC_STACKSIZE     (THREAD_STACKSIZE_DEFAULT)
#ifndef AT86RF2XX_MAC_PRIO
#define AT86RF2XX_MAC_PRIO          (GNRC_NETDEV_MAC_PRIO)
#endif

#define AT86RF2XX_NUM (sizeof(at86rf2xx_params) / sizeof(at86rf2xx_params[0]))

static at86rf2xx_t at86rf2xx_devs[AT86RF2XX_NUM];
#ifdef MODULE_GNRC
static gnrc_netdev_t gnrc_adpt[AT86RF2XX_NUM];
static char _at86rf2xx_stacks[AT86RF2XX_NUM][AT86RF2XX_MAC_STACKSIZE];
#endif

void auto_init_at86rf2xx(void)
{
    for (unsigned i = 0; i < AT86RF2XX_NUM; i++) {
        const netif_params_t *_netif_params = netif_params_get_by_dev(
                                                        netif_params,
                                                        &at86rf2xx_params[i]);

        LOG_DEBUG("[auto_init_netif] initializing at86rf2xx #%u\n", i);
        at86rf2xx_setup(&at86rf2xx_devs[i], &at86rf2xx_params[i]);
#ifdef MODULE_GNRC
        /* XXX super hacky way to provide the stack without requiring additional
         * fields in gnrc_netdev_t just for initialization */
        gnrc_adpt[i].dev = (netdev_t *)_at86rf2xx_stacks[i];
        gnrc_adpt[i].pid = AT86RF2XX_MAC_STACKSIZE;
#endif
        netif_setup(_netif_params, GNRC_NETDEV_TYPE_IEEE802154,
                    (netdev_t *)&at86rf2xx_devs[i], &gnrc_adpt[i]);
    }
}
#else
typedef int dont_be_pedantic;
#endif /* MODULE_AT86RF2XX */

/** @} */
