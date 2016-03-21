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
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <errno.h>
#include "net/af.h"
#include "net/gnrc/conn.h"
#include "net/gnrc/ipv6.h"

#include "net/conn/ip.h"

int conn_ip_create(conn_ip_t *conn, const void *addr, size_t addr_len, int family, int proto)
{
    switch (family) {
#ifdef MODULE_GNRC_IPV6
        case AF_INET6:
            if (addr_len != sizeof(ipv6_addr_t)) {
                return -EINVAL;
            }
            if (gnrc_conn6_set_local_addr(conn->local_addr, addr)) {
                conn->l3_type = GNRC_NETTYPE_IPV6;
                conn->local_addr_len = addr_len;
                conn_ip_close(conn);       /* unregister possibly registered netreg entry */
                gnrc_conn_reg(&conn->netreg_entry, conn->l3_type, (uint32_t)proto);
            }
            else {
                return -EADDRNOTAVAIL;
            }
            break;
#endif
        default:
            (void)addr;
            (void)addr_len;
            (void)proto;
            return -EAFNOSUPPORT;
    }
    conn->l4_type = GNRC_NETTYPE_UNDEF;
    return 0;
}

void conn_ip_close(conn_ip_t *conn)
{
    assert(conn->l4_type == GNRC_NETTYPE_UNDEF);
    if (conn->netreg_entry.pid != KERNEL_PID_UNDEF) {
        gnrc_netreg_unregister(conn->l3_type, &conn->netreg_entry);
    }
}

int conn_ip_getlocaladdr(conn_ip_t *conn, void *addr)
{
    assert(conn->l4_type == GNRC_NETTYPE_UNDEF);
    memcpy(addr, &conn->local_addr, conn->local_addr_len);
    return conn->local_addr_len;
}

int conn_ip_recvfrom(conn_ip_t *conn, void *data, size_t max_len, void *addr, size_t *addr_len)
{
    assert(conn->l4_type == GNRC_NETTYPE_UNDEF);
    switch (conn->l3_type) {
#ifdef MODULE_GNRC_IPV6
        case GNRC_NETTYPE_IPV6:
            return gnrc_conn_recvfrom((conn_t *)conn, data, max_len, addr, addr_len, NULL);
#endif
        default:
            (void)data;
            (void)max_len;
            (void)addr;
            (void)addr_len;
            return -EBADF;
    }
}

int conn_ip_sendto(const void *data, size_t len, const void *src, size_t src_len,
                   void *dst, size_t dst_len, int family, int proto)
{
    gnrc_pktsnip_t *pkt, *hdr = NULL;
    gnrc_nettype_t l3_type;

    pkt = gnrc_pktbuf_add(NULL, (void *)data, len, GNRC_NETTYPE_UNDEF); /* data will only be copied */

    switch (family) {
#ifdef MODULE_GNRC_IPV6
        case AF_INET6:
            if (((src != NULL) && (src_len != sizeof(ipv6_addr_t))) ||
                (dst_len != sizeof(ipv6_addr_t)) ||
                (((unsigned)proto) > 256U)) {
                gnrc_pktbuf_release(pkt);
                return -EINVAL;
            }
            /* addr will only be copied */
            hdr = gnrc_ipv6_hdr_build(pkt, src, dst);
            if (hdr == NULL) {
                gnrc_pktbuf_release(pkt);
                return -ENOMEM;
            }
            /* set next header to connection's proto */
            ipv6_hdr_t *ipv6_hdr = hdr->data;
            ipv6_hdr->nh = (uint8_t)proto;
            pkt = hdr;
            l3_type = GNRC_NETTYPE_IPV6;
            break;
#endif /* MODULE_GNRC_IPV6 */
        default:
            (void)src;
            (void)src_len;
            (void)dst;
            (void)dst_len;
            (void)proto;
            (void)hdr;
            gnrc_pktbuf_release(pkt);
            return -EAFNOSUPPORT;
    }

    gnrc_netapi_dispatch_send(l3_type, GNRC_NETREG_DEMUX_CTX_ALL, pkt);

    return len;
}
