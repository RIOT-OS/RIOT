/*
 * Copyright (C) 2022
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_auto_init
 * @{
 * @file
 * @brief       Initializes CSP CAN interfaces
 *
 * @}
 */

#include "thread.h"

#include "csp_drivers_can.h"
#include "csp_drivers_can_params.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef CSP_CAN_NUMOFF
#define CSP_CAN_NUMOFF          (ARRAY_SIZE(csp_can_iface_params))
#endif

#ifndef CSP_CAN_STACKSIZE
#define CSP_CAN_STACKSIZE       (THREAD_STACKSIZE_LARGE)
#endif

#ifndef CSP_CAN_PRIORITY
#define CSP_CAN_PRIORITY        (THREAD_PRIORITY_MAIN - 2)
#endif

static char _csp_can_stacks[CSP_CAN_NUMOFF][CSP_CAN_STACKSIZE];
static csp_can_iface_t _csp_can_iface[CSP_CAN_NUMOFF];

void auto_init_csp_drivers_can(void)
{
    for (size_t i = 0; i < CSP_CAN_NUMOFF; i++) {
        DEBUG("auto_init_csp_drivers_can: init ifnum=(%d)\n", csp_can_iface_params[i].ifnum);
        csp_can_iface_init(&_csp_can_iface[i], &csp_can_iface_params[i]);
        csp_can_iface_create(&_csp_can_iface[i], _csp_can_stacks[i], sizeof(_csp_can_stacks[i]),
                              CSP_CAN_PRIORITY, "csp_can_rx");
    }
}
