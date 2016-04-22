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
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <assert.h>
#include <errno.h>
#include <stdbool.h>

#include "evproc.h"
#include "msg.h"
#include "mutex.h"
#include "net/af.h"
#include "net/conn/udp.h"
#include "net/ipv6/hdr.h"
#include "sched.h"
#include "uip.h"

#define _MSG_TYPE_CLOSE     (0x4123)
#define _MSG_TYPE_RCV       (0x4124)

/* struct to describe a sendto command for emb6 thread */
typedef struct {
    struct udp_socket sock;
    mutex_t mutex;
    const void *data;
    int res;
    uint16_t data_len;
} _send_cmd_t;

extern uint16_t uip_slen;

static bool send_registered = false;

static void _input_callback(struct udp_socket *c, void *ptr,
                            const uip_ipaddr_t *src_addr, uint16_t src_port,
                            const uip_ipaddr_t *dst_addr, uint16_t dst_port,
                            const uint8_t *data, uint16_t datalen);
static void _output_callback(c_event_t c_event, p_data_t p_data);

static int _reg_and_bind(struct udp_socket *c, void *ptr,
                         udp_socket_input_callback_t cb, uint16_t port)
{
    if (udp_socket_register(c, ptr, cb) < 0) {
        return -EMFILE;
    }
    if (udp_socket_bind(c, port) < 0) {
        udp_socket_close(c);
        return -EALREADY;
    }
    return 0;
}

int conn_udp_create(conn_udp_t *conn, const void *addr, size_t addr_len,
                    int family, uint16_t port)
{
    int res;

    (void)addr;
    (void)addr_len;
    if (family != AF_INET6) {
        return -EAFNOSUPPORT;
    }
    if (conn->sock.input_callback != NULL) {
        return -EINVAL;
    }
    mutex_init(&conn->mutex);
    mutex_lock(&conn->mutex);
    if ((res = _reg_and_bind(&conn->sock, conn, _input_callback, port)) < 0) {
        conn->sock.input_callback = NULL;
        mutex_unlock(&conn->mutex);
        return res;
    }
    conn->waiting_thread = KERNEL_PID_UNDEF;
    mutex_unlock(&conn->mutex);
    return 0;
}

void conn_udp_close(conn_udp_t *conn)
{
    if (conn->sock.input_callback != NULL) {
        mutex_lock(&conn->mutex);
        if (conn->waiting_thread != KERNEL_PID_UNDEF) {
            msg_t msg;
            msg.type = _MSG_TYPE_CLOSE;
            msg.content.ptr = (char *)conn;
            mutex_unlock(&conn->mutex);
            msg_send(&msg, conn->waiting_thread);
            mutex_lock(&conn->mutex);
        }
        udp_socket_close(&conn->sock);
        conn->sock.input_callback = NULL;
        mutex_unlock(&conn->mutex);
    }
}

int conn_udp_getlocaladdr(conn_udp_t *conn, void *addr, uint16_t *port)
{
    if (conn->sock.input_callback != NULL) {
        mutex_lock(&conn->mutex);
        memset(addr, 0, sizeof(ipv6_addr_t));
        *port = NTOHS(conn->sock.udp_conn->lport);
        mutex_unlock(&conn->mutex);
        return sizeof(ipv6_addr_t);
    }
    return -EBADF;
}

int conn_udp_recvfrom(conn_udp_t *conn, void *data, size_t max_len, void *addr,
                      size_t *addr_len, uint16_t *port)
{
    int res = -EIO;
    msg_t msg;

    if (conn->sock.input_callback == NULL) {
        return -ENOTSOCK;
    }
    mutex_lock(&conn->mutex);
    if (conn->waiting_thread != KERNEL_PID_UNDEF) {
        mutex_unlock(&conn->mutex);
        return -EALREADY;
    }
    conn->waiting_thread = sched_active_pid;
    mutex_unlock(&conn->mutex);
    msg_receive(&msg);
    if (msg.type == _MSG_TYPE_CLOSE) {
        conn->waiting_thread = KERNEL_PID_UNDEF;
        return -EINTR;
    }
    else if (msg.type == _MSG_TYPE_RCV) {
        mutex_lock(&conn->mutex);
        if (msg.content.ptr == (char *)conn) {
            if (max_len < conn->recv_info.datalen) {
                conn->waiting_thread = KERNEL_PID_UNDEF;
                mutex_unlock(&conn->mutex);
                return -ENOBUFS;
            }
            memcpy(data, conn->recv_info.data, conn->recv_info.datalen);
            memcpy(addr, conn->recv_info.src, sizeof(ipv6_addr_t));
            *addr_len = sizeof(ipv6_addr_t);
            *port = conn->recv_info.src_port;
            res = (int)conn->recv_info.datalen;
        }
        conn->waiting_thread = KERNEL_PID_UNDEF;
        mutex_unlock(&conn->mutex);
    }
    return res;
}

int conn_udp_sendto(const void *data, size_t len, const void *src, size_t src_len,
                    const void *dst, size_t dst_len, int family, uint16_t sport,
                    uint16_t dport)
{
    int res;
    _send_cmd_t send_cmd;

    if (!send_registered) {
        if (evproc_regCallback(EVENT_TYPE_CONN_SEND, _output_callback) != E_SUCCESS) {
            return -EIO;
        }
        else {
            send_registered = true;
        }
    }
    mutex_init(&send_cmd.mutex);
    if ((len > (UIP_BUFSIZE - (UIP_LLH_LEN + UIP_IPUDPH_LEN))) ||
        (len > UINT16_MAX)) {
        return -EMSGSIZE;
    }
    if ((dst_len > sizeof(ipv6_addr_t)) || (family != AF_INET6)) {
        return -EAFNOSUPPORT;
    }
    mutex_lock(&send_cmd.mutex);
    send_cmd.data = data;
    send_cmd.data_len = (uint16_t)len;
    if ((res = _reg_and_bind(&send_cmd.sock, NULL, NULL, sport)) < 0) {
        mutex_unlock(&send_cmd.mutex);
        return res;
    }
    udp_socket_connect(&send_cmd.sock, (uip_ipaddr_t *)dst, dport); /* can't fail at this point */
    /* change to emb6 thread context */
    if (evproc_putEvent(E_EVPROC_TAIL, EVENT_TYPE_CONN_SEND, &send_cmd) != E_SUCCESS) {
        udp_socket_close(&send_cmd.sock);
        mutex_unlock(&send_cmd.mutex);
        return -EIO;
    }
    /* block thread until data was send */
    mutex_lock(&send_cmd.mutex);
    udp_socket_close(&send_cmd.sock);
    mutex_unlock(&send_cmd.mutex);

    return send_cmd.res;
}

static void _input_callback(struct udp_socket *c, void *ptr,
                            const uip_ipaddr_t *src_addr, uint16_t src_port,
                            const uip_ipaddr_t *dst_addr, uint16_t dst_port,
                            const uint8_t *data, uint16_t datalen)
{
    conn_udp_t *conn = ptr;

    (void)dst_addr;
    (void)dst_port;
    mutex_lock(&conn->mutex);
    if (conn->waiting_thread != KERNEL_PID_UNDEF) {
        msg_t msg;
        conn->recv_info.src_port = src_port;
        conn->recv_info.src = (const ipv6_addr_t *)src_addr;
        conn->recv_info.data = data;
        conn->recv_info.datalen = datalen - sizeof(ipv6_hdr_t);
        msg.type = _MSG_TYPE_RCV;
        msg.content.ptr = (char *)conn;
        mutex_unlock(&conn->mutex);
        msg_send(&msg, conn->waiting_thread);
    }
    else {
        mutex_unlock(&conn->mutex);
    }
}

static void _output_callback(c_event_t c_event, p_data_t p_data)
{
    _send_cmd_t *send_cmd = (_send_cmd_t *)p_data;

    if ((c_event != EVENT_TYPE_CONN_SEND) || (p_data == NULL)) {
        return;
    }
    if ((send_cmd->res = udp_socket_send(&send_cmd->sock, send_cmd->data, send_cmd->data_len)) < 0) {
        send_cmd->res = -EHOSTUNREACH;
    }
    mutex_unlock(&send_cmd->mutex);
}

/** @} */
