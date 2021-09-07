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
#include <fcntl.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <string.h>

#include "bitfield.h"
#include "mutex.h"
#include "net/ipv4/addr.h"
#include "net/ipv6/addr.h"
#include "random.h"
#include "vfs.h"

#include "sys/socket.h"
#include "netinet/in.h"

#include "net/sock/ip.h"
#include "net/sock/udp.h"
#include "net/sock/tcp.h"

#if IS_USED(MODULE_SOCK_ASYNC)
#include "net/sock/async.h"
#endif
#if IS_USED(MODULE_POSIX_SELECT)
#include <sys/select.h>

#include "thread.h"
#include "thread_flags.h"
#endif

/* enough to create sockets both with socket() and accept() */
#define _ACTUAL_SOCKET_POOL_SIZE   (SOCKET_POOL_SIZE + \
                                    (SOCKET_POOL_SIZE * SOCKET_TCP_QUEUE_SIZE))
#define SOCKET_BLKSIZE             (512)

/**
 * @brief   Unitfied connection type.
 */
typedef union {
    /* is not supposed to be used, this is only for the case that no
     * sock module was added (maybe useful for UNIX sockets?) */
    /* cppcheck-suppress unusedStructMember
     * (reason: is not supposed to be used) */
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
#ifdef POSIX_SETSOCKOPT
    uint32_t recv_timeout;
#endif
    socket_sock_t *sock;
#ifdef MODULE_SOCK_TCP
    sock_tcp_t *queue_array;
    unsigned queue_array_len;
#endif
#if IS_USED(MODULE_SOCK_ASYNC)
    atomic_uint available;
#endif
#if IS_USED(MODULE_POSIX_SELECT)
    thread_t *selecting_thread;
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

static ssize_t socket_recvfrom(socket_t *s, void *restrict buffer,
                               size_t length, int flags,
                               struct sockaddr *restrict address,
                               socklen_t *restrict address_len);
static ssize_t socket_sendto(socket_t *s, const void *buffer, size_t length,
                             int flags, const struct sockaddr *address,
                             socklen_t address_len);

static socket_t *_get_free_socket(void)
{
    for (int i = 0; i < _ACTUAL_SOCKET_POOL_SIZE; i++) {
        if (_socket_pool[i].domain == AF_UNSPEC) {
#if IS_USED(MODULE_SOCK_ASYNC)
            atomic_init(&_socket_pool[i].available, 0U);
#endif
#if IS_USED(MODULE_POSIX_SELECT)
            _socket_pool[i].selecting_thread = NULL;
#endif
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
    const vfs_file_t *file = vfs_file_get(fd);
    /* we know what to do with `socket`, so it's okay to discard the const */
    socket_t *socket = (file == NULL)
                     ? NULL
                     : file->private_data.ptr;
    if ((socket >= &_socket_pool[0]) &&
        (socket <= &_socket_pool[_ACTUAL_SOCKET_POOL_SIZE - 1])) {
        assert(socket->fd == fd);
        return socket;
    }
    else {
        return NULL;
    }
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
    assert(address != NULL);

    switch (address->sa_family) {
        case AF_INET:
            if (address_len < sizeof(struct sockaddr_in)) {
                errno = EINVAL;
                return -1;
            }
            struct sockaddr_in *in_addr = (struct sockaddr_in *)address;
            memset(out, 0, sizeof(*out));
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
            memset(out, 0, sizeof(*out));
            out->family = AF_INET6;
            memcpy(&out->addr.ipv6, &in6_addr->sin6_addr, sizeof(out->addr.ipv6));
            out->port = ntohs(in6_addr->sin6_port);
            if (in6_addr->sin6_scope_id != 0) {
                out->netif = (uint16_t) in6_addr->sin6_scope_id;
            }
            break;
#endif
        default:
            errno = EAFNOSUPPORT;
            return -1;
    }
    return 0;
}

static int socket_close(vfs_file_t *filp)
{
    socket_t *s = filp->private_data.ptr;
    int res = 0;

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
                if (s->queue_array == NULL) {
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

static inline int socket_fstat(vfs_file_t *filp, struct stat *buf)
{
    (void)filp;
    memset(buf, 0, sizeof(struct stat));
    buf->st_mode |= (S_IFSOCK | S_IRWXU | S_IRWXG | S_IRWXO);
    buf->st_blksize = SOCKET_BLKSIZE;
    return 0;
}

static inline off_t socket_lseek(vfs_file_t *filp, off_t off, int whence)
{
    (void)filp;
    (void)off;
    (void)whence;
    return -ESPIPE; /* see http://pubs.opengroup.org/onlinepubs/9699919799/functions/lseek.html */
}

static inline ssize_t socket_read(vfs_file_t *filp, void *buf, size_t n)
{
    return socket_recvfrom(filp->private_data.ptr, buf, n, 0, NULL, NULL);
}

static inline ssize_t socket_write(vfs_file_t *filp, const void *buf, size_t n)
{
    return socket_sendto(filp->private_data.ptr, buf, n, 0, NULL, 0);
}

static const vfs_file_ops_t socket_ops = {
    .close = socket_close,
    .fcntl = NULL,          /* TODO: provide when needed */
    .fstat = socket_fstat,
    .lseek = socket_lseek,
    .read = socket_read,
    .write = socket_write,
};

#if IS_USED(MODULE_SOCK_ASYNC)
static void _async_cb(void *sock, sock_async_flags_t type,
                      void *arg)
{
    socket_t *socket = arg;

    (void)sock;
    if (type & SOCK_ASYNC_MSG_RECV) {
        atomic_fetch_add(&socket->available, 1);
#if IS_USED(MODULE_POSIX_SELECT)
        if (socket->selecting_thread) {
            thread_flags_set(socket->selecting_thread,
                             POSIX_SELECT_THREAD_FLAG);
        }
#endif
    }
}

static void _sock_set_cb(socket_t *socket)
{
    union {
        void (*sock_pool)(void *, sock_async_flags_t, void *);
#ifdef MODULE_SOCK_IP
        sock_ip_cb_t ip;
#endif
#ifdef MODULE_SOCK_TCP
        sock_tcp_cb_t tcp;
        sock_tcp_queue_cb_t tcp_queue;
#endif
#ifdef MODULE_SOCK_UDP
        sock_udp_cb_t udp;
#endif
    } callback = { .sock_pool = _async_cb };

    switch (socket->type) {
#ifdef MODULE_SOCK_IP
        case SOCK_RAW:
            sock_ip_set_cb(&socket->sock->raw, callback.ip, socket);
            break;
#endif
#ifdef MODULE_SOCK_TCP
        case SOCK_STREAM:
            /* is a TCP client socket */
            if (socket->queue_array == NULL) {
                sock_tcp_set_cb(&socket->sock->tcp.sock, callback.tcp, socket);
            }
            /* is a TCP listening socket */
            else {
                sock_tcp_queue_set_cb(&socket->sock->tcp.queue,
                                      callback.tcp_queue, socket);
            }
            break;
#endif
#ifdef MODULE_SOCK_UDP
        case SOCK_DGRAM:
            sock_udp_set_cb(&socket->sock->udp, callback.udp, socket);
            break;
#endif
        default:
            break;
    }
}
#endif

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
            int fd = vfs_bind(VFS_ANY_FD, O_RDWR, &socket_ops, s);

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
#ifdef POSIX_SETSOCKOPT
            s->recv_timeout = SOCK_NO_TIMEOUT;
#endif
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

#ifdef POSIX_SETSOCKOPT
    const uint32_t recv_timeout = s->recv_timeout;
#else
    const uint32_t recv_timeout = SOCK_NO_TIMEOUT;
#endif

    switch (s->type) {
        case SOCK_STREAM:
            new_s = _get_free_socket();
            if (new_s == NULL) {
                errno = ENFILE;
                res = -1;
                break;
            }
            sock = (sock_tcp_t *)new_s->sock;
            if ((res = sock_tcp_accept(&s->sock->tcp.queue, &sock,
                                       recv_timeout)) < 0) {
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
                int fd = vfs_bind(VFS_ANY_FD, O_RDWR, &socket_ops, new_s);
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
                new_s->sock = (socket_sock_t *)sock;
#if IS_USED(MODULE_SOCK_ASYNC)
                _sock_set_cb(new_s);
#endif
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
#if IS_USED(MODULE_SOCK_ASYNC)
    _sock_set_cb(s);
#endif

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
        return -ENOTCONN;
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
    if (res >= 0) {
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
    int res;

    mutex_lock(&_socket_pool_mutex);
    s = _get_socket(socket);
    mutex_unlock(&_socket_pool_mutex);
    if (s == NULL) {
        errno = ENOTSOCK;
        return -1;
    }
    if ((res = _getpeername(s, address, address_len)) < 0) {
        errno = -res;
        return -1;
    }
    return res;
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
#if IS_USED(MODULE_SOCK_ASYNC)
        _sock_set_cb(s);
#endif
    }
    else {
        errno = -res;
        res = -1;
        mutex_lock(&_socket_pool_mutex);
        bf_unset(_sock_pool_used, _get_sock_idx(sock));
        mutex_unlock(&_socket_pool_mutex);
    }
    return res;
#else
    (void)socket;
    (void)backlog;
    errno = EOPNOTSUPP;
    return -1;
#endif
}

static ssize_t socket_recvfrom(socket_t *s, void *restrict buffer,
                               size_t length, int flags,
                               struct sockaddr *restrict address,
                               socklen_t *restrict address_len)
{
    int res = 0;
    struct _sock_tl_ep ep = { .port = 0 };

    (void)flags;
    if (s == NULL) {
        return -ENOTSOCK;
    }
    if (s->sock == NULL) {  /* socket is not connected */
#ifdef MODULE_SOCK_TCP
        if (s->type == SOCK_STREAM) {
            return -ENOTCONN;
        }
#endif
        /* bind implicitly */
        if ((res = _bind_connect(s, NULL, 0)) < 0) {
            return res;
        }
    }

#ifdef POSIX_SETSOCKOPT
    const uint32_t recv_timeout = s->recv_timeout;
#else
    const uint32_t recv_timeout = SOCK_NO_TIMEOUT;
#endif

    switch (s->type) {
#ifdef MODULE_SOCK_IP
        case SOCK_RAW:
            res = sock_ip_recv(&s->sock->raw, buffer, length, recv_timeout,
                               (sock_ip_ep_t *)&ep);
            break;
#endif
#ifdef MODULE_SOCK_TCP
        case SOCK_STREAM:
            res = sock_tcp_read(&s->sock->tcp.sock, buffer, length,
                                recv_timeout);
            break;
#endif
#ifdef MODULE_SOCK_UDP
        case SOCK_DGRAM:
            res = sock_udp_recv(&s->sock->udp, buffer, length, recv_timeout,
                                &ep);
            break;
#endif
        default:
#if !defined(MODULE_SOCK_IP) && !defined(MODULE_SOCK_TCP) && !defined(MODULE_SOCK_UDP)
            (void) recv_timeout;
#endif
            res = -EOPNOTSUPP;
            break;
    }
    if ((res >= 0) && (address != NULL) && (address_len != NULL)) {
#ifdef MODULE_SOCK_ASYNC
        atomic_fetch_sub(&s->available, 1);
#endif
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

ssize_t recvfrom(int socket, void *restrict buffer, size_t length, int flags,
                 struct sockaddr *restrict address,
                 socklen_t *restrict address_len)
{
    socket_t *s;
    int res;

    mutex_lock(&_socket_pool_mutex);
    s = _get_socket(socket);
    mutex_unlock(&_socket_pool_mutex);
    res = socket_recvfrom(s, buffer, length, flags, address, address_len);
    if (res < 0) {
        errno = -res;
        return -1;
    }
    return res;
}

static ssize_t socket_sendto(socket_t *s, const void *buffer, size_t length,
                             int flags, const struct sockaddr *address,
                             socklen_t address_len)
{
    int res = 0;
#if defined(MODULE_SOCK_IP) || defined(MODULE_SOCK_UDP)
    struct _sock_tl_ep ep = { .port = 0 };
#endif

    (void)flags;
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
            return res;
        }
    }
#if defined(MODULE_SOCK_IP)
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
            if (address == NULL) {
                res = sock_udp_get_remote(&s->sock->udp, &ep);
            } else {
                res = _sockaddr_to_ep(address, address_len, &ep);
            }
            if ((res < 0) ||
                (res = sock_udp_send(&s->sock->udp, buffer, length, &ep)) < 0) {
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

ssize_t sendto(int socket, const void *buffer, size_t length, int flags,
               const struct sockaddr *address, socklen_t address_len)
{
    socket_t *s;
    int res;

    mutex_lock(&_socket_pool_mutex);
    s = _get_socket(socket);
    mutex_unlock(&_socket_pool_mutex);
    res = socket_sendto(s, buffer, length, flags, address, address_len);
    if (res < 0) {
        errno = -res;
        return -1;
    }
    return res;
}

/*
 * This is a partial implementation of setsockopt for changing the receive
 * timeout value of a socket.
 */
int setsockopt(int socket, int level, int option_name, const void *option_value,
               socklen_t option_len)
{
#ifdef POSIX_SETSOCKOPT
    socket_t *s;
    struct timeval *tv;
    const uint32_t max_timeout_secs = UINT32_MAX / (1000 * 1000);

    if (level != SOL_SOCKET
    ||  option_name != SO_RCVTIMEO) {
        errno = ENOTSUP;
        return -1;
    }
    if (option_value == NULL
    ||  option_len != sizeof(struct timeval)) {
        errno = EINVAL;
        return -1;
    }

    mutex_lock(&_socket_pool_mutex);
    s = _get_socket(socket);
    mutex_unlock(&_socket_pool_mutex);
    if (s == NULL) {
        errno = ENOTSOCK;
        return -1;
    }

    tv = (struct timeval *) option_value;

    if (tv->tv_sec < 0 || tv->tv_usec < 0) {
        errno = EINVAL;
        return -1;
    }

    if ((uint32_t)tv->tv_sec > max_timeout_secs
    || ((uint32_t)tv->tv_sec == max_timeout_secs && (uint32_t)tv->tv_usec > UINT32_MAX - max_timeout_secs * 1000 * 1000)) {
        errno = EDOM;
        return -1;
    }

    s->recv_timeout = tv->tv_sec * 1000 * 1000 + tv->tv_usec;
    return 0;
#else
    (void)socket;
    (void)level;
    (void)option_name;
    (void)option_value;
    (void)option_len;
    return -1;
#endif
}

bool posix_socket_is(int fd)
{
    return IS_USED(MODULE_SOCK_ASYNC) && (_get_socket(fd) != NULL);
}

unsigned posix_socket_avail(int fd)
{
#if IS_USED(MODULE_SOCK_ASYNC)
    socket_t *socket = _get_socket(fd);

    return (socket != NULL) ? atomic_load(&socket->available) : 0U;
#else
    (void)fd;
    return 0U;
#endif
}

int posix_socket_select(int fd)
{
#if IS_USED(MODULE_POSIX_SELECT)
    socket_t *socket = _get_socket(fd);

    if (socket != NULL) {
        if (socket->sock == NULL) {  /* socket is not connected */
            int res;

            /* bind implicitly */
            if ((res = _bind_connect(socket, NULL, 0)) < 0) {
                return res;
            }
        }
        socket->selecting_thread = thread_get_active();
        return 0;
    }
#else
    (void)fd;
#endif
    errno = ENOTSUP;
    return -1;
}

/**
 * @}
 */
