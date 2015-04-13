/*
 * Copyright (C) 2014 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

#include <stdio.h>

#include "thread.h"

#include "transceiver.h"
#include "l2_ping.h"
#include "vtimer.h"
#include "posix_io.h"
#include "board_uart0.h"
#include "shell.h"
#include "shell_commands.h"

#define SEC_IN_USEC (1000 * 1000)
#define PING_COUNT  (1000)
#define BACKOFF_TIME    (60 * SEC_IN_USEC)
#define GUARD_TIME      (5 * SEC_IN_USEC)

static char worker_stack[KERNEL_CONF_STACKSIZE_MAIN];

void *worker_thread(void *unused)
{
    (void) unused;

    msg_t mesg;
    transceiver_command_t tcmd;
    radio_address_t a;
    l2_probe_stat_entry_t *stats;
    uint16_t count;

    puts("Worker thread spawned.");

    while (1) {
        if (transceiver_pid < 0) {
            puts("Transceiver not initialized, sleep 1 second");
            vtimer_usleep(SEC_IN_USEC);
            continue;
        }

        tcmd.transceivers = TRANSCEIVER_DEFAULT;
        tcmd.data = &a;
        mesg.content.ptr = (char *) &tcmd;
        mesg.type = GET_ADDRESS;

        msg_send_receive(&mesg, &mesg, transceiver_pid);
        printf("[transceiver] got address: %" PRIu16 "\n", a);
        if (!a) {
            puts("Address is not initialized, sleep 1 second");
            vtimer_usleep(SEC_IN_USEC);
            continue;
        }

        break;
    }

    uint32_t wait_time = a * (PING_COUNT * L2_PING_DEFAULT_INTERVAL) + (a * GUARD_TIME) + BACKOFF_TIME;
    printf("I am node %" PRIu16 ", going to sleep for %" PRIu32 " seconds\n", a, (wait_time / SEC_IN_USEC));
    vtimer_usleep(wait_time);

    puts("I'm awake again, start probing...");
    l2_ping(0, PING_COUNT, L2_PING_DEFAULT_INTERVAL, NULL, 0, 1);

    puts("...done");

    while (1) {

        l2_probe_stats(&stats, &count);

        printf("Current link layer probe statistics:\n");
        for (uint16_t i = 0; i < count; i++) {
            printf("...received %" PRIu16 " probes from node %" PRIu16 ".\n", stats[i].count, stats[i].src);
        }
        vtimer_usleep(BACKOFF_TIME);
    }

    return NULL;
}

int main(void)
{
    puts("Link Layer Ping test application starting.");

    thread_create(worker_stack, KERNEL_CONF_STACKSIZE_MAIN, PRIORITY_MAIN-1, CREATE_STACKTEST, worker_thread, NULL, "worker");

    shell_t shell;
    (void) posix_open(uart0_handler_pid, 0);

    shell_init(&shell, NULL, UART0_BUFSIZE, uart0_readc, uart0_putc);

    shell_run(&shell);
    return 0;
}
