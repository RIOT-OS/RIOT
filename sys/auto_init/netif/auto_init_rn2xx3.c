/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief       Auto initialization for RN2483 and RN2903 devices (LoRaMAC only)
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifdef MODULE_RN2XX3

#include "log.h"
#include "board.h"
#include "net/gnrc/netif/raw.h"
#include "net/gnrc.h"

#include "rn2xx3.h"
#include "rn2xx3_params.h"

/**
 * @brief   Calculate the number of configured RN2XX3 devices
 */
#define RN2XX3_NUMOF        (sizeof(rn2xx3_params) / sizeof(rn2xx3_params[0]))

/**
 * @brief   Define stack parameters for the MAC layer thread
 */
#define RN2XX3_STACKSIZE           (THREAD_STACKSIZE_DEFAULT)
#ifndef RN2XX3_PRIO
#define RN2XX3_PRIO                (GNRC_NETIF_PRIO)
#endif

/**
 * @brief   Allocate memory for device descriptors, stacks, and GNRC adaption
 */
static rn2xx3_t rn2xx3_devs[RN2XX3_NUMOF];
static char rn2xx3_stacks[RN2XX3_NUMOF][RN2XX3_STACKSIZE];

void auto_init_rn2xx3(void)
{
    for (unsigned i = 0; i < RN2XX3_NUMOF; ++i) {
#if defined(MODULE_RN2483)
        LOG_DEBUG("[auto_init_netif] initializing rn2483 #%u\n", i);
#else /* MODULE_RN2903 */
        LOG_DEBUG("[auto_init_netif] initializing rn2903 #%u\n", i);
#endif

        rn2xx3_setup(&rn2xx3_devs[i], &rn2xx3_params[i]);
        gnrc_netif_raw_create(rn2xx3_stacks[i], RN2XX3_STACKSIZE, RN2XX3_PRIO,
                              "rn2xx3", (netdev_t *)&rn2xx3_devs[i]);
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_RN2XX3 */
/** @} */
