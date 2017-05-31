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
 * @file        dns.h
 * @brief       DNS definition
 *
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 *
 */

#ifndef DNS_H_
#define DNS_H_

#include <stdint.h>

typedef struct {
    uint16_t type;
    uint16_t class;
} dns_querry_t;

typedef struct {
    uint16_t transaction_id;
    uint16_t flags;
    uint16_t questions;
    uint16_t answer_rrs;
    uint16_t authority_rrs;
    uint16_t additional_rrs;
} dns_packet_t;

#define DNS_RECURSIVE (1 << 8)

#define DNS_TYPE_A (1 << 0)
#define DNS_CLASS_IN (1 << 0)

#define DNS_PACKET_BUF_SIZE 64

#define DNS_SERVER_IP "::1" //"ff02::1:2"
#define DNS_SERVER_PORT 53

int dns_resolv(char *host, uint16_t type, uint8_t *res_buf);

/** @} */
#endif /* DNS_H_ */
