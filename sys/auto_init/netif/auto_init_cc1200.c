/*
 * Copyright (C) 2017 TU Berlin DAI Labor
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
 * @brief   Auto initialization for cc1200 network interfaces
 *
 * @author  Anon Malll <anon.mall@mail.gt-arc.com>
 */

#ifdef MODULE_CC1200

#define ENABLE_DEBUG    (0)
#include "log.h"
#include "debug.h"
#include "board.h"
#include "net/gnrc/netdev.h"
#include "gnrc_netdev_cc1200.h"
#include "net/gnrc/netdev/ieee802154.h"
#include "net/gnrc.h"

#include "cc1200.h"
#include "cc1200_params.h"


/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define CC1200_MAC_STACKSIZE     (THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE)
#ifndef CC1200_MAC_PRIO
#define CC1200_MAC_PRIO          (GNRC_NETDEV_MAC_PRIO)
#endif

#define CC1200_NUM (sizeof(cc1200_params)/sizeof(cc1200_params[0]))

static netdev_cc1200_t cc1200_devs[CC1200_NUM];
static char _stacks[CC1200_NUM][CC1200_MAC_STACKSIZE];

static gnrc_netdev_t _gnrc_netdev_devs[CC1200_NUM];

void auto_init_cc1200(void)
{
    for (unsigned i = 0; i < CC1200_NUM; i++) {
        const cc1200_params_t *p = &cc1200_params[i];

        LOG_DEBUG("[auto_init_netif] initializing cc1200 #%u\n", i);
        DEBUG("[auto_init_netif] initializing cc1200 #%u\n", i);

        int res = netdev_cc1200_setup(&cc1200_devs[i], p);
        res = gnrc_netdev_ieee802154_init(&_gnrc_netdev_devs[i],
                                      (netdev_ieee802154_t *)&(cc1200_devs[i]));
        DEBUG("[auto_init_netif] finished: res=%i\n", res);
        if (res < 0) {
            LOG_ERROR("[auto_init_netif] error initializing cc1200 #%u\n", i);
        }
        else {
            res = gnrc_netdev_init(_stacks[i], CC1200_MAC_STACKSIZE,
                    CC1200_MAC_PRIO, "cc1200", &_gnrc_netdev_devs[i]);
            if (res < 0) {
                LOG_ERROR("[auto_init_netif] error starting gnrc_cc1200 thread\n");
            }
        }
    }
}
#else
typedef int dont_be_pedantic;
#endif /* MODULE_CC1200 */

/** @} */
