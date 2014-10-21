/*
 * Copyright (C) 2010 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */


/**
 * @defgroup    sys_ping Ping
 * @ingroup     sys
 * @brief       Ping
 */

#include "radio/radio.h"

void init_payload(void);
void ping_init(protocol_t protocol, uint8_t channr, radio_address_t addr);
void ping(radio_address_t addr, uint8_t channr);
void calc_rtt(void);
void print_success(void);
void print_failed(void);
void gpio_n_timer_init(void);
void adjust_timer(void);
static void ping_handler(void *payload, int payload_size,
                         packet_info_t *packet_info);
static void pong_handler(void *payload, int payload_size,
                         packet_info_t *packet_info);
void pong(uint16_t src);

typedef struct pong {
    int hopcount;
    int ttl;
    radio_address_t radio_address;
} ping_r;

typedef struct ping_payload {
    char *payload;
} ping_payload;
