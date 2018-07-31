/*
 * Copyright (C) 2018 HERE Deutchland GmbH
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
 * @author      Anton Gerasimov <anton.gerasimov@here.com>
 * @}
 */

#ifdef MODULE_PERIPH_MSCAN
#include "can/device.h"
#include "mscan.h"

#ifndef CANDEV_KEA_STACKSIZE
#define CANDEV_KEA_STACKSIZE THREAD_STACKSIZE_DEFAULT
#endif

#ifndef CANDEV_KEA_PRIORITY
#define CANDEV_KEA_PRIORITY (THREAD_PRIORITY_MAIN - 4)
#endif

static candev_dev_t candev_dev_kea;
static char _can_kea_stack[CANDEV_KEA_STACKSIZE];
static candev_kea_t candev_kea;

extern const candev_kea_conf_t candev_kea_config[];

void auto_init_can_mscan(void)
{

    candev_kea_init(&candev_kea, &candev_kea_config[0]);
    candev_dev_kea.dev = (candev_t *)&candev_kea;
    candev_dev_kea.name = candev_kea_config[0].params.name;
#ifdef MODULE_CAN_TRX
    candev_dev_kea.trx = candev_kea_config[0].params.trx;
#endif
#ifdef MODULE_CAN_PM
    candev_dev_kea.rx_inactivity_timeout = candev_kea_config[0].params.rx_inactivity_timeout;
    candev_dev_kea.tx_wakeup_timeout = candev_kea_config[0].params.tx_wakeup_timeout;
#endif

    can_device_init(_can_kea_stack, CANDEV_KEA_STACKSIZE, CANDEV_KEA_PRIORITY,
                    candev_kea_config[0].params.name, &candev_dev_kea);
}
#else
typedef int dont_be_pedantic;
#endif
