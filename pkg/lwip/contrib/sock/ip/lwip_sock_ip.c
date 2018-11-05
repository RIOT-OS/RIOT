/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <errno.h>

#include "net/ipv4/addr.h"
#include "net/ipv6/addr.h"
#include "net/ipv6/hdr.h"
#include "net/sock/ip.h"
#include "timex.h"

#include "lwip/api.h"
#include "lwip/ip4.h"
#include "lwip/ip6.h"
#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/sock_internal.h"


int sock_ip_create(sock_ip_t *sock, const sock_ip_ep_t *local,
                   const sock_ip_ep_t *remote, uint8_t proto, uint16_t flags)
{
    assert(sock != NULL);

    int res;
    struct netconn *tmp = NULL;

    /* we pay attention in lwip_sock_create that _sock_tl_ep::port is not
     * touched for RAW */
    if ((res = lwip_sock_create(&tmp, (struct _sock_tl_ep *)local,
                                (struct _sock_tl_ep *)remote, proto, flags,
                                NETCONN_RAW)) == 0) {
        sock->conn = tmp;
    }
    return res;
}

void sock_ip_close(sock_ip_t *sock)
{
    assert(sock != NULL);
    if (sock->conn != NULL) {
        netconn_delete(sock->conn);
        sock->conn = NULL;
    }
}

int sock_ip_get_local(sock_ip_t *sock, sock_ip_ep_t *ep)
{
    assert(sock != NULL);
    return (lwip_sock_get_addr(sock->conn, (struct _sock_tl_ep *)ep,
                               1)) ? -EADDRNOTAVAIL : 0;
}

int sock_ip_get_remote(sock_ip_t *sock, sock_ip_ep_t *ep)
{
    assert(sock != NULL);
    return (lwip_sock_get_addr(sock->conn, (struct _sock_tl_ep *)ep,
                               0)) ? -ENOTCONN : 0;
}

#if LWIP_IPV4
static uint16_t _ip4_addr_to_netif(const ip4_addr_p_t *addr)
{
    assert(addr != NULL);

    if (!ip4_addr_isany(addr)) {
        for (struct netif *netif = netif_list; netif != NULL; netif = netif->next) {
            if (netif_ip4_addr(netif)->addr == addr->addr) {
                return (int)netif->num + 1;
            }
        }
    }
    return SOCK_ADDR_ANY_NETIF;
}
#endif

#if LWIP_IPV6
static uint16_t _ip6_addr_to_netif(const ip6_addr_p_t *_addr)
{
    ip6_addr_t addr;

    assert(_addr != NULL);
    ip6_addr_copy_from_packed(addr, *_addr);
    if (!ip6_addr_isany_val(addr)) {
        for (struct netif *netif = netif_list; netif != NULL; netif = netif->next) {
            if (netif_get_ip6_addr_match(netif, &addr) >= 0) {
                return (int)netif->num + 1;
            }
        }
    }
    return SOCK_ADDR_ANY_NETIF;
}
#endif

static int _parse_iphdr(const struct netbuf *buf, void *data, size_t max_len,
                        sock_ip_ep_t *remote)
{
    uint8_t *data_ptr = buf->p->payload;
    size_t data_len = buf->p->len;

    assert(buf->p->next == NULL);   /* TODO this might not be generally the case
                                     * check later with larger payloads */
    switch (data_ptr[0] >> 4) {
#if LWIP_IPV4
        case 4:
            if ((data_len - sizeof(struct ip_hdr)) > max_len) {
                return -ENOBUFS;
            }
            if (remote != NULL) {
                struct ip_hdr *iphdr = (struct ip_hdr *)data_ptr;

                assert(buf->p->len > sizeof(struct ip_hdr));
                remote->family = AF_INET;
                memcpy(&remote->addr, &iphdr->src, sizeof(ip4_addr_t));
                remote->netif = _ip4_addr_to_netif(&iphdr->dest);
            }
            data_ptr += sizeof(struct ip_hdr);
            data_len -= sizeof(struct ip_hdr);
            break;
#endif
#if LWIP_IPV6
        case 6:
            if ((data_len - sizeof(struct ip6_hdr)) > max_len) {
                return -ENOBUFS;
            }
            if (remote != NULL) {
                struct ip6_hdr *iphdr = (struct ip6_hdr *)data_ptr;

                assert(buf->p->len > sizeof(struct ip6_hdr));
                remote->family = AF_INET6;
                memcpy(&remote->addr, &iphdr->src, sizeof(ip6_addr_t));
                remote->netif = _ip6_addr_to_netif(&iphdr->dest);
            }
            data_ptr += sizeof(struct ip6_hdr);
            data_len -= sizeof(struct ip6_hdr);
            break;
#endif
        default:
            return -EPROTO;
    }
    memcpy(data, data_ptr, data_len);
    return (ssize_t)data_len;
}

ssize_t sock_ip_recv(sock_ip_t *sock, void *data, size_t max_len,
                     uint32_t timeout, sock_ip_ep_t *remote)
{
    struct netbuf *buf;
    int res;

    assert((sock != NULL) && (data != NULL) && (max_len > 0));
    if ((res = lwip_sock_recv(sock->conn, timeout, &buf)) < 0) {
        return res;
    }
    res = _parse_iphdr(buf, data, max_len, remote);
    netbuf_delete(buf);
    return res;
}

ssize_t sock_ip_send(sock_ip_t *sock, const void *data, size_t len,
                     uint8_t proto, const sock_ip_ep_t *remote)
{
    assert((sock != NULL) || (remote != NULL));
    assert((len == 0) || (data != NULL)); /* (len != 0) => (data != NULL) */
    return lwip_sock_send(&sock->conn, data, len, proto,
                          (struct _sock_tl_ep *)remote, NETCONN_RAW);
}

/** @} */
