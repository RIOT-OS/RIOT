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
#include <assert.h>

#include "net/sock/udp.h"
#include "net/sock/util.h"

#ifdef MODULE_FMT
#include "fmt.h"
#endif

#define PORT_STR_LEN    (5)
#define NETIF_STR_LEN   (5)

int sock_tl_ep_fmt(const struct _sock_tl_ep *endpoint,
                   char *addr_str, uint16_t *port)
{
    void *addr_ptr;
    *addr_str = '\0';

    switch (endpoint->family) {
        case AF_INET:
            {
                addr_ptr = (void*)&endpoint->addr.ipv4;
                break;
            }
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
#ifdef MODULE_FMT
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
    /* Increment CONFIG_SOCK_SCHEME_MAXLEN due to comparison with the colon after the
     * scheme part
     */
    size_t remaining = CONFIG_SOCK_SCHEME_MAXLEN + 1;
    char *urlpos = (char*)url;
    while(*urlpos && remaining) {
        remaining--;
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
    size_t remaining = CONFIG_SOCK_HOSTPORT_MAXLEN;
    char *urlpos = (char*)url;
    while(*urlpos && remaining) {
        remaining--;
        if (*urlpos == '/') {
            return urlpos;
        }
        urlpos++;
    }
    return urlpos;
}

int sock_urlsplit(const char *url, char *hostport, char *urlpath)
{
    assert(url);
    char *hoststart = _find_hoststart(url);
    if (!hoststart) {
        return -EINVAL;
    }

    char *pathstart = _find_pathstart(hoststart);

    if (hostport) {
        size_t hostlen = pathstart - hoststart;
        /* hostlen must be smaller CONFIG_SOCK_HOSTPORT_MAXLEN to have space for the null
        * terminator */
        if (hostlen > CONFIG_SOCK_HOSTPORT_MAXLEN - 1) {
            return -EOVERFLOW;
        }
        memcpy(hostport, hoststart, hostlen);
        hostport[hostlen] = '\0';
    }

    if (urlpath) {
        size_t pathlen = strlen(pathstart);
        if (pathlen > CONFIG_SOCK_URLPATH_MAXLEN - 1) {
            return -EOVERFLOW;
        }
        memcpy(urlpath, pathstart, pathlen);
        urlpath[pathlen] = '\0';
    }
    return 0;
}

int _parse_port(sock_udp_ep_t *ep_out, const char *portstart)
{
    int port_len = strlen(portstart);

    /* Checks here verify that the supplied port number is up to 5 (random)
     * chars in size and result is smaller or equal to UINT16_MAX. */
    if (port_len > PORT_STR_LEN) {
        return -EINVAL;
    }
    uint32_t port = atol(portstart);
    if (port > UINT16_MAX) {
        return -EINVAL;
    }
    ep_out->port = (uint16_t)port;
    return port_len;
}

int _parse_netif(sock_udp_ep_t *ep_out, char *netifstart)
{
    char *netifend;
    size_t netiflen;
    char netifbuf[NETIF_STR_LEN + 1] = {0};

    for (netifend = netifstart; *netifend && *netifend != ']';
         netifend++);
    netiflen = netifend - netifstart;
    if (!*netifend || (netiflen >= NETIF_STR_LEN) || (netiflen == 0)) {
        /* no netif found, bail out */
        return -EINVAL;
    }
    strncpy(netifbuf, netifstart, netiflen);
    int netif = strtol(netifbuf, NULL, 10);
    if ((netif < 0) || (((unsigned)netif) > UINT16_MAX)) {
        return -EINVAL;
    }
    ep_out->netif = (uint16_t)netif;
    return (netifend - netifstart);
}

int sock_tl_str2ep(struct _sock_tl_ep *ep_out, const char *str)
{
    unsigned brackets_flag;
    char *hoststart = (char*)str;
    char *hostend;

    char hostbuf[CONFIG_SOCK_HOSTPORT_MAXLEN];

    memset(ep_out, 0, sizeof(sock_udp_ep_t));

    if (*hoststart == '[') {
        brackets_flag = 1;
        for (hostend = ++hoststart;
             *hostend && *hostend != ']' && *hostend != '%';
             hostend++);
        if (! *hostend || ((size_t)(hostend - hoststart) >= sizeof(hostbuf))) {
            /* none found, bail out */
            return -EINVAL;
        }
    }
    else {
        brackets_flag = 0;
        for (hostend = hoststart; *hostend && (*hostend != ':') && \
                ((size_t)(hostend - hoststart) < sizeof(hostbuf)); hostend++) {}
    }
    size_t hostlen = hostend - hoststart;
    if (*(hostend + brackets_flag) == ':') {
        int res = _parse_port(ep_out, hostend + brackets_flag + 1);
        if (res < 0) {
            return res;
        }
    }
    else if (brackets_flag && (*hostend == '%')) {
        int res = _parse_netif(ep_out, hostend + 1);
        if (res < 0) {
            return res;
        }
        char *colon_ptr = hostend + res + brackets_flag + 1;
        if ((*colon_ptr == ':') &&
            ((res = _parse_port(ep_out, colon_ptr + 1)) < 0)) {
            return res;
        }
    }

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

bool sock_tl_ep_equal(const struct _sock_tl_ep *a,
                      const struct _sock_tl_ep *b)
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
