/*
 * Copyright (C) 2017 Neo Nenaco <neo@nenaco.de>
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
 * @brief   Auto initialization for mrf24j40 network interfaces
 *
 * @author  Neo Nenaco <neo@nenaco.de>
 */

#ifdef MODULE_MRF24J40

#include "log.h"
#include "board.h"
#include "net/gnrc/netif/ieee802154.h"
#include "net/gnrc.h"

#include "mrf24j40.h"
#include "mrf24j40_params.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define MRF24J40_MAC_STACKSIZE     (THREAD_STACKSIZE_DEFAULT)
#ifndef MRF24J40_MAC_PRIO
#define MRF24J40_MAC_PRIO          (GNRC_NETIF_PRIO)
#endif

#define MRF24J40_NUM (sizeof(mrf24j40_params) / sizeof(mrf24j40_params[0]))

static mrf24j40_t mrf24j40_devs[MRF24J40_NUM];
static char _mrf24j40_stacks[MRF24J40_NUM][MRF24J40_MAC_STACKSIZE];

void auto_init_mrf24j40(void)
{
    for (unsigned i = 0; i < MRF24J40_NUM; i++) {
        LOG_DEBUG("[auto_init_netif] initializing mrf24j40 #%u\n", i);

        mrf24j40_setup(&mrf24j40_devs[i], &mrf24j40_params[i]);
        gnrc_netif_ieee802154_create(_mrf24j40_stacks[i],
                                     MRF24J40_MAC_STACKSIZE, MRF24J40_MAC_PRIO,
                                     "mrf24j40",
                                     (netdev_t *)&mrf24j40_devs[i]);
    }
}
#else
typedef int dont_be_pedantic;
#endif /* MODULE_MRF24J40 */

/** @} */
