/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Xbee device initialization
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "kernel.h"
#include "xbee.h"
#include "net/ng_nomac.h"
#include "net/ng_netbase.h"

/* make sure an UART to device is defined in the Makefile */
#ifndef XBEE_UART
#error "XBEE_UART not defined"
#endif

/**
 * @brief   This is the default baudrate the Xbee modules are programmed to
 *          when you buy them
 */
#define XBEE_BAUDRATE           (9600U)

/**
 * @brief   MAC layer stack configuration
 * @{
 */
#define STACKSIZE               (KERNEL_CONF_STACKSIZE_DEFAULT)
#define PRIO                    (0)
/** @} */

/**
 * @brief   The Xbee device descriptor
 */
static xbee_t dev;

/**
 * @brief   Stack for the nomac thread
 */
static char nomac_stack[STACKSIZE];


void auto_init_ng_netif(void)
{
    int res;
    kernel_pid_t iface;

    /* setup Xbee device */
    printf("Initializing the Xbee S1 device UART_%i... \n", XBEE_UART);
    res = xbee_init(&dev, XBEE_UART, XBEE_BAUDRATE, GPIO_NUMOF, GPIO_NUMOF);
    if (res < 0) {
        puts("Error initializing xbee device driver");
        return;
    }
    /* start MAC layer */
    iface = ng_nomac_init(nomac_stack, sizeof(nomac_stack), PRIO, "xbee",
                          (ng_netdev_t *)(&dev));
    if (iface <= KERNEL_PID_UNDEF) {
        puts("Error initializing MAC layer");
        return;
    }
}
