/*
 * SPDX-FileCopyrightText: 2019 Juergen Fitschen <me@jue.yt>
 * SPDX-License-Identifier: LGPL-2.1-only
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
#include "include/init_devs.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define DOSE_MAC_STACKSIZE (GNRC_NETIF_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE)
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
