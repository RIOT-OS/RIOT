/*
 * Copyright (C) 2015 Freie Universität Berlin
 * Copyright (C) 2015 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @file
 * @brief   Providing implementation for POSIX socket wrapper.
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @todo
 */

#include <assert.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>

#include "bitfield.h"
#include "fd.h"
#include "mutex.h"
#include "net/ipv4/addr.h"
#include "net/ipv6/addr.h"
#include "random.h"

#include "sys/socket.h"
#include "netinet/in.h"

#include "net/sock/ip.h"
#include "net/sock/udp.h"
#include "net/sock/tcp.h"

/* enough to create sockets both with socket() and accept() */
#define _ACTUAL_SOCKET_POOL_SIZE   (SOCKET_POOL_SIZE + \
                                    (SOCKET_POOL_SIZE * SOCKET_TCP_QUEUE_SIZE))

/**
 * @brief   Unitfied connection type.
 */
typedef union {
    /* is not supposed to be used, this is only for the case that no
     * sock module was added (maybe useful for UNIX sockets?) */
    /* cppcheck-suppress unusedStructMember */
    int undef;
#ifdef MODULE_SOCK_IP
    sock_ip_t raw;              /**< raw IP sock */
#endif /* MODULE_SOCK_IP */
#ifdef MODULE_SOCK_TCP
    union {
        sock_tcp_t sock;        /**< TCP sock */
        sock_tcp_queue_t queue; /**< TCP queue */
    } tcp;                      /**< both TCP types */
#endif /* MODULE_SOCK_TCP */
#ifdef MODULE_SOCK_UDP
    sock_udp_t udp;             /**< UDP sock */
#endif /* MODULE_SOCK_UDP */
} socket_sock_t;

typedef struct {
    int fd;
    sa_family_t domain;
    int type;
    int protocol;
    bool bound;
    socket_sock_t *sock;
#ifdef MODULE_SOCK_TCP
    sock_tcp_t *queue_array;
    unsigned queue_array_len;
#endif
    sock_tcp_ep_t local;        /* to store bind before connect/listen */
} socket_t;

static socket_t _socket_pool[_ACTUAL_SOCKET_POOL_SIZE];
static socket_sock_t _sock_pool[SOCKET_POOL_SIZE];
#ifdef MODULE_SOCK_TCP
static sock_tcp_t _tcp_sock_pool[SOCKET_POOL_SIZE][SOCKET_TCP_QUEUE_SIZE];
#endif
BITFIELD(_sock_pool_used, SOCKET_POOL_SIZE);
static mutex_t _socket_pool_mutex = MUTEX_INIT;

const struct in6_addr in6addr_any = IN6ADDR_ANY_INIT;
const struct in6_addr in6addr_loopback = IN6ADDR_LOOPBACK_INIT;

static socket_t *_get_free_socket(void)
{
    for (int i = 0; i < _ACTUAL_SOCKET_POOL_SIZE; i++) {
        if (_socket_pool[i].domain == AF_UNSPEC) {
            return &_socket_pool[i];
        }
    }
    return NULL;
}

static socket_sock_t *_get_free_sock(void)
{
    int i = bf_get_unset(_sock_pool_used, SOCKET_POOL_SIZE);
    if (i < 0) {
        return NULL;
    }
    return &_sock_pool[i];
}

static socket_t *_get_socket(int fd)
{
    for (int i = 0; i < _ACTUAL_SOCKET_POOL_SIZE; i++) {
        if (_socket_pool[i].fd == fd) {
            return &_socket_pool[i];
        }
    }
    return NULL;
}

static int _get_sock_idx(socket_sock_t *sock)
{
    if ((sock < &_sock_pool[0]) || (sock > &_sock_pool[SOCKET_POOL_SIZE - 1])) {
        return -1;
    }
    return sock - &_sock_pool[0];
}

static inline int _choose_ipproto(int type, int protocol)
{
    switch (type) {
#ifdef MODULE_SOCK_TCP
        case SOCK_STREAM:
            if ((protocol == 0) || (protocol == IPPROTO_TCP)) {
                return protocol;
            }
            else {
                errno = EPROTOTYPE;
            }
            break;
#endif
#ifdef MODULE_SOCK_UDP
        case SOCK_DGRAM:
            if ((protocol == 0) || (protocol == IPPROTO_UDP)) {
                return protocol;
            }
            else {
                errno = EPROTOTYPE;
            }
            break;
#endif
#ifdef MODULE_SOCK_IP
        case SOCK_RAW:
            return protocol;
#endif
        default:
            (void)protocol;
            break;
    }
    errno = EPROTONOSUPPORT;
    return -1;
}

static inline socklen_t _addr_truncate(struct sockaddr *out, socklen_t out_len,
                                       struct sockaddr_storage *in,
                                       socklen_t target_size) {
    out_len = (out_len < target_size) ?  out_len : target_size;
    memcpy(out, in, out_len);
    return out_len;
}

static int _ep_to_sockaddr(const struct _sock_tl_ep *ep,
                           struct sockaddr_storage *out)
{
    assert((ep->family == AF_INET) || (ep->family == AF_INET6));
    switch (ep->family) {
        case AF_INET: {
            struct sockaddr_in *in_addr = (struct sockaddr_in *)out;

            in_addr->sin_family = AF_INET;
            in_addr->sin_addr.s_addr = ep->addr.ipv4_u32;
            in_addr->sin_port = htons(ep->port);
            return sizeof(struct sockaddr_in);
        }
#ifdef SOCK_HAS_IPV6
        case AF_INET6: {
            struct sockaddr_in6 *in6_addr = (struct sockaddr_in6 *)out;

            in6_addr->sin6_family = AF_INET6;
            memcpy(&in6_addr->sin6_addr, &ep->addr.ipv6, sizeof(ep->addr.ipv6));
            in6_addr->sin6_port = htons(ep->port);
            return sizeof(struct sockaddr_in6);
        }
#endif
        default:
            /* should not happen */
            return 0;
    }
}

static int _sockaddr_to_ep(const struct sockaddr *address, socklen_t address_len,
                           struct _sock_tl_ep *out)
{
    switch (address->sa_family) {
        case AF_INET:
            if (address_len < sizeof(struct sockaddr_in)) {
                errno = EINVAL;
                return -1;
            }
            struct sockaddr_in *in_addr = (struct sockaddr_in *)address;
            out->family = AF_INET;
            out->addr.ipv4_u32 = in_addr->sin_addr.s_addr;
            out->port = ntohs(in_addr->sin_port);
            break;
#ifdef SOCK_HAS_IPV6
        case AF_INET6:
            if (address_len < sizeof(struct sockaddr_in6)) {
                errno = EINVAL;
                return -1;
            }
            struct sockaddr_in6 *in6_addr = (struct sockaddr_in6 *)address;
            out->family = AF_INET6;
            memcpy(&out->addr.ipv6, &in6_addr->sin6_addr, sizeof(out->addr.ipv6));
            out->port = ntohs(in6_addr->sin6_port);
            break;
#endif
        default:
            errno = EAFNOSUPPORT;
            return -1;
    }
    return 0;
}

static int socket_close(int socket)
{
    socket_t *s;
    int res = 0;

    assert((unsigned)(socket - 1) > (_ACTUAL_SOCKET_POOL_SIZE - 1));
    s = &_socket_pool[socket];
    assert((s->domain == AF_INET) || (s->domain == AF_INET6));
    mutex_lock(&_socket_pool_mutex);
    if (s->sock != NULL) {
        int idx = _get_sock_idx(s->sock);
        switch (s->type) {
#ifdef MODULE_SOCK_UDP
            case SOCK_DGRAM:
                sock_udp_close(&s->sock->udp);
                break;
#endif
#ifdef MODULE_SOCK_IP
            case SOCK_RAW:
                sock_ip_close(&s->sock->raw);
                break;
#endif
#ifdef MODULE_SOCK_TCP
            case SOCK_STREAM:
                if (s->queue_array == 0) {
                    sock_tcp_disconnect(&s->sock->tcp.sock);
                }
                else {
                    sock_tcp_stop_listen(&s->sock->tcp.queue);
                }
                break;
#endif
            default:
                errno = EOPNOTSUPP;
                res = -1;
                break;
        }
        if (idx >= 0) {
            bf_unset(_sock_pool_used, idx);
        }
    }
    mutex_unlock(&_socket_pool_mutex);
    s->sock = NULL;
    s->domain = AF_UNSPEC;
    return res;
}

static ssize_t socket_read(int socket, void *buf, size_t n)
{
    return recv(socket, buf, n, 0);
}

static ssize_t socket_write(int socket, const void *buf, size_t n)
{
    return send(socket, buf, n, 0);
}

int socket(int domain, int type, int protocol)
{
    int res = 0;
    socket_t *s;

    mutex_lock(&_socket_pool_mutex);
    s = _get_free_socket();
    if (s == NULL) {
        errno = ENFILE;
        mutex_unlock(&_socket_pool_mutex);
        return -1;
    }
    switch (domain) {
        case AF_INET:
#ifdef SOCK_HAS_IPV6
        case AF_INET6:
#endif
        {
            int fd = fd_new(s - _socket_pool, socket_read, socket_write,
                            socket_close);

            if (fd < 0) {
                errno = ENFILE;
                res = -1;
                break;
            }
            else {
                s->fd = res = fd;
            }
            s->domain = domain;
            s->type = type;
            if ((s->protocol = _choose_ipproto(type, protocol)) < 0) {
                res = -1;
                break;
            }
            s->bound = false;
            s->sock = NULL;
#ifdef MODULE_SOCK_TCP
            if (type == SOCK_STREAM)  {
                s->queue_array = NULL;
                s->queue_array_len = 0;
                memset(&s->local, 0, sizeof(sock_tcp_ep_t));
            }
#endif
            break;
        }
        default:
            (void)type;
            (void)protocol;
            errno = EAFNOSUPPORT;
            res = -1;
    }
    mutex_unlock(&_socket_pool_mutex);
    return res;
}


int accept(int socket, struct sockaddr *restrict address,
           socklen_t *restrict address_len)
{
#ifdef MODULE_SOCK_TCP
    sock_tcp_t *sock = NULL;
    socket_t *s, *new_s = NULL;
    int res = 0;

    mutex_lock(&_socket_pool_mutex);
    s = _get_socket(socket);
    if (s == NULL) {
        mutex_unlock(&_socket_pool_mutex);
        errno = ENOTSOCK;
        return -1;
    }
    if (s->sock == NULL) {
        mutex_unlock(&_socket_pool_mutex);
        errno = EINVAL;
        return -1;
    }
    switch (s->type) {
        case SOCK_STREAM:
            new_s = _get_free_socket();
            sock = (sock_tcp_t *)new_s->sock;
            if (new_s == NULL) {
                errno = ENFILE;
                res = -1;
                break;
            }
            /* TODO: apply configured timeout */
            if ((res = sock_tcp_accept(&s->sock->tcp.queue, &sock,
                                       SOCK_NO_TIMEOUT)) < 0) {
                errno = -res;
                res = -1;
                break;
            }
            else {
                if ((address != NULL) && (address_len != NULL)) {
                    sock_tcp_ep_t ep;
                    struct sockaddr_storage sa;
                    socklen_t sa_len;

                    if ((res = sock_tcp_get_remote(sock, &ep)) < 0) {
                        errno = -res;
                        res = -1;
                        break;
                    }
                    sa.ss_family = s->domain;
                    sa_len = _ep_to_sockaddr(&ep, &sa);
                    *address_len = _addr_truncate(address, *address_len, &sa,
                                                  sa_len);

                }
                int fd = fd_new(new_s - _socket_pool, socket_read, socket_write,
                                socket_close);
                if (fd < 0) {
                    errno = ENFILE;
                    res = -1;
                    break;
                }
                else {
                    new_s->fd = res = fd;
                }
                new_s->domain = s->domain;
                new_s->type = s->type;
                new_s->protocol = s->protocol;
                new_s->bound = true;
                new_s->queue_array = NULL;
                new_s->queue_array_len = 0;
                memset(&s->local, 0, sizeof(sock_tcp_ep_t));
            }
            break;
        default:
            errno = EOPNOTSUPP;
            res = -1;
            break;
    }
    if ((res < 0) && (sock != NULL)) {
        sock_tcp_disconnect(sock);
    }
    mutex_unlock(&_socket_pool_mutex);
    return res;
#else
    (void)socket;
    (void)address;
    (void)address_len;
    errno = -EOPNOTSUPP;
    return -1;
#endif
}

int bind(int socket, const struct sockaddr *address, socklen_t address_len)
{
    socket_t *s;
    int res = 0;

    /* only store bind data, real bind happens in _bind_connect/listen */
    mutex_lock(&_socket_pool_mutex);
    s = _get_socket(socket);
    mutex_unlock(&_socket_pool_mutex);
    if (s == NULL) {
        errno = ENOTSOCK;
        return -1;
    }
    if (s->bound) {
        errno = EINVAL;
        return -1;
    }
    if (address->sa_family != s->domain) {
        errno = EAFNOSUPPORT;
        return -1;
    }
    switch (s->type) {
#ifdef MODULE_SOCK_IP
        case SOCK_RAW:
            break;
#endif
#ifdef MODULE_SOCK_TCP
        case SOCK_STREAM:
            break;
#endif
#ifdef MODULE_SOCK_UDP
        case SOCK_DGRAM:
            break;
#endif
        default:
            (void)res;
            errno = EOPNOTSUPP;
            return -1;
    }
    if (_sockaddr_to_ep(address, address_len, &s->local) < 0) {
        return -1;
    }
    s->bound = true;
    return 0;
}

static int _bind_connect(socket_t *s, const struct sockaddr *address,
                         socklen_t address_len)
{
    struct _sock_tl_ep r, *remote = NULL, *local = NULL;
    int res;
    socket_sock_t *sock;

    assert((s != NULL) && ((address == NULL) || (address_len > 0)));
    if (address != NULL) {
        if (address->sa_family != s->domain) {
            errno = EAFNOSUPPORT;
            return -1;
        }
        if (_sockaddr_to_ep(address, address_len, &r) < 0) {
            return -1;
        }
        remote = &r;
    }
    if (s->bound) {
        local = &s->local;
    }
    mutex_lock(&_socket_pool_mutex);
    sock = _get_free_sock();
    mutex_unlock(&_socket_pool_mutex);
    if (sock == NULL) {
        errno = ENOMEM;
        return -1;
    }
    switch (s->type) {
#ifdef MODULE_SOCK_IP
        case SOCK_RAW:
            /* TODO apply flags if possible */
            res = sock_ip_create(&sock->raw, (sock_ip_ep_t *)local,
                                 (sock_ip_ep_t *)remote, s->protocol, 0);
            break;
#endif
#ifdef MODULE_SOCK_TCP
        case SOCK_STREAM:
            /* TODO apply flags if possible */
            assert(remote != NULL);
            res = sock_tcp_connect(&sock->tcp.sock, remote,
                                   (local == NULL) ? 0 : local->port, 0);
            break;
#endif
#ifdef MODULE_SOCK_UDP
        case SOCK_DGRAM:
            /* TODO apply flags if possible */
            res = sock_udp_create(&sock->udp, local, remote, 0);
            break;
#endif
        default:
            (void)local;
            (void)remote;
            res = -EOPNOTSUPP;
            break;
    }
    if (res < 0) {
        errno = -res;
        /* free sock again */
        mutex_lock(&_socket_pool_mutex);
        bf_unset(_sock_pool_used, _get_sock_idx(sock));
        mutex_unlock(&_socket_pool_mutex);
        return -1;
    }
    s->sock = sock;
    return 0;
}

int connect(int socket, const struct sockaddr *address, socklen_t address_len)
{
    socket_t *s;

    mutex_lock(&_socket_pool_mutex);
    s = _get_socket(socket);
    mutex_unlock(&_socket_pool_mutex);
    if (s == NULL) {
        errno = ENOTSOCK;
        return -1;
    }
    if (s->sock != NULL) {
#ifdef MODULE_SOCK_TCP
        if (s->queue_array != NULL) {
            errno = EOPNOTSUPP;
        }
        else
#endif
        {
            errno = EISCONN;
        }
        return -1;
    }
    return _bind_connect(s, address, address_len);
}

static int _getpeername(socket_t *s, struct sockaddr *__restrict address,
                        socklen_t *__restrict address_len)
{
    struct _sock_tl_ep ep;
    int res = 0;

    if (s->sock == NULL) {
        errno = ENOTCONN;
        return -1;
    }
    switch (s->type) {
#ifdef MODULE_SOCK_IP
        case SOCK_RAW:
            res = sock_ip_get_remote(&s->sock->raw, (sock_ip_ep_t *)&ep);
            break;
#endif
#ifdef MODULE_SOCK_TCP
        case SOCK_STREAM:
            if (s->queue_array == NULL) {
                res = sock_tcp_get_remote(&s->sock->tcp.sock, &ep);
            }
            else {
                res = -ENOTCONN;
            }
            break;
#endif
#ifdef MODULE_SOCK_UDP
        case SOCK_DGRAM:
            res = sock_udp_get_remote(&s->sock->udp, &ep);
            break;
#endif
        default:
            res = -EOPNOTSUPP;
            break;
    }
    if (res < 0) {
        errno = -res;
        res = -1;
    }
    else {
        struct sockaddr_storage sa;
        socklen_t sa_len = _ep_to_sockaddr(&ep, &sa);
        *address_len = _addr_truncate(address, *address_len, &sa,
                                      sa_len);
    }
    return res;
}

int getpeername(int socket, struct sockaddr *__restrict address,
                socklen_t *__restrict address_len)
{
    socket_t *s;

    mutex_lock(&_socket_pool_mutex);
    s = _get_socket(socket);
    mutex_unlock(&_socket_pool_mutex);
    if (s == NULL) {
        errno = ENOTSOCK;
        return -1;
    }
    return _getpeername(s, address, address_len);
}

int getsockname(int socket, struct sockaddr *__restrict address,
                socklen_t *__restrict address_len)
{
    socket_t *s;
    struct sockaddr_storage sa;
    socklen_t sa_len;
    int res = 0;

    mutex_lock(&_socket_pool_mutex);
    s = _get_socket(socket);
    mutex_unlock(&_socket_pool_mutex);
    if (s == NULL) {
        errno = ENOTSOCK;
        return -1;
    }
    if (s->sock == NULL) {
        sa_len = _ep_to_sockaddr(&s->local, &sa);
    }
    else {
        struct _sock_tl_ep ep;
        switch (s->type) {
#ifdef MODULE_SOCK_IP
            case SOCK_RAW:
                res = sock_ip_get_local(&s->sock->raw, (sock_ip_ep_t *)&ep);
                break;
#endif
#ifdef MODULE_SOCK_TCP
            case SOCK_STREAM:
                if (s->queue_array == NULL) {
                    res = sock_tcp_get_local(&s->sock->tcp.sock, &ep);
                }
                else {
                    res = sock_tcp_queue_get_local(&s->sock->tcp.queue, &ep);
                }
                break;
#endif
#ifdef MODULE_SOCK_UDP
            case SOCK_DGRAM:
                res = sock_udp_get_local(&s->sock->udp, &ep);
                break;
#endif
            default:
                res = -EOPNOTSUPP;
                break;
        }
        sa_len = _ep_to_sockaddr(&ep, &sa);
    }
    if (res < 0) {
        errno = -res;
        res = -1;
    }
    else {
        *address_len = _addr_truncate(address, *address_len, &sa,
                                      sa_len);
    }
    return res;
}

int listen(int socket, int backlog)
{
#ifdef MODULE_SOCK_TCP
    socket_t *s;
    socket_sock_t *sock;
    int res = 0;

    mutex_lock(&_socket_pool_mutex);
    s = _get_socket(socket);
    if (s == NULL) {
        errno = ENOTSOCK;
        mutex_unlock(&_socket_pool_mutex);
        return -1;
    }
    if (s->sock != NULL) {
        /* or this socket is already connected, this is an error */
        if (s->queue_array == NULL) {
            errno = EINVAL;
            res = -1;
        }
        mutex_unlock(&_socket_pool_mutex);
        return res;
    }
    sock = _get_free_sock();
    mutex_unlock(&_socket_pool_mutex);
    if (sock == NULL) {
        errno = ENOMEM;
        return -1;
    }
    s->queue_array = _tcp_sock_pool[_get_sock_idx(sock)];
    s->queue_array_len = (backlog < SOCKET_TCP_QUEUE_SIZE) ? backlog :
                         SOCKET_TCP_QUEUE_SIZE;
    switch (s->type) {
        case SOCK_STREAM:
            if (s->bound) {
                /* TODO apply flags if possible */
                res = sock_tcp_listen(&sock->tcp.queue, &s->local,
                                      s->queue_array, s->queue_array_len, 0);
            }
            else {
                res = -EDESTADDRREQ;
            }
            break;
        default:
            res = -EOPNOTSUPP;
            break;
    }
    if (res == 0) {
        s->sock = sock;
    }
    else {
        errno = -res;
        res = -1;
        mutex_lock(&_socket_pool_mutex);
        bf_unset(_sock_pool_used, _get_sock_idx(sock));
        mutex_unlock(&_socket_pool_mutex);
    }
    return -res;
#else
    (void)socket;
    (void)backlog;
    errno = EOPNOTSUPP;
    return -1;
#endif
}

ssize_t recvfrom(int socket, void *restrict buffer, size_t length, int flags,
                 struct sockaddr *restrict address,
                 socklen_t *restrict address_len)
{
    socket_t *s;
    int res = 0;
    struct _sock_tl_ep ep = { .port = 0 };

    (void)flags;
    mutex_lock(&_socket_pool_mutex);
    s = _get_socket(socket);
    mutex_unlock(&_socket_pool_mutex);
    if (s == NULL) {
        errno = ENOTSOCK;
        return -1;
    }
    if (s->sock == NULL) {  /* socket is not connected */
#ifdef MODULE_SOCK_TCP
        if (s->type == SOCK_STREAM) {
            errno = ENOTCONN;
            return -1;
        }
#endif
        /* bind implicitly */
        if ((res = _bind_connect(s, NULL, 0)) < 0) {
            errno = -res;
            return -1;
        }
    }
    switch (s->type) {
#ifdef MODULE_SOCK_IP
        case SOCK_RAW:
            /* TODO: apply configured timeout */
            if ((res = sock_ip_recv(&s->sock->raw, buffer, length, SOCK_NO_TIMEOUT,
                               (sock_ip_ep_t *)&ep)) < 0) {
                errno = -res;
                res = -1;
            }
            break;
#endif
#ifdef MODULE_SOCK_TCP
        case SOCK_STREAM:
            /* TODO: apply configured timeout */
            if ((res = sock_tcp_read(&s->sock->tcp.sock, buffer, length,
                                SOCK_NO_TIMEOUT)) < 0) {
                errno = -res;
                res = -1;
            }
            break;
#endif
#ifdef MODULE_SOCK_UDP
        case SOCK_DGRAM:
            /* TODO: apply configured timeout */
            if ((res = sock_udp_recv(&s->sock->udp, buffer, length, 0,
                                &ep)) < 0) {
                errno = -res;
                res = -1;
            }
            break;
#endif
        default:
            errno = EOPNOTSUPP;
            res = -1;
            break;
    }
    if ((res == 0) && (address != NULL) && (address_len != 0)) {
        switch (s->type) {
#ifdef MODULE_SOCK_TCP
            case SOCK_STREAM:
                res = _getpeername(s, address, address_len);
                break;
#endif
            default: {
                struct sockaddr_storage sa;
                socklen_t sa_len;

                sa_len = _ep_to_sockaddr(&ep, &sa);
                *address_len = _addr_truncate(address, *address_len, &sa,
                                              sa_len);
                break;
            }
        }
    }
    return res;
}

ssize_t sendto(int socket, const void *buffer, size_t length, int flags,
               const struct sockaddr *address, socklen_t address_len)
{
    socket_t *s;
    int res = 0;
#if defined(MODULE_SOCK_IP) || defined(MODULE_SOCK_UDP)
    struct _sock_tl_ep ep = { .port = 0 };
#endif

    (void)flags;
    mutex_lock(&_socket_pool_mutex);
    s = _get_socket(socket);
    mutex_unlock(&_socket_pool_mutex);
    if (s == NULL) {
        errno = ENOTSOCK;
        return -1;
    }
    if (s->sock == NULL) {  /* socket is not connected */
#ifdef MODULE_SOCK_TCP
        if (s->type == SOCK_STREAM) {
            errno = ENOTCONN;
            return -1;
        }
#endif
        /* bind implicitly */
        if ((res = _bind_connect(s, NULL, 0)) < 0) {
            errno = -res;
            return -1;
        }
    }
#if defined(MODULE_SOCK_IP) || defined(MODULE_SOCK_UDP)
    if ((res = _sockaddr_to_ep(address, address_len, &ep)) < 0)
        return res;
#endif
    switch (s->type) {
#ifdef MODULE_SOCK_IP
        case SOCK_RAW:
            if ((res = sock_ip_send(&s->sock->raw, buffer, length,
                               s->protocol, (sock_ip_ep_t *)&ep)) < 0) {
                errno = -res;
                res = -1;
            }
            break;
#endif
#ifdef MODULE_SOCK_TCP
        case SOCK_STREAM:
            if (address == NULL) {
                (void)address_len;
                if ((res = sock_tcp_write(&s->sock->tcp.sock, buffer, length)) < 0) {
                    errno = -res;
                    res = -1;
                }
            }
            else {
                res = -1;
                errno = EISCONN;
            }
            break;
#endif
#ifdef MODULE_SOCK_UDP
        case SOCK_DGRAM:
            if ((res = sock_udp_send(&s->sock->udp, buffer, length, &ep)) < 0) {
                errno = -res;
                res = -1;
            }
            break;
#endif
        default:
            res = -1;
            errno = EOPNOTSUPP;
            break;
    }
    return res;
}

/**
 * @}
 */
