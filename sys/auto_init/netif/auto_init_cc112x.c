/*
 * Copyright (C) 2015 Kubaszek Mateusz <mathir.km.riot@gmail.com>
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
 * @brief   Auto initialization for cc112x network interfaces
 *
 * @author  Kubaszek Mateusz <mathir.km.riot@gmail.com>
 */

#ifdef MODULE_CC112X

#include "board.h"
#include "net/gnrc/netdev2.h"
#include "gnrc_netdev2_cc112x.h"
#include "net/gnrc.h"

#include "cc112x.h"
#include "cc112x_params.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define CC112X_MAC_STACKSIZE     (THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE)
#define CC112X_MAC_PRIO          (THREAD_PRIORITY_MAIN - 3)

#define CC112X_NUM (sizeof(cc112x_params)/sizeof(cc112x_params[0]))

static netdev2_cc112x_t cc112x_devs[CC112X_NUM];
static char _stacks[CC112X_MAC_STACKSIZE][CC112X_NUM];

static gnrc_netdev2_t _gnrc_netdev2_devs[CC112X_NUM];

void auto_init_cc112x(void)
{
    for (int i = 0; i < CC112X_NUM; i++) {
        const cc112x_params_t *p = &cc112x_params[i];
        DEBUG("Initializing CC112X radio at SPI_%i\n", p->spi);
        int res = netdev2_cc112x_setup(&cc112x_devs[i], p);
        if (res < 0) {
            DEBUG("Error initializing CC112X radio device!");
        }
        else {
            gnrc_netdev2_cc112x_init(&_gnrc_netdev2_devs[i], &cc112x_devs[i]);
            res = gnrc_netdev2_init(_stacks[i], CC112X_MAC_STACKSIZE,
                    CC112X_MAC_PRIO, "cc112x", &_gnrc_netdev2_devs[i]);
            if (res < 0) {
                DEBUG("Error starting gnrc_cc112x thread for CC112X!");
            }
        }
    }
}
#else
typedef int dont_be_pedantic;
#endif /* MODULE_CC112X */

/** @} */
