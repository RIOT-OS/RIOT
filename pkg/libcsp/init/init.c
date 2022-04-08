/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_auto_init
 * @{
 * @file
 * @brief
 *
 * @author
 * @}
 */

#include <stdio.h>

#include "kernel_defines.h"

#include "csp/csp_iflist.h"
#include "csp/csp_rtable.h"
#include "csp_riot.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void auto_init_csp(void)
{
    DEBUG("auto_init_csp: init csp\n");
    libcsp_init();

    if (IS_USED(MODULE_LIBCSP_DRIVERS_CAN)) {
        DEBUG("auto_init_csp: init csp can drivers\n");
        extern void auto_init_csp_drivers_can(void);
        auto_init_csp_drivers_can();
    }

    if (CONFIG_CSP_ROUTE_TABLE != NULL) {
        DEBUG("auto_init_csp: load rtable %s\n", CONFIG_CSP_ROUTE_TABLE != NULL ? CONFIG_CSP_ROUTE_TABLE : "");
        csp_rtable_load(CONFIG_CSP_ROUTE_TABLE);
    }
    else {
        /* route everything though first interface */
        csp_iface_t * iface = csp_iflist_get();
        if (iface) {
            DEBUG("auto_init_csp: default route through %s\n", iface->name);
            csp_rtable_set(0, 0, iface, CSP_NO_VIA_ADDRESS);
        }
    }

    if (IS_USED(MODULE_LIBCSP_SOCK_UDP)) {
        DEBUG("auto_init_csp: init csp sock\n");
        extern void sock_udp_init(void);
        sock_udp_init();
    }
}
