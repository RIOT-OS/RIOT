/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Shell commands for interacting with OpenThread
 *
 * @author      José Ignacio Alamos <jialamos@uc.cl>
 */

#include <openthread/openthread.h>
#include <stdio.h>
#include "byteorder.h"
#include "net/ipv6/addr.h"

#if 0
int _openthread_state(int argc, char **argv)
{
    switch (otGetDeviceRole()) {
        case kDeviceRoleDisabled:
            printf("disabled\n");
            break;
        case kDeviceRoleDetached:
            printf("detached\n");
            break;
        case kDeviceRoleChild:
            printf("child\n");
            break;
        case kDeviceRoleRouter:
            printf("router\n");
            break;
        case kDeviceRoleLeader:
            printf("leader\n");
            break;
    }
    return 0;
}

int _openthread_ipaddr(int argc, char **argv)
{
    for (const otNetifAddress *addr = otGetUnicastAddresses(); addr; addr = addr->mNext) {
        ipv6_addr_print((ipv6_addr_t *) &addr->mAddress.mFields);
        printf("\n");
    }
    return 0;
}
#endif
