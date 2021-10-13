/*
 * Copyright (C)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     net
 * @file
 * @brief       Implementation of OpenThread functions wrapper. They are used to call OT functions from OT thread
 *
 * @author      Jose Ignacio Alamos <jialamos@inria.cl>
 * @author      Baptiste CLENET <bapclenet@gmail.com>
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "thread.h"
#include "openthread/ip6.h"
#include "openthread/thread.h"
#include "openthread/udp.h"
#include "ot.h"

#define ENABLE_DEBUG 0
#include "debug.h"

typedef uint8_t OT_COMMAND;

OT_COMMAND ot_channel(otInstance* ot_instance, void* arg, void* answer);
OT_COMMAND ot_eui64(otInstance* ot_instance, void* arg, void* answer);
OT_COMMAND ot_extaddr(otInstance* ot_instance, void* arg, void* answer);
OT_COMMAND ot_ipaddr(otInstance* ot_instance, void* arg, void* answer);
OT_COMMAND ot_masterkey(otInstance* ot_instance, void* arg, void* answer);
OT_COMMAND ot_networkname(otInstance* ot_instance, void* arg, void* answer);
OT_COMMAND ot_mode(otInstance* ot_instance, void* arg, void* answer);
OT_COMMAND ot_panid(otInstance* ot_instance, void* arg, void* answer);
OT_COMMAND ot_parent(otInstance* ot_instance, void* arg, void* answer);
OT_COMMAND ot_state(otInstance* ot_instance, void* arg, void* answer);
OT_COMMAND ot_thread(otInstance* ot_instance, void* arg, void* answer);

/**
 * @brief   Struct containing an OpenThread job command
 */
typedef struct {
    const char *name;                                   /**< A pointer to the job name string. */
    OT_COMMAND (*function)(otInstance*, void*, void*);  /**< function to be called */
} ot_command_t;

const ot_command_t otCommands[] =
{
    /* channel: arg NULL: get channel in answer | arg not NULL: set channel */
    { "channel", &ot_channel },
    /* eui64 : arg NULL: get eui64 in answer | arg not NULL: set eui64 */
    { "eui64", &ot_eui64 },
    /* extaddr: arg NULL: get extaddr in answer | arg not NULL: set extaddr */
    { "extaddr", &ot_extaddr },
    /* ipaddr: arg NULL: get nb ipaddr in answer | arg not NULL: get ipaddr[arg] */
    { "ipaddr", &ot_ipaddr },
    /* masterkey: arg NULL: get masterkey in answer | arg not NULL: set masterkey */
    { "masterkey", &ot_masterkey },
    /* mode: arg NULL: get mode in answer | arg not NULL: set mode */
    { "mode", ot_mode },
    /* networkname: arg NULL: get networkname in answer | arg not NULL: set networkname */
    { "networkname", &ot_networkname },
    /* panid: arg NULL: get panid in answer | arg not NULL: set panid */
    { "panid", &ot_panid },
    /* parent: arg NULL: get parent in answer */
    { "parent", &ot_parent },
    /* state: arg NULL: get state in answer */
    { "state", &ot_state },
    /* thread: arg "start"/"stop": start/stop thread operation */
    { "thread", &ot_thread },
};

void _exec_cmd(event_t *event) {
    ot_job_t *job = (ot_job_t*) event;

    uint8_t res = 0xFF;
    /* Check running thread */
    for (uint8_t i = 0; i < ARRAY_SIZE(otCommands); i++) {
        if (strcmp(job->command, otCommands[i].name) == 0) {
            res = (*otCommands[i].function)(openthread_get_instance(), job->arg, job->answer);
            break;
        }
    }
    if (res == 0xFF) {
        DEBUG("Wrong ot_COMMAND name\n");
        res = 1;
    }
    job->status = res;
}

uint8_t ot_call_command(char* command, void *arg, void* answer)
{
    ot_job_t job = {.ev.handler = _exec_cmd};

    job.command = command;
    job.arg = arg;
    job.answer = answer;

    event_post(openthread_get_evq(), &job.ev);
    return job.status;
}

void output_bytes(const char* name, const uint8_t *aBytes, uint8_t aLength)
{
    if (IS_ACTIVE(ENABLE_DEBUG)) {
        DEBUG("%s: ", name);
        for (int i = 0; i < aLength; i++) {
            DEBUG("%02x", aBytes[i]);
        }
        DEBUG("\n");
    }
}

OT_COMMAND ot_channel(otInstance* ot_instance, void* arg, void* answer) {
    if (answer != NULL) {
        *((uint8_t *) answer) = otLinkGetChannel(ot_instance);
        DEBUG("Channel: %04x\n", *((uint8_t *) answer));
    } else if (arg != NULL) {
        uint8_t channel = *((uint8_t *) arg);
        otLinkSetChannel(ot_instance, channel);
    } else {
        DEBUG("ERROR: wrong argument\n");
    }
    return 0;
}

OT_COMMAND ot_eui64(otInstance* ot_instance, void* arg, void* answer) {
    (void)arg;

    if (answer != NULL) {
        otExtAddress address;
        otLinkGetFactoryAssignedIeeeEui64(ot_instance, &address);
        output_bytes("eui64", address.m8, OT_EXT_ADDRESS_SIZE);
        *((otExtAddress *) answer) = address;
    } else {
        DEBUG("ERROR: wrong argument\n");
    }
    return 0;
}

OT_COMMAND ot_extaddr(otInstance* ot_instance, void* arg, void* answer) {
    (void)arg;

    if (answer != NULL) {
        answer = (void*)otLinkGetExtendedAddress(ot_instance);
        output_bytes("extaddr", (const uint8_t *)answer, OT_EXT_ADDRESS_SIZE);
    } else {
        DEBUG("ERROR: wrong argument\n");
    }
    return 0;
}

OT_COMMAND ot_ipaddr(otInstance* ot_instance, void* arg, void* answer) {
    uint8_t cnt = 0;
    for (const otNetifAddress *addr = otIp6GetUnicastAddresses(ot_instance); addr; addr = addr->mNext) {
        if (arg != NULL && answer != NULL && cnt == *((uint8_t *) arg)) {
            *((otNetifAddress *) answer) = *addr;
            return 0;
        }
        cnt++;
    }
    if (answer != NULL) {
        *((uint8_t *) answer) = cnt;
    } else {
        DEBUG("ERROR: wrong argument\n");
    }
    return 0;
}

OT_COMMAND ot_masterkey(otInstance* ot_instance, void* arg, void* answer) {
    if (answer != NULL) {
        const otMasterKey* masterkey = otThreadGetMasterKey(ot_instance);
        *((otMasterKey *) answer) = *masterkey;
        output_bytes("masterkey", (const uint8_t *)answer, OT_MASTER_KEY_SIZE);
    } else if (arg != NULL) {
        otThreadSetMasterKey(ot_instance, (otMasterKey*)arg);
    } else {
        DEBUG("ERROR: wrong argument\n");
    }
    return 0;
}

OT_COMMAND ot_mode(otInstance* ot_instance, void* arg, void* answer) {
    (void)answer;

    if (arg != NULL) {
        otLinkModeConfig link_mode;
        memset(&link_mode, 0, sizeof(otLinkModeConfig));
        char mode[6];
        memcpy(mode, (char*)arg, 5);
        mode[5] = '\0';
        for (char *arg = &mode[0]; *arg != '\0'; arg++) {
            switch (*arg) {
                case 'r':
                    link_mode.mRxOnWhenIdle = 1;
                    break;
                case 's':
                    link_mode.mSecureDataRequests = 1;
                    break;
                case 'd':
                    link_mode.mDeviceType = 1;
                    break;
                case 'n':
                    link_mode.mNetworkData = 1;
                    break;
            }
        }
        otThreadSetLinkMode(ot_instance, link_mode);
        DEBUG("OT mode changed to %s\n", (char*)arg);
    } else {
        DEBUG("ERROR: wrong argument\n");
    }
    return 0;
}

OT_COMMAND ot_networkname(otInstance* ot_instance, void* arg, void* answer) {
    if (answer != NULL) {
        const char* networkName = otThreadGetNetworkName(ot_instance);
        strcpy((char*) answer, networkName);
        DEBUG("networkname: %.*s\n", OT_NETWORK_NAME_MAX_SIZE, networkName);
    } else if (arg != NULL) {
        otThreadSetNetworkName(ot_instance, (char*) arg);
    } else {
        DEBUG("ERROR: wrong argument\n");
    }
    return 0;
}

OT_COMMAND ot_panid(otInstance* ot_instance, void* arg, void* answer) {
    if (answer != NULL) {
        *((uint16_t *) answer) = otLinkGetPanId(ot_instance);
        DEBUG("PanID: %04x\n", *((uint16_t *) answer));
    } else if (arg != NULL) {
        /* Thread operation needs to be stopped before setting panid */
        otThreadSetEnabled(ot_instance, false);
        uint16_t panid = *((uint16_t *) arg);
        otLinkSetPanId(ot_instance, panid);
        otThreadSetEnabled(ot_instance, true);
    } else {
        DEBUG("ERROR: wrong argument\n");
    }
    return 0;
}

OT_COMMAND ot_parent(otInstance* ot_instance, void* arg, void* answer) {
    (void)arg;

    if (answer != NULL) {
        otRouterInfo parentInfo;
        otThreadGetParentInfo(ot_instance, &parentInfo);
        output_bytes("parent", (const uint8_t *)parentInfo.mExtAddress.m8, sizeof(parentInfo.mExtAddress));
        DEBUG("Rloc: %x\n", parentInfo.mRloc16);
        *((otRouterInfo *) answer) = parentInfo;
    } else {
        DEBUG("ERROR: wrong argument\n");
    }
    return 0;
}

OT_COMMAND ot_state(otInstance* ot_instance, void* arg, void* answer) {
    (void)arg;

    if (answer != NULL) {
        otDeviceRole state = otThreadGetDeviceRole(ot_instance);
        *((otDeviceRole *) answer) = state;
        DEBUG("state: ");
        switch (state) {
            case OT_DEVICE_ROLE_DISABLED:
                puts("disabled");
                break;
            case OT_DEVICE_ROLE_DETACHED:
                puts("detached");
                break;
            case OT_DEVICE_ROLE_CHILD:
                puts("child");
                break;
            case OT_DEVICE_ROLE_ROUTER:
                puts("router");
                break;
            case OT_DEVICE_ROLE_LEADER:
                puts("leader");
                break;
            default:
                puts("invalid state");
                break;
        }
    } else {
        DEBUG("ERROR: wrong argument\n");
    }
    return 0;
}

OT_COMMAND ot_thread(otInstance* ot_instance, void* arg, void* answer) {
    (void)answer;

    if (arg != NULL) {
        if (strcmp((char*)arg, "start") == 0) {
            otThreadSetEnabled(ot_instance, true);
            DEBUG("Thread start\n");
        } else if (strcmp((char*)arg, "stop") == 0) {
            otThreadSetEnabled(ot_instance, false);
            DEBUG("Thread stop\n");
        } else {
            DEBUG("ERROR: thread available args: start/stop\n");
        }
    } else {
        DEBUG("ERROR: wrong argument\n");
    }
    return 0;
}
