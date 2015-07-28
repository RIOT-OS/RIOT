/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_ng_slip
 * @{
 *
 * @file
 * @brief       SLIP device implementation
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "kernel.h"
#include "kernel_types.h"
#include "msg.h"
#include "net/ng_netbase.h"
#include "periph/uart.h"
#include "ringbuffer.h"
#include "thread.h"
#include "net/ng_ipv6/hdr.h"

#include "net/ng_slip.h"

#define _SLIP_END               ('\xc0')
#define _SLIP_ESC               ('\xdb')
#define _SLIP_END_ESC           ('\xdc')
#define _SLIP_ESC_ESC           ('\xdd')

#define _SLIP_MSG_TYPE          (0xc1dc)    /* chosen randomly */
#define _SLIP_NAME              "SLIP"
#define _SLIP_MSG_QUEUE_SIZE    (8U)

#define _SLIP_DEV(arg)    ((ng_slip_dev_t *)arg)

/* UART callbacks */
static void _slip_rx_cb(void *arg, char data)
{
    if (data == _SLIP_END) {
        msg_t msg;

        msg.type = _SLIP_MSG_TYPE;
        msg.content.value = _SLIP_DEV(arg)->in_bytes;

        msg_send_int(&msg, _SLIP_DEV(arg)->slip_pid);

        _SLIP_DEV(arg)->in_bytes = 0;
    }

    if (_SLIP_DEV(arg)->in_esc) {
        _SLIP_DEV(arg)->in_esc = 0;

        switch (data) {
            case (_SLIP_END_ESC):
                if (ringbuffer_add_one(_SLIP_DEV(arg)->in_buf, _SLIP_END) < 0) {
                    _SLIP_DEV(arg)->in_bytes++;
                }

                break;

            case (_SLIP_ESC_ESC):
                if (ringbuffer_add_one(_SLIP_DEV(arg)->in_buf, _SLIP_ESC) < 0) {
                    _SLIP_DEV(arg)->in_bytes++;
                }

                break;

            default:
                break;
        }
    }
    else if (data == _SLIP_ESC) {
        _SLIP_DEV(arg)->in_esc = 1;
    }
    else {
        if (ringbuffer_add_one(_SLIP_DEV(arg)->in_buf, data) < 0) {
            _SLIP_DEV(arg)->in_bytes++;
        }
    }
}

int _slip_tx_cb(void *arg)
{
    if (_SLIP_DEV(arg)->out_buf->avail > 0) {
        char c = (char)ringbuffer_get_one(_SLIP_DEV(arg)->out_buf);
        uart_write((uart_t)(_SLIP_DEV(arg)->uart), c);
        return 1;
    }

    return 0;
}

/* SLIP receive handler */
static void _slip_receive(ng_slip_dev_t *dev, size_t bytes)
{
    ng_netif_hdr_t *hdr;
    ng_netreg_entry_t *sendto;
    ng_pktsnip_t *pkt, *netif_hdr;

    pkt = ng_pktbuf_add(NULL, NULL, bytes, NG_NETTYPE_UNDEF);

    if (pkt == NULL) {
        LOG_ERROR("slip: no space left in packet buffer\n");
        return;
    }

    netif_hdr = ng_pktbuf_add(pkt, NULL, sizeof(ng_netif_hdr_t),
                              NG_NETTYPE_NETIF);

    if (netif_hdr == NULL) {
        LOG_ERROR("slip: no space left in packet buffer\n");
        ng_pktbuf_release(pkt);
        return;
    }

    hdr = netif_hdr->data;
    ng_netif_hdr_init(hdr, 0, 0);
    hdr->if_pid = thread_getpid();

    if (ringbuffer_get(dev->in_buf, pkt->data, bytes) != bytes) {
        LOG_ERROR("slip: could not read %zu bytes from ringbuffer\n", bytes);
        ng_pktbuf_release(pkt);
        return;
    }

#ifdef MODULE_NG_IPV6
    if ((pkt->size >= sizeof(ng_ipv6_hdr_t)) && ng_ipv6_hdr_is_ipv6_hdr(pkt->data)) {
        pkt->type = NG_NETTYPE_IPV6;
    }
#endif

    sendto = ng_netreg_lookup(pkt->type, NG_NETREG_DEMUX_CTX_ALL);

    if (sendto == NULL) {
        LOG_ERROR("slip: unable to forward packet of type %i\n", pkt->type);
        ng_pktbuf_release(pkt);
    }

    ng_pktbuf_hold(pkt, ng_netreg_num(pkt->type, NG_NETREG_DEMUX_CTX_ALL) - 1);

    while (sendto != NULL) {
        LOG_DEBUG("slip: sending pkt %p to PID %u\n", pkt, sendto->pid);
        ng_netapi_receive(sendto->pid, pkt);
        sendto = ng_netreg_getnext(sendto);
    }
}

static inline void _slip_send_char(ng_slip_dev_t *dev, char c)
{
    ringbuffer_add_one(dev->out_buf, c);
    uart_tx_begin(dev->uart);
}

/* SLIP send handler */
static void _slip_send(ng_slip_dev_t *dev, ng_pktsnip_t *pkt)
{
    ng_pktsnip_t *ptr;

    ptr = pkt->next;    /* ignore ng_netif_hdr_t, we don't need it */

    while (ptr != NULL) {
        LOG_DEBUG("slip: send pktsnip of length %zu over UART_%d\n", ptr->size, uart);
        char *data = ptr->data;

        for (size_t i = 0; i < ptr->size; i++) {
            switch (data[i]) {
                case _SLIP_END:
                    LOG_DEBUG("slip: encountered END byte on send: stuff with ESC\n");
                    _slip_send_char(dev, _SLIP_ESC);
                    _slip_send_char(dev, _SLIP_END_ESC);
                    break;

                case _SLIP_ESC:
                    LOG_DEBUG("slip: encountered ESC byte on send: stuff with ESC\n");
                    _slip_send_char(dev, _SLIP_ESC);
                    _slip_send_char(dev, _SLIP_ESC_ESC);
                    break;

                default:
                    _slip_send_char(dev, data[i]);

                    break;
            }
        }

        ptr = ptr->next;
    }

    _slip_send_char(dev, _SLIP_END);

    ng_pktbuf_release(pkt);
}

static void *_slip(void *args)
{
    ng_slip_dev_t *dev = _SLIP_DEV(args);
    msg_t msg, reply, msg_q[_SLIP_MSG_QUEUE_SIZE];

    msg_init_queue(msg_q, _SLIP_MSG_QUEUE_SIZE);
    dev->slip_pid = thread_getpid();
    ng_netif_add(dev->slip_pid);

    LOG_DEBUG("slip: SLIP runs on UART_%d\n", uart);

    while (1) {
        LOG_DEBUG("slip: waiting for incoming messages\n");
        msg_receive(&msg);

        switch (msg.type) {
            case _SLIP_MSG_TYPE:
                LOG_DEBUG("slip: incoming message from UART in buffer\n");
                _slip_receive(dev, (size_t)msg.content.value);
                break;

            case NG_NETAPI_MSG_TYPE_SND:
                LOG_DEBUG("slip: NG_NETAPI_MSG_TYPE_SND received\n");
                _slip_send(dev, (ng_pktsnip_t *)msg.content.ptr);
                break;

            case NG_NETAPI_MSG_TYPE_GET:
            case NG_NETAPI_MSG_TYPE_SET:
                LOG_DEBUG("slip: NG_NETAPI_MSG_TYPE_GET or NG_NETAPI_MSG_TYPE_SET received\n");
                reply.type = NG_NETAPI_MSG_TYPE_ACK;
                reply.content.value = (uint32_t)(-ENOTSUP);
                LOG_WARNING("slip: I don't support these but have to reply.\n");
                msg_reply(&msg, &reply);
                break;
        }
    }

    /* should be never reached */
    return NULL;
}

kernel_pid_t ng_slip_init(ng_slip_dev_t *dev, uart_t uart, uint32_t baudrate,
                          char *stack, size_t stack_size, char priority)
{
    int res;
    kernel_pid_t pid;

    /* reset device descriptor fields */
    dev->in_bytes = 0;
    dev->in_esc = 0;
    dev->slip_pid = KERNEL_PID_UNDEF;

    /* initialize buffers */
    ringbuffer_init(dev->in_buf, dev->rx_mem, sizeof(dev->rx_mem));
    ringbuffer_init(dev->out_buf, dev->tx_mem, sizeof(dev->tx_mem));

    /* initialize UART */
    LOG_DEBUG("slip: initialize UART_%d\n", uart);
    res = uart_init(uart, baudrate, _slip_rx_cb, _slip_tx_cb, dev);
    if (res < 0) {
        LOG_ERROR("slip: error initializing UART_%i with baudrate %u\n",
              uart, baudrate);
        return -ENODEV;
    }

    /* start SLIP thread */
    LOG_DEBUG("slip: starting SLIP thread\n");
    pid = thread_create(stack, stack_size, priority, CREATE_STACKTEST,
                        _slip, dev, _SLIP_NAME);
    if (pid < 0) {
        LOG_ERROR("slip: unable to create SLIP thread\n");
        return -EFAULT;
    }
    return res;
}
