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
#include <stdio.h>

#include "ipv6.h"
#include "kernel.h"
#include "netapi.h"
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
static netdev_hlist_t ulhs = {NULL, NULL, NULL, 0};

static void answer_icmp_echo(ipv6_hdr_t *ipv6_pkt, size_t size)
{
    icmpv6_hdr_t *data;

    if ((ipv6_pkt->version_trafficclass >> 4) != 6 ||
        ipv6_pkt->nextheader != IPV6_PROTO_NUM_ICMPV6 ||
        sizeof(ipv6_hdr_t) + ipv6_pkt->length > size) {
        printf("ipv6 wrong\n");
        return;
    }

    ulhs.header = ipv6_pkt;
    ulhs.header_len = sizeof(ipv6_hdr_t);

    data = (icmpv6_hdr_t *)(ipv6_pkt + 1);

    if (data->type != ICMPV6_TYPE_ECHO_REQUEST) {
        printf("icmpv6 wrong\n");
        return;
    }

    printf("swap addresses\n");

    for (uint8_t i = 0; i < sizeof(ipv6_addr_t) / 4; i++) {
        uint32_t tmp;
        tmp = ipv6_pkt->srcaddr.uint32[i];
        ipv6_pkt->srcaddr.uint32[i] = ipv6_pkt->destaddr.uint32[i];
        ipv6_pkt->destaddr.uint32[i] = tmp;
    }

    slip_send_l3_packet(slip_pid, &ulhs, data, ipv6_pkt->length);
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
        printf("received\n");

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

        answer_icmp_echo(rcv->data, rcv->data_len);
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

    netapi_register(slip_pid, reader_pid);

    return 0;
}
