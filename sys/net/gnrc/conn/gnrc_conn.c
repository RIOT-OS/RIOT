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

#include "cib.h"
#include "net/conn.h"
#include "net/ipv6/hdr.h"
#include "net/gnrc/conn.h"
#include "net/gnrc/ipv6/hdr.h"
#include "net/gnrc/ipv6/netif.h"
#include "net/udp.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/**
 * Queue for buffering packets while conn is not read from
 */
#define CONN_BUFFER_QUEUE (8)

static msg_t _conn_buf[CONN_BUFFER_QUEUE];
static cib_t _conn_buf_cib;

void gnrc_conn_init(void)
{
    cib_init(&_conn_buf_cib, CONN_BUFFER_QUEUE);
}

int gnrc_conn_recvfrom(conn_t *conn, void *data, size_t max_len, void *addr, size_t *addr_len,
                       uint16_t *port)
{
    msg_t msg;
    /* TODO: this is a rather arbitrary value and creates the risk of starvation. */
    int timeout = 3;
    unsigned enqueued_msgs = cib_avail(&_conn_buf_cib);
    while ((timeout--) > 0) {
        gnrc_pktsnip_t *pkt, *l3hdr;
        size_t size = 0;
        int idx = -1;
        /* first check if there's packet waiting in the queue (and we have not
         * already iterated through all of them */
        if ((enqueued_msgs-- > 0) && (idx = cib_get(&_conn_buf_cib)) != -1) {
            DEBUG("Found queued message\n");
            memcpy(&msg, &_conn_buf[idx], sizeof(msg_t));
        }
        else {
            msg_receive(&msg);
        }
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
                    else if (byteorder_ntohs(((udp_hdr_t *)l4hdr->data)->dst_port) !=
                             (uint16_t)conn->netreg_entry.demux_ctx) {
                        /* if the packet in the queue does not match port
                         * number, we enqueue it again */
                        gnrc_conn_enqueue(&msg);
                        /* reset the timeout value to the previous one */
                        timeout++;
                        continue;
                    }
                    *port = byteorder_ntohs(((udp_hdr_t *)l4hdr->data)->src_port);
                }
#endif  /* defined(MODULE_CONN_UDP) */
                if (((ipv6_hdr_t *)l3hdr->data)->nh != (uint8_t)conn->netreg_entry.demux_ctx) {
                    /* if the packet in the queue does not match protocol
                     * number, we enqueue it again */
                    gnrc_conn_enqueue(&msg);
                    /* reset the timeout value to the previous one */
                    timeout++;
                    continue;
                }

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

int gnrc_conn_enqueue(msg_t *m)
{
    int n = cib_put(&_conn_buf_cib);
    if (n < 0) {
        DEBUG("gnrc_conn_enqueue(): conn buffer queue is full\n");
        return n;
    }

    DEBUG("gnrc_conn_enqueue(): queuing message\n");
    msg_t *q = &_conn_buf[n];
    *q = *m;
    return n;
}

/** @} */
