/*
 * Copyright (C) 2016 José Ignacio Alamos
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  José Ignacio Alamos <jialamos@uc.cl>
 */

#include "net/conn/udp.h"
#include "openthread/conn.h"
#include "stdio.h"
#include <openthread.h>
#include "string.h"
#include <net/ipv6/addr.h>
#include "ot.h"
#include "errno.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static msg_t _msg;

static conn_udp_msg_t msg_info;
static otSockAddr sockaddr = {};

void HandleUdpReceived(void *aContext, otMessage aMessage, const otMessageInfo *aMessageInfo)
{
    DEBUG("openthread: Received UDP packet\n");
    msg_info.message = aMessage;
    msg_info.message_info  = (otMessageInfo *) aMessageInfo;
    _msg.type = OPENTHREAD_MSG_TYPE_RECV;
    _msg.content.ptr = (void *) &msg_info;
    conn_udp_t *conn = (conn_udp_t *) aContext;
    msg_send(&_msg, conn->receiver_pid);
}

int conn_udp_create(conn_udp_t *conn, const void *addr, size_t addr_len, int family, uint16_t port)
{
    DEBUG("openthread: conn_udp_create\n");
    memcpy(&sockaddr.mAddress.mFields, addr, addr_len);
    sockaddr.mPort = port;

    begin_mutex();
    otOpenUdpSocket(&conn->mSocket, &HandleUdpReceived, conn);
    otBindUdpSocket(&conn->mSocket, &sockaddr);
    end_mutex();
    return 0;
}

int conn_udp_sendto(const void *data, size_t len, const void *src, size_t src_len,
                    const void *dst, size_t dst_len, int family, uint16_t sport,
                    uint16_t dport)
{
    DEBUG("openthread: conn_udp_sendto\n");
    if (dst_len != sizeof(ipv6_addr_t) || (src != NULL && src_len != sizeof(ipv6_addr_t))) {
        return -EINVAL;
    }

    otMessage message;
    otUdpSocket mSocket;

    begin_mutex();
    message = otNewUdpMessage();
    otSetMessageLength(message, len);
    otWriteMessage(message, 0, data, len);
    end_mutex();

    otMessageInfo mPeer;

    //Set source address
    memcpy(&mPeer.mSockAddr.mFields, src, src_len);

    //Set source port
    mSocket.mSockName.mPort = sport;

    //Set dest address
    memcpy(&mPeer.mPeerAddr.mFields, dst, dst_len);

    //Set dest port
    mPeer.mPeerPort = dport;

    //Send UDP packet through OT
    begin_mutex();
    otSendUdp(&mSocket, message, &mPeer);
    end_mutex();
    return 0;
}

void conn_udp_close(conn_udp_t *conn)
{
    DEBUG("openthread: conn_udp_close\n");
    begin_mutex();
    otCloseUdpSocket(&conn->mSocket);
    end_mutex();
}

int conn_udp_getlocaladdr(conn_udp_t *conn, void *addr, uint16_t *port)
{
    DEBUG("openthread: conn_udp_getlocaladdr\n");
    memcpy(addr, &sockaddr.mAddress.mFields, sizeof(ipv6_addr_t));
    *port = sockaddr.mPort;
    return sizeof(ipv6_addr_t);
}

int conn_udp_recvfrom(conn_udp_t *conn, void *data, size_t max_len, void *addr, size_t *addr_len, uint16_t *port)
{
    DEBUG("openthread: conn_udp_recv_from\n");
    msg_t msg;
    conn->receiver_pid = thread_getpid();
    conn_udp_msg_t *cudp;
    int msg_length, msg_offset;
    while (1) {
        msg_receive(&msg);
        if (msg.type == OPENTHREAD_MSG_TYPE_RECV) {
            cudp = (conn_udp_msg_t *) msg.content.ptr;
            begin_mutex();
            msg_offset = otGetMessageOffset(cudp->message);
            msg_length = otGetMessageLength(cudp->message) - msg_offset;
            otReadMessage(cudp->message, msg_offset, data, msg_length);
            end_mutex();
            memcpy(addr, &cudp->message_info->mPeerAddr.mFields, sizeof(ipv6_addr_t));
            *addr_len = sizeof(ipv6_addr_t);;
            *port = cudp->message_info->mPeerPort;
            break;
        }
    }
    return msg_length;
}

ipv6_addr_t *conn_find_best_source(const ipv6_addr_t *dst)
{
    DEBUG("openthread: Finding best source\n");

    /* This function will be unused in the new conn API. Return NULL for now */
    return NULL;
}
