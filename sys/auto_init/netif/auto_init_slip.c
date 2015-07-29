/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup auto_init_ng_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for XBee network interfaces
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifdef MODULE_NG_SLIP

#include "board.h"
#include "net/ng_nomac.h"
#include "net/ng_netbase.h"

#include "slip.h"
#include "slip_params.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define SLIP_NUM (sizeof(ng_slip_params)/sizeof(ng_slip_params_t))

static ng_slip_dev_t slip_devs[SLIP_NUM];

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define SLIP_STACKSIZE          (KERNEL_CONF_STACKSIZE_DEFAULT)
#define SLIP_PRIO               (PRIORITY_MAIN - 4)

/**
 * @brief   Stacks for the MAC layer threads
 */
static char _slip_stacks[SLIP_STACKSIZE][SLIP_NUM];

void auto_init_slip(void)
{
    for (int i = 0; i < SLIP_NUM; i++) {
        const ng_slip_params_t *p = &ng_slip_params[i];
        DEBUG("Initializing SLIP radio at UART_%d\n", p->uart);
        kernel_pid_t res = ng_slip_init(&slip_devs[i], p->uart, p->baudrate,
                                        _slip_stacks[i], SLIP_STACKSIZE,
                                        SLIP_PRIO);

        if (res <= KERNEL_PID_UNDEF) {
            DEBUG("Error initializing XBee radio device!");
        }
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_NG_SLIP */
/** @} */
