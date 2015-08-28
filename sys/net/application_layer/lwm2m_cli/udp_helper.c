/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       UART helper function implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stddef.h>
#include <stdint.h>

#include "log.h"
#include "net/gnrc.h"
#include "net/gnrc/ipv6.h"
//#include "net/ipv6/addr.h"
//#include "net/ipv6/hdr.h"
#include "net/gnrc/udp.h"

size_t udp_helper_send(ipv6_addr_t *addr, uint16_t port,
                       uint8_t *data, size_t len)
{
    uint8_t p[2];
    gnrc_pktsnip_t *payload, *udp, *ip;

    /* put port into network byteorder */
    p[0] = port & 0xff;
    p[1] = port >> 8;

    /* allocate payload */
    payload = gnrc_pktbuf_add(NULL, data, len, GNRC_NETTYPE_UNDEF);
    if (payload == NULL) {
        LOG_INFO("[udp_helper] unable to allocate payload\n");
        return 0;
    }
    /* allocate UDP header, set source port := destination port */
    udp = gnrc_udp_hdr_build(payload, p, 2, p, 2);
    if (udp == NULL) {
        LOG_INFO("[udp_helper] unable to allocate UDP header\n");
        gnrc_pktbuf_release(payload);
        return 0;
    }
    /* allocate IPv6 header */
    ip = gnrc_ipv6_hdr_build(udp, NULL, 0, (uint8_t *)addr, sizeof(ipv6_addr_t));
    if (ip == NULL) {
        LOG_INFO("[udp_helper] unable to allocate IPv6 header\n");
        gnrc_pktbuf_release(udp);
        return 0;
    }

    /* send out the packet */
    gnrc_netapi_dispatch_send(GNRC_NETTYPE_UDP, GNRC_NETREG_DEMUX_CTX_ALL, ip);
    return len;
}
