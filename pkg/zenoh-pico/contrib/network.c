/*
 * SPDX-FileCopyrightText: 2026 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     pkg_zenoh-pico
 * @{
 *
 * @file
 * @brief       zenoh-pico network layer for RIOT, TCP links over sock_tcp
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "irq.h"
#include "net/ipv6/addr.h"
#include "timex.h"

#include "zenoh-pico/config.h"
#include "zenoh-pico/link/transport/socket.h"
#include "zenoh-pico/link/transport/tcp.h"
#include "zenoh-pico/system/platform.h"
#include "zenoh-pico/utils/logging.h"

/*------------------ Socket ------------------*/
z_result_t _z_socket_set_blocking(const _z_sys_net_socket_t *sock, bool blocking)
{
    /* reads always use the socket timeout */
    (void)sock;
    (void)blocking;
    return _Z_RES_OK;
}

z_result_t _z_socket_wait_readable(_z_socket_wait_iter_t *iter, uint32_t timeout_ms)
{
    /* sock has no readiness notification without the sock_async module: report
     * every socket as ready, the following reads use the socket timeout */
    (void)timeout_ms;

    _z_socket_wait_iter_reset(iter);
    while (_z_socket_wait_iter_next(iter)) {
        _z_socket_wait_iter_set_ready(iter, true);
    }
    return _Z_RES_OK;
}

void _z_socket_close(_z_sys_net_socket_t *sock)
{
#if Z_FEATURE_LINK_TCP == 1
    _z_tcp_close(sock);
#else
    (void)sock;
#endif
}

#if Z_FEATURE_LINK_TCP == 1
/*------------------ TCP ------------------*/
z_result_t _z_tcp_endpoint_init(_z_sys_net_endpoint_t *ep, const char *address, const char *port)
{
    char addr[IPV6_ADDR_MAX_STR_LEN + 8];
    size_t len = strlen(address);

    /* accept the address with or without the brackets */
    if ((len > 1) && (address[0] == '[') && (address[len - 1] == ']')) {
        address++;
        len -= 2;
    }
    if (len >= sizeof(addr)) {
        _Z_ERROR_RETURN(_Z_ERR_CONFIG_LOCATOR_INVALID);
    }
    memcpy(addr, address, len);
    addr[len] = '\0';

    memset(&ep->_ep, 0, sizeof(ep->_ep));
    ep->_ep.family = AF_INET6;

    /* optional interface of a link local address: fe80::1%5 */
    char *iface = strchr(addr, '%');

    if (iface) {
        *iface++ = '\0';
        ep->_ep.netif = atoi(iface);
    }
    if (ipv6_addr_from_str((ipv6_addr_t *)ep->_ep.addr.ipv6, addr) == NULL) {
        _Z_ERROR_RETURN(_Z_ERR_CONFIG_LOCATOR_INVALID);
    }
    ep->_ep.port = atoi(port);

    return _Z_RES_OK;
}

void _z_tcp_endpoint_clear(_z_sys_net_endpoint_t *ep)
{
    (void)ep;
}

z_result_t _z_tcp_open(_z_sys_net_socket_t *sock, const _z_sys_net_endpoint_t endpoint,
                       uint32_t tout)
{
    memset(sock, 0, sizeof(*sock));
    sock->_tcp = z_malloc(sizeof(sock_tcp_t));
    if (sock->_tcp == NULL) {
        _Z_ERROR_RETURN(_Z_ERR_SYSTEM_OUT_OF_MEMORY);
    }

    int res = sock_tcp_connect(sock->_tcp, &endpoint._ep, 0, 0);

    if (res < 0) {
        _Z_ERROR("sock_tcp_connect failed: %d", res);
        z_free(sock->_tcp);
        sock->_tcp = NULL;
        _Z_ERROR_RETURN(_Z_ERR_GENERIC);
    }
    sock->_timeout_us = tout * US_PER_MS;

    return _Z_RES_OK;
}

/* drops a reference to the listening queue, freed after the last one */
static void _listener_release(_z_tcp_listener_t *listener)
{
    unsigned state = irq_disable();
    bool last = (--listener->refs == 0);

    irq_restore(state);
    if (last) {
        z_free(listener);
    }
}

z_result_t _z_tcp_listen(_z_sys_net_socket_t *sock, const _z_sys_net_endpoint_t endpoint)
{
    memset(sock, 0, sizeof(*sock));

    _z_tcp_listener_t *listener = z_malloc(sizeof(*listener));

    if (listener == NULL) {
        _Z_ERROR_RETURN(_Z_ERR_SYSTEM_OUT_OF_MEMORY);
    }
    /* a single connection can be accepted */
    if (sock_tcp_listen(&listener->queue, &endpoint._ep, &listener->sock, 1, 0) < 0) {
        z_free(listener);
        _Z_ERROR_RETURN(_Z_ERR_GENERIC);
    }
    listener->refs = 1;
    sock->_listener = listener;
    sock->_timeout_us = Z_CONFIG_SOCKET_TIMEOUT * US_PER_MS;

    return _Z_RES_OK;
}

z_result_t _z_tcp_accept(const _z_sys_net_socket_t *sock_in, _z_sys_net_socket_t *sock_out)
{
    _z_tcp_listener_t *listener = sock_in->_listener;
    sock_tcp_t *sock;

    memset(sock_out, 0, sizeof(*sock_out));
    if (sock_tcp_accept(&listener->queue, &sock, Z_TRANSPORT_ACCEPT_TIMEOUT * US_PER_MS) < 0) {
        _Z_ERROR_RETURN(_Z_ERR_GENERIC);
    }
    /* the accepted sock is the one of the queue: the queue outlives it */
    unsigned state = irq_disable();

    listener->refs++;
    irq_restore(state);
    sock_out->_tcp = sock;
    sock_out->_listener = listener;
    sock_out->_timeout_us = sock_in->_timeout_us;

    return _Z_RES_OK;
}

void _z_tcp_close(_z_sys_net_socket_t *sock)
{
    if (sock->_listener) {
        if (sock->_tcp) {
            /* accepted connection: its sock listens again */
            sock_tcp_disconnect(sock->_tcp);
        }
        else {
            /* listening socket: GNRC closes the accepted connection too */
            sock_tcp_stop_listen(&sock->_listener->queue);
        }
        _listener_release(sock->_listener);
    }
    else if (sock->_tcp) {
        sock_tcp_disconnect(sock->_tcp);
        z_free(sock->_tcp);
    }
    sock->_listener = NULL;
    sock->_tcp = NULL;
}

size_t _z_tcp_read(_z_sys_net_socket_t sock, uint8_t *ptr, size_t len)
{
    ssize_t res = sock_tcp_read(sock._tcp, ptr, len, sock._timeout_us);

    /* timeouts are reported as errors, as by recv() with SO_RCVTIMEO */
    return (res < 0) ? SIZE_MAX : (size_t)res;
}

size_t _z_tcp_read_exact(_z_sys_net_socket_t sock, uint8_t *ptr, size_t len)
{
    size_t n = 0;

    while (n < len) {
        size_t res = _z_tcp_read(sock, &ptr[n], len - n);

        if ((res == SIZE_MAX) || (res == 0)) {
            return res;
        }
        n += res;
    }
    return n;
}

size_t _z_tcp_write(_z_sys_net_socket_t sock, const uint8_t *ptr, size_t len)
{
    size_t n = 0;

    while (n < len) {
        ssize_t res = sock_tcp_write(sock._tcp, &ptr[n], len - n);

        if (res <= 0) {
            return SIZE_MAX;
        }
        n += res;
    }
    return n;
}
#endif /* Z_FEATURE_LINK_TCP == 1 */
