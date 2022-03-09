/*
 * Copyright (C) 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup sys_auto_init
 * @{
 * @file
 * @brief       initializes mcp2515 can device
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @}
 */

#ifdef MODULE_MCP2515
#include "can/device.h"
#include "mcp2515_params.h"

#define CANDEV_MCP2515_NUMOF ((ARRAY_SIZE(candev_mcp2515_params) / ARRAY_SIZE(candev_params_t)))

#ifndef CANDEV_MCP2515_STACKSIZE
#define CANDEV_MCP2515_STACKSIZE (THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF)
#endif

#ifndef CANDEV_MCP2515_BASE_PRIORITY
#define CANDEV_MCP2515_BASE_PRIORITY (THREAD_PRIORITY_MAIN - CANDEV_MCP2515_NUMOF - 2)
#endif

static candev_dev_t candev_dev_mcp2515[CANDEV_MCP2515_NUMOF];
static char _can_mcp2515_stacks[CANDEV_MCP2515_NUMOF][CANDEV_MCP2515_STACKSIZE];
static candev_mcp2515_t candev_mcp2515[CANDEV_MCP2515_NUMOF];

void auto_init_can_mcp2515(void) {

    for (size_t i = 0; i < CANDEV_MCP2515_NUMOF; i++) {
        candev_mcp2515_init(&candev_mcp2515[i], &candev_mcp2515_conf[i]);
        candev_dev_mcp2515[i].dev = (candev_t *)&candev_mcp2515[i];
        candev_dev_mcp2515[i].name = candev_mcp2515_params[i].name;
#ifdef MODULE_CAN_TRX
        candev_dev_mcp2515[i].trx = candev_mcp2515_params[i].trx;
#endif
#ifdef MODULE_CAN_PM
        candev_dev_mcp2515[i].rx_inactivity_timeout = candev_mcp2515_params[i].rx_inactivity_timeout;
        candev_dev_mcp2515[i].tx_wakeup_timeout = candev_mcp2515_params[i].tx_wakeup_timeout;
#endif

        can_device_init(_can_mcp2515_stacks[i],
                            CANDEV_MCP2515_STACKSIZE,
                            CANDEV_MCP2515_BASE_PRIORITY + i,
                            candev_mcp2515_params[i].name,
                            &candev_dev_mcp2515[i]);
    }
}
#else
typedef int dont_be_pedantic;
#endif
