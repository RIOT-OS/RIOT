/*
 * Copyright (C) 2008, 2009, 2010  Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2013 INRIA
 * Copyright (C) 2013 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
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
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "thread.h"
#include "posix_io.h"
#include "shell.h"
#include "shell_commands.h"
#include "board_uart0.h"

#ifdef MODULE_LTC4150
#include "ltc4150.h"
#endif

#ifdef MODULE_TRANSCEIVER
#include "transceiver.h"
#endif

#define SND_BUFFER_SIZE     (100)
#define RCV_BUFFER_SIZE     (64)
#define RADIO_STACK_SIZE    (KERNEL_CONF_STACKSIZE_DEFAULT)

#ifdef MODULE_TRANSCEIVER

char radio_stack_buffer[RADIO_STACK_SIZE];
blip_t msg_q[RCV_BUFFER_SIZE];

void *radio(void *arg)
{
    (void) arg;

    blip_t m;
    radio_packet_t *p;
    radio_packet_length_t i;

    msg_init_queue(msg_q, RCV_BUFFER_SIZE);

    while (1) {
        blip_receive(&m);

        if (m.type == PKT_PENDING) {
            p = (radio_packet_t *) m.content.ptr;
            printf("Got radio packet:\n");
            printf("\tLength:\t%u\n", p->length);
            printf("\tSrc:\t%u\n", p->src);
            printf("\tDst:\t%u\n", p->dst);
            printf("\tLQI:\t%u\n", p->lqi);
            printf("\tRSSI:\t%u\n", p->rssi);

            for (i = 0; i < p->length; i++) {
                printf("%02X ", p->data[i]);
            }

            p->processing--;
            puts("\n");
        }
        else if (m.type == ENOBUFFER) {
            puts("Transceiver buffer full");
        }
        else {
            puts("Unknown packet received");
        }
    }
}

void init_transceiver(void)
{
    kernel_pid_t radio_pid = thread_create(
                        radio_stack_buffer,
                        sizeof(radio_stack_buffer),
                        PRIORITY_MAIN - 2,
                        CREATE_STACKTEST,
                        radio,
                        NULL,
                        "radio");

    uint16_t transceivers = TRANSCEIVER_DEFAULT;

    transceiver_init(transceivers);
    (void) transceiver_start();
    transceiver_register(transceivers, radio_pid);
}
#endif /* MODULE_TRANSCEIVER */

static int shell_readc(void)
{
    char c = 0;
    (void) posix_read(uart0_handler_pid, &c, 1);
    return c;
}

static void shell_putchar(int c)
{
    (void) putchar(c);
}

int main(void)
{
    shell_t shell;
    (void) posix_open(uart0_handler_pid, 0);

#ifdef MODULE_LTC4150
    ltc4150_start();
#endif

#ifdef MODULE_TRANSCEIVER
    init_transceiver();
#endif

    (void) puts("Welcome to RIOT!");

    shell_init(&shell, NULL, UART0_BUFSIZE, shell_readc, shell_putchar);

    shell_run(&shell);
    return 0;
}
