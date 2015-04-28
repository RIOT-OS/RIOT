/*
 * Copyright (C) 2013 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief Nativenet test application
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>

#include "hwtimer.h"
#include "board.h"
#include "transceiver.h"
#include "nativenet.h"
#include "msg.h"
#include "thread.h"

#define SENDER_ADDR         (1)
#define DEFAULT_RCV_ADDR    (2)

#define PACKET_SIZE         (42)
#define WAIT_TIME           (60)
#define SECOND              (1000 * 1000)
#define SENDING_DELAY       (10 * 1000)

#define RCV_BUFFER_SIZE     (64)
#define RADIO_STACK_SIZE    (THREAD_STACKSIZE_DEFAULT)

static char radio_stack_buffer[RADIO_STACK_SIZE];
static msg_t msg_q[RCV_BUFFER_SIZE];
static uint8_t snd_buffer[NATIVE_MAX_DATA_LENGTH];
static uint8_t receiving = 1;
static unsigned int last_seq = 0, missed_cnt = 0;
static int first = -1;

void *radio(void *arg)
{
    (void) arg;

    msg_t m;
    radio_packet_t *p;
    unsigned int tmp = 0, cur_seq = 0;

    msg_init_queue(msg_q, RCV_BUFFER_SIZE);

    puts("Start receiving");

    while (receiving) {
        msg_receive(&m);

        if (m.type == PKT_PENDING) {
            p = (radio_packet_t *) m.content.ptr;

            if ((p->src == SENDER_ADDR) && (p->length == PACKET_SIZE)) {
                puts("received");
                cur_seq = (p->data[0] << 8) + p->data[1];

                if (first < 0) {
                    first = cur_seq;
                }
                else {
                    tmp = cur_seq - last_seq;

                    if (last_seq && (tmp > 1)) {
                        missed_cnt += (tmp - 1);
                    }
                }

                last_seq = cur_seq;
            }
            else {
                printf("sender was %i\n", p->src);
            }

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

void sender(void)
{
    unsigned int i = 0;
    msg_t mesg;
    transceiver_command_t tcmd;
    radio_packet_t p;

    mesg.type = SND_PKT;
    mesg.content.ptr = (char *) &tcmd;

    tcmd.transceivers = TRANSCEIVER_NATIVE;
    tcmd.data = &p;

    p.length = PACKET_SIZE;
    p.dst = 0;

    puts("Start sending packets");

    while (1) {
        /* filling uint8_t buffer with uint16_t sequence number */
        snd_buffer[0] = (i & 0xFF00) >> 8;
        snd_buffer[1] = i & 0x00FF;
        p.data = snd_buffer;
        i++;
        msg_send(&mesg, transceiver_pid);
        hwtimer_wait(HWTIMER_TICKS(SENDING_DELAY));
    }
}

int main(void)
{
    int16_t a;
    msg_t mesg;
    transceiver_command_t tcmd;

    printf("\n\tmain(): initializing transceiver\n");
    transceiver_init(TRANSCEIVER_NATIVE);

    printf("\n\tmain(): starting transceiver\n");
    transceiver_start();

#ifndef SENDER
    printf("\n\tmain(): starting radio thread\n");
    kernel_pid_t radio_pid = thread_create(
            radio_stack_buffer, sizeof(radio_stack_buffer),
            THREAD_PRIORITY_MAIN - 2, CREATE_STACKTEST,
            radio, NULL, "radio");
    transceiver_register(TRANSCEIVER_NATIVE, radio_pid);
#endif

#ifdef SENDER
    a = SENDER_ADDR;
#elif defined ADDR
    a = ADDR;
#else
    a = DEFAULT_RCV_ADDR;
#endif
    tcmd.transceivers = TRANSCEIVER_NATIVE;
    tcmd.data = &a;
    mesg.content.ptr = (char *) &tcmd;
    mesg.type = SET_ADDRESS;

    printf("[nativenet] trying to set address %" PRIi16 "\n", a);
    msg_send_receive(&mesg, &mesg, transceiver_pid);

#ifdef SENDER
    hwtimer_wait(HWTIMER_TICKS(SECOND));
    sender();
#else
    hwtimer_wait(HWTIMER_TICKS(WAIT_TIME * SECOND));
    receiving = 0;
    printf("Missed %u of %u packets after %u seconds\n", missed_cnt, (last_seq - first),  WAIT_TIME);
#endif

    return 0;
}
