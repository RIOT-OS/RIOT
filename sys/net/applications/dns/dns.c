/*
 * Copyright (C) 2014  Christian Mehlis  <mehlis@inf.fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "dns.h"
#include "dns_internal.h"

#define IN6ADDRSZ 16

static int dns_send_receive(dns_t *dns, uint8_t *send_buf, int send_buf_len,
		uint8_t *receive_buf, int receive_buf_len) {
	int sock;
	struct sockaddr_in6 server_addr = {0};

	/* create a DGRAM (UDP) socket in the INET6 (IPv6) protocol */
	sock = socket(AF_INET6, SOCK_DGRAM, 0);

	if (sock < 0) {
		printf("creating socket failed");
		exit(1);
	}

	/* create server address: where we want to send to */

	/* it is an INET address */
	server_addr.sin6_family = AF_INET6;

	/* copy server address in packet */
	server_addr.sin6_addr = dns->server_address;

	/* the port we are going to send to, in network byte order */
	server_addr.sin6_port = htons(dns->server_port);

	sendto(sock, send_buf, send_buf_len, 0, (struct sockaddr *) &server_addr,
			sizeof(server_addr));

	int n = recvfrom(sock, receive_buf, receive_buf_len, 0, NULL, NULL);

	return n;
}

static int dns_copy_host(char *buf, char *host)
{
	char host_copy[64];
	strcpy(host_copy, host);

	int pos = 0;
	char *save_ptr;
	char *pch = strtok_r(host_copy, ".", &save_ptr);

	while (pch != NULL) {
		/* every part of the name */
		/* write part length */
		buf[pos++] = strlen(pch);
		/* write part */
		strcpy(buf+pos, pch);
		pos += strlen(pch);
		pch = strtok_r(NULL, ".", &save_ptr);
	}

	/* terminate with 0 */
	buf[pos++] = 0;
	return pos;
}

static int dns_packet(dns_query_packet_t *p, char *host, dns_query_t *q, uint8_t *buf) {
	int pos = 0;

	memcpy(buf + pos, p, sizeof(dns_query_packet_t));
	pos += sizeof(dns_query_packet_t);

	pos += dns_copy_host((char *) buf + pos, host);

	memcpy(buf + pos, q, sizeof(dns_query_t));
	pos += sizeof(dns_query_t);

	return pos;
}

static int dns_binary(uint8_t *rep, struct in6_addr *res_ip)
{
	/* TODO check for type */

	int res_len = IN6ADDRSZ;
	memcpy(res_ip, rep + sizeof(dns_response_packet_t) - 4, res_len);
	return res_len;
}

void dns_init(dns_t *dns, char *ip, unsigned short port)
{
	/* set the server IP address, in network byte order */
	inet_pton(AF_INET6, ip, &dns->server_address);
	dns->server_port = port;
}

int dns_resolve(dns_t *dns, char *host, enum dns_query_type type, struct in6_addr *res_ip) {
	dns_query_packet_t p;
	p.transaction_id = htons(9191); /* random */
	p.flags = htons(DNS_RECURSIVE | DNS_AD);
	p.questions = htons(1);
	p.answer_rrs = 0;
	p.authority_rrs = 0;
	p.additional_rrs = 0;

	dns_query_t q;
	q.type = htons(type);
	q.class = htons(DNS_CLASS_IN);

	uint8_t send_buf[DNS_PACKET_BUF_SIZE] = {0};
	int query_len = dns_packet(&p, host, &q, send_buf);

	uint8_t receive_buf[DNS_PACKET_BUF_SIZE] = {0};
	int answer_len = dns_send_receive(dns, send_buf, query_len, receive_buf,
	DNS_PACKET_BUF_SIZE);

	int res_len = dns_binary(receive_buf + query_len, res_ip);
	return res_len;
}

/** @} */
