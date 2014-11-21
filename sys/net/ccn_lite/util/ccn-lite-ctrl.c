/*
 * @f util/ccn-lite-ctrl.c
 * @b control utility to steer a ccn-lite relay
 *
 * Copyright (C) 2012-13, Christian Tschudin, University of Basel
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
 * 2012-06-01  created
 * 2013-07     <christopher.scherb@unibas.ch> heavy reworking and parsing
 *             of return message
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ccnx.h"
#include "ccnl.h"
#include "ccnl-pdu.h"
#include "ccnl-core.h"

// ----------------------------------------------------------------------

int
mkNewFaceRequest(unsigned char *out, char *macsrc, char *ip4src,
                 char *host, char *port, char *flags)
{
    int len = 0, len2, len3;

    unsigned char *contentobj = ccnl_malloc(500);
    if (!contentobj) {
        puts("mkNewFaceRequest: malloc failed");
        return 0;
    }

    unsigned char *faceinst = ccnl_malloc(500);
    if (!faceinst) {
        free(contentobj);
        puts("mkNewFaceRequest: malloc failed");
        return 0;
    }

    len = mkHeader(out, CCN_DTAG_INTEREST, CCN_TT_DTAG);   // interest
    len += mkHeader(out + len, CCN_DTAG_NAME, CCN_TT_DTAG); // name

    len += mkStrBlob(out + len, CCN_DTAG_COMPONENT, CCN_TT_DTAG, "ccnx");
    len += mkStrBlob(out + len, CCN_DTAG_COMPONENT, CCN_TT_DTAG, "");
    len += mkStrBlob(out + len, CCN_DTAG_COMPONENT, CCN_TT_DTAG, "newface");

    // prepare FACEINSTANCE
    len3 = mkHeader(faceinst, CCN_DTAG_FACEINSTANCE, CCN_TT_DTAG);
    len3 += mkStrBlob(faceinst + len3, CCN_DTAG_ACTION, CCN_TT_DTAG, "newface");

    if (macsrc) {
        len3 += mkStrBlob(faceinst + len3, CCNL_DTAG_MACSRC, CCN_TT_DTAG, macsrc);
    }

    if (ip4src) {
        len3 += mkStrBlob(faceinst + len3, CCNL_DTAG_IP4SRC, CCN_TT_DTAG, ip4src);
        len3 += mkStrBlob(faceinst + len3, CCN_DTAG_IPPROTO, CCN_TT_DTAG, "17");
    }

    if (host) {
        len3 += mkStrBlob(faceinst + len3, CCN_DTAG_HOST, CCN_TT_DTAG, host);
    }

    if (port) {
        len3 += mkStrBlob(faceinst + len3, CCN_DTAG_PORT, CCN_TT_DTAG, port);
    }

    /*
    if (frag)
    len3 += mkStrBlob(faceinst+len3, CCNL_DTAG_FRAG, CCN_TT_DTAG, frag);
    */
    if (flags) {
        len3 += mkStrBlob(faceinst + len3, CCNL_DTAG_FACEFLAGS, CCN_TT_DTAG, flags);
    }

    faceinst[len3++] = 0; // end-of-faceinst

    // prepare CONTENTOBJ with CONTENT
    len2 = mkHeader(contentobj, CCN_DTAG_CONTENTOBJ, CCN_TT_DTAG);   // contentobj
    len2 += mkBlob(contentobj + len2, CCN_DTAG_CONTENT, CCN_TT_DTAG, // content
                   (char *) faceinst, len3);
    contentobj[len2++] = 0; // end-of-contentobj

    // add CONTENTOBJ as the final name component
    len += mkBlob(out + len, CCN_DTAG_COMPONENT, CCN_TT_DTAG, // comp
                  (char *) contentobj, len2);

    out[len++] = 0; // end-of-name
    out[len++] = 0; // end-of-interest

    free(contentobj);
    free(faceinst);

    return len;
}

// ----------------------------------------------------------------------

int
mkPrefixregRequest(unsigned char *out, char reg, char *path, char *faceid)
{
    int len = 0, len2, len3;
    char *cp;

    unsigned char *contentobj = ccnl_malloc(500);
    if (!contentobj) {
        puts("mkNewFaceRequest: malloc failed");
        return 0;
    }

    unsigned char *fwdentry = ccnl_malloc(500);
    if (!fwdentry) {
        free(contentobj);
        puts("mkNewFaceRequest: malloc failed");
        return 0;
    }

    len = mkHeader(out, CCN_DTAG_INTEREST, CCN_TT_DTAG);   // interest
    len += mkHeader(out + len, CCN_DTAG_NAME, CCN_TT_DTAG); // name

    len += mkStrBlob(out + len, CCN_DTAG_COMPONENT, CCN_TT_DTAG, "ccnx");
    len += mkStrBlob(out + len, CCN_DTAG_COMPONENT, CCN_TT_DTAG, "");
    len += mkStrBlob(out + len, CCN_DTAG_COMPONENT, CCN_TT_DTAG,
                     reg ? "prefixreg" : "prefixunreg");

    // prepare FWDENTRY
    len3 = mkHeader(fwdentry, CCN_DTAG_FWDINGENTRY, CCN_TT_DTAG);
    len3 += mkStrBlob(fwdentry + len3, CCN_DTAG_ACTION, CCN_TT_DTAG,
                      reg ? "prefixreg" : "prefixunreg");
    len3 += mkHeader(fwdentry + len3, CCN_DTAG_NAME, CCN_TT_DTAG); // prefix

    cp = strtok(path, "/");

    while (cp) {
        len3 += mkBlob(fwdentry + len3, CCN_DTAG_COMPONENT, CCN_TT_DTAG,
                       cp, strlen(cp));
        cp = strtok(NULL, "/");
    }

    fwdentry[len3++] = 0; // end-of-prefix
    len3 += mkStrBlob(fwdentry + len3, CCN_DTAG_FACEID, CCN_TT_DTAG, faceid);
    fwdentry[len3++] = 0; // end-of-fwdentry

    // prepare CONTENTOBJ with CONTENT
    len2 = mkHeader(contentobj, CCN_DTAG_CONTENTOBJ, CCN_TT_DTAG);   // contentobj
    len2 += mkBlob(contentobj + len2, CCN_DTAG_CONTENT, CCN_TT_DTAG, // content
                   (char *) fwdentry, len3);
    contentobj[len2++] = 0; // end-of-contentobj

    // add CONTENTOBJ as the final name component
    len += mkBlob(out + len, CCN_DTAG_COMPONENT, CCN_TT_DTAG, // comp
                  (char *) contentobj, len2);

    out[len++] = 0; // end-of-name
    out[len++] = 0; // end-of-interest

    free(contentobj);
    free(fwdentry);

    //    ccnl_prefix_free(p);
    return len;
}

// ----------------------------------------------------------------------
