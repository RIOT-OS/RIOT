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
#include "random.h"

#include "ccnl.h"
#include "ccnl-core.h"
#include "ccnl-riot-compat.h"
#include "ccn-lite-ctrl.h"
#include "ccnl-pdu.h"

int ccnl_riot_client_get(kernel_pid_t relay_pid, char *name, char *reply_buf)
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
        unsigned char *interest_pkg = malloc(PAYLOAD_SIZE);
        if (!interest_pkg) {
            puts("ccnl_riot_client_get: malloc failed");
            return 0;
        }
        unsigned int interest_nonce = genrand_uint32();
        int interest_len = mkInterest(prefix, &interest_nonce, interest_pkg);
        DEBUGMSG(1, "relay_pid=%" PRIkernel_pid " interest_len=%d\n", relay_pid, interest_len);

        riot_ccnl_msg_t rmsg;
        rmsg.payload = interest_pkg;
        rmsg.size = interest_len;

        blip_t m, rep;
        m.content.ptr = (char *) &rmsg;
        m.type = CCNL_RIOT_MSG;
        blip_send(&m, relay_pid, 1);

        /* ######################################################################### */

        blip_receive(&rep);
        free(interest_pkg);
        if (rep.type == CCNL_RIOT_NACK) {
            /* network stack was not able to fetch this chunk */
            return 0;
        }

        /* we got a chunk of data from the network stack */
        riot_ccnl_msg_t *rmsg_reply = (riot_ccnl_msg_t *) rep.content.ptr;

        unsigned char *data = rmsg_reply->payload;
        int datalen = (int) rmsg_reply->size;
        DEBUGMSG(1, "%d bytes left; msg from=%" PRIkernel_pid "\n", datalen, rep.sender_pid);

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

        DEBUGMSG(1, "contentlen=%d CCNL_RIOT_CHUNK_SIZE=%d\n", contlen, CCNL_RIOT_CHUNK_SIZE);
        if (contlen < CCNL_RIOT_CHUNK_SIZE || CCNL_RIOT_CHUNK_SIZE < contlen) {
            /* last chunk */
            break;
        }
    }

    return content_len;
}

int ccnl_riot_client_new_face(kernel_pid_t relay_pid, char *type, char *faceid,
                  unsigned char *reply_buf)
{
    DEBUGMSG(1, "riot_new_face: mkNewFaceRquest\n");
    int len = mkNewFaceRequest(reply_buf, type, NULL, NULL, faceid, NULL);

    riot_ccnl_msg_t rmsg;
    rmsg.payload = reply_buf;
    rmsg.size = len;

    blip_t m, rep;
    m.content.ptr = (char *) &rmsg;
    m.type = CCNL_RIOT_MSG;
    DEBUGMSG(1, "  sending face req to relay\n");
    blip_send(&m, relay_pid, 1);

    /* ######################################################################### */

    blip_receive(&rep);
    DEBUGMSG(1, "  received reply from relay\n");
    riot_ccnl_msg_t *rmsg_reply = (riot_ccnl_msg_t *) rep.content.ptr;
    memcpy(reply_buf, rmsg_reply->payload, rmsg_reply->size);
    int size = rmsg_reply->size;

    ccnl_free(rmsg_reply);

    return size;
}

int ccnl_riot_client_register_prefix(kernel_pid_t relay_pid, char *prefix, char *faceid,
                         unsigned char *reply_buf)
{
    DEBUGMSG(1, "riot_register_prefix: mkPrefixregRequest\n");
    int len = mkPrefixregRequest(reply_buf, 1, prefix, faceid);

    riot_ccnl_msg_t rmsg;
    rmsg.payload = reply_buf;
    rmsg.size = len;

    blip_t m, rep;
    m.content.ptr = (char *) &rmsg;
    m.type = CCNL_RIOT_MSG;
    DEBUGMSG(1, "  sending prefix req to relay\n");
    blip_send(&m, relay_pid, 1);

    /* ######################################################################### */

    blip_receive(&rep);
    DEBUGMSG(1, "  received reply from relay\n");
    riot_ccnl_msg_t *rmsg_reply = (riot_ccnl_msg_t *) rep.content.ptr;
    memcpy(reply_buf, rmsg_reply->payload, rmsg_reply->size);
    reply_buf[rmsg_reply->size] = '\0';
    int size = rmsg_reply->size;

    ccnl_free(rmsg_reply);

    return size;
}

int ccnl_riot_client_publish(kernel_pid_t relay_pid, char *prefix, char *faceid, char *type, unsigned char *reply_buf)
{
    ccnl_riot_client_new_face(relay_pid, type, faceid, reply_buf);
    int content_len = ccnl_riot_client_register_prefix(relay_pid, prefix, faceid, reply_buf);
    return content_len;
}
