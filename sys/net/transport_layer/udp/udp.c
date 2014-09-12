/*
 * Copyright (C) 2013, 2014 INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup udp
 * @{
 * @file    udp.c
 * @brief   UDP implementation
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 * @author  Cenk Gündoğan <cnkgndgn@gmail.com>
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "ipv6.h"
#include "msg.h"
#include "sixlowpan.h"
#include "thread.h"

#include "socket_base/in.h"

#include "net_help.h"

#include "msg_help.h"
#include "socket.h"

#include "udp.h"

msg_t udp_msg_queue[UDP_PKT_RECV_BUF_SIZE];

char udp_stack_buffer[UDP_STACK_SIZE];

uint16_t udp_csum(ipv6_hdr_t *ipv6_header, udp_hdr_t *udp_header)
{
    uint16_t sum;
    uint16_t len = NTOHS(udp_header->length);

    sum = len + IPPROTO_UDP;
    sum = csum(sum, (uint8_t *)&ipv6_header->srcaddr, 2 * sizeof(ipv6_addr_t));
    sum = csum(sum, (uint8_t *)udp_header, len);
    return (sum == 0) ? 0xffff : HTONS(sum);
}

socket_internal_t *get_udp_socket(udp_hdr_t *udp_header)
{
    uint8_t i = 1;

    while (i < MAX_SOCKETS + 1) {
        if (udp_socket_compliancy(i) &&
            (socket_base_get_socket(i)->socket_values.local_address.sin6_port ==
             udp_header->dst_port)) {
            return socket_base_get_socket(i);
        }

        i++;
    }

    return NULL;
}

void *udp_packet_handler(void *arg)
{
    (void) arg;

    msg_t m_recv_ip, m_send_ip, m_recv_udp, m_send_udp;
    socket_internal_t *udp_socket = NULL;

    msg_init_queue(udp_msg_queue, UDP_PKT_RECV_BUF_SIZE);

    while (1) {
        msg_receive(&m_recv_ip);
        ipv6_hdr_t *ipv6_header = ((ipv6_hdr_t *)m_recv_ip.content.ptr);
        udp_hdr_t *udp_header = ((udp_hdr_t *)(m_recv_ip.content.ptr + IPV6_HDR_LEN));

        uint16_t chksum = ipv6_csum(ipv6_header, (uint8_t*) udp_header, NTOHS(udp_header->length), IPPROTO_UDP);

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

bool udp_socket_compliancy(int s)
{
    if ((socket_base_exists_socket(s)) &&
        (socket_base_get_socket(s)->socket_values.domain == PF_INET6) &&
        (socket_base_get_socket(s)->socket_values.type == SOCK_DGRAM) &&
        ((socket_base_get_socket(s)->socket_values.protocol == IPPROTO_UDP) ||
         (socket_base_get_socket(s)->socket_values.protocol == 0))) {
        return true;
    }
    else {
        return false;
    }
}

int udp_bind_socket(int s, sockaddr6_t *name, int namelen, uint8_t pid)
{
    int i;

    if (!socket_base_exists_socket(s)) {
        return -1;
    }

    for (i = 1; i < MAX_SOCKETS + 1; i++) {
        if (udp_socket_compliancy(i) &&
            (socket_base_get_socket(i)->socket_values.local_address.sin6_port == name->sin6_port)) {
            return -1;
        }
    }

    memcpy(&socket_base_get_socket(s)->socket_values.local_address, name, namelen);
    socket_base_get_socket(s)->recv_pid = pid;
    return 0;
}

int32_t udp_recvfrom(int s, void *buf, uint32_t len, int flags, sockaddr6_t *from, uint32_t *fromlen)
{
    (void) flags;

    msg_t m_recv, m_send;
    ipv6_hdr_t *ipv6_header;
    udp_hdr_t *udp_header;
    uint8_t *payload;
    socket_base_get_socket(s)->recv_pid = thread_getpid();

    msg_receive(&m_recv);

    ipv6_header = ((ipv6_hdr_t *)m_recv.content.ptr);
    udp_header = ((udp_hdr_t *)(m_recv.content.ptr + IPV6_HDR_LEN));
    payload = (uint8_t *)(m_recv.content.ptr + IPV6_HDR_LEN + UDP_HDR_LEN);

    memset(buf, 0, len);
    memcpy(buf, payload, NTOHS(udp_header->length) - UDP_HDR_LEN);
    memcpy(&from->sin6_addr, &ipv6_header->srcaddr, 16);
    from->sin6_family = AF_INET6;
    from->sin6_flowinfo = 0;
    from->sin6_port = NTOHS(udp_header->src_port);
    *fromlen = sizeof(sockaddr6_t);

    msg_reply(&m_recv, &m_send);
    return NTOHS(udp_header->length) - UDP_HDR_LEN;
}

int32_t udp_sendto(int s, const void *buf, uint32_t len, int flags,
                              sockaddr6_t *to, uint32_t tolen)
{
    (void) flags;
    (void) tolen;

    if (udp_socket_compliancy(s) &&
        (socket_base_get_socket(s)->socket_values.foreign_address.sin6_port == 0)) {
        uint8_t send_buffer[BUFFER_SIZE];

        ipv6_hdr_t *temp_ipv6_header = ((ipv6_hdr_t *)(&send_buffer));
        udp_hdr_t *current_udp_packet = ((udp_hdr_t *)(&send_buffer[IPV6_HDR_LEN]));
        uint8_t *payload = &send_buffer[IPV6_HDR_LEN + UDP_HDR_LEN];

        memcpy(&(temp_ipv6_header->destaddr), &to->sin6_addr, 16);
        ipv6_net_if_get_best_src_addr(&(temp_ipv6_header->srcaddr), &(temp_ipv6_header->destaddr));

        current_udp_packet->src_port = socket_base_get_free_source_port(IPPROTO_UDP);
        current_udp_packet->dst_port = to->sin6_port;
        current_udp_packet->checksum = 0;

        memcpy(payload, buf, len);
        current_udp_packet->length = HTONS(UDP_HDR_LEN + len);
        temp_ipv6_header->length = UDP_HDR_LEN + len;

        current_udp_packet->checksum = ~ipv6_csum(temp_ipv6_header,
                                       (uint8_t *) current_udp_packet,
                                       UDP_HDR_LEN + len,
                                       IPPROTO_UDP);

        return ipv6_sendto(&to->sin6_addr, IPPROTO_UDP,
                           (uint8_t *)(current_udp_packet),
                           NTOHS(current_udp_packet->length));
    }
    else {
        return -1;
    }
}

int udp_init_transport_layer(void)
{
    printf("Initializing transport layer protocol: udp\n");
    /* SOCKETS */
    memset(socket_base_sockets, 0, MAX_SOCKETS * sizeof(socket_internal_t));

    int udp_thread_pid = thread_create(udp_stack_buffer, UDP_STACK_SIZE, PRIORITY_MAIN,
                                        CREATE_STACKTEST, udp_packet_handler, NULL, "udp_packet_handler");

    if (udp_thread_pid < 0) {
        return -1;
    }

    ipv6_register_next_header_handler(IPV6_PROTO_NUM_UDP, udp_thread_pid);

    return 0;
}
