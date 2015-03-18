/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @file    sys_socket.c
 * @brief   Providing implementation for POSIX socket wrapper.
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#include <errno.h>

#include "socket_base/socket.h"
#include "fd.h"

#include "sys/socket.h"

int flagless_send(int fd, const void *buf, size_t len)
{
    return (int)socket_base_send(fd, buf, (uint32_t)len, 0);
}

int flagless_recv(int fd, void *buf, size_t len)
{
    return (int)socket_base_recv(fd, buf, (uint32_t)len, 0);
}

int socket(int domain, int type, int protocol)
{
    int internal_socket = socket_base_socket(domain, type, protocol);

    if (internal_socket < 0) {
        errno = ENFILE;
        return -1;
    }

    return fd_new(internal_socket, flagless_recv, flagless_send,
                  socket_base_close);
}


#define sock_func_wrapper(func, sockfd, ...) \
    ((fd_get(sockfd)) ? \
<<<<<<< HEAD
        func(fd_get(sockfd)->fd, __VA_ARGS__) : \
=======
        func(fd_get(sockfd)->internal_fd, __VA_ARGS__) : \
>>>>>>> upstream/master
        (errno = EBADF, -1))

int accept(int socket, struct sockaddr *restrict address,
           socklen_t *restrict address_len)
{
    int res = sock_func_wrapper(socket_base_accept, socket,
                                (sockaddr6_t *)address,
                                (socklen_t *)address_len);

    if (res < 0) {
        // transport_layer needs more granular error handling
        errno = EOPNOTSUPP;
        return -1;
    }

    return fd_new(res, flagless_recv, flagless_send,
                  socket_base_close);
}

int bind(int socket, const struct sockaddr *address, socklen_t address_len)
{
    int res = sock_func_wrapper(socket_base_bind, socket,
                                (sockaddr6_t *)address, address_len);

    if (res < 0) {
        // transport_layer needs more granular error handling
        errno = EOPNOTSUPP;
        return -1;
    }

    return res;
}

int connect(int socket, const struct sockaddr *address, socklen_t address_len)
{
    int res = sock_func_wrapper(socket_base_connect, socket,
                                (sockaddr6_t *)address, address_len);

    if (res < 0) {
        // transport_layer needs more granular error handling
        errno = ECONNREFUSED;
        return -1;
    }

    return res;
}

int getsockopt(int socket, int level, int option_name,
               void *restrict option_value, socklen_t *restrict option_len)
{
    // TODO
    (void) socket;
    (void) level;
    (void) option_name;
    (void) option_value;
    (void) option_len;

    return -1;
}

int listen(int socket, int backlog)
{
    int res = sock_func_wrapper(socket_base_listen, socket, backlog);

    if (res < 0) {
        // transport_layer needs more granular error handling
        errno = ECONNREFUSED;
        return -1;
    }

    return res;
}

ssize_t recv(int socket, void *buffer, size_t length, int flags)
{
    int32_t res = sock_func_wrapper(socket_base_recv, socket, buffer,
                                    (uint32_t) length, flags);

    if (res < 0) {
        // transport_layer needs more granular error handling
        errno = ENOTCONN;
        return -1;
    }

    return (ssize_t)res;
}

ssize_t recvfrom(int socket, void *restrict buffer, size_t length, int flags,
                 struct sockaddr *restrict address,
                 socklen_t *restrict address_len)
{
    int32_t res = sock_func_wrapper(socket_base_recvfrom, socket, buffer,
                                    (uint32_t) length, flags,
                                    (sockaddr6_t *)address,
                                    (socklen_t *)address_len);

    if (res < 0) {
        // transport_layer needs more granular error handling
        errno = ENOTCONN;
        return -1;
    }

    return (ssize_t)res;
}

ssize_t send(int socket, const void *buffer, size_t length, int flags)
{
    int32_t res = sock_func_wrapper(socket_base_send, socket, buffer,
                                    (uint32_t) length, flags);

    if (res < 0) {
        // transport_layer needs more granular error handling
        errno = ENOTCONN;
        return -1;
    }

    return (ssize_t)res;
}

ssize_t sendto(int socket, const void *message, size_t length, int flags,
               const struct sockaddr *dest_addr, socklen_t dest_len)
{
    int32_t res = sock_func_wrapper(socket_base_sendto, socket, message,
                                    (uint32_t) length, flags,
                                    (sockaddr6_t *)dest_addr,
                                    (socklen_t)dest_len);

    if (res < 0) {
        // transport_layer needs more granular error handling
        errno = ENOTCONN;
        return -1;
    }

    return (ssize_t)res;
}

int setsockopt(int socket, int level, int option_name, const void *option_value,
               socklen_t option_len)
{
    // TODO
    (void) socket;
    (void) level;
    (void) option_name;
    (void) option_value;
    (void) option_len;

    return -1;
}

/**
 * @}
 */
