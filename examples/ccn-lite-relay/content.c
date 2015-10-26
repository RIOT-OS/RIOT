/*
 * Copyright (C) 2015 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>
#include <string.h>

#include "ccn-lite-riot.h"
#include "ccnl-pkt-ndntlv.h"
#include "ccnl-defs.h"
#include "ccnl-ext.h"

#include "ccnl-pkt-ndntlv.h"

#define MAX_CONTENT (64)

static char *_default_content = "Start the RIOT!";
static unsigned char _out[CCNL_MAX_PACKET_SIZE];

static void _usage(char *argv)
{
    printf("usage: %s <URI> [content]\n"
            "%% %s /riot/peter/schmerzl             (default content)\n"
            "%% %s /riot/peter/schmerzl RIOT\n",
            argv, argv, argv);
}


int _ccnl_content(int argc, char **argv)
{
    char *body = _default_content;
    int arg_len = strlen(_default_content) + 1;
    int offs = CCNL_MAX_PACKET_SIZE;
    if (argc < 2) {
        _usage(argv[0]);
        return -1;
    }
    if (argc > 2) {
        char buf[MAX_CONTENT];
        memset(buf, ' ', MAX_CONTENT);
        char *buf_ptr = buf;
        for (int i = 2; (i < argc) && (buf_ptr < (buf + MAX_CONTENT)); i++) {
            arg_len = strlen(argv[i]);
            if ((buf_ptr + arg_len) > (buf + MAX_CONTENT)) {
                arg_len = (buf + MAX_CONTENT) - buf_ptr;
            }
            strncpy(buf_ptr, argv[i], arg_len);
            buf_ptr += arg_len + 1;
        }
        *buf_ptr = '\0';
        body = buf;
        arg_len = strlen(body);
    }

    int suite = CCNL_SUITE_NDNTLV;

    struct ccnl_prefix_s *prefix = ccnl_URItoPrefix(argv[1], suite, NULL, NULL);

    arg_len = ccnl_ndntlv_prependContent(prefix, (unsigned char*) body, arg_len, NULL, NULL, &offs, _out);

    unsigned char *olddata;
    unsigned char *data = olddata = _out + offs;

    int len;
    unsigned typ;
    if (ccnl_ndntlv_dehead(&data, &arg_len, (int*) &typ, &len) ||
        typ != NDN_TLV_Data) {
        return -1;
    }
    struct ccnl_content_s *c = 0;
    struct ccnl_pkt_s *pk = ccnl_ndntlv_bytes2pkt(typ, olddata, &data, &arg_len);
    c = ccnl_content_new(&theRelay, &pk);
    ccnl_content_add2cache(&theRelay, c);
    c->flags |= CCNL_CONTENT_FLAGS_STATIC;

    return 0;
}
