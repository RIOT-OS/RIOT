/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
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

int relay_pid;

char t2_stack[KERNEL_CONF_STACKSIZE_MAIN];

#define CCNL_DEFAULT_MAX_CACHE_ENTRIES  1
#define CCNL_DEFAULT_THRESHOLD_PREFIX   1
#define CCNL_DEFAULT_THRESHOLD_AGGREGATE 2

void set_address_handler(uint16_t a)
{
    msg_t mesg;
    transceiver_command_t tcmd;

    tcmd.transceivers = TRANSCEIVER;
    tcmd.data = &a;
    mesg.content.ptr = (char *) &tcmd;

    printf("trying to set address %" PRIu16 "\n", a);
    mesg.type = SET_ADDRESS;

    printf("transceiver_pid=%d\n", transceiver_pid);

    msg_send_receive(&mesg, &mesg, transceiver_pid);
    printf("got address: %" PRIu16 "\n", a);
}

void populate_cache(void)
{
    msg_t m;
    m.content.value = 0;
    m.type = CCNL_RIOT_POPULATE;
    msg_send(&m, relay_pid, 1);
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

    relay_pid = thread_getpid();

    thread_create(t2_stack, KERNEL_CONF_STACKSIZE_MAIN, PRIORITY_MAIN + 1,
                  CREATE_STACKTEST, second_thread, NULL, "helper thread");

    printf("starting ccn-lite relay...\n");
    ccnl_riot_relay_start(CCNL_DEFAULT_MAX_CACHE_ENTRIES,
                          CCNL_DEFAULT_THRESHOLD_PREFIX,
                          CCNL_DEFAULT_THRESHOLD_AGGREGATE);

    return 0;
}
