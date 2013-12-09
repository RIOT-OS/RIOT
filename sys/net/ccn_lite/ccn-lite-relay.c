/*
 * @f ccn-lite-relay.c
 * @b CCN relay
 *
 * Copyright (C) 2011-13, Christian Tschudin, University of Basel
 * Copyright (C) 2013, Christian Mehlis, Freie Universität Berlin
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
 *
 * File history:
 * 2011-11-22 created
 */

#include <inttypes.h>

#define CCNL_RIOT

#define USE_CCNxDIGEST
#define USE_RIOT_MSG
#define USE_RIOT_TRANS
#define USE_APPSERVER
#define USE_MGMT
#define RIOT_CCNL_POPULATE (1)

#include "ccnl-includes.h"

#include "ccnx.h"
#include "ccnl.h"
#include "ccnl-core.h"

#include "ccnl-ext.h"
#include "ccnl-platform.h"

#include "ccnl-core.h"

#include "ccnl-pdu.h"

#include "msg.h"
#include "thread.h"
#include "transceiver.h"

#include "ccnl-riot-compat.h"
#include "test_data/text.txt.ccnb.h"

/** The size of the message queue between router daemon and transceiver AND clients */
#define RELAY_MSG_BUFFER_SIZE (64)

/** message buffer */
msg_t msg_buffer_relay[RELAY_MSG_BUFFER_SIZE];

// ----------------------------------------------------------------------

struct ccnl_relay_s theRelay;

struct timeval *
ccnl_run_events(void)
{
    static struct timeval now;
    long usec;

    rtc_time(&now);
    DEBUGMSG(1, "ccnl_run_events now: %ld:%ld\n", now.tv_sec, now.tv_usec);

    while (eventqueue) {
        struct ccnl_timer_s *t = eventqueue;
        usec = timevaldelta(&(t->timeout), &now);

        if (usec >= 0) {
            DEBUGMSG(1, "ccnl_run_events nothing to do: %ld:%ld\n", now.tv_sec, now.tv_usec);
            now.tv_sec = usec / 1000000;
            now.tv_usec = usec % 1000000;
            return &now;
        }

        DEBUGMSG(1, "ccnl_run_events run event handler: %ld:%ld\n", now.tv_sec, now.tv_usec);
        if (t->fct) {
            (t->fct)(t->node, t->intarg);
        }
        else if (t->fct2) {
            (t->fct2)(t->aux1, t->aux2);
        }

        eventqueue = t->next;
        ccnl_free(t);
    }

    return NULL;
}

// ----------------------------------------------------------------------

int ccnl_open_riotmsgdev(void)
{
    /*
     * nothing to do here, msg system just needs a buffer, and this is
     * generated staticly
     */
    return RIOT_MSG_DEV; /* sock id */
}

int ccnl_open_riottransdev(void)
{

    transceiver_init(transceiver_ids);
    transceiver_start();

    /** register for transceiver events */
    transceiver_register(transceiver_ids, thread_getpid());

    return RIOT_TRANS_DEV; /* sock id */
}

void ccnl_ll_TX(struct ccnl_relay_s *ccnl, struct ccnl_if_s *ifc,
                sockunion *dest, struct ccnl_buf_s *buf)
{
    (void) ccnl; /* unused */

    ifc->sendfunc(buf->data, (uint16_t) buf->datalen, (uint16_t) dest->id);
}

// ----------------------------------------------------------------------

void ccnl_ageing(void *relay, void *aux)
{
    ccnl_do_ageing(relay, aux);
    ccnl_set_timer(1000000, ccnl_ageing, relay, 0);
}

// ----------------------------------------------------------------------

void ccnl_relay_config(struct ccnl_relay_s *relay, int max_cache_entries)
{
    struct ccnl_if_s *i;

    DEBUGMSG(99, "ccnl_relay_config\n");

    relay->max_cache_entries = max_cache_entries;

    if (RIOT_MSG_IDX != relay->ifcount) {
        DEBUGMSG(1, "sorry, idx did not match: riot msg device\n");
    }

    i = &relay->ifs[relay->ifcount];
    i->sock = ccnl_open_riotmsgdev();
    i->sendfunc = &riot_send_msg;
    i->mtu = 4000;
    i->reflect = 0;
    i->fwdalli = 0;

    if (i->sock >= 0) {
        relay->ifcount++;

        if (relay->defaultInterfaceScheduler) {
            i->sched = relay->defaultInterfaceScheduler(relay,
                       ccnl_interface_CTS);
        }
    }
    else {
        DEBUGMSG(1, "sorry, could not open riot msg device\n");
    }

    if (RIOT_TRANS_IDX != relay->ifcount) {
        DEBUGMSG(1, "sorry, idx did not match: riot trans device\n");
    }

    i = &relay->ifs[relay->ifcount];
    i->sock = ccnl_open_riottransdev();
    i->sendfunc = &riot_send_transceiver;
#ifdef USE_FRAG
    i->mtu = 120;
#else
    i->mtu = 1500;
#endif
    i->reflect = 0;
    i->fwdalli = 0;

    if (i->sock >= 0) {
        relay->ifcount++;

        if (relay->defaultInterfaceScheduler) {
            i->sched = relay->defaultInterfaceScheduler(relay,
                       ccnl_interface_CTS);
        }
    }
    else {
        DEBUGMSG(1, "sorry, could not open riot trans device\n");
    }

    ccnl_set_timer(1000000, ccnl_ageing, relay, 0);
}

#if RIOT_CCNL_POPULATE

void ccnl_populate_cache(struct ccnl_relay_s *ccnl, unsigned char *buf, int datalen)
{
    if (buf[0] == 0x04 && buf[1] == 0x82) {
        struct ccnl_prefix_s *prefix = 0;
        struct ccnl_content_s *c = 0;
        struct ccnl_buf_s *nonce = 0, *ppkd = 0, *pkt = 0;
        unsigned char *content, *data = buf + 2;
        int contlen;

        datalen -= 2;

        pkt = ccnl_extract_prefix_nonce_ppkd(&data, &datalen, 0, 0,
                                             0, 0, &prefix, &nonce, &ppkd, &content, &contlen);

        if (!pkt) {
            DEBUGMSG(6, "  parsing error\n");
            goto Done;
        }

        if (!prefix) {
            DEBUGMSG(6, "  no prefix error\n");
            goto Done;
        }

        printf("populating: %s\n", ccnl_prefix_to_path(prefix));

        c = ccnl_content_new(ccnl, &pkt, &prefix, &ppkd, content,
                             contlen);

        if (!c) {
            goto Done;
        }

        ccnl_content_add2cache(ccnl, c);
        c->flags |= CCNL_CONTENT_FLAGS_STATIC;
    Done:
        free_prefix(prefix);
        ccnl_free(pkt);
        ccnl_free(nonce);
        ccnl_free(ppkd);
    }
    else {
        DEBUGMSG(6, "  not a content object\n");
    }
}

void handle_populate_cache(void)
{
    DEBUGMSG(1, "ccnl_populate_cache with: text_txt_ccnb\n");
    ccnl_populate_cache(&theRelay, (unsigned char *) text_txt_ccnb_0, (int) text_txt_ccnb_0_len);
    ccnl_populate_cache(&theRelay, (unsigned char *) text_txt_ccnb_1, (int) text_txt_ccnb_1_len);
    ccnl_populate_cache(&theRelay, (unsigned char *) text_txt_ccnb_2, (int) text_txt_ccnb_2_len);
    ccnl_populate_cache(&theRelay, (unsigned char *) text_txt_ccnb_3, (int) text_txt_ccnb_3_len);
    ccnl_populate_cache(&theRelay, (unsigned char *) text_txt_ccnb_4, (int) text_txt_ccnb_4_len);
    ccnl_populate_cache(&theRelay, (unsigned char *) text_txt_ccnb_5, (int) text_txt_ccnb_5_len);
    ccnl_populate_cache(&theRelay, (unsigned char *) text_txt_ccnb_6, (int) text_txt_ccnb_6_len);
    ccnl_populate_cache(&theRelay, (unsigned char *) text_txt_ccnb_7, (int) text_txt_ccnb_7_len);
    ccnl_populate_cache(&theRelay, (unsigned char *) text_txt_ccnb_8, (int) text_txt_ccnb_8_len);
    ccnl_populate_cache(&theRelay, (unsigned char *) text_txt_ccnb_9, (int) text_txt_ccnb_9_len);
    ccnl_populate_cache(&theRelay, (unsigned char *) text_txt_ccnb_10, (int) text_txt_ccnb_10_len);
    ccnl_populate_cache(&theRelay, (unsigned char *) text_txt_ccnb_11, (int) text_txt_ccnb_11_len);
    ccnl_populate_cache(&theRelay, (unsigned char *) text_txt_ccnb_12, (int) text_txt_ccnb_12_len);
    ccnl_populate_cache(&theRelay, (unsigned char *) text_txt_ccnb_13, (int) text_txt_ccnb_13_len);
    ccnl_populate_cache(&theRelay, (unsigned char *) text_txt_ccnb_14, (int) text_txt_ccnb_14_len);
    ccnl_populate_cache(&theRelay, (unsigned char *) text_txt_ccnb_15, (int) text_txt_ccnb_15_len);
    ccnl_populate_cache(&theRelay, (unsigned char *) text_txt_ccnb_16, (int) text_txt_ccnb_16_len);
    ccnl_populate_cache(&theRelay, (unsigned char *) text_txt_ccnb_17, (int) text_txt_ccnb_17_len);
    ccnl_populate_cache(&theRelay, (unsigned char *) text_txt_ccnb_18, (int) text_txt_ccnb_18_len);
    ccnl_populate_cache(&theRelay, (unsigned char *) text_txt_ccnb_19, (int) text_txt_ccnb_19_len);
    ccnl_populate_cache(&theRelay, (unsigned char *) text_txt_ccnb_20, (int) text_txt_ccnb_20_len);
    ccnl_populate_cache(&theRelay, (unsigned char *) text_txt_ccnb_21, (int) text_txt_ccnb_21_len);
    ccnl_populate_cache(&theRelay, (unsigned char *) text_txt_ccnb_22, (int) text_txt_ccnb_22_len);
    ccnl_populate_cache(&theRelay, (unsigned char *) text_txt_ccnb_23, (int) text_txt_ccnb_23_len);
    ccnl_populate_cache(&theRelay, (unsigned char *) text_txt_ccnb_24, (int) text_txt_ccnb_24_len);
    ccnl_populate_cache(&theRelay, (unsigned char *) text_txt_ccnb_25, (int) text_txt_ccnb_25_len);
}

#endif

// ----------------------------------------------------------------------

int ccnl_io_loop(struct ccnl_relay_s *ccnl)
{
    int i, maxfd = -1;

    if (ccnl->ifcount == 0) {
        DEBUGMSG(1, "no socket to work with, not good, quitting\n");
        return -1;
    }

    for (i = 0; i < ccnl->ifcount; i++)
        if (ccnl->ifs[i].sock > maxfd) {
            maxfd = ccnl->ifs[i].sock;
        }

    maxfd++;

    DEBUGMSG(1, "starting main event and IO loop\n");

    if (msg_init_queue(msg_buffer_relay, RELAY_MSG_BUFFER_SIZE) != 0) {
        DEBUGMSG(1, "msg init queue failed...abording\n");
        return -1;
    }

    msg_t in;
    radio_packet_t *p;
    riot_ccnl_msg_t *m;

    while (!ccnl->halt_flag) {
        DEBUGMSG(1, "waiting for incomming msg\n");
        msg_receive(&in);
        struct timeval *timeout = ccnl_run_events();

        switch (in.type) {
            case PKT_PENDING:
                p = (radio_packet_t *) in.content.ptr;
                DEBUGMSG(1, "%s Packet waiting\n", riot_ccnl_event_to_string(in.type));
                DEBUGMSG(1, "\tLength:\t%u\n", p->length);
                DEBUGMSG(1, "\tSrc:\t%u\n", p->src);
                DEBUGMSG(1, "\tDst:\t%u\n", p->dst);

                // p->src must be > 0
                if (!p->src) {
                    p->src = RIOT_BROADCAST;
                }

                ccnl_core_RX(ccnl, RIOT_TRANS_IDX, (unsigned char *) p->data, (int) p->length, p->src);
                p->processing--;
                break;

            case (CCNL_RIOT_MSG):
                m = (riot_ccnl_msg_t *) in.content.ptr;
                DEBUGMSG(1, "%s Packet waiting\n", riot_ccnl_event_to_string(in.type));
                DEBUGMSG(1, "\tLength:\t%u\n", m->size);
                DEBUGMSG(1, "\tSrc:\t%u\n", in.sender_pid);

                ccnl_core_RX(ccnl, RIOT_MSG_IDX, (unsigned char *) m->payload, m->size,
                             in.sender_pid);
                break;

            case (CCNL_RIOT_HALT):
                DEBUGMSG(1, "%s Packet waiting\n", riot_ccnl_event_to_string(in.type));
                DEBUGMSG(1, "\tSrc:\t%u\n", in.sender_pid);
                DEBUGMSG(1, "\tNumb:\t%" PRIu32 "\n", in.content.value);

                ccnl->halt_flag = 1;
                break;
#if RIOT_CCNL_POPULATE
            case (CCNL_RIOT_POPULATE):
                DEBUGMSG(1, "%s Packet waiting\n", riot_ccnl_event_to_string(in.type));
                DEBUGMSG(1, "\tSrc:\t%u\n", in.sender_pid);
                DEBUGMSG(1, "\tNumb:\t%" PRIu32 "\n", in.content.value);

                handle_populate_cache();
                break;
#endif
            default:
                DEBUGMSG(1, "%s Packet waiting\n", riot_ccnl_event_to_string(in.type));
                DEBUGMSG(1, "\tSrc:\t%u\n", in.sender_pid);
                DEBUGMSG(1, "\tdropping it...\n");
                break;
        }
    }

    return 0;
}

/**
 * @brief initializing routing system
 * @param pointer to count transceiver pids
 *
 */
void ccnl_riot_relay_start(int max_cache_entries)
{
    struct timeval now;
    theRelay.startup_time = rtc_time(&now);

    DEBUGMSG(1, "This is ccn-lite-relay, starting at %lu:%lu\n", now.tv_sec, now.tv_usec);
    DEBUGMSG(1, "  compile time: %s %s\n", __DATE__, __TIME__);
    DEBUGMSG(1, "  max_cache_entries: %d\n", max_cache_entries);
    DEBUGMSG(1, "  compile options: %s\n", compile_string());

    ccnl_relay_config(&theRelay, max_cache_entries);

    ccnl_io_loop(&theRelay);
    DEBUGMSG(1, "ioloop stopped\n");

    while (eventqueue) {
        ccnl_rem_timer(eventqueue);
    }

    ccnl_core_cleanup(&theRelay);
}

// eof
