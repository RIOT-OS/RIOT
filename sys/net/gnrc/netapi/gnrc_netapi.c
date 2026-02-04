/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     net_gnrc_netapi
 * @file
 * @brief       This file contains a number of helper functions that provide
 *              some shortcuts for some always repeating code snippets when
 *              servicing the netapi interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include <assert.h>
#include <errno.h>

#include "mbox.h"
#include "msg.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/netapi/notify.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/netapi.h"
#include "sema_inv.h"

#define ENABLE_DEBUG 0
#include "debug.h"

int _gnrc_netapi_get_set(kernel_pid_t pid, netopt_t opt, uint16_t context,
                         void *data, size_t data_len, uint16_t type)
{
    msg_t cmd;
    msg_t ack;
    gnrc_netapi_opt_t o;
    /* set netapi's option struct */
    o.opt = opt;
    o.context = context;
    o.data = data;
    o.data_len = data_len;
    /* set outgoing message's fields */
    cmd.type = type;
    cmd.content.ptr = (void *)&o;
    /* trigger the netapi */
    msg_send_receive(&cmd, &ack, pid);
    assert(ack.type == GNRC_NETAPI_MSG_TYPE_ACK);
    /* return the ACK message's value */
    return (int)ack.content.value;
}

int _gnrc_netapi_send_recv(kernel_pid_t pid, gnrc_pktsnip_t *pkt, uint16_t type)
{
    msg_t msg;
    /* set the outgoing message's fields */
    msg.type = type;
    msg.content.ptr = (void *)pkt;
    /* send message */
    int ret = msg_try_send(&msg, pid);
    if (ret < 1) {
        DEBUG("gnrc_netapi: dropped message to %" PRIkernel_pid " (%s)\n", pid,
              (ret == 0) ? "receiver queue is full" : "invalid receiver");
    }
    return ret;
}

#ifdef MODULE_GNRC_NETAPI_MBOX
static inline int _snd_rcv_mbox(mbox_t *mbox, uint16_t type, gnrc_pktsnip_t *pkt)
{
    msg_t msg;
    /* set the outgoing message's fields */
    msg.type = type;
    msg.content.ptr = (void *)pkt;
    /* send message */
    int ret = mbox_try_put(mbox, &msg);
    if (ret < 1) {
        DEBUG("gnrc_netapi: dropped message to %p (was full)\n", (void*)mbox);
    }
    return ret;
}
#endif

static int _dispatch_single(gnrc_netreg_entry_t *sendto, uint16_t cmd, void *data)
{
    int status = 0;

#if defined(MODULE_GNRC_NETAPI_MBOX) || defined(MODULE_GNRC_NETAPI_CALLBACKS)
    switch (sendto->type) {
    case GNRC_NETREG_TYPE_DEFAULT:
        if (_gnrc_netapi_send_recv(sendto->target.pid, data, cmd) < 1) {
            /* unable to dispatch packet */
            status = -EIO;
        }
        break;
#  ifdef MODULE_GNRC_NETAPI_MBOX
    case GNRC_NETREG_TYPE_MBOX:
        if (_snd_rcv_mbox(sendto->target.mbox, cmd, data) < 1) {
            /* unable to dispatch packet */
            status = -EIO;
        }
        break;
#  endif
#  ifdef MODULE_GNRC_NETAPI_CALLBACKS
    case GNRC_NETREG_TYPE_CB:
        sendto->target.cbd->cb(cmd, data, sendto->target.cbd->ctx);
        break;
#  endif
    default:
        /* unknown dispatch type */
        status = -ECANCELED;
        break;
    }
#else
    status = _gnrc_netapi_send_recv(sendto->target.pid, data, cmd);
#endif
    return status;
}

int gnrc_netapi_dispatch(gnrc_nettype_t type, uint32_t demux_ctx,
                         uint16_t cmd, gnrc_pktsnip_t *pkt)
{
    gnrc_netreg_acquire_shared();

    int numof = gnrc_netreg_num(type, demux_ctx);

    if (numof != 0) {
        gnrc_netreg_entry_t *sendto = gnrc_netreg_lookup(type, demux_ctx);

        /* the packet is replicated over all interfaces that is's being sent on */
        gnrc_pktbuf_hold(pkt, numof - 1);

        while (sendto) {
            int status = _dispatch_single(sendto, cmd, pkt);
            if (status < 0) {
                gnrc_pktbuf_release_error(pkt, status);
            }
            sendto = gnrc_netreg_getnext(sendto);
        }
    }

    gnrc_netreg_release_shared();

    return numof;
}

int gnrc_netapi_notify(gnrc_nettype_t type, uint32_t demux_ctx, netapi_notify_t event,
                       void *data, size_t data_len)
{
    gnrc_netreg_acquire_shared();

    int numof = gnrc_netreg_num(type, demux_ctx);
    if (numof <= 0) {
        gnrc_netreg_release_shared();
        return numof;
    }

    gnrc_netapi_notify_t notify = {
        .event = event,
        ._data = data,
        ._data_len = data_len,
    };

    /* Use inverse semaphore to count the acknowledgments from the receivers. */
    sema_inv_init(&notify.ack, numof);

    /* Look up the registered threads for this message type. */
    gnrc_netreg_entry_t *sendto = gnrc_netreg_lookup(type, demux_ctx);

    /* Dispatch to all registered threads. */
    while (sendto) {
        int status = _dispatch_single(sendto, GNRC_NETAPI_MSG_TYPE_NOTIFY, &notify);
        if (status < 0) {
            numof--;
            /* Decrease semaphore counter manually when sending failed. */
            sema_inv_post(&notify.ack);
        }
        sendto = gnrc_netreg_getnext(sendto);
    }

    gnrc_netreg_release_shared();

    if (data != NULL) {
        /* Wait for ACK from all receivers to ensure that the data was read and
         * that there won't be any dangling pointers after the function returned. */
        sema_inv_wait(&notify.ack);
    }

    return numof;
}
