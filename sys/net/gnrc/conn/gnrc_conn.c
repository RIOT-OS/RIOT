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

#include "net/conn.h"
#include "net/ipv6/hdr.h"
#include "net/gnrc/conn.h"
#include "net/gnrc/ipv6/hdr.h"
#include "net/gnrc/ipv6/netif.h"
#include "net/udp.h"

int gnrc_conn_recvfrom(conn_t *conn, void *data, size_t max_len, void *addr, size_t *addr_len,
                       uint16_t *port)
{
    msg_t msg;
    int timeout = 3;
    while ((timeout--) > 0) {
        gnrc_pktsnip_t *pkt, *l3hdr;
        size_t size = 0;
        msg_receive(&msg);
        switch (msg.type) {
            case GNRC_NETAPI_MSG_TYPE_RCV:
                pkt = (gnrc_pktsnip_t *)msg.content.ptr;
                if (pkt->size > max_len) {
                    return -ENOMEM;
                }
                LL_SEARCH_SCALAR(pkt, l3hdr, type, conn->l3_type);
                if (l3hdr == NULL) {
                    msg_send_to_self(&msg); /* requeue invalid messages */
                    continue;
                }
#if defined(MODULE_CONN_UDP) || defined(MODULE_CONN_TCP)
                if ((conn->l4_type != GNRC_NETTYPE_UNDEF) && (port != NULL)) {
                    gnrc_pktsnip_t *l4hdr;
                    LL_SEARCH_SCALAR(pkt, l4hdr, type, conn->l4_type);
                    if (l4hdr == NULL) {
                        msg_send_to_self(&msg); /* requeue invalid messages */
                        continue;
                    }
                    *port = byteorder_ntohs(((udp_hdr_t *)l4hdr->data)->src_port);
                }
#endif  /* defined(MODULE_CONN_UDP) */
                if (addr != NULL) {
                    memcpy(addr, &((ipv6_hdr_t *)l3hdr->data)->src, sizeof(ipv6_addr_t));
                    *addr_len = sizeof(ipv6_addr_t);
                }
                memcpy(data, pkt->data, pkt->size);
                size = pkt->size;
                gnrc_pktbuf_release(pkt);
                return (int)size;
            default:
                (void)port;
                msg_send_to_self(&msg); /* requeue invalid messages */
                break;
        }
    }
    return -ETIMEDOUT;
}

#ifdef MODULE_GNRC_IPV6
bool gnrc_conn6_set_local_addr(uint8_t *conn_addr, const ipv6_addr_t *addr)
{
    ipv6_addr_t *tmp;
    if (!ipv6_addr_is_unspecified(addr) &&
        !ipv6_addr_is_loopback(addr) &&
        gnrc_ipv6_netif_find_by_addr(&tmp, addr) == KERNEL_PID_UNDEF) {
        return false;
    }
    else if (ipv6_addr_is_loopback(addr) || ipv6_addr_is_unspecified(addr)) {
        ipv6_addr_set_unspecified((ipv6_addr_t *)conn_addr);
    }
    else {
        memcpy(conn_addr, addr, sizeof(ipv6_addr_t));
    }
    return true;
}
#endif

/** @} */
