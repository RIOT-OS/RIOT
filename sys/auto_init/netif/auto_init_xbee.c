/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for XBee network interfaces
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifdef MODULE_XBEE

#include "board.h"
#include "net/gnrc/nomac.h"
#include "net/gnrc.h"

#include "xbee.h"
#include "xbee_params.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define XBEE_NUM (sizeof(xbee_params)/sizeof(xbee_params_t))

static xbee_t xbee_devs[XBEE_NUM];

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define XBEE_MAC_STACKSIZE           (THREAD_STACKSIZE_DEFAULT)
#define XBEE_MAC_PRIO                (THREAD_PRIORITY_MAIN - 4)

/**
 * @brief   Stacks for the MAC layer threads
 */
static char _nomac_stacks[XBEE_MAC_STACKSIZE][XBEE_NUM];

void auto_init_xbee(void)
{
    for (size_t i = 0; i < XBEE_NUM; i++) {
        const xbee_params_t *p = &xbee_params[i];
        DEBUG("Initializing XBee radio at UART_%i\n", p->uart);
        int res = xbee_init(&xbee_devs[i],
                p->uart,
                p->baudrate,
                p->sleep_pin,
                p->status_pin);

        if (res < 0) {
            DEBUG("Error initializing XBee radio device!");
        }
        else {
            gnrc_nomac_init(_nomac_stacks[i],
                            XBEE_MAC_STACKSIZE, XBEE_MAC_PRIO, "xbee",
                            (gnrc_netdev_t *)&xbee_devs[i]);
        }
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_XBEE */
/** @} */
