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

#include "net/sock/util.h"
#include "net/iana/portrange.h"

#if MODULE_GNRC_SOCK_UDP || MODULE_LWIP_SOCK_UDP
#  include "net/sock/udp.h"
#endif

#if MODULE_DNS
#  include "net/dns.h"
#endif

#if MODULE_RANDOM
#  include "random.h"
#endif

#if MODULE_FMT
#  include "fmt.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

#define PORT_STR_LEN    (5)
#define NETIF_STR_LEN   (5)

static char* _find_hoststart(const char *url)
{
    /* Increment CONFIG_SOCK_SCHEME_MAXLEN due to comparison with the colon after the
     * scheme part
     */
    size_t remaining = CONFIG_SOCK_SCHEME_MAXLEN + 1;
    char *urlpos = (char*)url;
    while (*urlpos && remaining) {
        remaining--;
        if (*urlpos++ == ':') {
            if (strncmp(urlpos, "//", 2) == 0) {
                return urlpos + 2;
            }
            break;
        }
    }
    return NULL;
}

static char* _find_pathstart(const char *url)
{
    size_t remaining = CONFIG_SOCK_HOSTPORT_MAXLEN;
    char *urlpos = (char*)url;
    while (*urlpos && remaining) {
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

const char *sock_urlpath(const char *url)
{
    assert(url);
    char *hoststart = _find_hoststart(url);
    if (!hoststart) {
        return NULL;
    }

    return _find_pathstart(hoststart);
}

#if HAVE_SOCK_TL_EP
int sock_tl_ep_fmt(const struct _sock_tl_ep *endpoint,
                   char *addr_str, uint16_t *port)
{
    const void *addr_ptr;
    *addr_str = '\0';

    switch (endpoint->family) {
#ifdef SOCK_HAS_IPV4
    case AF_INET:
        addr_ptr = &endpoint->addr.ipv4;
        break;
#endif
#ifdef SOCK_HAS_IPV6
    case AF_INET6:
        addr_ptr = &endpoint->addr.ipv6;
        break;
#endif
    default:
        return -ENOTSUP;
    }

    if (!inet_ntop(endpoint->family, addr_ptr, addr_str, INET6_ADDRSTRLEN)) {
        return 0;
    }

    if (IS_ACTIVE(SOCK_HAS_IPV6) && (endpoint->family == AF_INET6) && endpoint->netif) {
#ifdef MODULE_FMT
        char *tmp = addr_str + strlen(addr_str);
        *tmp++ = '%';
        tmp += fmt_u16_dec(tmp, endpoint->netif);
        *tmp = '\0';
#else
        sprintf(addr_str + strlen(addr_str), "%%%4u", endpoint->netif);
#endif
    }

    if (port) {
        *port = endpoint->port;
    }

    return strlen(addr_str);
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
         netifend++) {}
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
             hostend++) {}
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
#  ifdef SOCK_HAS_IPV4
        if (inet_pton(AF_INET, hostbuf, &ep_out->addr.ipv4) == 1) {
            ep_out->family = AF_INET;
            return 0;
        }
#  endif
    }

#  ifdef SOCK_HAS_IPV6
    if (inet_pton(AF_INET6, hostbuf, ep_out->addr.ipv6) == 1) {
        ep_out->family = AF_INET6;
        return 0;
    }
#  endif

    return -EINVAL;
}

int sock_tl_name2ep(struct _sock_tl_ep *ep_out, const char *str)
{
    int res = sock_tl_str2ep(ep_out, str);
    if (res == 0) {
        return 0;
    }

#  if MODULE_SOCK_DNS || MODULE_SOCK_DNS_MOCK
    int family;
    char hostbuf[CONFIG_SOCK_HOSTPORT_MAXLEN];
    const char *host;
    char *hostend = strchr(str, ':');
    if (hostend == NULL) {
        host = str;
        ep_out->port = 0;
    } else {
        size_t host_len = hostend - str;
        if (host_len >= sizeof(hostbuf)) {
            return -EINVAL;
        }
        memcpy(hostbuf, str, host_len);
        hostbuf[host_len] = 0;
        host = hostbuf;
        ep_out->port = atoi(hostend + 1);;
    }

    if (IS_ACTIVE(SOCK_HAS_IPV4) && IS_ACTIVE(SOCK_HAS_IPV6)) {
        family = AF_UNSPEC;
    } else if (IS_ACTIVE(SOCK_HAS_IPV4)) {
        family = AF_INET;
    } else if (IS_ACTIVE(SOCK_HAS_IPV6)) {
        family = AF_INET6;
    } else {
        assert(0);
        return -EINVAL;
    }

    switch (dns_query(host, &ep_out->addr, family)) {
#    ifdef SOCK_HAS_IPV4
    case 4:
        ep_out->family = AF_INET;
        return 0;
#    endif
#    ifdef SOCK_HAS_IPV6
    case 16:
        ep_out->family = AF_INET6;
        return 0;
#    endif
    default:
        return -EINVAL;
    }
#  endif
    return res;
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
#  ifdef SOCK_HAS_IPV4
    case AF_INET:
        return memcmp(a->addr.ipv4, b->addr.ipv4, 4) == 0;
#  endif
#  ifdef SOCK_HAS_IPV6
    case AF_INET6:
        return memcmp(a->addr.ipv6, b->addr.ipv6, 16) == 0;
#  endif
    default:
        return false;
    }
}
#endif

#if defined(MODULE_SOCK_DTLS)
int sock_dtls_establish_session(sock_udp_t *sock_udp, sock_dtls_t *sock_dtls,
                                sock_dtls_session_t *session, credman_tag_t tag,
                                sock_udp_ep_t *local, const sock_udp_ep_t *remote,
                                void *work_buf, size_t work_buf_len)
{
    int res;
    uint32_t timeout_ms = CONFIG_SOCK_DTLS_TIMEOUT_MS;
    uint8_t retries = CONFIG_SOCK_DTLS_RETRIES;

    bool auto_port = local->port == 0;
    do {
        if (auto_port) {
            /* choose random ephemeral port, since DTLS requires a local port */
            local->port = random_uint32_range(IANA_DYNAMIC_PORTRANGE_MIN,
                                              IANA_DYNAMIC_PORTRANGE_MAX);
        }
        /* connect UDP socket */
        res = sock_udp_create(sock_udp, local, remote, 0);
    } while (auto_port && (res == -EADDRINUSE));

    if (res < 0) {
        return res;
    }

    /* create DTLS socket on to of UDP socket */
    res = sock_dtls_create(sock_dtls, sock_udp, tag,
                           SOCK_DTLS_1_2, SOCK_DTLS_CLIENT);
    if (res < 0) {
        DEBUG("Unable to create DTLS sock: %s\n", strerror(-res));
        sock_udp_close(sock_udp);
        return res;
    }

    while (1) {
        mutex_t lock = MUTEX_INIT_LOCKED;
        ztimer_t timeout;

        /* unlock lock after timeout */
        ztimer_mutex_unlock(ZTIMER_MSEC, &timeout, timeout_ms, &lock);

        /* create DTLS session */
        res = sock_dtls_session_init(sock_dtls, remote, session);
        if (res >= 0) {
            /* handle handshake */
            res = sock_dtls_recv(sock_dtls, session, work_buf,
                                 work_buf_len, timeout_ms * US_PER_MS);
            if (res == -SOCK_DTLS_HANDSHAKE) {
                DEBUG("DTLS handshake successful\n");
                ztimer_remove(ZTIMER_MSEC, &timeout);
                return 0;
            }
            DEBUG("Unable to establish DTLS handshake: %s\n", strerror(-res));

        } else {
            DEBUG("Unable to initialize DTLS session: %s\n", strerror(-res));
        }

        sock_dtls_session_destroy(sock_dtls, session);

        if (retries--) {
            /* wait for timeout to expire */
            mutex_lock(&lock);
        } else {
            ztimer_remove(ZTIMER_MSEC, &timeout);
            break;
        }

        /* see https://datatracker.ietf.org/doc/html/rfc6347#section-4.2.4.1 */
        timeout_ms *= 2U;
    }

    sock_dtls_close(sock_dtls);
    sock_udp_close(sock_udp);
    return res;
}
#endif

#if defined(MODULE_SOCK_UDP)
ssize_t sock_udp_recv_aux(sock_udp_t *sock, void *data, size_t max_len,
                         uint32_t timeout, sock_udp_ep_t *remote,
                         sock_udp_aux_rx_t *aux)
{
    void *pkt = NULL, *ctx = NULL;
    uint8_t *ptr = data;
    ssize_t res, ret = 0;
    bool nobufs = false;

    assert((sock != NULL) && (data != NULL) && (max_len > 0));
    while ((res = sock_udp_recv_buf_aux(sock, &pkt, &ctx, timeout, remote,
                                        aux)) > 0) {
        if (res > (ssize_t)max_len) {
            nobufs = true;
            res = max_len;
        }
        memcpy(ptr, pkt, res);
        ptr += res;
        ret += res;
        max_len -= res;
    }
    return (nobufs) ? -ENOBUFS : ((res < 0) ? res : ret);
}
#endif
