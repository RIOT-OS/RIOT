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

#include <openthread.h>
#include <stdio.h>
#include "byteorder.h"

int _openthread_state(int argc, char **argv)
{
    switch(otGetDeviceRole())
    {
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
    for (const otNetifAddress *addr = otGetUnicastAddresses(); addr; addr = addr->mNext)
        {
            printf("%x:%x:%x:%x:%x:%x:%x:%x\r\n",
                                  byteorder_ntohs(*((network_uint16_t*) &addr->mAddress.mFields.m16[0])),
                                  byteorder_ntohs(*((network_uint16_t*) &addr->mAddress.mFields.m16[1])),
                                  byteorder_ntohs(*((network_uint16_t*) &addr->mAddress.mFields.m16[2])),
                                  byteorder_ntohs(*((network_uint16_t*) &addr->mAddress.mFields.m16[3])),
                                  byteorder_ntohs(*((network_uint16_t*) &addr->mAddress.mFields.m16[4])),
                                  byteorder_ntohs(*((network_uint16_t*) &addr->mAddress.mFields.m16[5])),
                                  byteorder_ntohs(*((network_uint16_t*) &addr->mAddress.mFields.m16[6])),
                                  byteorder_ntohs(*((network_uint16_t*) &addr->mAddress.mFields.m16[7])));
        }
    return 0;
}
