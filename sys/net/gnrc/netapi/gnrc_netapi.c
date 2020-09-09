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

#include <errno.h>

#include "mbox.h"
#include "msg.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/netapi.h"

#define ENABLE_DEBUG    (0)
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

int gnrc_netapi_dispatch(gnrc_nettype_t type, uint32_t demux_ctx,
                         uint16_t cmd, gnrc_pktsnip_t *pkt)
{
    int numof = gnrc_netreg_num(type, demux_ctx);

    if (numof != 0) {
        gnrc_netreg_entry_t *sendto = gnrc_netreg_lookup(type, demux_ctx);

        gnrc_pktbuf_hold(pkt, numof - 1);

        while (sendto) {
#if defined(MODULE_GNRC_NETAPI_MBOX) || defined(MODULE_GNRC_NETAPI_CALLBACKS)
            uint32_t status = 0;
            switch (sendto->type) {
                case GNRC_NETREG_TYPE_DEFAULT:
                    if (_gnrc_netapi_send_recv(sendto->target.pid, pkt,
                                               cmd) < 1) {
                        /* unable to dispatch packet */
                        status = EIO;
                    }
                    break;
#ifdef MODULE_GNRC_NETAPI_MBOX
                case GNRC_NETREG_TYPE_MBOX:
                    if (_snd_rcv_mbox(sendto->target.mbox, cmd, pkt) < 1) {
                        /* unable to dispatch packet */
                        status = EIO;
                    }
                    break;
#endif
#ifdef MODULE_GNRC_NETAPI_CALLBACKS
                case GNRC_NETREG_TYPE_CB:
                    sendto->target.cbd->cb(cmd, pkt, sendto->target.cbd->ctx);
                    break;
#endif
                default:
                    /* unknown dispatch type */
                    status = ECANCELED;
                    break;
            }
            if (status != 0) {
                gnrc_pktbuf_release_error(pkt, status);
            }
#else
            if (_gnrc_netapi_send_recv(sendto->target.pid, pkt, cmd) < 1) {
                /* unable to dispatch packet */
                gnrc_pktbuf_release_error(pkt, EIO);
            }
#endif
            sendto = gnrc_netreg_getnext(sendto);
        }
    }

    return numof;
}
