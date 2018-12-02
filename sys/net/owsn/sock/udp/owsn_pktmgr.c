/*
 * Copyright (C) 2016 Martine Lenders <mlenders@inf.fu-berlin.de>
 * Copyright (C) 2018 Ken Bannister <kb2ma@runbox.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_owsn
 * @{
 *
 * @file
 * @brief       OpenWSN implementation of @ref net_sock_udp
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author  Ken Bannister <kb2ma@runbox.com>
 */

#include <errno.h>
#include "od.h"
#include "xtimer.h"
#include "net/sock/udp.h"
#include "net/udp.h"

#include "opendefs.h"
#include "scheduler.h"
#include "04-TRAN/openudp.h"
#include "cross-layers/openqueue.h"

#include "owsn_sock_internal.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* packet to be sent; waits on OpenWSN callback*/
static OpenQueueEntry_t *_out_pkt;

/* mailbox to receive from OpenWSN stack */
static mbox_t _mbox;

#define _MBOX_QUEUE_SIZE (1)
#define _MSG_TYPE_RECV_PKT (0x1601)

/* queue for mbox */
static msg_t _mbox_queue[_MBOX_QUEUE_SIZE];

static udp_resource_desc_t _openudp_conf = { .port=0, .callbackReceive=NULL,
        .callbackSendDone=NULL, .next=NULL };

#ifdef MODULE_XTIMER
#define _TIMEOUT_MAGIC      (0xF38A0B63U)
#define _TIMEOUT_MSG_TYPE   (0x8474)

static void _callback_put(void *arg)
{
    msg_t timeout_msg = { .sender_pid = KERNEL_PID_UNDEF,
                          .type = _TIMEOUT_MSG_TYPE,
                          .content = { .value = _TIMEOUT_MAGIC } };

    /* should be safe, because otherwise if mbox were filled this callback is
     * senseless */
    mbox_try_put(&_mbox, &timeout_msg);
}
#endif

void _recv_ready_cb(OpenQueueEntry_t *pkt);
void _send_ready_cb(void);


void owsn_sock_create(uint32_t demux_ctx)
{
    if ((_openudp_conf.port == 0) && (demux_ctx > 0)) {
        _openudp_conf.port = (uint16_t)demux_ctx;
        _openudp_conf.callbackReceive = _recv_ready_cb;
        openudp_register(&_openudp_conf);
        DEBUG("owsn_pktmgr: registered openudp on port: %u\n", _openudp_conf.port);

        mbox_init(&_mbox, _mbox_queue, _MBOX_QUEUE_SIZE);
    }
}

ssize_t owsn_sock_recv(uint32_t timeout, OpenQueueEntry_t **pkt,
                       sock_ip_ep_t *remote)
{
    msg_t msg;
    
    /* should have been initialized on send for client, or on create for server */
    assert(_openudp_conf.port > 0);
#ifdef MODULE_XTIMER
    xtimer_t timeout_timer;

    if ((timeout != SOCK_NO_TIMEOUT) && (timeout != 0)) {
        timeout_timer.callback = _callback_put;
        timeout_timer.arg = NULL;
        xtimer_set(&timeout_timer, timeout);
    }
#endif

    /* wait for msg */
    DEBUG("owsn_pktmgr: waiting to recv pkt on port %u\n", _openudp_conf.port);
    mbox_get(&_mbox, &msg);
#ifdef MODULE_XTIMER
    xtimer_remove(&timeout_timer);
#endif

    switch (msg.type) {
        case _MSG_TYPE_RECV_PKT:
            *pkt = msg.content.ptr;
            break;
#ifdef MODULE_XTIMER
        case _TIMEOUT_MSG_TYPE:
            if (msg.content.value == _TIMEOUT_MAGIC) {
                return -ETIMEDOUT;
            }
#endif
            /* Falls Through. */
        default:
            return -EINVAL;
    }

    /* populate remote host info */
    remote->family = AF_INET6;
    memcpy((void *)&remote->addr.ipv6[0],
           (void *)(&(*pkt)->l3_sourceAdd.addr_128b[0]), 16);
    DEBUG("owsn_pktmgr: from src addr:\n");
    if (ENABLE_DEBUG) {
        od_hex_dump(&(*pkt)->l3_sourceAdd.addr_128b[0], 16, OD_WIDTH_DEFAULT);
    }

    return (*pkt)->length;
}

ssize_t owsn_sock_send(OpenQueueEntry_t *pkt)
{
    assert(_openudp_conf.port > 0);

    _out_pkt = pkt;
    scheduler_push_task(_send_ready_cb, TASKPRIO_COAP);

    return 0;
}

/*
 * Callback from OpenWSN when a packet has been received.
 *
 * Put reference to pkt into mbox.
 */
void _recv_ready_cb(OpenQueueEntry_t *pkt)
{
    msg_t msg;

    DEBUG("Received %i bytes on port %i\n", (int)pkt->length,
          pkt->l4_destination_port);

    msg.type =_MSG_TYPE_RECV_PKT;
    msg.content.ptr = pkt;
    mbox_put(&_mbox, &msg);
}

/* Callback from OpenWSN to actually send the packet. */
void _send_ready_cb(void) {
    assert(_out_pkt != NULL);

    owerror_t ret = openudp_send(_out_pkt);
    if (ret == E_FAIL) {
        DEBUG("owsn_pktmgr: could not send\n");
    }
    else {
        DEBUG("owsn_pktmgr: pkt sent\n");
    }
    _out_pkt = NULL;
}

/** @} */
