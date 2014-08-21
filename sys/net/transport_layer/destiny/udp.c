/**
 * Destiny UDP implementation
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup destiny
 * @{
 * @file    udp.c
 * @brief   UDP implementation
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "ipv6.h"
#include "msg.h"
#include "sixlowpan.h"
#include "thread.h"

#include "destiny/in.h"

#include "net_help.h"

#include "msg_help.h"
#include "socket.h"

#include "udp.h"

static char udp_queue_buf[MSG_QUEUE_SPACE(UDP_PKT_RECV_BUF_SIZE)];

uint16_t udp_csum(ipv6_hdr_t *ipv6_header, udp_hdr_t *udp_header)
{
    uint16_t sum;
    uint16_t len = NTOHS(udp_header->length);

    sum = len + IPPROTO_UDP;
    sum = csum(sum, (uint8_t *)&ipv6_header->srcaddr, 2 * sizeof(ipv6_addr_t));
    sum = csum(sum, (uint8_t *)udp_header, len);
    return (sum == 0) ? 0xffff : HTONS(sum);
}

void *udp_packet_handler(void *arg)
{
    (void) arg;

    msg_pulse_t m_recv_ip, m_send_ip, m_recv_udp, m_send_udp;
    ipv6_hdr_t *ipv6_header;
    udp_hdr_t *udp_header;
    socket_internal_t *udp_socket = NULL;
    uint16_t chksum;

    thread_msg_queue_init(udp_queue_buf, sizeof(udp_queue_buf), 0);

    while (1) {
        msg_receive_pulse(&m_recv_ip);
        ipv6_header = ((ipv6_hdr_t *)m_recv_ip.content.ptr);
        udp_header = ((udp_hdr_t *)(m_recv_ip.content.ptr + IPV6_HDR_LEN));

        chksum = ipv6_csum(ipv6_header, (uint8_t*) udp_header, NTOHS(udp_header->length), IPPROTO_UDP);

        if (chksum == 0xffff) {
            udp_socket = get_udp_socket(udp_header);

            if (udp_socket != NULL) {
                m_send_udp.content.ptr = (char *)ipv6_header;
                msg_send_receive_pulse(&m_send_udp, &m_recv_udp, udp_socket->recv_pid);
            }
            else {
                printf("Dropped UDP Message because no thread ID was found for delivery!\n");
            }
        }
        else {
            printf("Wrong checksum (%x)!\n", chksum);
        }

        msg_reply_pulse(&m_recv_ip, &m_send_ip);
    }
}
