/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @addtogroup  net
 * @{
 *
 * @file        dns.c
 * @brief       DNS
 *
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 *
 */

#include <string.h>
#include <arpa/inet.h>

#include "dns.h"

#define IN6ADDRSZ 16

static int dns_send_receive(uint8_t *send_buf, int send_buf_len, uint8_t *receive_buf, int receive_buf_len)
{
    int sockfd, n;
    struct sockaddr_in6 servaddr, cliaddr;

    sockfd = socket(AF_INET6, SOCK_DGRAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin6_family = AF_INET6;
    uint8_t ip_buf[IN6ADDRSZ];
    inet_pton(AF_INET6, DNS_SERVER_IP, &ip_buf);
    memcpy(&servaddr.sin6_addr, &ip_buf, IN6ADDRSZ);
    servaddr.sin6_port = htons(DNS_SERVER_PORT);

    sendto(sockfd, send_buf, send_buf_len, 0, (struct sockaddr *) &servaddr, sizeof(servaddr));

    n = recvfrom(sockfd, receive_buf, receive_buf_len, 0, NULL, NULL);

    return n;
}

static int dns_packet(dns_packet_t *p, char *host, dns_querry_t *q, uint8_t *buf, int buf_size)
{
    int pos = 0;

    memcpy(buf + pos, p, sizeof(dns_packet_t));
    pos += sizeof(dns_packet_t);

    int host_len = strlen(host);

    if (pos + host_len >= buf_size) {
        return -1;
    }

    buf[pos] = host_len;
    pos += 1;

    memcpy(buf + pos, host, host_len);
    pos += host_len;

    if (pos - 4 >= buf_size) {
        return -1;
    }

    dns_querry_t *qb = (dns_querry_t *) &buf[pos];
    memcpy(qb, q, sizeof(dns_querry_t));
    pos += sizeof(dns_querry_t);

    return pos;
}

static int dns_binary(uint8_t *buf, int buf_size, uint8_t *res_buf)
{
    return 0;
}

int dns_resolv(char *host, uint16_t type, uint8_t *res_buf)
{
    dns_packet_t p;
    p.transaction_id = htons(9191); /* random */
    p.flags = htons(DNS_RECURSIVE);
    p.questions = htons(1);
    p.answer_rrs = 0;
    p.authority_rrs = 0;
    p.additional_rrs = 0;

    dns_querry_t q;
    q.type = htons(type);
    q.class = htons(DNS_CLASS_IN);

    uint8_t send_buf[DNS_PACKET_BUF_SIZE];
    int querry_len = dns_packet(&p, host, &q, send_buf, DNS_PACKET_BUF_SIZE);

    uint8_t receive_buf[DNS_PACKET_BUF_SIZE];
    int answer_len = dns_send_receive(send_buf, DNS_PACKET_BUF_SIZE, receive_buf, DNS_PACKET_BUF_SIZE);

    int ret = dns_binary(receive_buf, DNS_PACKET_BUF_SIZE, res_buf);

    return ret;
}

/** @} */
