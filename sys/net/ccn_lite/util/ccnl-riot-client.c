/*
 * @f ccnl-riot-client.c client side api for ccnl on RIOT
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

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>

#include "msg.h"

#include "ccnl.h"
#include "ccnl-core.h"
#include "ccnl-riot-compat.h"
#include "ccn-lite-ctrl.h"
#include "ccnl-pdu.h"

msg_t mesg, rep;

unsigned char compat_small_buf[PAYLOAD_SIZE];

int ccnl_riot_client_get(unsigned int relay_pid, char *name, char *reply_buf)
{
    char *prefix[CCNL_MAX_NAME_COMP];
    char *cp = strtok(name, "/");
    int i = 0;

    while (i < (CCNL_MAX_NAME_COMP - 1) && cp) {
        prefix[i++] = cp;
        cp = strtok(NULL, "/");
    }

    //prefix[i] = 0; //segment to request
    prefix[i + 1] = 0;

    int content_len = 0;

    for (int segment = 0; ; segment++) {
        char segment_string[16]; //max=999\0
        memset(segment_string, 0, 16);
        snprintf(segment_string, 16, "%d", segment);
        prefix[i] = segment_string;
        int interest_len = mkInterest(prefix, NULL, compat_small_buf);
        DEBUGMSG(1, "relay_pid=%u interest_len=%d\n", relay_pid, interest_len);

        riot_ccnl_msg_t rmsg;
        rmsg.payload = &compat_small_buf;
        rmsg.size = interest_len;

        msg_t m, rep;
        m.content.ptr = (char *) &rmsg;
        m.type = CCNL_RIOT_MSG;
        msg_send(&m, relay_pid, 1);

        /* ######################################################################### */

        msg_receive(&rep);
        riot_ccnl_msg_t *rmsg_reply = (riot_ccnl_msg_t *) rep.content.ptr;

        unsigned char *data = rmsg_reply->payload;
        int datalen = (int) rmsg_reply->size;
        DEBUGMSG(1, "%d bytes left; msg from=%u '%s'\n", datalen, rep.sender_pid, data);

        int scope = 3, aok = 3, minsfx = 0, maxsfx = CCNL_MAX_NAME_COMP,
            contlen = 0;
        struct ccnl_buf_s *buf = 0, *nonce = 0, *ppkd = 0;
        struct ccnl_prefix_s *p = 0;
        unsigned char *content = 0;

        buf = ccnl_extract_prefix_nonce_ppkd(&data, &datalen, &scope, &aok, &minsfx,
                                             &maxsfx, &p, &nonce, &ppkd, &content, &contlen);

        if (!buf) {
            DEBUGMSG(6, "  parsing error or no prefix\n");
            return 0;
        }

        DEBUGMSG(1, "content_len=%d contlen=%d\n", content_len, contlen);
        memcpy(reply_buf + content_len, content, contlen);
        content_len += contlen;

        free_prefix(p);
        free_3ptr_list(buf, nonce, ppkd);
        ccnl_free(rmsg_reply);

        if (contlen < CCNL_RIOT_CHUNK_SIZE || CCNL_RIOT_CHUNK_SIZE < contlen) {
            /* last chunk */
            break;
        }
    }

    return content_len;
}

int ccnl_riot_client_new_face(unsigned int relay_pid, char *type, char *faceid,
                  unsigned char *reply_buf)
{
    DEBUGMSG(1, "riot_new_face: mkNewFaceRquest\n");
    int len = mkNewFaceRequest(reply_buf, type, NULL, NULL, faceid, NULL);

    riot_ccnl_msg_t rmsg;
    rmsg.payload = reply_buf;
    rmsg.size = len;

    msg_t m, rep;
    m.content.ptr = (char *) &rmsg;
    m.type = CCNL_RIOT_MSG;
    DEBUGMSG(1, "  sending face req to relay\n");
    msg_send(&m, relay_pid, 1);

    /* ######################################################################### */

    msg_receive(&rep);
    DEBUGMSG(1, "  received reply from relay\n");
    riot_ccnl_msg_t *rmsg_reply = (riot_ccnl_msg_t *) rep.content.ptr;
    memcpy(reply_buf, rmsg_reply->payload, rmsg_reply->size);
    return rmsg_reply->size;
}

int ccnl_riot_client_register_prefix(unsigned int relay_pid, char *prefix, char *faceid,
                         unsigned char *reply_buf)
{
    DEBUGMSG(1, "riot_register_prefix: mkPrefixregRequest\n");
    int len = mkPrefixregRequest(reply_buf, 1, prefix, faceid);

    riot_ccnl_msg_t rmsg;
    rmsg.payload = reply_buf;
    rmsg.size = len;

    msg_t m, rep;
    m.content.ptr = (char *) &rmsg;
    m.type = CCNL_RIOT_MSG;
    DEBUGMSG(1, "  sending prefix req to relay\n");
    msg_send(&m, relay_pid, 1);

    /* ######################################################################### */

    msg_receive(&rep);
    DEBUGMSG(1, "  received reply from relay\n");
    riot_ccnl_msg_t *rmsg_reply = (riot_ccnl_msg_t *) rep.content.ptr;
    memcpy(reply_buf, rmsg_reply->payload, rmsg_reply->size);
    reply_buf[rmsg_reply->size] = '\0';

    return rmsg_reply->size;
}

int ccnl_riot_client_publish(unsigned int relay_pid, char *prefix, char *faceid, char *type, unsigned char *reply_buf)
{
    ccnl_riot_client_new_face(relay_pid, type, faceid, reply_buf);
    int content_len = ccnl_riot_client_register_prefix(relay_pid, prefix, faceid, reply_buf);
    return content_len;
}
