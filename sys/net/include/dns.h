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
 * @file        dns.h
 * @brief       DNS definition
 *
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 *
 */

#ifndef DNS_H_
#define DNS_H_

#include <stdint.h>

/**
 * @brief DNS server handle
 */
typedef struct {
	struct in6_addr server_address;
	unsigned short server_port;
} dns_t;

/**
 * @brief DNS query type
 */
enum dns_query_type {
	DNS_TYPE_A    = (1),
	DNS_TYPE_AAAA = (28)
};

/**
 * @brief initializes the dns client by setting the server IP and port
 *
 * @param[out] dns       dns context to initialize
 * @param[in] host       c string representation of the server IP address
 * @param[in] port       UDP port to use for dns packets
 */
void dns_init(dns_t *dns, char *ip, unsigned short port);

/**
 * @brief do a dns query
 *
 * @param[in] dns        dns context to use
 * @param[in] host       host to resolve
 * @param[in] type       dns type to resolve
 * @param[out] res_addr  dns resolution result
 *
 * @return       number of bytes written in res_addr
 *
 */
int dns_resolve(dns_t *dns, char *host, enum dns_query_type type, struct in6_addr *res_addr);

/** @} */
#endif /* DNS_H_ */
