/*
 * Copyright (C) 2008, 2009, 2010  Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2013 INRIA
 * Copyright (C) 2013 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Default application that shows a lot of functionality of RIOT
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "thread.h"
#include "shell.h"
#include "shell_commands.h"
#include "msg.h"

#if FEATURE_PERIPH_RTC
#include "periph/rtc.h"
#endif

#ifdef MODULE_LTC4150
#include "ltc4150.h"
#endif

#ifdef MODULE_NETIF
#include "net/gnrc/pktdump.h"
#include "net/gnrc.h"
#endif

/* set up a timer to send packages */
#include "xtimer.h"
#include "timex.h"
#include "board.h"

/* set interval to 1 second */
//#define INTERVAL (500U * MS_IN_USEC)
#define INTERVAL (2U * SEC_IN_USEC)


void *second_thread(void *arg)
{
    (void) arg;

    uint32_t last_wakeup = xtimer_now();
    while(1) {
        xtimer_periodic_wakeup(&last_wakeup, INTERVAL);
        // printf("slept until %" PRIu32 "\n", xtimer_now());
        // Toggle red led
        //LED_PORT |= BLUE|GREEN|RED;
        LED_PORT ^= BLUE;

        //txtsnd <if> [<L2 addr>|bcast] <data>
        // handle_input_line(NULL, "txtsnd 4 bcast HelloWolrd\n \n");
    }
    return NULL;
}

char second_thread_stack[THREAD_STACKSIZE_MAIN];

int main(void)
{
#ifdef MODULE_LTC4150
    ltc4150_start();
#endif

#ifdef FEATURE_PERIPH_RTC
    rtc_init();
#endif

#ifdef MODULE_NETIF
    (void) puts("GNRC NETIF");
    gnrc_netreg_entry_t dump = GNRC_NETREG_ENTRY_INIT_PID(GNRC_NETREG_DEMUX_CTX_ALL,
                                                          gnrc_pktdump_pid);
    gnrc_netreg_register(GNRC_NETTYPE_UNDEF, &dump);
#endif

    (void) puts("Welcome to RIOT!");

    //kernel_pid_t pid =
    		thread_create(second_thread_stack, sizeof(second_thread_stack),
                            THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
                            second_thread, NULL, "SEND_DATA");


    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
