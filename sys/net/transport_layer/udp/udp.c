/**
 * UDP implementation
 *
 * Copyright (C) 2014  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup socket 
 * @{
 * @file    udp.c
 * @brief   UDP implementation
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "ipv6.h"
#include "msg.h"
#include "sixlowpan.h"
#include "thread.h"

#include "tl_socket/in.h"

#include "net_help.h"

#include "msg_help.h"
#include "socket_internal.h"

#include "udp.h"

static char udp_stack_buffer[UDP_STACK_SIZE];

static msg_t udp_msg_queue[UDP_PKT_RECV_BUF_SIZE];

int udp_init(void)
{
    printf("Initializing UDP transport layer. Size of socket_type: %u\n",
           (unsigned int) sizeof(socket_internal_t));
    /* UDP */
    int udp_thread_pid = thread_create(udp_stack_buffer, UDP_STACK_SIZE,
                                       PRIORITY_MAIN, CREATE_STACKTEST,
                                       udp_packet_handler, "udp_packet_handler");

    if (udp_thread_pid < 0) {
        return -1;
    }

    ipv6_register_next_header_handler(IPV6_PROTO_NUM_UDP, udp_thread_pid);

    return 0;
}

int udp_bind_socket(int s, sockaddr6_t *name, int namelen, uint8_t pid)
{
    int i;

    if (!socket_exist(s)) {
        return -1;
    }

    for (i = 1; i < MAX_SOCKETS + 1; i++) {
        if (socket_is_udp(i) &&
            (get_socket(i)->socket_values.local_address.sin6_port == name->sin6_port)) {
            return -1;
        }
    }

    memcpy(&get_socket(s)->socket_values.local_address, name, namelen);
    get_socket(s)->recv_pid = pid;
    return 0;
}

socket_internal_t *get_udp_socket(udp_hdr_t *udp_header)
{
    uint8_t i = 1;

    while (i < MAX_SOCKETS + 1) {
        if (socket_is_udp(i) &&
            (get_socket(i)->socket_values.local_address.sin6_port ==
             udp_header->dst_port)) {
            return get_socket(i);
        }

        i++;
    }

    return NULL;
}


uint16_t udp_csum(ipv6_hdr_t *ipv6_header, udp_hdr_t *udp_header)
{
    uint16_t sum;
    uint16_t len = NTOHS(udp_header->length);

    sum = len + IPPROTO_UDP;
    sum = csum(sum, (uint8_t *)&ipv6_header->srcaddr, 2 * sizeof(ipv6_addr_t));
    sum = csum(sum, (uint8_t *)udp_header, len);
    return (sum == 0) ? 0xffff : HTONS(sum);
}

void udp_packet_handler(void)
{
    msg_t m_recv_ip, m_send_ip, m_recv_udp, m_send_udp;
    ipv6_hdr_t *ipv6_header;
    udp_hdr_t *udp_header;
    socket_internal_t *udp_socket = NULL;
    uint16_t chksum;

    msg_init_queue(udp_msg_queue, UDP_PKT_RECV_BUF_SIZE);

    while (1) {
        msg_receive(&m_recv_ip);
        ipv6_header = ((ipv6_hdr_t *)m_recv_ip.content.ptr);
        udp_header = ((udp_hdr_t *)(m_recv_ip.content.ptr + IPV6_HDR_LEN));

        chksum = ipv6_csum(ipv6_header, (uint8_t*) udp_header, NTOHS(udp_header->length), IPPROTO_UDP);

        if (chksum == 0xffff) {
            udp_socket = get_udp_socket(udp_header);

            if (udp_socket != NULL) {
                m_send_udp.content.ptr = (char *)ipv6_header;
                msg_send_receive(&m_send_udp, &m_recv_udp, udp_socket->recv_pid);
            }
            else {
                printf("Dropped UDP Message because no thread ID was found for delivery!\n");
            }
        }
        else {
            printf("Wrong checksum (%x)!\n", chksum);
        }

        msg_reply(&m_recv_ip, &m_send_ip);
    }
}
