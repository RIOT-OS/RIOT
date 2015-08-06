/*
 * Copyright (C) 2013 Freie Universität Berlin
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

#include "sys/socket.h"

int flagless_send(int fd, const void *buf, size_t len)
{
    (void)fd;
    (void)buf;
    (void)len;

    return -1;
}

int flagless_recv(int fd, void *buf, size_t len)
{
    return (int)socket_base_recv(fd, buf, (uint32_t)len, 0);
}

int socket(int domain, int type, int protocol)
{
    (void)domain;
    (void)type;
    (void)protocol;

    return -1;
}


int accept(int socket, struct sockaddr *restrict address,
           socklen_t *restrict address_len)
{
    (void)socket;
    (void)address;
    (void)address_len;

    return -1;
}

int bind(int socket, const struct sockaddr *address, socklen_t address_len)
{
    (void)socket;
    (void)address;
    (void)address_len;

    return -1;
}

int connect(int socket, const struct sockaddr *address, socklen_t address_len)
{
    (void)socket;
    (void)address;
    (void)address_len;

    return -1;
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
    (void)socket;
    (void)backlog;

    return -1;
}

ssize_t recv(int socket, void *buffer, size_t length, int flags)
{
    (void)socket;
    (void)buffer;
    (void)length;
    (void)flags;

    return -1;
}

ssize_t recvfrom(int socket, void *restrict buffer, size_t length, int flags,
                 struct sockaddr *restrict address,
                 socklen_t *restrict address_len)
{
    (void)socket;
    (void)buffer;
    (void)length;
    (void)flags;
    (void)address;
    (void)address_len;

    return -1;
}

ssize_t send(int socket, const void *buffer, size_t length, int flags)
{
    (void)socket;
    (void)buffer;
    (void)length;
    (void)flags;
    return -1;
}

ssize_t sendto(int socket, const void *message, size_t length, int flags,
               const struct sockaddr *dest_addr, socklen_t dest_len)
{
    // TODO
    (void)socket;
    (void)message;
    (void)length;
    (void)flags;
    (void)dest_addr;
    (void)dest_len;

    return -1;
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
