/*
 * Copyright (C) 2016-2018 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_auto_init
 * @{
 * @file
 * @brief       initializes periph_can devices
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @}
 */

#include "periph/can.h"
#include "can/device.h"
#include "can_params.h"
#include "periph_conf.h"

#define CANDEV_NUMOF (ARRAY_SIZE(candev_params))

#ifndef CANDEV_STACKSIZE
#define CANDEV_STACKSIZE (THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF)
#endif

#ifndef CANDEV_BASE_PRIORITY
#define CANDEV_BASE_PRIORITY (THREAD_PRIORITY_MAIN - CANDEV_NUMOF - 2)
#endif

static candev_dev_t candev_dev[CANDEV_NUMOF];
static char _can_stacks[CANDEV_NUMOF][CANDEV_STACKSIZE];
static can_t candev[CANDEV_NUMOF];

void auto_init_periph_can(void) {

    for (size_t i = 0; i < CANDEV_NUMOF; i++) {
        can_init(&candev[i], &candev_conf[i]);
        candev_dev[i].dev = (candev_t *)&candev[i];
        candev_dev[i].name = candev_params[i].name;
#ifdef MODULE_CAN_TRX
        candev_dev[i].trx = candev_params[i].trx;
#endif
#ifdef MODULE_CAN_PM
        candev_dev[i].rx_inactivity_timeout = candev_params[i].rx_inactivity_timeout;
        candev_dev[i].tx_wakeup_timeout = candev_params[i].tx_wakeup_timeout;
#endif

        can_device_init(_can_stacks[i], CANDEV_STACKSIZE, CANDEV_BASE_PRIORITY + i,
                        candev_params[i].name, &candev_dev[i]);
    }
}
