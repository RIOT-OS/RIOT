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
 * @brief       initializes can device init function
 *
 * @author      Toon Stegen <toon.stegen@altran.com>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Aurelien Gonce <aurelien.gonce@altran.com>
 * @}
 */

#include <stdio.h>

#define ENABLE_DEBUG 0
#include "debug.h"

#include "can/dll.h"

#ifdef MODULE_CAN_ISOTP
#include "can/isotp.h"

#ifndef ISOTP_STACK_SIZE
#define ISOTP_STACK_SIZE (THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF)
#endif

#ifndef ISOTP_PRIORITY
#define ISOTP_PRIORITY (THREAD_PRIORITY_MAIN - 2)
#endif

static char isotp_stack[ISOTP_STACK_SIZE];
#endif

void auto_init_candev(void)
{
    DEBUG("auto_init_can: init dll\n");
    can_dll_init();

#ifdef MODULE_CAN_ISOTP
    DEBUG("auto_init_can: init isotp\n");
    isotp_init(isotp_stack, ISOTP_STACK_SIZE, ISOTP_PRIORITY, "isotp");
#endif

#ifdef MODULE_PERIPH_CAN
    extern void auto_init_periph_can(void);
    auto_init_periph_can();
#endif

#ifdef MODULE_MCP2515
    extern void auto_init_can_mcp2515(void);
    auto_init_can_mcp2515();
#endif
}
