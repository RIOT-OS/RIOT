/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *               2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_sock_util
 * @{
 *
 * @file
 * @brief       sock utility functions implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "net/sock/udp.h"
#include "net/sock/util.h"

#ifdef RIOT_VERSION
#include "fmt.h"
#endif

#define SOCK_HOST_MAXLEN    (64U)   /**< maximum length of host part for
                                         sock_udp_str2ep() */

int sock_udp_ep_fmt(const sock_udp_ep_t *endpoint, char *addr_str, uint16_t *port)
{
    void *addr_ptr;
    *addr_str = '\0';

    switch (endpoint->family) {
#if defined(SOCK_HAS_IPV4)
        case AF_INET:
            {
                addr_ptr = (void*)&endpoint->addr.ipv4;
                break;
            }
#endif
#if defined(SOCK_HAS_IPV6)
        case AF_INET6:
            {
                addr_ptr = (void*)&endpoint->addr.ipv6;
                break;
            }
#endif /* else fall through */
        default:
            return -ENOTSUP;
    }

    if (!inet_ntop(endpoint->family, addr_ptr, addr_str, INET6_ADDRSTRLEN)) {
        return 0;
    }

#if defined(SOCK_HAS_IPV6)
    if ((endpoint->family == AF_INET6) && endpoint->netif) {
#ifdef RIOT_VERSION
        char *tmp = addr_str + strlen(addr_str);
        *tmp++ = '%';
        tmp += fmt_u16_dec(tmp, endpoint->netif);
        *tmp = '\0';
#else
        sprintf(addr_str + strlen(addr_str), "%%%4u", endpoint->netif);
#endif
    }
#endif

    if (port) {
        *port = endpoint->port;
    }

    return strlen(addr_str);
}

static char* _find_hoststart(const char *url)
{
    char *urlpos = (char*)url;
    while(*urlpos) {
        if (*urlpos++ == ':') {
            if (strncmp(urlpos, "//", 2) == 0) {
                return urlpos + 2;
            }
            break;
        }
        urlpos++;
    }
    return NULL;
}

static char* _find_pathstart(const char *url)
{
    char *urlpos = (char*)url;
    while(*urlpos) {
        if (*urlpos == '/') {
            return urlpos;
        }
        urlpos++;
    }
    return NULL;
}

int sock_urlsplit(const char *url, char *hostport, char *urlpath)
{
    char *hoststart = _find_hoststart(url);
    if (!hoststart) {
        return -EINVAL;
    }

    char *pathstart = _find_pathstart(hoststart);
    if(!pathstart) {
        return -EINVAL;
    }

    memcpy(hostport, hoststart, pathstart - hoststart);

    size_t pathlen = strlen(pathstart);
    if (pathlen) {
        memcpy(urlpath, pathstart, pathlen);
    }
    else {
        *urlpath = '\0';
    }
    return 0;
}

int sock_udp_str2ep(sock_udp_ep_t *ep_out, const char *str)
{
    unsigned brackets_flag;
    char *hoststart = (char*)str;
    char *hostend = hoststart;

    char hostbuf[SOCK_HOST_MAXLEN];

    memset(ep_out, 0, sizeof(sock_udp_ep_t));

    if (*hoststart == '[') {
        brackets_flag = 1;
        for (hostend = ++hoststart; *hostend && *hostend != ']';
                hostend++);
        if (! *hostend) {
            /* none found, bail out */
            return -EINVAL;
        }
    }
    else {
        brackets_flag = 0;
        for (hostend = hoststart; *hostend && *hostend != ':';
                hostend++);
    }

    if (*(hostend + brackets_flag) == ':') {
        ep_out->port = atoi(hostend + brackets_flag + 1);
    }

    size_t hostlen = hostend - hoststart;
    if (hostlen >= sizeof(hostbuf)) {
        return -EINVAL;
    }

    memcpy(hostbuf, hoststart, hostlen);

    hostbuf[hostlen] = '\0';

    if (!brackets_flag) {
        if (inet_pton(AF_INET, hostbuf, &ep_out->addr.ipv4) == 1) {
            ep_out->family = AF_INET;
            return 0;
        }
    }
#if defined(SOCK_HAS_IPV6)
    if (inet_pton(AF_INET6, hostbuf, ep_out->addr.ipv6) == 1) {
        ep_out->family = AF_INET6;
        return 0;
    }
#endif
    return -EINVAL;
}

bool sock_udp_ep_equal(const sock_udp_ep_t *a, const sock_udp_ep_t *b)
{
    assert(a && b);

    /* compare family and port */
    if ((a->family != b->family) || (a->port != b->port)) {
        return false;
    }

    /* compare addresses */
    switch (a->family) {
#ifdef SOCK_HAS_IPV6
        case AF_INET6:
            return (memcmp(a->addr.ipv6, b->addr.ipv6, 16) == 0);

#endif
        case AF_INET:
            return (memcmp(a->addr.ipv4, b->addr.ipv4, 4) == 0);
        default:
            return false;
    }
}
