/*
 * Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Tests for general network device interface
 *
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * @}
 */

#include <errno.h>

#include "board.h"
#include "kernel.h"
#include "netapi.h"
#include "netdev/base.h"
#include "periph/uart.h"
#include "pktbuf.h"
#include "ringbuffer.h"
#include "slip.h"
#include "thread.h"

#define MSG_QUEUE_SIZE      (4)
#define INBUF_SIZE          (64)
#define READER_STACK_SIZE   (KERNEL_CONF_STACKSIZE_DEFAULT)

static char inbuf_mem[INBUF_SIZE], reader_stack[READER_STACK_SIZE];
static ringbuffer_t inbuf = RINGBUFFER_INIT(inbuf_mem);
static kernel_pid_t slip_pid;

static void blink_led(uint8_t *data, size_t data_len)
{
    for (size_t i = 0; i < data_len; i++) {
        if (data[i] > 127) {
            LED_RED_ON;
            LED_GREEN_ON;
        }
        else {
            LED_RED_OFF;
            LED_GREEN_OFF;
        }
    }
}

static inline void echo(void *data, size_t size)
{
    slip_send_l3_packet(slip_pid, NULL, data, size);
}

static void *reader(void *args)
{
    msg_t msg_rcv, msg_ack, msg_queue[MSG_QUEUE_SIZE];

    netapi_rcv_pkt_t *rcv;
    netapi_ack_t *ack;

    (void)args;

    msg_init_queue(msg_queue, MSG_QUEUE_SIZE);

    msg_ack.type = NETAPI_MSG_TYPE;

    while (1) {
        msg_receive(&msg_rcv);

        if (msg_rcv.type != NETAPI_MSG_TYPE) {
            msg_ack.type = 0;   /* Send random reply, API handles this case */
            msg_reply(&msg_rcv, &msg_ack);
            continue;
        }

        rcv = (netapi_rcv_pkt_t *)(msg_rcv.content.ptr);
        msg_ack.content.ptr = (char *)rcv->ack;
        ack = rcv->ack;

        if (rcv->type != NETAPI_CMD_RCV) {
            ack->result = -ENOMSG;
            msg_reply(&msg_rcv, &msg_ack);
            continue;
        }

        pktbuf_hold(rcv->data);
        msg_reply(&msg_rcv, &msg_ack);

        blink_led(rcv->data, rcv->data_len);
        echo(rcv->data, rcv->data_len);
        pktbuf_release(rcv->data);
    }

    return NULL;
}

int main(void)
{
    kernel_pid_t reader_pid = thread_create(reader_stack, READER_STACK_SIZE,
                                            PRIORITY_MAIN - 1, 0, reader, NULL,
                                            "slip_reader");

#if UART_NUMOF
    slip_pid = slip_init(UART_0, 115200, &inbuf);
#else
    slip_pid = slip_init(0, 115200, &inbuf);
#endif

    netapi_register(slip_pid, reader_pid, 0);

    return 0;
}
