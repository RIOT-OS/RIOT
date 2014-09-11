/*
 * @f ccnl-riot-compat.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "msg.h"
#include "thread.h"

#include "ieee802154_frame.h"

#include "ccnl.h"
#include "ccnl-core.h"
#include "ccnl-pdu.h"
#include "ccnl-riot-compat.h"

#if defined (MODULE_AT86RF231) || defined(MODULE_CC2420) || defined(MODULE_MC1322X)
ieee802154_packet_t p;
#else
radio_packet_t p;
#endif

transceiver_command_t tcmd;
blip_t mesg, rep;

char relay_helper_stack[KERNEL_CONF_STACKSIZE_MAIN];

int riot_send_transceiver(uint8_t *buf, uint16_t size, uint16_t to)
{
    DEBUGMSG(1, "this is a RIOT TRANSCEIVER based connection\n");
    DEBUGMSG(1, "size=%" PRIu16 " to=%" PRIu16 "\n", size, to);

    if (size > PAYLOAD_SIZE) {
        DEBUGMSG(1, "size > PAYLOAD_SIZE: %d > %d\n", size, PAYLOAD_SIZE);
        return 0;
    }

#if MODULE_AT86RF231 || MODULE_CC2420 || MODULE_MC1322X
    p.frame.payload_len = size;
    p.frame.fcf.dest_addr_m = IEEE_802154_SHORT_ADDR_M;
    memset(p.frame.dest_addr, 0, sizeof(p.frame.dest_addr));
    memcpy(&(p.frame.dest_addr[6]), &to, sizeof(uint16_t));
    p.frame.payload = buf;
#else
    p.length = size;
    p.dst = (to == RIOT_BROADCAST) ? 0 : to;
    p.data = buf;
#endif

    tcmd.transceivers = TRANSCEIVER;
    tcmd.data = &p;

    mesg.type = SND_PKT;
    mesg.content.ptr = (char *) &tcmd;
    blip_send_receive(&mesg, &rep, transceiver_pid);

    return size;
}

int riot_send_msg(uint8_t *buf, uint16_t size, uint16_t to)
{
    DEBUGMSG(1, "this is a RIOT MSG based connection\n");
    DEBUGMSG(1, "size=%" PRIu16 " to=%" PRIu16 "\n", size, to);

    uint8_t *buf2 = malloc(sizeof(riot_ccnl_msg_t) + size);
    if (!buf2) {
        DEBUGMSG(1, "  malloc failed...dorpping msg!\n");
        return 0;
    }

    riot_ccnl_msg_t *rmsg = (riot_ccnl_msg_t *) buf2;
    rmsg->payload = buf2 + sizeof(riot_ccnl_msg_t);
    rmsg->size = size;

    memcpy(rmsg->payload, buf, size);

    blip_t m;
    m.type = CCNL_RIOT_MSG;
    m.content.ptr = (char *) rmsg;
    DEBUGMSG(1, "sending msg to pid=%" PRIkernel_pid "\n", to);
    blip_send(&m, to, 1);

    return size;
}

void riot_send_nack(uint16_t to)
{
    blip_t m;
    m.type = CCNL_RIOT_NACK;
    DEBUGMSG(1, "sending NACK msg to pid=%" PRIkernel_pid"\n", to);
    blip_send(&m, to, 0);
}

void *ccnl_riot_relay_helper_start(void *);

kernel_pid_t riot_start_helper_thread(void)
{
    return thread_create(relay_helper_stack, sizeof(relay_helper_stack),
                         PRIORITY_MAIN - 2, CREATE_STACKTEST,
                         ccnl_riot_relay_helper_start, NULL, "relay-helper");
}

char *riot_ccnl_event_to_string(int event)
{
    switch (event) {
        case PKT_PENDING:
            return "PKT_PENDING";

        case CCNL_RIOT_MSG:
            return "RIOT_MSG";

        case CCNL_RIOT_HALT:
            return "RIOT_HALT";

        case CCNL_RIOT_POPULATE:
            return "RIOT_POPULATE";

        case CCNL_RIOT_PRINT_STAT:
            return "CCNL_RIOT_PRINT_STAT";

        case ENOBUFFER:
            return "ENOBUFFER";

        default:
            return "UNKNOWN";
    }
}
