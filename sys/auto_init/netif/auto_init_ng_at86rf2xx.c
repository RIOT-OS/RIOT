/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/*
 * @ingroup auto_init_ng_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for nx_at86rf2xx network interfaces
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifdef MODULE_NG_AT86RF2XX

#include "board.h"
#include "net/ng_nomac.h"
#include "net/ng_netbase.h"

#include "ng_at86rf2xx.h"
#include "ng_at86rf2xx_params.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define AT86RF2XX_MAC_STACKSIZE     (THREAD_STACKSIZE_DEFAULT)
#define AT86RF2XX_MAC_PRIO          (THREAD_PRIORITY_MAIN - 3)

#define AT86RF2XX_NUM (sizeof(at86rf2xx_params)/sizeof(at86rf2xx_params[0]))

static ng_at86rf2xx_t ng_at86rf2xx_devs[AT86RF2XX_NUM];
static char _nomac_stacks[AT86RF2XX_MAC_STACKSIZE][AT86RF2XX_NUM];

void auto_init_ng_at86rf2xx(void)
{
    for (int i = 0; i < AT86RF2XX_NUM; i++) {
        DEBUG("Initializing AT86RF2xx radio at SPI_%i\n", i);
        const at86rf2xx_params_t *p = &at86rf2xx_params[i];
        int res = ng_at86rf2xx_init(&ng_at86rf2xx_devs[i],
                p->spi,
                p->spi_speed,
                p->cs_pin,
                p->int_pin,
                p->sleep_pin,
                p->reset_pin);

        if (res < 0) {
            DEBUG("Error initializing AT86RF2xx radio device!");
        }
        else {
            ng_nomac_init(_nomac_stacks[i],
                    AT86RF2XX_MAC_STACKSIZE, AT86RF2XX_MAC_PRIO,
                    "at86rfxx", (ng_netdev_t *)&ng_at86rf2xx_devs[i]);
        }
    }
}
#else
typedef int dont_be_pedantic;
#endif /* MODULE_NG_AT86RF2XX */

/** @} */
