/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <assert.h>
#include <errno.h>
#include <stdbool.h>

#include "byteorder.h"
#include "evproc.h"
#include "msg.h"
#include "mutex.h"
#include "net/af.h"
#include "net/sock/udp.h"
#include "net/ipv6/hdr.h"
#include "sched.h"
#include "uip.h"
#include "xtimer.h"

#define _MSG_TYPE_CLOSE     (0x4123)
#define _MSG_TYPE_TIMEOUT   (0x4124)
#define _MSG_TYPE_RCV       (0x4125)

/* struct to describe a sendto command for emb6 thread */
typedef struct {
    mutex_t block;
    struct udp_socket *sock;
    const sock_udp_ep_t *remote;
    int res;
    const void *data;
    size_t len;
} _send_cmd_t;

extern uint16_t uip_slen;

static bool send_registered = false;

static void _timeout_callback(void *arg);
static void _input_callback(struct udp_socket *c, void *ptr,
                            const uip_ipaddr_t *src_addr, uint16_t src_port,
                            const uip_ipaddr_t *dst_addr, uint16_t dst_port,
                            const uint8_t *data, uint16_t datalen);
static void _output_callback(c_event_t c_event, p_data_t p_data);

static int _reg(struct udp_socket *c, void *ptr, udp_socket_input_callback_t cb,
                const sock_udp_ep_t *local, const sock_udp_ep_t *remote)
{
    if (((local != NULL) && (local->family != AF_INET6)) ||
        ((remote != NULL) && (remote->family != AF_INET6))) {
        return -EAFNOSUPPORT;
    }
    if (udp_socket_register(c, ptr, cb) < 0) {
        return -ENOMEM;
    }
    if (local != NULL) {
        if (udp_socket_bind(c, local->port) < 0) {
            udp_socket_close(c);
            return -EADDRINUSE;
        }
    }
    if (remote != NULL) {
        /* check of return value not necessary, since neither c nor
         * c->udp_conn is NULL (only error case) at this point */
        udp_socket_connect(c, (uip_ipaddr_t *)&remote->addr, remote->port);
    }
    return 0;
}

int sock_udp_create(sock_udp_t *sock, const sock_udp_ep_t *local,
                    const sock_udp_ep_t *remote, uint16_t flags)
{
    int res;

    (void)flags;
    assert((sock != NULL));
    assert((remote == NULL) || (remote->port != 0));
    if (sock->sock.input_callback != NULL) {
        sock_udp_close(sock);
    }
    mutex_init(&sock->mutex);
    mutex_lock(&sock->mutex);
    mbox_init(&sock->mbox, sock->mbox_queue, SOCK_MBOX_SIZE);
    atomic_init(&sock->receivers, 0);
    if ((res = _reg(&sock->sock, sock, _input_callback, local, remote)) < 0) {
        sock->sock.input_callback = NULL;
    }
    mutex_unlock(&sock->mutex);
    return res;
}

void sock_udp_close(sock_udp_t *sock)
{
    assert(sock != NULL);
    if (sock->sock.input_callback != NULL) {
        while (atomic_fetch_sub(&sock->receivers, 1) > 0) {
            msg_t msg = { .type = _MSG_TYPE_CLOSE };
            mbox_put(&sock->mbox, &msg);
        }
        mutex_lock(&sock->mutex);
        udp_socket_close(&sock->sock);
        sock->sock.input_callback = NULL;
        mutex_unlock(&sock->mutex);
    }
}

int sock_udp_get_local(sock_udp_t *sock, sock_udp_ep_t *ep)
{
    assert((sock != NULL) && (ep != NULL));
    if ((sock->sock.input_callback != NULL) &&
        (sock->sock.udp_conn->lport != 0)) {
        mutex_lock(&sock->mutex);
        /* local UDP endpoints do not have addresses in emb6 */
        memset(&ep->addr, 0, sizeof(ipv6_addr_t));
        ep->port = ntohs(sock->sock.udp_conn->lport);
        mutex_unlock(&sock->mutex);
        return sizeof(ipv6_addr_t);
    }
    return -EADDRNOTAVAIL;
}

int sock_udp_get_remote(sock_udp_t *sock, sock_udp_ep_t *ep)
{
    assert((sock != NULL) && (ep != NULL));
    if ((sock->sock.input_callback != NULL) &&
        (sock->sock.udp_conn->rport != 0)) {
        mutex_lock(&sock->mutex);
        memcpy(&ep->addr, &sock->sock.udp_conn->ripaddr, sizeof(ipv6_addr_t));
        ep->port = ntohs(sock->sock.udp_conn->rport);
        mutex_unlock(&sock->mutex);
        return sizeof(ipv6_addr_t);
    }
    return -ENOTCONN;
}

int sock_udp_recv(sock_udp_t *sock, void *data, size_t max_len,
                  uint32_t timeout, sock_udp_ep_t *remote)
{
    xtimer_t timeout_timer;
    int blocking = BLOCKING;
    int res = -EIO;
    msg_t msg;

    assert((sock != NULL) && (data != NULL) && (max_len > 0));
    if (sock->sock.input_callback == NULL) {
        return -EADDRNOTAVAIL;
    }
    if (timeout == 0) {
        blocking = NON_BLOCKING;
    }
    else if (timeout != SOCK_NO_TIMEOUT) {
        timeout_timer.callback = _timeout_callback;
        timeout_timer.arg = &sock->mbox;
        xtimer_set(&timeout_timer, timeout);
    }
    atomic_fetch_add(&sock->receivers, 1);
    if (_mbox_get(&sock->mbox, &msg, blocking) == 0) {
        /* do not need to remove xtimer, since we only get here in non-blocking
         * mode (timeout > 0) */
        return -EAGAIN;
    }
    switch (msg.type) {
        case _MSG_TYPE_CLOSE:
            res = -EADDRNOTAVAIL;
            break;
        case _MSG_TYPE_TIMEOUT:
            res = -ETIMEDOUT;
            break;
        case _MSG_TYPE_RCV:
            mutex_lock(&sock->mutex);
            if (max_len < sock->recv_info.datalen) {
                res = -ENOBUFS;
                mutex_unlock(&sock->mutex);
                break;
            }
            memcpy(data, sock->recv_info.data, sock->recv_info.datalen);
            if (remote != NULL) {
                remote->family = AF_INET6;
                remote->netif = SOCK_ADDR_ANY_NETIF;
                memcpy(&remote->addr, &sock->recv_info.src, sizeof(ipv6_addr_t));
                remote->port = sock->recv_info.src_port;
            }
            res = (int)sock->recv_info.datalen;
            mutex_unlock(&sock->mutex);
            break;
    }
    atomic_fetch_sub(&sock->receivers, 1);
    return res;
}

int sock_udp_send(sock_udp_t *sock, const void *data, size_t len,
                  const sock_udp_ep_t *remote)
{
    struct udp_socket tmp;
    _send_cmd_t send_cmd = { .block = MUTEX_INIT,
                             .remote = remote,
                             .data = data,
                             .len = len };

    assert((sock != NULL) || (remote != NULL));
    assert((len == 0) || (data != NULL));   /* (len != 0) => (data != NULL) */
    /* we want the send in the uip thread (which udp_socket_send does not offer)
     * so we need to do it manually */
    if (!send_registered) {
        if (evproc_regCallback(EVENT_TYPE_SOCK_SEND, _output_callback) != E_SUCCESS) {
            return -ENOMEM;
        }
        else {
            send_registered = true;
        }
    }
    if ((len > (UIP_BUFSIZE - (UIP_LLH_LEN + UIP_IPUDPH_LEN))) ||
        (len > UINT16_MAX)) {
        return -ENOMEM;
    }
    if (remote != NULL) {
        if (remote->family != AF_INET6) {
            return -EAFNOSUPPORT;
        }
        if (remote->port == 0) {
            return -EINVAL;
        }
        send_cmd.remote = remote;
    }
    else if (sock->sock.udp_conn->rport == 0) {
        return -ENOTCONN;
    }
    /* cppcheck-supress nullPointerRedundantCheck
     * remote == NULL implies that sock != NULL (see assert at start of
     * function) * that's why it is okay in the if-statement above to check
     * sock->... without checking (sock != NULL) first => this check afterwards
     * isn't redundant */
    if (sock == NULL) {
        int res;
        if ((res = _reg(&tmp, NULL, NULL, NULL, NULL)) < 0) {
            return res;
        }
        send_cmd.sock = &tmp;
    }
    else {
        send_cmd.sock = &sock->sock;
    }
    mutex_lock(&send_cmd.block);
    /* change to emb6 thread context */
    if (evproc_putEvent(E_EVPROC_TAIL, EVENT_TYPE_SOCK_SEND, &send_cmd) == E_SUCCESS) {
        /* block thread until data was sent */
        mutex_lock(&send_cmd.block);
    }
    else {
        /* most likely error: event queue was full */
        send_cmd.res = -ENOMEM;
    }
    if (send_cmd.sock == &tmp) {
        udp_socket_close(&tmp);
    }
    mutex_unlock(&send_cmd.block);

    return send_cmd.res;
}

static void _timeout_callback(void *arg)
{
    msg_t msg = { .type = _MSG_TYPE_TIMEOUT };
    mbox_t *mbox = arg;

    /* should be safe, because otherwise if mbox were filled this callback is
     * senseless */
    mbox_try_put(mbox, &msg);
}

static void _input_callback(struct udp_socket *c, void *ptr,
                            const uip_ipaddr_t *src_addr, uint16_t src_port,
                            const uip_ipaddr_t *dst_addr, uint16_t dst_port,
                            const uint8_t *data, uint16_t datalen)
{
    msg_t msg = { .type = _MSG_TYPE_RCV };
    sock_udp_t *sock = ptr;

    (void)dst_addr;
    (void)dst_port;
    mutex_lock(&sock->mutex);
    sock->recv_info.src_port = src_port;
    sock->recv_info.src = (const ipv6_addr_t *)src_addr;
    sock->recv_info.data = data;
    sock->recv_info.datalen = datalen - sizeof(ipv6_hdr_t);
    mutex_unlock(&sock->mutex);
    mbox_put(&sock->mbox, &msg);
}

static void _output_callback(c_event_t c_event, p_data_t p_data)
{

    if ((c_event != EVENT_TYPE_SOCK_SEND) || (p_data == NULL)) {
        return;
    }

    _send_cmd_t *send_cmd = (_send_cmd_t *)p_data;

    if (send_cmd->remote != NULL) {
        /* send_cmd->len was previously checked */
        send_cmd->res = udp_socket_sendto(send_cmd->sock, send_cmd->data,
                                          (uint16_t)send_cmd->len,
                                          (uip_ipaddr_t *)&send_cmd->remote->addr,
                                          send_cmd->remote->port);
    }
    else {
        /* send_cmd->len was previously checked */
        send_cmd->res = udp_socket_send(send_cmd->sock, send_cmd->data,
                                        (uint16_t)send_cmd->len);
    }
    send_cmd->res = (send_cmd->res < 0) ? -EHOSTUNREACH : send_cmd->res;
    /* notify notify waiting thread */
    mutex_unlock(&send_cmd->block);
}
/** @} */
