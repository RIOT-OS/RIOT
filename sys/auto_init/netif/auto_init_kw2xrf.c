/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
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
 * @brief   Auto initialization for kw2xrf network interfaces
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 * @author  Jonas Remmert <j.remmert@phytec.de>
 */

#ifdef MODULE_KW2XRF

#include "board.h"
#include "net/ng_nomac.h"
#include "net/ng_netbase.h"

#include "kw2xrf.h"
#include "kw2xrf_params.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define KW2XRF_MAC_STACKSIZE     (THREAD_STACKSIZE_DEFAULT)
#define KW2XRF_MAC_PRIO          (THREAD_PRIORITY_MAIN - 4)

#define KW2XRF_NUM (sizeof(kw2xrf_params)/sizeof(kw2xrf_params[0]))

static kw2xrf_t kw2xrf_devs[KW2XRF_NUM];
static char _nomac_stacks[KW2XRF_MAC_STACKSIZE][KW2XRF_NUM];

void auto_init_kw2xrf(void)
{
    for (int i = 0; i < KW2XRF_NUM; i++) {
        DEBUG("Initializing KW2xrf radio at SPI_%i\n", i);
        const kw2xrf_params_t *p = &kw2xrf_params[i];

        int res = kw2xrf_init(&kw2xrf_devs[i],
                p->spi,
                p->spi_speed,
                p->cs_pin,
                p->int_pin);

        if (res < 0) {
            DEBUG("Error initializing KW2xrf radio device!");
        }
        else {
            ng_nomac_init(_nomac_stacks[i],
                    KW2XRF_MAC_STACKSIZE, KW2XRF_MAC_PRIO,
                    "kw2xrf", (ng_netdev_t *)&kw2xrf_devs[i]);
        }
    }
}
#else
typedef int dont_be_pedantic;
#endif /* MODULE_NG_KW2XRF */

/** @} */
