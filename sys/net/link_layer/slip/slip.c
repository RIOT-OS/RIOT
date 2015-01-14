/*
 * Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "kernel.h"
#include "netapi.h"
#include "netdev/base.h"
#include "periph/uart.h"
#include "pktbuf.h"
#include "ringbuffer.h"
#include "thread.h"

#include "slip.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define _SLIP_END         ((uint8_t)0xC0)
#define _SLIP_ESC         ((uint8_t)0xDB)
#define _SLIP_END_ESC     ((uint8_t)0xDC)
#define _SLIP_ESC_ESC     ((uint8_t)0xDD)

#define _SLIP_MSG_TYPE          (NETAPI_MSG_TYPE + 1)
#define _SLIP_STACK_SIZE        (KERNEL_CONF_STACKSIZE_DEFAULT)
#define _SLIP_MSG_QUEUE_SIZE    (16)
#define _SLIP_MAX_NAME_LEN      (24)

#ifndef _SLIP_REGISTRY_SIZE
#define _SLIP_REGISTRY_SIZE     (1)
#endif
#define _SLIP_BUFFER_SIZE       (sizeof(border_l3_header_t) + IPV6_MTU)

typedef struct {
    kernel_pid_t control_pid;
    kernel_pid_t registry[_SLIP_REGISTRY_SIZE];
    uint8_t esc;
    size_t bytes;
    ringbuffer_t *in_buf;
} _uart_ctx_t;

#if UART_NUMOF
_uart_ctx_t _uart_ctx[UART_NUMOF];
#else
#include "posix_io.h"
#include "board_uart0.h"

_uart_ctx_t _uart_ctx[1];
#endif

#define CTX(arg) ((_uart_ctx_t *)arg)

static char _slip_control_stack[_SLIP_STACK_SIZE];

void _slip_rx_cb(void *arg, char data)
{
    if (((uint8_t)data) == _SLIP_END) {
        msg_t msg;

        msg.type = _SLIP_MSG_TYPE;
        msg.content.ptr = arg;

        msg_send_receive(&msg, &msg, CTX(arg)->control_pid);

        CTX(arg)->bytes = 0;
    }

    if (CTX(arg)->esc) {
        CTX(arg)->esc = 0;

        switch ((uint8_t)data) {
            case (_SLIP_END_ESC):
                ringbuffer_add_one(CTX(arg)->in_buf, _SLIP_ESC);
                CTX(arg)->bytes++;
                break;

            case (_SLIP_ESC_ESC):
                ringbuffer_add_one(CTX(arg)->in_buf, _SLIP_ESC);
                CTX(arg)->bytes++;
                break;

            default:
                break;
        }
    }
    else if ((uint8_t)data == _SLIP_ESC) {
        CTX(arg)->esc = 1;
    }
    else {
        ringbuffer_add_one(CTX(arg)->in_buf, data);
        CTX(arg)->bytes++;
    }
}

#if UART_NUMOF
static inline int _slip_putc(uart_t uart, uint8_t c)
{
    return uart_write_blocking(uart, (char)c);
}
#else
static char _slip_reader_stack[_SLIP_STACK_SIZE];

static void *_slip_reader(void *arg)
{
    msg_t msg_queue[_SLIP_MSG_QUEUE_SIZE];

    (void)arg;

    msg_init_queue(msg_queue, _SLIP_REGISTRY_SIZE);

    posix_open(uart0_handler_pid, 0);

    while (1) {
        _slip_rx_cb(&(_uart_ctx[0]), uart0_readc());
    }

    return NULL;
}

static int _slip_putc(uart_t uart, char c)
{
    (void)uart;

    uart0_putc(c);
    return 1;
}
#endif

static void _slip_receive(msg_t *uart_msg, _uart_ctx_t *ctx)
{
    netapi_pkt_t rcv;
    netapi_ack_t ack_mem;
    msg_t l3_pkt, l3_ack, uart_reply;
    size_t bytes = ctx->bytes;

    msg_reply(uart_msg, &uart_reply);

    rcv.type = NETAPI_CMD_RCV;
    rcv.ack = &ack_mem;
    rcv.src = NULL;
    rcv.src_len = 0;
    rcv.dest = NULL;
    rcv.dest_len = 0;
    l3_pkt.type = NETAPI_MSG_TYPE;
    l3_pkt.content.ptr = (char *)(&rcv);

    for (int i = 0; i < _SLIP_REGISTRY_SIZE; i++) {
        if (ctx->registry[i] != KERNEL_PID_UNDEF) {
            size_t offset = 0;

            while (offset < ctx->bytes) {
                pkt_t *pkt = pktbuf_alloc(bytes);

                ringbuffer_get(ctx->in_buf, pkt->payload_data, bytes - offset);
                rcv.pkt = pkt;
                netapi_ack_t *ack;

                msg_send_receive(&l3_pkt, &l3_ack, ctx->registry[i]);
                pktbuf_release(pkt);
                ack = (netapi_ack_t *)(l3_ack.content.ptr);

                if (l3_ack.type == NETAPI_MSG_TYPE &&
                    ack->type == NETAPI_CMD_ACK &&
                    ack->orig == NETAPI_CMD_RCV) {
                    if (ack->result > 0) {
                        offset += (ack->result);
                    }
                    else {
                        DEBUG("Error code received from %d: %d",
                              ctx->registry[i], -ack->result);

                        return;
                    }
                }
                else {
                    DEBUG("Unexpected msg instead of ACK. Abort for %d: "
                          "msg.type = %d, ack->type = %d, ack->orig = %d",
                          ctx->registry[i], l3_ack->type, ack->type, ack - orig);
                    return;
                }
            }
        }
    }
}

static int _slip_write(uart_t uart, uint8_t *bytes, size_t len)
{
    int res;

    for (unsigned int i = 0; i < len; i++) {
        switch (bytes[i]) {
            case _SLIP_END:
                if (_slip_putc(uart, _SLIP_ESC) < 0) {
                    return -EIO;
                }

                res = _slip_putc(uart, _SLIP_END_ESC);
                break;

            case _SLIP_ESC:
                if (_slip_putc(uart, _SLIP_ESC) < 0) {
                    return -EIO;
                }

                res = _slip_putc(uart, _SLIP_ESC_ESC);
                break;

            default:
                res = _slip_putc(uart, bytes[i]);
                break;
        }

        if (res < 0) {
            return -EIO;
        }
    }

    return len;
}

static int _slip_send(uart_t uart, netapi_pkt_t *snd)
{
    int snd_byte = 0, res;
    pkt_hlist_t *ptr = snd->pkt->headers;

    pktbuf_hold(snd->pkt);

    while (ptr) {
        if ((res = _slip_write(uart, ptr->header_data, ptr->header_len)) < 0) {
            return res;
        }

        snd_byte += res;
        pkt_hlist_advance(&ptr);
    }

    if (snd->pkt->payload_data != NULL) {
        if ((res = _slip_write(uart, snd->pkt->payload_data,
                               snd->pkt->payload_len))) {
            return res;
        }

        snd_byte += res;
    }

    _slip_putc(uart, _SLIP_END);

    pktbuf_release(snd->pkt);

    return snd_byte;
}

static void _slip_handle_cmd(uart_t uart, msg_t *msg_cmd)
{
    msg_t msg_ack;
    netapi_cmd_t *cmd = (netapi_cmd_t *)(msg_cmd->content.ptr);
    netapi_ack_t *ack = cmd->ack;

    msg_ack.type = NETAPI_MSG_TYPE;
    msg_ack.content.ptr = (char *)ack;

    ack->type = NETAPI_CMD_ACK;
    ack->orig = cmd->type;

    switch (cmd->type) {
        case NETAPI_CMD_SND:
            ack->result = _slip_send(uart, (netapi_pkt_t *)cmd);
            break;

        case NETAPI_CMD_GET:
            switch (((netapi_conf_t *)cmd)->param) {
                case NETAPI_CONF_PROTO:
                    if (((netapi_conf_t *)cmd)->data_len < sizeof(netdev_proto_t)) {
                        ack->result = -EOVERFLOW;
                    }
                    else {
                        netapi_conf_t *conf = (netapi_conf_t *)cmd;
                        *((netdev_proto_t *)conf->data) = NETDEV_PROTO_SLIP;
                        conf->data_len = sizeof(netdev_proto_t);
                        ack->result = sizeof(netdev_proto_t);
                    }

                default:
                    ack->result = -ENOTSUP;
                    break;
            }

        case NETAPI_CMD_REG:
            ack->result = -ENOBUFS;

            for (int i = 0; i < _SLIP_REGISTRY_SIZE; i++) {
                netapi_reg_t *reg = (netapi_reg_t *)cmd;

                if (_uart_ctx[uart].registry[i] == KERNEL_PID_UNDEF) {
                    _uart_ctx[uart].registry[i] = reg->reg_pid;
                    ack->result = NETAPI_STATUS_OK;
                    break;
                }
            }

            break;

        case NETAPI_CMD_UNREG:
            ack->result = NETAPI_STATUS_OK;

            for (int i = 0; i < _SLIP_REGISTRY_SIZE; i++) {
                netapi_reg_t *reg = (netapi_reg_t *)cmd;

                if (_uart_ctx[uart].registry[i] == reg->reg_pid) {
                    _uart_ctx[uart].registry[i] = KERNEL_PID_UNDEF;
                    break;
                }
            }

            break;

        default:
            ack->result = -ENOTSUP;
            break;
    }

    msg_reply(msg_cmd, &msg_ack);
}

static void *_slip_control(void *arg)
{
    uart_t uart = *((uart_t *)arg);
    msg_t msg, msg_queue[_SLIP_MSG_QUEUE_SIZE];

    (void)arg;

    msg_init_queue(msg_queue, _SLIP_MSG_QUEUE_SIZE);

    while (1) {
        msg_receive(&msg);

        switch (msg.type) {
            case _SLIP_MSG_TYPE:
                _slip_receive(&msg, (_uart_ctx_t *)msg.content.ptr);
                break;

            case NETAPI_MSG_TYPE:
                _slip_handle_cmd(uart, &msg);
                break;

            default:
                break;
        }
    }

    return NULL;
}

kernel_pid_t slip_init(uart_t uart, uint32_t baudrate, ringbuffer_t *in_buf)
{
    char control_name[_SLIP_MAX_NAME_LEN];

    sprintf(control_name, "slip_contol_uart%02u", uart);

#if UART_NUMOF
    _uart_ctx[uart].bytes = 0;
    _uart_ctx[uart].esc = 0;
    _uart_ctx[uart].in_buf = in_buf;

    for (int i = 0; i < _SLIP_REGISTRY_SIZE; i++) {
        _uart_ctx[uart].registry[i] = KERNEL_PID_UNDEF;
    }

    uart_init(uart, baudrate, _slip_rx_cb, NULL, &(_uart_ctx[uart]));
#else
    (void)baudrate;

    _uart_ctx[0].bytes = 0;
    _uart_ctx[0].esc = 0;
    _uart_ctx[0].in_buf = in_buf;


    for (int i = 0; i < _SLIP_REGISTRY_SIZE; i++) {
        _uart_ctx[0].registry[i] = KERNEL_PID_UNDEF;
    }

    thread_create(_slip_reader_stack, _SLIP_STACK_SIZE, PRIORITY_MAIN - 1, 0,
                  _slip_reader, NULL, "slip_uart_null_reader");

#endif

    _uart_ctx[uart].control_pid = thread_create(_slip_control_stack, _SLIP_STACK_SIZE,
                                  PRIORITY_MAIN - 1, 0, _slip_control, &uart, control_name);

    return _uart_ctx[uart].control_pid;
}

#ifndef MODULE_NETAPI
int slip_send_l3_packet(kernel_pid_t pid, pkt_t *pkt)
{
    msg_t msg_pkt, msg_ack;
    netapi_pkt_t snd;
    netapi_ack_t ack;
    int ack_result;

    msg_pkt.type = NETAPI_MSG_TYPE;
    msg_pkt.content.ptr = (char *)(&snd);

    pktbuf_hold(pkt);

    snd.type = NETAPI_CMD_SND;
    snd.ack = &ack;
    snd.dest = NULL;
    snd.dest_len = 0;
    snd.pkt = pkt;

    msg_send_receive(&msg_pkt, &msg_ack, pid);

    pktbuf_release(pkt);

    if (msg_ack.content.ptr != (char *)(&ack) ||
        msg_ack.type != NETAPI_MSG_TYPE ||
        ack.type != NETAPI_CMD_ACK ||
        ack.orig != snd.type) {
        return -ENOMSG;
    }

    ack_result = ack.result;

    return ack_result;
}
#endif
