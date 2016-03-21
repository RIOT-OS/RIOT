/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief       GNRC implementation of the udp interface defined by net/gnrc/udp.h
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <errno.h>
#include "net/af.h"
#include "net/gnrc/conn.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/udp.h"

#include "net/conn/udp.h"

int conn_udp_create(conn_udp_t *conn, const void *addr, size_t addr_len,
                    int family, uint16_t port)
{
    conn->l4_type = GNRC_NETTYPE_UDP;
    switch (family) {
#ifdef MODULE_GNRC_IPV6
        case AF_INET6:
            if (addr_len != sizeof(ipv6_addr_t)) {
                return -EINVAL;
            }
            if (gnrc_conn6_set_local_addr(conn->local_addr, addr)) {
                conn->l3_type = GNRC_NETTYPE_IPV6;
                conn->local_addr_len = addr_len;
                conn_udp_close(conn);       /* unregister possibly registered netreg entry */
                gnrc_conn_reg(&conn->netreg_entry, conn->l4_type, (uint32_t)port);
            }
            else {
                return -EADDRNOTAVAIL;
            }
            break;
#endif
        default:
            (void)addr;
            (void)addr_len;
            (void)port;
            return -EAFNOSUPPORT;
    }
    return 0;
}

void conn_udp_close(conn_udp_t *conn)
{
    assert(conn->l4_type == GNRC_NETTYPE_UDP);
    if (conn->netreg_entry.pid != KERNEL_PID_UNDEF) {
        gnrc_netreg_unregister(GNRC_NETTYPE_UDP, &conn->netreg_entry);
        conn->netreg_entry.pid = KERNEL_PID_UNDEF;
    }
}

int conn_udp_getlocaladdr(conn_udp_t *conn, void *addr, uint16_t *port)
{
    assert(conn->l4_type == GNRC_NETTYPE_UDP);
    memcpy(addr, &conn->local_addr, conn->local_addr_len);
    *port = (uint16_t)conn->netreg_entry.demux_ctx;
    return conn->local_addr_len;
}

int conn_udp_recvfrom(conn_udp_t *conn, void *data, size_t max_len, void *addr, size_t *addr_len,
                      uint16_t *port)
{
    assert(conn->l4_type == GNRC_NETTYPE_UDP);
    switch (conn->l3_type) {
#ifdef MODULE_GNRC_IPV6
        case GNRC_NETTYPE_IPV6:
            return gnrc_conn_recvfrom((conn_t *)conn, data, max_len, addr, addr_len, port);
#endif
        default:
            (void)data;
            (void)max_len;
            (void)addr;
            (void)addr_len;
            (void)port;
            return -EBADF;
    }
}

int conn_udp_sendto(const void *data, size_t len, const void *src, size_t src_len,
                    const void *dst, size_t dst_len, int family, uint16_t sport,
                    uint16_t dport)
{
    gnrc_pktsnip_t *pkt, *hdr = NULL;

    pkt = gnrc_pktbuf_add(NULL, (void *)data, len, GNRC_NETTYPE_UNDEF); /* data will only be copied */
    hdr = gnrc_udp_hdr_build(pkt, sport, dport);
    if (hdr == NULL) {
        gnrc_pktbuf_release(pkt);
        return -ENOMEM;
    }
    pkt = hdr;
    switch (family) {
#ifdef MODULE_GNRC_IPV6
        case AF_INET6:
            if (((src != NULL) && (src_len != sizeof(ipv6_addr_t))) ||
                (dst_len != sizeof(ipv6_addr_t))) {
                gnrc_pktbuf_release(pkt);
                return -EINVAL;
            }
            /* addr will only be copied */
            hdr = gnrc_ipv6_hdr_build(pkt, src, dst);
            if (hdr == NULL) {
                gnrc_pktbuf_release(pkt);
                return -ENOMEM;
            }
            pkt = hdr;
            break;
#endif /* MODULE_GNRC_IPV6 */
        default:
            (void)hdr;
            (void)src;
            (void)src_len;
            (void)dst;
            (void)dst_len;
            gnrc_pktbuf_release(pkt);
            return -EAFNOSUPPORT;
    }

    gnrc_netapi_dispatch_send(GNRC_NETTYPE_UDP, GNRC_NETREG_DEMUX_CTX_ALL, pkt);

    return len;
}

/** @} */
