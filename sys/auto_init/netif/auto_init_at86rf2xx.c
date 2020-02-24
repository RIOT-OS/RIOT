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

#ifdef MODULE_AT86RF2XX
#include "kernel_defines.h"
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

static char _at86rf2xx_stacks[AT86RF2XX_NUM][AT86RF2XX_MAC_STACKSIZE];

static void _auto_init_at86rf2xx(char *stack, at86rf2xx_t *dev) {
#if defined(MODULE_GNRC_GOMACH)
    gnrc_netif_gomach_create(stack,
                                AT86RF2XX_MAC_STACKSIZE,
                                AT86RF2XX_MAC_PRIO, "at86rf2xx-gomach",
                                (netdev_t *)(&(dev->base.netdev)));
#elif defined(MODULE_GNRC_LWMAC)
    gnrc_netif_lwmac_create(stack,
                            AT86RF2XX_MAC_STACKSIZE,
                            AT86RF2XX_MAC_PRIO, "at86rf2xx-lwmac",
                            (netdev_t *)(&(dev->base.netdev)));
#else
    gnrc_netif_ieee802154_create(stack,
                                    AT86RF2XX_MAC_STACKSIZE,
                                    AT86RF2XX_MAC_PRIO, "at86rf2xx",
                                    (netdev_t *)(&(dev->base.netdev)));
#endif
}

void auto_init_at86rf2xx(void)
{
    unsigned stack = 0;
#if IS_USED(MODULE_AT86RF212B)
    static at86rf212b_t at86rf212b_devs[AT86RF212B_NUM_OF];
    at86rf212b_setup(at86rf212b_devs, at86rf212b_params, AT86RF212B_NUM_OF);
    for (unsigned i = 0; i < AT86RF212B_NUM_OF; i++) {
        LOG_DEBUG("[auto_init_netif] initializing at86rf212b #%u\n", i);
        _auto_init_at86rf2xx(_at86rf2xx_stacks[stack++],
                             (at86rf2xx_t *)&at86rf212b_devs[i]);
    }
#endif
#if IS_USED(MODULE_AT86RF231)
    static at86rf231_t at86rf231_devs[AT86RF231_NUM_OF];
    at86rf231_setup(at86rf231_devs, at86rf231_params, AT86RF231_NUM_OF);
    for (unsigned i = 0; i < AT86RF231_NUM_OF; i++) {
        LOG_DEBUG("[auto_init_netif] initializing at86rf231 #%u\n", i);
        _auto_init_at86rf2xx(_at86rf2xx_stacks[stack++],
                             (at86rf2xx_t *)&at86rf231_devs[i]);
    }
#endif
#if IS_USED(MODULE_AT86RF232)
    static at86rf232_t at86rf232_devs[AT86RF232_NUM_OF];
    at86rf232_setup(at86rf232_devs, at86rf232_params, AT86RF232_NUM_OF);
    for (unsigned i = 0; i < AT86RF232_NUM_OF; i++) {
        LOG_DEBUG("[auto_init_netif] initializing at86rf232 #%u\n", i);
        _auto_init_at86rf2xx(_at86rf2xx_stacks[stack++],
                             (at86rf2xx_t *)&at86rf232_devs[i]);
    }
#endif
#if IS_USED(MODULE_AT86RF233)
    static at86rf233_t at86rf233_devs[AT86RF233_NUM_OF];
    at86rf233_setup(at86rf233_devs, at86rf233_params, AT86RF233_NUM_OF);
    for (unsigned i = 0; i < AT86RF233_NUM_OF; i++) {
        LOG_DEBUG("[auto_init_netif] initializing at86rf233 #%u\n", i);
        _auto_init_at86rf2xx(_at86rf2xx_stacks[stack++],
                             (at86rf2xx_t *)&at86rf233_devs[i]);
    }
#endif
#if IS_USED(MODULE_AT86RFA1)
    static at86rfa1_t at86rfa1_dev;
    at86rfa1_setup(&at86rfa1_dev);
    LOG_DEBUG("[auto_init_netif] initializing at86rfa1\n");
    _auto_init_at86rf2xx(_at86rf2xx_stacks[stack++],
                         (at86rf2xx_t *)&at86rfa1_dev);
#elif IS_USED(MODULE_AT86RFR2)
    static at86rfr2_t at86rfr2_dev;
    at86rfr2_setup(&at86rfr2_dev);
    LOG_DEBUG("[auto_init_netif] initializing at86rfr2\n");
    _auto_init_at86rf2xx(_at86rf2xx_stacks[stack++],
                         (at86rf2xx_t *)&at86rfr2_dev);
#endif
}
#else
typedef int dont_be_pedantic;
#endif /* MODULE_AT86RF2XX */

/** @} */
