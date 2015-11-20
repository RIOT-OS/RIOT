/*
 * Copyright (C) 2015 Freie Universität Berlin
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
 * @todo
 */

#include <arpa/inet.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>

#include "fd.h"
#include "mutex.h"
#include "net/conn.h"
#include "net/ipv4/addr.h"
#include "net/ipv6/addr.h"
#include "random.h"

#include "sys/socket.h"
#include "netinet/in.h"

#define SOCKET_POOL_SIZE    (4)

/**
 * @brief   Unitfied connection type.
 */
typedef union {
    /* is not supposed to be used */
    /* cppcheck-suppress unusedStructMember */
    int undef;                  /**< for case that no connection module is present */
#ifdef  MODULE_CONN_IP
    conn_ip_t raw;              /**< raw IP connection */
#endif  /* MODULE_CONN_IP */
#ifdef  MODULE_CONN_TCP
    conn_tcp_t tcp;             /**< TCP connection */
#endif  /* MODULE_CONN_TCP */
#ifdef  MODULE_CONN_UDP
    conn_udp_t udp;             /**< UDP connection */
#endif  /* MODULE_CONN_UDP */
} socket_conn_t;

typedef struct {
    int fd;
    sa_family_t domain;
    int type;
    int protocol;
    bool bound;
    socket_conn_t conn;
    uint16_t src_port;
} socket_t;

socket_t _pool[SOCKET_POOL_SIZE];
mutex_t _pool_mutex = MUTEX_INIT;

const struct in6_addr in6addr_any = IN6ADDR_ANY_INIT;
const struct in6_addr in6addr_loopback = IN6ADDR_LOOPBACK_INIT;

static socket_t *_get_free_socket(void)
{
    for (int i = 0; i < SOCKET_POOL_SIZE; i++) {
        if (_pool[i].domain == AF_UNSPEC) {
            return &_pool[i];
        }
    }
    return NULL;
}

static socket_t *_get_socket(int fd)
{
    for (int i = 0; i < SOCKET_POOL_SIZE; i++) {
        if (_pool[i].fd == fd) {
            return &_pool[i];
        }
    }
    return NULL;
}

static inline int _choose_ipproto(int type, int protocol)
{
    switch (type) {
#ifdef MODULE_CONN_TCP
        case SOCK_STREAM:
            if ((protocol == 0) || (protocol == IPPROTO_TCP)) {
                return protocol;
            }
            else {
                errno = EPROTOTYPE;
            }
            break;
#endif
#ifdef MODULE_CONN_UDP
        case SOCK_DGRAM:
            if ((protocol == 0) || (protocol == IPPROTO_UDP)) {
                return protocol;
            }
            else {
                errno = EPROTOTYPE;
            }
            break;
#endif
#ifdef MODULE_CONN_IP
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

static inline void _htons_port(uint16_t *port)
{
    *port = htons(*port);
}

static inline ipv4_addr_t *_in_addr_ptr(struct sockaddr_storage *addr)
{
    return (ipv4_addr_t *)(&((struct sockaddr_in *)addr)->sin_addr);
}

static inline uint16_t *_in_port_ptr(struct sockaddr_storage *addr)
{
    return &((struct sockaddr_in *)addr)->sin_port;
}

static inline ipv6_addr_t *_in6_addr_ptr(struct sockaddr_storage *addr)
{
    return (ipv6_addr_t *)(&((struct sockaddr_in6 *)addr)->sin6_addr);
}

static inline uint16_t *_in6_port_ptr(struct sockaddr_storage *addr)
{
    return &((struct sockaddr_in6 *)addr)->sin6_port;
}

static inline socklen_t _addr_truncate(struct sockaddr *out, socklen_t out_len,
                                       struct sockaddr_storage *in, socklen_t target_size)
{
    out_len = (out_len < target_size) ? out_len : target_size;
    memcpy(out, in, out_len);
    return out_len;
}

static inline int _get_data_from_sockaddr(const struct sockaddr *address, size_t address_len,
                                          void **addr, size_t *addr_len, uint16_t *port)
{
    switch (address->sa_family) {
        case AF_INET:
            if (address_len < sizeof(struct sockaddr_in)) {
                errno = EINVAL;
                return -1;
            }
            struct sockaddr_in *in_addr = (struct sockaddr_in *)address;
            *addr = &in_addr->sin_addr;
            *addr_len = sizeof(ipv4_addr_t);
            /* XXX sin_port is in network byteorder */
            *port = ntohs(in_addr->sin_port);
            break;
        case AF_INET6:
            if (address_len < sizeof(struct sockaddr_in6)) {
                errno = EINVAL;
                return -1;
            }
            struct sockaddr_in6 *in6_addr = (struct sockaddr_in6 *)address;
            *addr = &in6_addr->sin6_addr;
            *addr_len = sizeof(ipv6_addr_t);
            /* XXX sin6_port is in network byteorder */
            *port = ntohs(in6_addr->sin6_port);
            break;
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
    if ((unsigned)(socket - 1) > (SOCKET_POOL_SIZE - 1)) {
        return -1;
    }
    mutex_lock(&_pool_mutex);
    s = &_pool[socket];
    if (s->bound) {
        switch (s->domain) {
            case AF_INET:
            case AF_INET6:
                switch (s->type) {
#ifdef MODULE_CONN_UDP
                    case SOCK_DGRAM:
                        conn_udp_close(&s->conn.udp);
                        break;
#endif
#ifdef MODULE_CONN_IP
                    case SOCK_RAW:
                        conn_ip_close(&s->conn.raw);
                        break;
#endif
#ifdef MODULE_CONN_TCP
                    case SOCK_STREAM:
                        conn_tcp_close(&s->conn.tcp);
                        break;
#endif
                    default:
                        errno = EOPNOTSUPP;
                        res = -1;
                        break;
                }
            default:
                res = -1;
                break;
        }
    }
    s->domain = AF_UNSPEC;
    s->src_port = 0;
    mutex_unlock(&_pool_mutex);
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
    mutex_lock(&_pool_mutex);
    s = _get_free_socket();
    if (s == NULL) {
        errno = ENFILE;
        mutex_unlock(&_pool_mutex);
        return -1;
    }
    switch (domain) {
        case AF_INET:
        case AF_INET6:
            s->domain = domain;
            s->type = type;
            if ((s->protocol = _choose_ipproto(type, protocol)) < 0) {
                res = -1;
            }
            break;

        default:
            (void)type;
            (void)protocol;
            errno = EAFNOSUPPORT;
            res = -1;
    }
    if (res == 0) {
        /* TODO: add read and write */
        int fd = fd_new(s - _pool, socket_read, socket_write, socket_close);
        if (fd < 0) {
            errno = ENFILE;
            res = -1;
        }
        else {
            s->fd = res = fd;
        }
    }
    s->bound = false;
    s->src_port = 0;
    mutex_unlock(&_pool_mutex);
    return res;
}


int accept(int socket, struct sockaddr *restrict address,
           socklen_t *restrict address_len)
{
    socket_t *s, *new_s = NULL;
    int res = 0;
    /* May be kept unassigned if no conn module is available */
    /* cppcheck-suppress unassignedVariable */
    struct sockaddr_storage tmp;
    void *addr;
    uint16_t *port;
    socklen_t tmp_len;
    mutex_lock(&_pool_mutex);
    s = _get_socket(socket);
    if (s == NULL) {
        mutex_unlock(&_pool_mutex);
        errno = ENOTSOCK;
        return -1;
    }
    if (!s->bound) {
        mutex_unlock(&_pool_mutex);
        errno = EINVAL;
        return -1;
    }
    switch (s->domain) {
        case AF_INET:
            addr = _in_addr_ptr(&tmp);
            port = _in_port_ptr(&tmp);
            tmp_len = sizeof(struct sockaddr_in);
            break;
        case AF_INET6:
            addr = _in6_addr_ptr(&tmp);
            port = _in6_port_ptr(&tmp);
            tmp_len = sizeof(struct sockaddr_in6);
            break;
        default:
            (void)address;
            (void)address_len;
            (void)new_s;
            (void)tmp;
            (void)addr;
            (void)port;
            (void)tmp_len;
            errno = EPROTO;
            res = -1;
            break;
    }
    switch (s->type) {
#ifdef MODULE_CONN_TCP
        case SOCK_STREAM:
            new_s = _get_free_socket();
            if (new_s == NULL) {
                errno = ENFILE;
                res = -1;
                break;
            }
            if ((res = conn_tcp_accept(&s->conn.tcp, &new_s->conn.tcp)) < 0) {
                errno = -res;
                res = -1;
                break;
            }
            else if ((address != NULL) && (address_len != NULL)) {
                /* TODO: add read and write */
                int fd = fd_new(new_s - _pool, NULL, NULL, socket_close);
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
                tmp.ss_family = s->domain;
                if ((res = conn_tcp_getpeeraddr(&s->conn.tcp, addr, port)) < 0) {
                    errno = -res;
                    res = -1;
                    break;
                }
                _htons_port(port);  /* XXX: sin(6)_port is supposed to be network byte
                                     *      order */
                *address_len = _addr_truncate(address, *address_len, &tmp, tmp_len);
            }
            break;
#endif
        default:
            errno = EOPNOTSUPP;
            res = -1;
            break;
    }
    mutex_unlock(&_pool_mutex);
    return res;
}

int bind(int socket, const struct sockaddr *address, socklen_t address_len)
{
    socket_t *s;
    int res = 0;
    void *addr;
    size_t addr_len;
    uint16_t port;
    mutex_lock(&_pool_mutex);
    s = _get_socket(socket);
    mutex_unlock(&_pool_mutex);
    if (s == NULL) {
        errno = ENOTSOCK;
        return -1;
    }
    if (address->sa_family != s->domain) {
        errno = EAFNOSUPPORT;
        return -1;
    }
    if (_get_data_from_sockaddr(address, address_len, &addr, &addr_len, &port) < 0) {
        return -1;
    }
    switch (s->type) {
#ifdef MODULE_CONN_IP
        case SOCK_RAW:
            (void)port;
            if ((res = conn_ip_create(&s->conn.raw, addr, addr_len, s->domain, s->protocol)) < 0) {
                errno = -res;
                return -1;
            }
            break;
#endif
#ifdef MODULE_CONN_TCP
        case SOCK_STREAM:
            if ((res = conn_tcp_create(&s->conn.tcp, addr, addr_len, s->domain, port)) < 0) {
                errno = -res;
                return -1;
            }
            break;
#endif
#ifdef MODULE_CONN_UDP
        case SOCK_DGRAM:
            if ((res = conn_udp_create(&s->conn.udp, addr, addr_len, s->domain, port)) < 0) {
                errno = -res;
                return -1;
            }
            break;
#endif
        default:
            (void)addr;
            (void)addr_len;
            (void)port;
            (void)res;
            errno = EOPNOTSUPP;
            return -1;
    }
    s->src_port = byteorder_ntohs(port);
    s->bound = true;
    return 0;
}

int connect(int socket, const struct sockaddr *address, socklen_t address_len)
{
    socket_t *s;
    int res = 0;
    void *addr;
    size_t addr_len;
    uint16_t port;
    mutex_lock(&_pool_mutex);
    s = _get_socket(socket);
    mutex_unlock(&_pool_mutex);
    if (s == NULL) {
        errno = ENOTSOCK;
        return -1;
    }
    if (!s->bound) {
        errno = EINVAL;
        return -1;
    }
    if (address->sa_family != s->domain) {
        errno = EAFNOSUPPORT;
        return -1;
    }
    if (_get_data_from_sockaddr(address, address_len, &addr, &addr_len, &port) < 0) {
        return -1;
    }
    switch (s->type) {
#ifdef MODULE_CONN_TCP
        case SOCK_STREAM:
            /* XXX sin_port is in network byteorder */
            if ((res = conn_tcp_connect(&s->conn.tcp, addr, addr_len, port)) < 0) {
                errno = -res;
                return -1;
            }
            break;
#endif
        default:
            (void)res;
            errno = EPROTOTYPE;
            return -1;
    }
    return 0;
}

int getpeername(int socket, struct sockaddr *__restrict address,
                socklen_t *__restrict address_len)
{
    socket_t *s;
    int res = 0;
    /* May be kept unassigned if no conn module is available */
    /* cppcheck-suppress unassignedVariable */
    struct sockaddr_storage tmp;
    void *addr;
    uint16_t *port;
    socklen_t tmp_len;
    mutex_lock(&_pool_mutex);
    s = _get_socket(socket);
    mutex_unlock(&_pool_mutex);
    if (s == NULL) {
        errno = ENOTSOCK;
        return -1;
    }
    switch (s->domain) {
        case AF_INET:
            addr = _in_addr_ptr(&tmp);
            port = _in_port_ptr(&tmp);
            tmp_len = sizeof(struct sockaddr_in);
            break;
        case AF_INET6:
            addr = _in6_addr_ptr(&tmp);
            port = _in6_port_ptr(&tmp);
            tmp_len = sizeof(struct sockaddr_in6);
            break;
        default:
            (void)address;
            (void)address_len;
            (void)tmp;
            (void)addr;
            (void)port;
            (void)tmp_len;
            (void)res;
            errno = EBADF;
            return -1;
    }
    if (*address_len != tmp_len) {
        errno = EINVAL;
        return -1;
    }
    switch (s->type) {
#ifdef MODULE_CONN_TCP
        case SOCK_STREAM:
            if ((res = conn_tcp_getpeeraddr(&s->conn.tcp, addr, port)) < 0) {
                errno = -res;
                return -1;
            }
            break;
#endif
        default:
            errno = ENOTCONN;
            return -1;
    }
    tmp.ss_family = s->domain;
    _htons_port(port);  /* XXX: sin(6)_port is supposed to be network byte
                         *      order */
    *address_len = _addr_truncate(address, *address_len, &tmp, tmp_len);
    return 0;
}

int getsockname(int socket, struct sockaddr *__restrict address,
                socklen_t *__restrict address_len)
{
    socket_t *s;
    int res = 0;
    /* May be kept unassigned if no conn module is available */
    /* cppcheck-suppress unassignedVariable */
    struct sockaddr_storage tmp;
    void *addr;
    uint16_t *port;
    socklen_t tmp_len;
    mutex_lock(&_pool_mutex);
    s = _get_socket(socket);
    mutex_unlock(&_pool_mutex);
    if (s == NULL) {
        errno = ENOTSOCK;
        return -1;
    }
    if (!s->bound) {
        memset(address, 0, *address_len);
        return 0;
    }
    switch (s->domain) {
        case AF_INET:
            addr = _in_addr_ptr(&tmp);
            port = _in_port_ptr(&tmp);
            tmp_len = sizeof(struct sockaddr_in);
            break;
        case AF_INET6:
            addr = _in6_addr_ptr(&tmp);
            port = _in6_port_ptr(&tmp);
            tmp_len = sizeof(struct sockaddr_in6);
            break;
        default:
            (void)address;
            (void)address_len;
            (void)tmp;
            (void)addr;
            (void)port;
            (void)tmp_len;
            (void)res;
            errno = EBADF;
            return -1;
    }
    if (*address_len != tmp_len) {
        errno = EINVAL;
        return -1;
    }
    switch (s->type) {
#ifdef MODULE_CONN_UDP
        case SOCK_DGRAM:
            if ((res = conn_udp_getlocaladdr(&s->conn.udp, addr, port)) < 0) {
                errno = -res;
                return -1;
            }
            break;
#endif
#ifdef MODULE_CONN_IP
        case SOCK_RAW:
            if ((res = conn_ip_getlocaladdr(&s->conn.raw, addr)) < 0) {
                errno = -res;
                return -1;
            }
            break;
#endif
#ifdef MODULE_CONN_TCP
        case SOCK_STREAM:
            if ((res = conn_tcp_getlocaladdr(&s->conn.tcp, addr, port)) < 0) {
                errno = -res;
                return -1;
            }
            break;
#endif
        default:
            errno = EOPNOTSUPP;
            return -1;
    }
    tmp.ss_family = AF_INET;
    _htons_port(port);  /* XXX: sin(6)_port is supposed to be network byte
                         *      order */
    *address_len = _addr_truncate(address, *address_len, &tmp, tmp_len);
    return 0;
}

int listen(int socket, int backlog)
{
    socket_t *s;
    int res = 0;
    mutex_lock(&_pool_mutex);
    s = _get_socket(socket);
    mutex_unlock(&_pool_mutex);
    if (!s->bound) {
        errno = EINVAL;
        return -1;
    }
    switch (s->domain) {
        case AF_INET:
        case AF_INET6:
            switch (s->type) {
#ifdef MODULE_CONN_TCP
                case SOCK_STREAM:
                    if ((res = conn_tcp_listen(&s->conn.tcp, backlog)) < 0) {
                        errno = -res;
                        return -1;
                    }
                    break;
#endif
                default:
                    errno = EOPNOTSUPP;
                    return -1;
            }
            break;
        default:
            (void)backlog;
            (void)res;
            errno = EAFNOSUPPORT;
            return -1;
    }
    return 0;
}

ssize_t recv(int socket, void *buffer, size_t length, int flags)
{
    return recvfrom(socket, buffer, length, flags, NULL, NULL);
}

ssize_t recvfrom(int socket, void *restrict buffer, size_t length, int flags,
                 struct sockaddr *restrict address,
                 socklen_t *restrict address_len)
{
    socket_t *s;
    int res = 0;
    /* May be kept unassigned if no conn module is available */
    /* cppcheck-suppress unassignedVariable */
    struct sockaddr_storage tmp;
    void *addr;
    size_t addr_len;
    uint16_t *port;
    socklen_t tmp_len;
    (void)flags;
    mutex_lock(&_pool_mutex);
    s = _get_socket(socket);
    mutex_unlock(&_pool_mutex);
    if (s == NULL) {
        errno = ENOTSOCK;
        return -1;
    }
    if (!s->bound) {
        errno = EINVAL;
        return -1;
    }
    memset(&tmp, 0, sizeof(struct sockaddr_storage));
    switch (s->domain) {
        case AF_INET:
            addr = _in_addr_ptr(&tmp);
            port = _in_port_ptr(&tmp);
            addr_len = sizeof(ipv4_addr_t);
            tmp_len = sizeof(struct sockaddr_in);
            break;
        case AF_INET6:
            addr = _in6_addr_ptr(&tmp);
            port = _in6_port_ptr(&tmp);
            addr_len = sizeof(ipv6_addr_t);
            tmp_len = sizeof(struct sockaddr_in6);
            break;
        default:
            (void)buffer;
            (void)length;
            (void)address;
            (void)address_len;
            (void)tmp;
            (void)addr;
            (void)port;
            (void)tmp_len;
            errno = EAFNOSUPPORT;
            return -1;
    }
    switch (s->type) {
#ifdef MODULE_CONN_UDP
        case SOCK_DGRAM:
            if ((res = conn_udp_recvfrom(&s->conn.udp, buffer, length, addr, &addr_len,
                                         port)) < 0) {
                errno = -res;
                return -1;
            }

            break;
#endif
#ifdef MODULE_CONN_IP
        case SOCK_RAW:
            if ((res = conn_ip_recvfrom(&s->conn.raw, buffer, length, addr, &addr_len)) < 0) {
                errno = -res;
                return -1;
            }
            break;
#endif
#ifdef MODULE_CONN_TCP
        case SOCK_STREAM:
            if ((res = conn_tcp_recv(&s->conn.tcp, buffer, length)) < 0) {
                errno = -res;
                return -1;
            }
            if ((res = conn_tcp_getpeeraddr(&s->conn.tcp, addr, port)) < 0) {
                errno = -res;
                return -1;
            }
            break;
#endif
        default:
            (void)addr_len;
            errno = EOPNOTSUPP;
            return -1;
    }
    if ((address != NULL) && (address_len != NULL)) {
        tmp.ss_family = s->domain;
        _htons_port(port);  /* XXX: sin_port is supposed to be network byte
                             *      order */
        *address_len = _addr_truncate(address, *address_len, &tmp, tmp_len);
    }
    return res;
}

ssize_t send(int socket, const void *buffer, size_t length, int flags)
{
    return sendto(socket, buffer, length, flags, NULL, 0);
}

ssize_t sendto(int socket, const void *buffer, size_t length, int flags,
               const struct sockaddr *address, socklen_t address_len)
{
    socket_t *s;
    int res = 0;
    void *addr = NULL;
    size_t addr_len = 0;
    uint16_t port = 0;
    (void)flags;
    mutex_lock(&_pool_mutex);
    s = _get_socket(socket);
    mutex_unlock(&_pool_mutex);
    if (s == NULL) {
        errno = ENOTSOCK;
        return -1;
    }
    if (address != NULL) {
        if (address->sa_family != s->domain) {
            errno = EAFNOSUPPORT;
            return -1;
        }
        if (_get_data_from_sockaddr(address, address_len, &addr, &addr_len, &port) < 0) {
            return -1;
        }
    }
    switch (s->type) {
#ifdef MODULE_CONN_IP
        case SOCK_RAW:
            if ((address != NULL) && (s->bound)) {
                uint8_t src_addr[sizeof(ipv6_addr_t)];
                size_t src_len;
                int res = conn_ip_getlocaladdr(&s->conn.raw, src_addr);
                if (res < 0) {
                    errno = ENOTSOCK;   /* Something seems to be wrong with the socket */
                    return -1;
                }
                src_len = (size_t)res;
                /* cppcheck bug? res is read below in l824 */
                /* cppcheck-suppress unreadVariable */
                res = conn_ip_sendto(buffer, length, src_addr, src_len, addr, addr_len, s->domain,
                                     s->protocol);
            }
            else if (address != NULL) {
                res = conn_ip_sendto(buffer, length, NULL, 0, addr, addr_len, s->domain,
                                     s->protocol);
            }
            else {
                errno = ENOTCONN;
                return -1;
            }
            if (res < 0) {
                errno = -res;
                return -1;
            }
            break;
#endif
#ifdef MODULE_CONN_TCP
        case SOCK_STREAM:
            if (!s->bound) {
                errno = ENOTCONN;
                return -1;
            }
            if (address != NULL) {
                errno = EISCONN;
                return -1;
            }
            if ((res = conn_tcp_send(&s->conn.tcp, buffer, length)) < 0) {
                errno = -res;
                return -1;
            }
            break;
#endif
#ifdef MODULE_CONN_UDP
        case SOCK_DGRAM:
            if ((address != NULL) && (s->bound)) {
                uint8_t src_addr[sizeof(ipv6_addr_t)];
                size_t src_len;
                uint16_t sport;
                int res = conn_udp_getlocaladdr(&s->conn.udp, src_addr, &sport);
                if (res < 0) {
                    errno = ENOTSOCK;   /* Something seems to be wrong with the socket */
                    return -1;
                }
                src_len = (size_t)res;
                /* cppcheck bug? res is read below in l824 */
                /* cppcheck-suppress unreadVariable */
                res = conn_udp_sendto(buffer, length, src_addr, src_len, addr, addr_len, s->domain,
                                      sport, port);
            }
            else if (address != NULL) {
                ipv6_addr_t local;
                s->src_port = (uint16_t)genrand_uint32_range(1LU << 10U, 1LU << 16U);
                /* implicitly bind the socket here */
                ipv6_addr_set_unspecified(&local);
                if ((res = conn_udp_create(&s->conn.udp, &local, sizeof(local),
                                           s->domain, s->src_port)) < 0) {
                    errno = -res;
                    return -1;
                }
                s->bound = true;
                res = conn_udp_sendto(buffer, length, NULL, 0, addr, addr_len, s->domain,
                                      s->src_port, port);
            }
            else {
                errno = ENOTCONN;
                return -1;
            }
            if (res < 0) {
                errno = -res;
                return -1;
            }
            break;
#endif
        default:
            (void)buffer;
            (void)length;
            errno = EOPNOTSUPP;
            return -1;
    }
    return res;
}


/**
 * @}
 */
