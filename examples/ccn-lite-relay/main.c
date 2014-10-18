/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup examples
 * @{
 *
 * @file
 * @brief CCN Lite relay example application
 *
 * @author Christian Mehlis <mehlis@inf.fu-berlin.de>
 *
 * @}
 */

// system
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

// riot
#include "thread.h"
#include "rtc.h"

// ccn
#include "ccn_lite/ccnl-riot.h"

static kernel_pid_t _relay_pid = KERNEL_PID_UNDEF;

char t2_stack[KERNEL_CONF_STACKSIZE_MAIN];

void set_address_handler(uint16_t a)
{
    msg_t mesg;
    transceiver_command_t tcmd;

    tcmd.transceivers = TRANSCEIVER;
    tcmd.data = &a;
    mesg.content.ptr = (char *) &tcmd;

    printf("trying to set address %" PRIu16 "\n", a);
    mesg.type = SET_ADDRESS;

    printf("transceiver_pid=%" PRIkernel_pid"\n", transceiver_pid);

    msg_send_receive(&mesg, &mesg, transceiver_pid);
    printf("got address: %" PRIu16 "\n", a);
}

void populate_cache(void)
{
    msg_t m;
    m.content.value = 0;
    m.type = CCNL_RIOT_POPULATE;
    msg_send(&m, _relay_pid, 1);
}

void *second_thread(void *arg)
{
    (void) arg;
    set_address_handler(42);
    populate_cache();
    return NULL;
}

int main(void)
{
    printf("CCN!\n");

    _relay_pid = sched_active_pid;

    thread_create(t2_stack, sizeof(t2_stack), PRIORITY_MAIN + 1,
                  CREATE_STACKTEST, second_thread, NULL, "helper thread");

    printf("starting ccn-lite relay...\n");
    ccnl_riot_relay_start(NULL);

    return 0;
}
