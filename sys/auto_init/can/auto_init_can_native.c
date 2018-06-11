/*
 * Copyright (C) 2016 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_auto_init
 * @{
 * @file
 * @brief       initializes native can device
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @}
 */

#ifdef MODULE_CAN_LINUX
#include "can/device.h"
#include "candev_linux_params.h"

#define CANDEV_LINUX_NUMOF ((sizeof(candev_linux_params) / sizeof(candev_params_t)))

#ifndef CANDEV_LINUX_STACKSIZE
#define CANDEV_LINUX_STACKSIZE (THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF)
#endif

#ifndef CANDEV_LINUX_BASE_PRIORITY
#define CANDEV_LINUX_BASE_PRIORITY (THREAD_PRIORITY_MAIN - CANDEV_LINUX_NUMOF - 2)
#endif

static candev_dev_t candev_dev_linux[CANDEV_LINUX_NUMOF];
static char _can_linux_stacks[CANDEV_LINUX_NUMOF][CANDEV_LINUX_STACKSIZE];
static candev_linux_t candev_linux[CANDEV_LINUX_NUMOF];

void auto_init_can_native(void) {

    for (size_t i = 0; i < CANDEV_LINUX_NUMOF; i++) {
        candev_linux_init(&candev_linux[i], &candev_linux_conf[i]);
        candev_dev_linux[i].dev = (candev_t *)&candev_linux[i];
        candev_dev_linux[i].name = candev_linux_params[i].name;
#ifdef MODULE_CAN_TRX
        candev_dev_linux[i].trx = candev_linux_params[i].trx;
#endif
#ifdef MODULE_CAN_PM
        candev_dev_linux[i].rx_inactivity_timeout = candev_linux_params[i].rx_inactivity_timeout;
        candev_dev_linux[i].tx_wakeup_timeout = candev_linux_params[i].tx_wakeup_timeout;
#endif

        can_device_init(_can_linux_stacks[i], CANDEV_LINUX_STACKSIZE, CANDEV_LINUX_BASE_PRIORITY + i,
                        candev_linux_params[i].name, &candev_dev_linux[i]);
    }
}
#else
typedef int dont_be_pedantic;
#endif
