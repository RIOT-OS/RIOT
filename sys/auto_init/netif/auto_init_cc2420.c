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
 * @brief   Auto initialization for nx_cc2420 network interfaces
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifdef MODULE_CC2420

#include "board.h"
#include "net/gnrc/nomac.h"
#include "net/gnrc.h"

#include "cc2420.h"
#include "cc2420_params.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define cc2420_MAC_STACKSIZE     (THREAD_STACKSIZE_DEFAULT)
#define cc2420_MAC_PRIO          (THREAD_PRIORITY_MAIN - 4)

#define cc2420_NUM (sizeof(cc2420_params)/sizeof(cc2420_params[0]))

static cc2420_t cc2420_devs[cc2420_NUM];
static char _nomac_stacks[cc2420_MAC_STACKSIZE][cc2420_NUM];

void auto_init_cc2420(void)
{
    for (int i = 0; i < cc2420_NUM; i++) {
        DEBUG("Initializing AT86RF2xx radio at SPI_%i\n", i);
        const cc2420_params_t *p = &cc2420_params[i];
        int res = cc2420_init(&cc2420_devs[i], p);

        if (res < 0) {
            DEBUG("Error initializing AT86RF2xx radio device!");
        }
        else {
            gnrc_nomac_init(_nomac_stacks[i],
                            cc2420_MAC_STACKSIZE, cc2420_MAC_PRIO,
                            "at86rfxx", (gnrc_netdev_t *)&cc2420_devs[i]);
        }
    }
}
#else
typedef int dont_be_pedantic;
#endif /* MODULE_CC2420 */

/** @} */
