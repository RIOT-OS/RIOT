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
 * @brief   Auto initialization for cc110x network interfaces
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifdef MODULE_CC110X

#include "board.h"
#include "net/gnrc/gnrc_netdev2.h"
#include "gnrc_netdev2_cc110x.h"
#include "net/gnrc.h"

#include "cc110x.h"
#include "cc110x_params.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define CC110X_MAC_STACKSIZE     (THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE)
#define CC110X_MAC_PRIO          (THREAD_PRIORITY_MAIN - 3)

#define CC110X_NUM (sizeof(cc110x_params)/sizeof(cc110x_params[0]))

static netdev2_cc110x_t cc110x_devs[CC110X_NUM];
static char _stacks[CC110X_MAC_STACKSIZE][CC110X_NUM];

static gnrc_netdev2_t _gnrc_netdev2_devs[CC110X_NUM];

void auto_init_cc110x(void)
{
    for (int i = 0; i < CC110X_NUM; i++) {
        const cc110x_params_t *p = &cc110x_params[i];
        DEBUG("Initializing CC110X radio at SPI_%i\n", p->spi);
        int res = netdev2_cc110x_setup(&cc110x_devs[i], p);
        if (res < 0) {
            DEBUG("Error initializing CC110X radio device!");
        }
        else {
            gnrc_netdev2_cc110x_init(&_gnrc_netdev2_devs[i], &cc110x_devs[i]);
            res = gnrc_netdev2_init(_stacks[i], CC110X_MAC_STACKSIZE,
                    CC110X_MAC_PRIO, "cc110x", &_gnrc_netdev2_devs[i]);
            if (res < 0) {
                DEBUG("Error starting gnrc_cc110x thread for CC110X!");
            }
        }
    }
}
#else
typedef int dont_be_pedantic;
#endif /* MODULE_CC110X */

/** @} */
