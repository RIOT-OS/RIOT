/*
 * Copyright (C) 2020 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     sys_auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief       Auto initialization for ATWINC15X0 WiFi devices
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifdef MODULE_ATWINC15X0

#include "log.h"
#include "atwinc15x0.h"
#include "atwinc15x0_params.h"
#include "net/gnrc/netif/ethernet.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define ATWINC15X0_MAC_STACKSIZE    (THREAD_STACKSIZE_DEFAULT)
#ifndef ATWINC15X0_MAC_PRIO
#define ATWINC15X0_MAC_PRIO         (GNRC_NETIF_PRIO)
#endif
/*** @} */

/**
 * @brief   Find out how many of these devices we need to care for
 */
#define ATWINC15X0_NUM    ARRAY_SIZE(atwinc15x0_params)

/**
 * @brief   Allocate memory for the device descriptors
 * @{
 */
static atwinc15x0_t dev[ATWINC15X0_NUM];
/** @} */

static gnrc_netif_t _netif[ATWINC15X0_NUM];

/**
 * @brief   Stacks for the MAC layer threads
 */
static char stack[ATWINC15X0_NUM][ATWINC15X0_MAC_STACKSIZE];

void auto_init_atwinc15x0(void)
{
    for (unsigned i = 0; i < ATWINC15X0_NUM; i++) {
        LOG_DEBUG("[auto_init_netif] initializing atwinc15x0 #%u\n", i);

        /* setup netdev device */
        atwinc15x0_setup(&dev[i], &atwinc15x0_params[i]);
        gnrc_netif_ethernet_create(&_netif[i], stack[i],
                                   ATWINC15X0_MAC_STACKSIZE,
                                   ATWINC15X0_MAC_PRIO, "atwinc15x0",
                                   &dev[i].netdev);
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_ATWINC15X0 */
/** @} */
