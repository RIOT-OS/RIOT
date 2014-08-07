/*
 * @f ccnl-riot-compat.h
 *
 * Copyright (C) 2013, Christian Mehlis, Freie University Berlin
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "ccn_lite/ccnl-riot.h"

#define RIOT_CCN_EVENT_NUMBER_OFFSET (1 << 8)

#define RIOT_BROADCAST (UINT16_MAX)

typedef struct riot_ccnl_msg {
    void *payload;
    uint16_t size;
} riot_ccnl_msg_t;

int riot_send_transceiver(uint8_t *buf, uint16_t size, uint16_t to);
int riot_send_msg(uint8_t *buf, uint16_t size, uint16_t to);
void riot_send_nack(uint16_t to);
kernel_pid_t riot_start_helper_thread(void);
char *riot_ccnl_event_to_string(int event);
