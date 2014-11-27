/*
 * Copyright (c) 2010, Mariano Alvira <mar@devl.org> and other contributors to
 * the MC1322x project (http://mc1322x.devl.org)
 * Copyright (C) 2014 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file        main.c
 *
 * @brief       Sniffer application for MSB-A2 and Wireshark
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Mariano Alvira <mar@devl.org>
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
#include "hwtimer.h"
#include "transceiver.h"
#if MODULE_AT86RF231 || MODULE_CC2420 || MODULE_MC1322X
#include "ieee802154_frame.h"
#endif

#define RCV_BUFFER_SIZE     (64)
#define RADIO_STACK_SIZE    (KERNEL_CONF_STACKSIZE_MAIN)
#define PER_ROW             (16)

char radio_stack_buffer[RADIO_STACK_SIZE];
msg_t msg_q[RCV_BUFFER_SIZE];
transceiver_command_t tcmd;

#if MODULE_AT86RF231 || MODULE_CC2420 || MODULE_MC1322X
void print_packet(ieee802154_packet_t *p)
#else
void print_packet(radio_packet_t *p)
#endif
{
    volatile uint8_t i, j, k;

    if (p) {
        printf("len 0x%02x lqi 0x%02x rx_time 0x%08lx", p->length, p->lqi, hwtimer_now());

        for (j = 0, k = 0; j <= ((p->length) / PER_ROW); j++) {
            printf("\n\r");

            for (i = 0; i < PER_ROW; i++, k++) {
                if (k >= p->length) {
                    printf("\n\r");
                    return;
                }

#if MODULE_AT86RF231 || MODULE_CC2420 || MODULE_MC1322X
                printf("%02x ", p->frame.payload[j * PER_ROW + i]);
#else
                printf("%02x ", p->data[j * PER_ROW + i]);
#endif
            }
        }
    }

    printf("\n\r");
    return;
}

void *radio(void *unused)
{
    (void) unused;

    msg_t m;
#if MODULE_AT86RF231 || MODULE_CC2420 || MODULE_MC1322X
    ieee802154_packet_t  *p;
#else
    radio_packet_t *p;
#endif

    msg_init_queue(msg_q, RCV_BUFFER_SIZE);

    while (1) {
        msg_receive(&m);

        if (m.type == PKT_PENDING) {
#if MODULE_AT86RF231 || MODULE_CC2420 || MODULE_MC1322X
            p = (ieee802154_packet_t *) m.content.ptr;
#else
            p = (radio_packet_t *) m.content.ptr;
#endif
            printf("rftest-rx --- ");
            print_packet(p);
            p->processing--;
        }
        else if (m.type == ENOBUFFER) {
            puts("Transceiver buffer full");
        }
        else {
            puts("Unknown packet received");
        }
    }

    return NULL;
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
    transceiver_start();
    transceiver_register(transceivers, radio_pid);

    msg_t mesg;
    mesg.type = SET_CHANNEL;
    mesg.content.ptr = (char *) &tcmd;

    uint16_t c = 10;

    tcmd.transceivers = TRANSCEIVER_DEFAULT;
    tcmd.data = &c;
    printf("Set transceiver to channel %u\n", c);
    msg_send(&mesg, transceiver_pid);

    mesg.type = SET_MONITOR;
    mesg.content.ptr = (char *) &tcmd;

    uint16_t v = 1;

    tcmd.data = &v;
    printf("Set transceiver into monitor mode\n");
    msg_send(&mesg, transceiver_pid);
}

static int shell_readc(void)
{
    char c = 0;
    posix_read(uart0_handler_pid, &c, 1);
    return c;
}

static void shell_putchar(int c)
{
    putchar(c);
}

int main(void)
{
    shell_t shell;
    posix_open(uart0_handler_pid, 0);
    init_transceiver();

    puts("Welcome to RIOT!");

    shell_init(&shell, NULL, UART0_BUFSIZE, shell_readc, shell_putchar);
    shell_run(&shell);

    return 0;
}
