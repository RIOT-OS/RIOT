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
 * @file        dns_internal.h
 * @brief       DNS definition
 *
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 *
 */

#ifndef DNS_INTERNAL_H_
#define DNS_INTERNAL_H_

#include <stdint.h>

typedef struct {
	uint16_t type;
	uint16_t class;
} dns_query_t;

typedef struct {
	uint16_t transaction_id;
	uint16_t flags;
	uint16_t questions;
	uint16_t answer_rrs;
	uint16_t authority_rrs;
	uint16_t additional_rrs;
} dns_query_packet_t;

typedef struct {
	uint16_t name;
	uint16_t type;
	uint16_t class;
	uint32_t ttl;
	uint16_t data_len;
} dns_response_packet_t;

#define DNS_RECURSIVE (1 << 8)
#define DNS_AD (1 << 5)

#define DNS_CLASS_IN (1)

#define DNS_PACKET_BUF_SIZE 128

/** @} */
#endif /* DNS_INTERNAL_H_ */
