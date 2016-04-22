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

#include "board.h"
#include "net/gnrc/netdev2.h"
#include "net/gnrc/netdev2/ieee802154.h"
#include "net/gnrc.h"

#include "at86rf2xx.h"
#include "at86rf2xx_params.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define AT86RF2XX_MAC_STACKSIZE     (THREAD_STACKSIZE_DEFAULT)
#define AT86RF2XX_MAC_PRIO          (THREAD_PRIORITY_MAIN - 4)

#define AT86RF2XX_NUM (sizeof(at86rf2xx_params) / sizeof(at86rf2xx_params[0]))

static at86rf2xx_t at86rf2xx_devs[AT86RF2XX_NUM];
static gnrc_netdev2_t gnrc_adpt[AT86RF2XX_NUM];
static char _at86rf2xx_stacks[AT86RF2XX_MAC_STACKSIZE][AT86RF2XX_NUM];

void auto_init_at86rf2xx(void)
{
    for (unsigned i = 0; i < AT86RF2XX_NUM; i++) {
        const at86rf2xx_params_t *p = &at86rf2xx_params[i];
        int res;

        DEBUG("Initializing AT86RF2xx radio at SPI_%i\n", p->spi);
        at86rf2xx_setup(&at86rf2xx_devs[i], (at86rf2xx_params_t*) p);
        res = gnrc_netdev2_ieee802154_init(&gnrc_adpt[i],
                                           (netdev2_ieee802154_t *)&at86rf2xx_devs[i]);

        if (res < 0) {
            DEBUG("Error initializing AT86RF2xx radio device!\n");
        }
        else {
            gnrc_netdev2_init(_at86rf2xx_stacks[i],
                              AT86RF2XX_MAC_STACKSIZE,
                              AT86RF2XX_MAC_PRIO,
                              "at86rf2xx",
                              &gnrc_adpt[i]);
        }
    }
}
#else
typedef int dont_be_pedantic;
#endif /* MODULE_AT86RF2XX */

/** @} */
