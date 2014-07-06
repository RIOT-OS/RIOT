/*
 * @f ccnl-ext-appserver.c
 *
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
 */

#define RIOT_CCN_APPSERVER (1)

#if RIOT_CCN_APPSERVER

#include "msg.h"
#include "thread.h"
#include "ccnl-riot-compat.h"
#include "ccn_lite/util/ccnl-riot-client.h"

#include "ccnl-includes.h"
#include "ccnl-core.h"
#include "ccnl-ext.h"
#include "ccnl-pdu.h"
#include "ccnx.h"

/** The size of the message queue between router daemon and transceiver AND clients */
#define APPSERVER_MSG_BUFFER_SIZE (64)

/** message buffer */
msg_t msg_buffer_appserver[APPSERVER_MSG_BUFFER_SIZE];

kernel_pid_t relay_pid;
char prefix[] = "/riot/appserver/";

static int appserver_sent_content(uint8_t *buf, int len, kernel_pid_t from)
{
    static riot_ccnl_msg_t rmsg;
    rmsg.payload = buf;
    rmsg.size = len;
    DEBUGMSG(1, "datalen=%d\n", rmsg.size);

    msg_t m;
    m.type = CCNL_RIOT_MSG;
    m.content.ptr = (char *) &rmsg;
    kernel_pid_t dest_pid = from;
    DEBUGMSG(1, "sending msg to pid=%" PRIkernel_pid "\n", dest_pid);
    int ret = msg_send(&m, dest_pid, 1);
    DEBUGMSG(1, "msg_reply returned: %d\n", ret);
    return ret;
}

static int appserver_create_content(char **prefix, uint8_t *out)
{
    char buf[CCNL_RIOT_CHUNK_SIZE - 1];

    for (int i = 0; i < CCNL_RIOT_CHUNK_SIZE - 1; i++) {
        buf[i] = 'a' + i%26;
    }

    int len = mkContent(prefix, buf, CCNL_RIOT_CHUNK_SIZE - 1, out);
    return len;
}

static int appserver_create_prefix(char *name, char **prefix)
{
    int i = 0;
    char *cp = strtok(name, "/");

    while (i < (CCNL_MAX_NAME_COMP - 1) && cp) {
        prefix[i++] = cp;
        cp = strtok(NULL, "/");
    }

    prefix[i] = NULL;

    return i;
}

static int appserver_handle_interest(char *data, uint16_t datalen, uint16_t from)
{
    (void) data;
    (void) datalen;

    char *prefix[CCNL_MAX_NAME_COMP];
    //struct ccnl_interest_s *i = appserver_parse_interest(data, datalen);

    char name[] = "/riot/appserver/test/0";
    appserver_create_prefix(name, prefix);

    unsigned char *content_pkg = malloc(PAYLOAD_SIZE);
    if (!content_pkg) {
        puts("appserver_handle_interest: malloc failed");
        return 0;
    }
    int len = appserver_create_content(prefix, content_pkg);
    /*
     struct ccnl_prefix *myprefix = ccnl_path_to_prefix(name);

     if (ccnl_prefix_cmp(myprefix, 0, i->prefix, CMP_EXACT) != CMP_EXACT) {
     DEBUGMSG(1, "APPSERVER: it's a match");
     }
     */
    int ret = appserver_sent_content(content_pkg, len, from);
    free(content_pkg);

    return ret;
}

static void riot_ccnl_appserver_ioloop(void)
{
    DEBUGMSG(1, "starting appserver main event and IO loop\n");

    if (msg_init_queue(msg_buffer_appserver, APPSERVER_MSG_BUFFER_SIZE) != 0) {
        DEBUGMSG(1, "msg init queue failed...abording\n");
    }

    msg_t in;
    riot_ccnl_msg_t *m;

    while (1) {
        DEBUGMSG(1, "appserver: waiting for incomming msg\n");
        msg_receive(&in);

        switch (in.type) {
            case (CCNL_RIOT_MSG):
                m = (riot_ccnl_msg_t *) in.content.ptr;
                DEBUGMSG(1, "new msg: size=%" PRIu16 " sender_pid=%" PRIkernel_pid "\n",
                         m->size, in.sender_pid);
                appserver_handle_interest(m->payload, m->size, in.sender_pid);

                ccnl_free(m);
                break;

            default:
                DEBUGMSG(1,
                         "received unknown msg type: '%" PRIu16 "' dropping it\n",
                         in.type);
                break;
        }
    }
}

static void riot_ccnl_appserver_register(void)
{
    char faceid[10];
    snprintf(faceid, sizeof(faceid), "%" PRIkernel_pid, thread_getpid());
    char *type = "newMSGface";

    unsigned char *mgnt_pkg = malloc(256);
    if (!mgnt_pkg) {
        puts("riot_ccnl_appserver_register: malloc failed");
        return;
    }
    int content_len = ccnl_riot_client_publish(relay_pid, prefix, faceid, type, mgnt_pkg);
    DEBUG("received %d bytes.\n", content_len);
    DEBUG("appserver received: '%s'\n", mgnt_pkg);

    free(mgnt_pkg);
}

void *ccnl_riot_appserver_start(void *arg)
{
	kernel_pid_t _relay_pid = (kernel_pid_t) arg;
    relay_pid = _relay_pid;
    riot_ccnl_appserver_register();
    riot_ccnl_appserver_ioloop();
    DEBUGMSG(1, "appserver terminated\n");
    return NULL;
}

#endif
