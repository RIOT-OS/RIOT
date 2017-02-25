/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for XBee network interfaces
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifdef MODULE_GNRC_SLIP

#include "log.h"
#include "board.h"
#include "net/gnrc/netdev2.h"
#include "net/gnrc.h"

#include "net/gnrc/slip.h"
#include "slip_params.h"

#define SLIP_NUM (sizeof(gnrc_slip_params)/sizeof(gnrc_slip_params_t))

static gnrc_slip_dev_t slip_devs[SLIP_NUM];

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define SLIP_STACKSIZE          (THREAD_STACKSIZE_DEFAULT)
#ifndef SLIP_PRIO
#define SLIP_PRIO               (GNRC_NETDEV2_MAC_PRIO)
#endif

/**
 * @brief   Stacks for the MAC layer threads
 */
static char _slip_stacks[SLIP_NUM][SLIP_STACKSIZE];

void auto_init_slip(void)
{
    for (unsigned i = 0; i < SLIP_NUM; i++) {
        const gnrc_slip_params_t *p = &gnrc_slip_params[i];

        LOG_DEBUG("[auto_init_netif] initializing slip #%u\n", i);

        kernel_pid_t res = gnrc_slip_init(&slip_devs[i], p->uart, p->baudrate,
                                          _slip_stacks[i], SLIP_STACKSIZE,
                                          SLIP_PRIO);

        if (res <= KERNEL_PID_UNDEF) {
            LOG_ERROR("[auto_init_netif] error initializing slip #%u\n", i);
        }
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_GNRC_SLIP */
/** @} */
