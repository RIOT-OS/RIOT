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
 * @brief   Auto initialization for cc110x network interfaces
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifdef MODULE_CC110X

#include "log.h"
#include "debug.h"
#include "board.h"
#include "gnrc_netif_cc110x.h"
#include "cc110x-netdev.h"
#include "net/gnrc.h"

#include "cc110x.h"
#include "cc110x_params.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define CC110X_MAC_STACKSIZE     (THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE)
#ifndef CC110X_MAC_PRIO
#define CC110X_MAC_PRIO          (GNRC_NETIF_PRIO)
#endif

#define CC110X_NUM (sizeof(cc110x_params)/sizeof(cc110x_params[0]))

static netdev_cc110x_t cc110x_devs[CC110X_NUM];
static char _stacks[CC110X_NUM][CC110X_MAC_STACKSIZE];

void auto_init_cc110x(void)
{
    for (unsigned i = 0; i < CC110X_NUM; i++) {
        const cc110x_params_t *p = &cc110x_params[i];

        LOG_DEBUG("[auto_init_netif] initializing cc110x #%u\n", i);

        int res = netdev_cc110x_setup(&cc110x_devs[i], p);
        if (res < 0) {
            LOG_ERROR("[auto_init_netif] error initializing cc110x #%u\n", i);
        }
        else {
            gnrc_netif_cc110x_create(_stacks[i], CC110X_MAC_STACKSIZE,
                                     CC110X_MAC_PRIO, "cc110x",
                                     (netdev_t *)&cc110x_devs[i]);
        }
    }
}
#else
typedef int dont_be_pedantic;
#endif /* MODULE_CC110X */

/** @} */
