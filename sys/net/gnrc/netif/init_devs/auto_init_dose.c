/*
 * Copyright (C) 2019 Juergen Fitschen <me@jue.yt>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup sys_auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for Differentially Operated Serial Ethernet module
 *
 * @author  Juergen Fitschen <me@jue.yt>
 */

#include "log.h"
#include "debug.h"
#include "dose.h"
#include "dose_params.h"
#include "net/gnrc/netif/ethernet.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define DOSE_MAC_STACKSIZE (THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE)
#ifndef DOSE_MAC_PRIO
#define DOSE_MAC_PRIO      (GNRC_NETIF_PRIO)
#endif

#define DOSE_NUM           ARRAY_SIZE(dose_params)

static char _netdev_eth_stack[DOSE_NUM][DOSE_MAC_STACKSIZE];
static dose_t dose[DOSE_NUM];
static gnrc_netif_t _netif[DOSE_NUM];

void auto_init_dose(void)
{
    /* setup netdev devices */
    for (unsigned i = 0; i < DOSE_NUM; i++) {
        LOG_DEBUG("[auto_init_netif] initializing dose #%d.\n", i);

        dose_setup(&dose[i], &dose_params[i], i);
        gnrc_netif_ethernet_create(&_netif[i], _netdev_eth_stack[i], DOSE_MAC_STACKSIZE,
                                   DOSE_MAC_PRIO, "dose", &dose[i].netdev);
    }
}
/** @} */
