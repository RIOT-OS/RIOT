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

#include "ccnl.h"
#include "ccnl-core.h"
#include "ccnl-pdu.h"
#include "ccnl-riot-compat.h"

radio_packet_t p;
transceiver_command_t tcmd;
msg_t mesg, rep;

int riot_send_transceiver(uint8_t *buf, uint16_t size, uint16_t to)
{
    DEBUGMSG(1, "this is a RIOT TRANSCEIVER based connection\n");
    DEBUGMSG(1, "size=%" PRIu16 " to=%" PRIu16 "\n", size, to);

    if (size > PAYLOAD_SIZE) {
        DEBUGMSG(1, "size > PAYLOAD_SIZE\n");
        return 0;
    }

    p.length = size;
    p.dst = (to == RIOT_BROADCAST) ? 0 : to;
    p.data = buf;

    tcmd.transceivers = transceiver_ids;
    tcmd.data = &p;

    mesg.type = SND_PKT;
    mesg.content.ptr = (char *) &tcmd;
    msg_send_receive(&mesg, &rep, transceiver_pid);

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

    msg_t m;
    m.type = CCNL_RIOT_MSG;
    m.content.ptr = (char *) rmsg;
    DEBUGMSG(1, "sending msg to pid=%u\n", to);
    msg_send(&m, to, 1);

    return size;
}

char *riot_ccnl_event_to_string(ccnl_riot_event_t event)
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

        default:
            return "UNKNOWN";
    }
}
