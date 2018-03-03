/*
 * Copyright (C) 2013-15 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <arpa/inet.h>
#include <errno.h>

#include "net/ipv4/addr.h"
#include "net/ipv6/addr.h"

const char *inet_ntop(int af, const void *restrict src, char *restrict dst,
                      socklen_t size)
{
    switch (af) {
#ifdef MODULE_IPV4_ADDR
        case AF_INET:
            if (ipv4_addr_to_str(dst, src, (size_t)size) == NULL) {
                errno = ENOSPC;
                return NULL;
            }
            break;
#endif
#ifdef MODULE_IPV6_ADDR
        case AF_INET6:
            if (ipv6_addr_to_str(dst, src, (size_t)size) == NULL) {
                errno = ENOSPC;
                return NULL;
            }
            break;
#endif
        default:
            (void)src;
            (void)dst;
            (void)size;
            errno = EAFNOSUPPORT;
            return NULL;
    }
    return dst;
}

int inet_pton(int af, const char *src, void *dst)
{
    switch (af) {
#ifdef MODULE_IPV4_ADDR
        case AF_INET:
            if (ipv4_addr_from_str(dst, src) == NULL) {
                return 0;
            }
            break;
#endif
#ifdef MODULE_IPV6_ADDR
        case AF_INET6:
            if (ipv6_addr_from_str(dst, src) == NULL) {
                return 0;
            }
            break;
#endif
        default:
            (void)src;
            (void)dst;
            errno = EAFNOSUPPORT;
            return -1;
    }
    return 1;
}
