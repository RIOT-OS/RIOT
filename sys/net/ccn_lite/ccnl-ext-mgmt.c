/*
 * @f ccnl-ext-mgmt.c
 * @b CCN lite extension, management logic (face mgmt and registration)
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
 * 2012-05-06 created
 */

#include <stdlib.h>
#include <string.h>

#include "ccnx.h"
#include "ccnl-pdu.h"
#include "ccnl.h"
#include "ccnl-includes.h"
#include "ccnl-core.h"
#include "ccnl-ext.h"

unsigned char contentobj_buf[500];
unsigned char faceinst_buf[500];
unsigned char out_buf[500];
unsigned char fwdentry_buf[500];

char *
ccnl_addr2ascii(sockunion *su)
{
    static char buf[10];
    snprintf(buf, sizeof(buf), "%d", su->id);
    return buf;
}

// ----------------------------------------------------------------------
int
ccnl_is_local_addr(struct ccnl_face_s *f)
{
    return (f->ifndx == RIOT_MSG_IDX);
}

struct ccnl_prefix_s *
ccnl_prefix_clone_strip(struct ccnl_prefix_s *p, int strip)
{
    int i, len;
    struct ccnl_prefix_s *p2;

    p2 = (struct ccnl_prefix_s *) ccnl_calloc(1, sizeof(struct ccnl_prefix_s));

    if (!p2) {
        return NULL;
    }

    int stripped_compcnt = p->compcnt;
    stripped_compcnt -= (p->compcnt > strip) ? strip : 0;

    for (i = 0, len = 0; i < stripped_compcnt; len += p->complen[i++]);

    p2->path = (unsigned char *) ccnl_malloc(len);
    p2->comp = (unsigned char **) ccnl_malloc(stripped_compcnt * sizeof(char *));
    p2->complen = (int *) ccnl_malloc(stripped_compcnt * sizeof(int));

    if (!p2->comp || !p2->complen || !p2->path) {
        goto Bail;
    }

    p2->compcnt = stripped_compcnt;

    for (i = 0, len = 0; i < stripped_compcnt; len += p2->complen[i++]) {
        p2->complen[i] = p->complen[i];
        p2->comp[i] = p2->path + len;
        memcpy(p2->comp[i], p->comp[i], p2->complen[i]);
    }

    return p2;
Bail:
    free_prefix(p2);
    return NULL;
}

struct ccnl_prefix_s *
ccnl_prefix_clone(struct ccnl_prefix_s *p)
{
    return ccnl_prefix_clone_strip(p, 0U);
}
// ----------------------------------------------------------------------
// management protocols

#define extractStr(VAR,DTAG) \
    if (typ == CCN_TT_DTAG && num == DTAG) { \
    char *s; unsigned char *valptr; int vallen; \
    if (consume(typ, num, &buf, &buflen, &valptr, &vallen) < 0) goto Bail; \
    s = ccnl_malloc(vallen+1); if (!s) goto Bail; \
    memcpy(s, valptr, vallen); s[vallen] = '\0'; \
    ccnl_free(VAR); \
    VAR = (unsigned char*) s; \
    continue; \
    } do {} while(0)

void
ccnl_mgmt_return_msg(struct ccnl_relay_s *ccnl, struct ccnl_buf_s *orig,
                     struct ccnl_face_s *from, char *msg)
{
    (void) orig; /* unused */

    struct ccnl_buf_s *buf;

    // this is a temporary non-solution: a CCN-content reply should
    // be returned instead of a string message

    buf = ccnl_buf_new(msg, strlen(msg));
    ccnl_face_enqueue(ccnl, from, buf);
}

int
ccnl_mgmt_newface(struct ccnl_relay_s *ccnl, struct ccnl_buf_s *orig,
                  struct ccnl_prefix_s *prefix, struct ccnl_face_s *from)
{
    (void) orig; /* unused */

    unsigned char *buf;
    int buflen, num, typ;
    unsigned char *action, *macsrc, *ip4src, *proto, *host, *port,
             *path, *frag, *flags;
    char *cp = "newface cmd failed";
    int rc = -1;
    struct ccnl_face_s *f = NULL;
    //varibales for answer
    struct ccnl_buf_s *retbuf;
    //    unsigned char out[2000];
    int len = 0, len2, len3;
    //    unsigned char contentobj[2000];
    //    unsigned char faceinst[2000];
    unsigned char faceidstr[100];
    unsigned char retstr[200];

    DEBUGMSG(99, "ccnl_mgmt_newface from=%p, ifndx=%d\n",
             (void *) from, from->ifndx);
    action = macsrc = ip4src = proto = host = port = NULL;
    path = frag = flags = NULL;

    buf = prefix->comp[3];
    buflen = prefix->complen[3];

    if (dehead(&buf, &buflen, &num, &typ) < 0) {
        goto Bail;
    }

    if (typ != CCN_TT_DTAG || num != CCN_DTAG_CONTENTOBJ) {
        goto Bail;
    }

    if (dehead(&buf, &buflen, &num, &typ) != 0) {
        goto Bail;
    }

    if (typ != CCN_TT_DTAG || num != CCN_DTAG_CONTENT) {
        goto Bail;
    }

    if (dehead(&buf, &buflen, &num, &typ) != 0) {
        goto Bail;
    }

    if (typ != CCN_TT_BLOB) {
        goto Bail;
    }

    buflen = num;

    if (dehead(&buf, &buflen, &num, &typ) != 0) {
        goto Bail;
    }

    if (typ != CCN_TT_DTAG || num != CCN_DTAG_FACEINSTANCE) {
        goto Bail;
    }

    while (dehead(&buf, &buflen, &num, &typ) == 0) {
        if (num == 0 && typ == 0) {
            break;    // end
        }

        extractStr(action, CCN_DTAG_ACTION);
        extractStr(macsrc, CCNL_DTAG_MACSRC);
        extractStr(ip4src, CCNL_DTAG_IP4SRC);
        extractStr(path, CCNL_DTAG_UNIXSRC);
        extractStr(proto, CCN_DTAG_IPPROTO);
        extractStr(host, CCN_DTAG_HOST);
        extractStr(port, CCN_DTAG_PORT);
        //  extractStr(frag, CCNL_DTAG_FRAG);
        extractStr(flags, CCNL_DTAG_FACEFLAGS);

        if (consume(typ, num, &buf, &buflen, 0, 0) < 0) {
            goto Bail;
        }
    }

    // should (re)verify that action=="newface"

    /***************************************************************************/

    int ifndx = 0;

    if (strcmp((const char *) macsrc, "newTRANSface") == 0) {
        ifndx = RIOT_TRANS_IDX;
    }
    else if (strcmp((const char *) macsrc, "newMSGface") == 0) {
        ifndx = RIOT_MSG_IDX;
    }
    else {
        goto Bail;
    }

    uint16_t faceid = 0;

    if (port) {
        faceid = atoi((const char *) port);
    }
    else {
        goto Bail;
    }

    // create face ontop of the traceceiver iface
    DEBUGMSG(1, "new %s=%d face gets created - faceid==%s==%d!\n", (const char *) macsrc, ifndx, (const char *) port, faceid);
    f = ccnl_get_face_or_create(
            ccnl,
            ifndx,
            faceid
        );

    /***************************************************************************/
    if (f) {
        int flagval = flags ?
                      strtol((const char *)flags, NULL, 0) : CCNL_FACE_FLAGS_STATIC;
        //  DEBUGMSG(1, "  flags=%s %d\n", flags, flagval);
        DEBUGMSG(99, "  adding a new face (id=%d) worked!\n", f->faceid);
        f->flags = flagval &
                   (CCNL_FACE_FLAGS_STATIC | CCNL_FACE_FLAGS_REFLECT);

#ifdef USE_FRAG

        if (frag) {
            int mtu = 1500;

            if (f->frag) {
                ccnl_frag_destroy(f->frag);
                f->frag = NULL;
            }

            if (f->ifndx >= 0 && ccnl->ifs[f->ifndx].mtu > 0) {
                mtu = ccnl->ifs[f->ifndx].mtu;
            }

            f->frag = ccnl_frag_new(strtol((const char *)frag, NULL, 0),
                                    mtu);
        }

#endif
        cp = "newface cmd worked";
    }
    else {
        DEBUGMSG(99, "  newface request for (macsrc=%s ip4src=%s proto=%s host=%s port=%s frag=%s flags=%s) failed or was ignored\n",
                 macsrc, ip4src, proto, host, port, frag, flags);
    }

    rc = 0;

Bail:
    /*ANSWER*/

    len = mkHeader(out_buf, CCN_DTAG_CONTENT, CCN_TT_DTAG);   // content
    len += mkHeader(out_buf + len, CCN_DTAG_NAME, CCN_TT_DTAG); // name

    len += mkStrBlob(out_buf + len, CCN_DTAG_COMPONENT, CCN_TT_DTAG, "ccnx");
    len += mkStrBlob(out_buf + len, CCN_DTAG_COMPONENT, CCN_TT_DTAG, "");

    len += mkStrBlob(out_buf + len, CCN_DTAG_COMPONENT, CCN_TT_DTAG, "newface");

    // prepare FACEINSTANCE
    len3 = mkHeader(faceinst_buf, CCN_DTAG_FACEINSTANCE, CCN_TT_DTAG);
    sprintf((char *)retstr, "newface:  %s", cp);
    len3 += mkStrBlob(faceinst_buf + len3, CCN_DTAG_ACTION, CCN_TT_DTAG, (char *) retstr);

    if (macsrc) {
        len3 += mkStrBlob(faceinst_buf + len3, CCNL_DTAG_MACSRC, CCN_TT_DTAG, (char *) macsrc);
    }

    if (ip4src) {
        len3 += mkStrBlob(faceinst_buf + len3, CCNL_DTAG_IP4SRC, CCN_TT_DTAG, (char *) ip4src);
        len3 += mkStrBlob(faceinst_buf + len3, CCN_DTAG_IPPROTO, CCN_TT_DTAG, "17");
    }

    if (host) {
        len3 += mkStrBlob(faceinst_buf + len3, CCN_DTAG_HOST, CCN_TT_DTAG, (char *) host);
    }

    if (port) {
        len3 += mkStrBlob(faceinst_buf + len3, CCN_DTAG_PORT, CCN_TT_DTAG, (char *) port);
    }

    /*
     if (frag)
     len3 += mkStrBlob(faceinst+len3, CCNL_DTAG_FRAG, CCN_TT_DTAG, frag);
     */
    if (flags) {
        len3 += mkStrBlob(faceinst_buf + len3, CCNL_DTAG_FACEFLAGS, CCN_TT_DTAG, (char *) flags);
    }

    if (f) {
        sprintf((char *)faceidstr, "%i", f->faceid);
        len3 += mkStrBlob(faceinst_buf + len3, CCN_DTAG_FACEID, CCN_TT_DTAG, (char *) faceidstr);
    }

    faceinst_buf[len3++] = 0; // end-of-faceinst

    // prepare CONTENTOBJ with CONTENT
    len2 = mkHeader(contentobj_buf, CCN_DTAG_CONTENTOBJ, CCN_TT_DTAG);// contentobj
    len2 += mkBlob(contentobj_buf + len2, CCN_DTAG_CONTENT, CCN_TT_DTAG, // content
                   (char *) faceinst_buf, len3);
    contentobj_buf[len2++] = 0;// end-of-contentobj

    // add CONTENTOBJ as the final name component
    len += mkBlob(out_buf + len, CCN_DTAG_COMPONENT, CCN_TT_DTAG, // comp
                  (char *) contentobj_buf, len2);

    out_buf[len++] = (unsigned char) 0;// end-of-name
    out_buf[len++] = (unsigned char) 0;// end-of-interest

    retbuf = ccnl_buf_new((char *)out_buf, len);
    ccnl_face_enqueue(ccnl, from, retbuf);

    /*END ANWER*/

    ccnl_free(action);
    ccnl_free(macsrc);
    ccnl_free(ip4src);
    ccnl_free(proto);
    ccnl_free(host);
    ccnl_free(port);
    ccnl_free(frag);
    ccnl_free(flags);

    //ccnl_mgmt_return_msg(ccnl, orig, from, cp);
    return rc;
}

int
ccnl_mgmt_prefixreg(struct ccnl_relay_s *ccnl, struct ccnl_buf_s *orig,
                    struct ccnl_prefix_s *prefix, struct ccnl_face_s *from)
{
    unsigned char *buf;
    int buflen, num, typ;
    struct ccnl_prefix_s *p = NULL;
    unsigned char *action, *faceid;
    char *cp = "prefixreg cmd failed";
    int rc = -1;
    //variables for answer
    int len = 0, len2, len3;

    DEBUGMSG(1, "ccnl_mgmt_prefixreg\n");
    action = faceid = NULL;

    buf = prefix->comp[3];
    buflen = prefix->complen[3];

    if (dehead(&buf, &buflen, &num, &typ) < 0) {
        goto Bail;
    }

    if (typ != CCN_TT_DTAG || num != CCN_DTAG_CONTENTOBJ) {
        goto Bail;
    }

    if (dehead(&buf, &buflen, &num, &typ) != 0) {
        goto Bail;
    }

    if (typ != CCN_TT_DTAG || num != CCN_DTAG_CONTENT) {
        goto Bail;
    }

    if (dehead(&buf, &buflen, &num, &typ) != 0) {
        goto Bail;
    }

    if (typ != CCN_TT_BLOB) {
        goto Bail;
    }

    buflen = num;

    if (dehead(&buf, &buflen, &num, &typ) != 0) {
        goto Bail;
    }

    if (typ != CCN_TT_DTAG || num != CCN_DTAG_FWDINGENTRY) {
        goto Bail;
    }

    p = (struct ccnl_prefix_s *) ccnl_calloc(1, sizeof(struct ccnl_prefix_s));

    if (!p) {
        goto Bail;
    }

    p->comp = (unsigned char **) ccnl_malloc(CCNL_MAX_NAME_COMP *
              sizeof(unsigned char *));
    p->complen = (int *) ccnl_malloc(CCNL_MAX_NAME_COMP * sizeof(int));

    if (!p->comp || !p->complen) {
        goto Bail;
    }

    while (dehead(&buf, &buflen, &num, &typ) == 0) {
        if (num == 0 && typ == 0) {
            break;    // end
        }

        if (typ == CCN_TT_DTAG && num == CCN_DTAG_NAME) {
            while (1) {
                if (dehead(&buf, &buflen, &num, &typ) != 0) {
                    goto Bail;
                }

                if (num == 0 && typ == 0) {
                    break;
                }

                if (typ == CCN_TT_DTAG && num == CCN_DTAG_COMPONENT &&
                    p->compcnt < CCNL_MAX_NAME_COMP) {
                    // if (ccnl_grow_prefix(p)) goto Bail;
                    if (consume(typ, num, &buf, &buflen,
                                p->comp + p->compcnt,
                                p->complen + p->compcnt) < 0) {
                        goto Bail;
                    }

                    p->compcnt++;
                }
                else {
                    if (consume(typ, num, &buf, &buflen, 0, 0) < 0) {
                        goto Bail;
                    }
                }
            }

            continue;
        }

        extractStr(action, CCN_DTAG_ACTION);
        extractStr(faceid, CCN_DTAG_FACEID);

        if (consume(typ, num, &buf, &buflen, 0, 0) < 0) {
            goto Bail;
        }
    }

    // should (re)verify that action=="prefixreg"
    if (faceid && p->compcnt > 0) {
        struct ccnl_face_s *f;
        int fi = strtol((const char *)faceid, NULL, 0);

        DEBUGMSG(1, "mgmt: adding prefix %s to faceid='%s'='%d'\n",
                 ccnl_prefix_to_path(p), faceid, fi);

        for (f = ccnl->faces; f && f->faceid != fi; f = f->next) {
            DEBUGMSG(1, "faceid=%d\n", f->faceid);
        }

        if (!f) {
            DEBUGMSG(1, "bail!\n");
            goto Bail;
        }

        DEBUGMSG(1, "Face %s found! ifndx=%d\n", faceid, f->ifndx);

        ccnl_content_learn_name_route(ccnl, p, f, 0, CCNL_FORWARD_FLAGS_STATIC);
        cp = "prefixreg cmd worked";
    }
    else {
        DEBUGMSG(1, "mgmt: ignored prefixreg faceid=%s\n", faceid);
    }

    rc = 0;

Bail:
    /*ANSWER*/

    len = mkHeader(out_buf, CCN_DTAG_CONTENT, CCN_TT_DTAG);   // interest
    len += mkHeader(out_buf + len, CCN_DTAG_NAME, CCN_TT_DTAG); // name

    len += mkStrBlob(out_buf + len, CCN_DTAG_COMPONENT, CCN_TT_DTAG, "ccnx");
    len += mkStrBlob(out_buf + len, CCN_DTAG_COMPONENT, CCN_TT_DTAG, "");
    len += mkStrBlob(out_buf + len, CCN_DTAG_COMPONENT, CCN_TT_DTAG, (char *) action);

    // prepare FWDENTRY
    len3 = mkHeader(fwdentry_buf, CCNL_DTAG_PREFIX, CCN_TT_DTAG);
    len3 += mkStrBlob(fwdentry_buf + len3, CCN_DTAG_ACTION, CCN_TT_DTAG, cp);
    len3 += mkStrBlob(fwdentry_buf + len3, CCN_DTAG_NAME, CCN_TT_DTAG, ccnl_prefix_to_path(p)); // prefix

    len3 += mkStrBlob(fwdentry_buf + len3, CCN_DTAG_FACEID, CCN_TT_DTAG, (char *) faceid);
    fwdentry_buf[len3++] = 0;// end-of-fwdentry

    // prepare CONTENTOBJ with CONTENT
    len2 = mkHeader(contentobj_buf, CCN_DTAG_CONTENTOBJ, CCN_TT_DTAG);// contentobj
    len2 += mkBlob(contentobj_buf + len2, CCN_DTAG_CONTENT, CCN_TT_DTAG, // content
                   (char *) fwdentry_buf, len3);
    contentobj_buf[len2++] = 0;// end-of-contentobj

    // add CONTENTOBJ as the final name component
    len += mkBlob(out_buf + len, CCN_DTAG_COMPONENT, CCN_TT_DTAG, // comp
                  (char *) contentobj_buf, len2);

    out_buf[len++] = 0;// end-of-name
    out_buf[len++] = 0;// end-of-interest

    /*END ANWER*/
    ccnl_free(faceid);
    ccnl_free(action);
    free_prefix(p);

    DEBUGMSG(1, "data='%s' faceid=%d cp='%s'\n", orig->data, from->faceid, cp);
    ccnl_mgmt_return_msg(ccnl, orig, from, cp);
    return rc;
}

static int ccnl_mgmt_handle(struct ccnl_relay_s *ccnl, struct ccnl_buf_s *orig,
        struct ccnl_prefix_s *prefix, struct ccnl_face_s *from, char *cmd,
        int verified)
{
    DEBUGMSG(99, "ccnl_mgmt_handle \"%s\"\n", cmd);
    if (!verified) {
        ccnl_mgmt_return_msg(ccnl, orig, from,
                "refused: error signature not verified");
        return -1;
    }

    if (!strcmp(cmd, "newface")) {
        DEBUGMSG(1, "ccnl_mgmt_newface msg\n");
        ccnl_mgmt_newface(ccnl, orig, prefix, from);
    }
    else if (!strcmp(cmd, "prefixreg")) {
        DEBUGMSG(1, "ccnl_mgmt_prefixreg msg\n");
        ccnl_mgmt_prefixreg(ccnl, orig, prefix, from);
    }
    else {
        DEBUGMSG(99, "unknown mgmt command %s\n", cmd);

        ccnl_mgmt_return_msg(ccnl, orig, from, "unknown mgmt command");
        return -1;
    }

    return 0;
}

char cmd[500];
int ccnl_mgmt(struct ccnl_relay_s *ccnl, struct ccnl_buf_s *orig,
        struct ccnl_prefix_s *prefix, struct ccnl_face_s *from)
{
    if (prefix->complen[2] < (int) sizeof(cmd)) {
        memcpy(cmd, prefix->comp[2], prefix->complen[2]);
        cmd[prefix->complen[2]] = '\0';
    }
    else {
        strcpy(cmd, "cmd-is-too-long-to-display");
    }

    DEBUGMSG(99, "ccnl_mgmt request \"%s\"\n", cmd);

    if (ccnl_is_local_addr(from)) {
        goto MGMT;
    }

    DEBUGMSG(99, " rejecting because src is not a local addr\n");
    ccnl_mgmt_return_msg(ccnl, orig, from,
            "refused: origin of mgmt cmd is not local");
    return -1;

    MGMT: ccnl_mgmt_handle(ccnl, orig, prefix, from, cmd, 1);

    return 0;
}

#if 0
    char *cmd = (char *) prefix->comp[2];

    if (!ccnl_is_local_addr(&from->peer)) {
        DEBUGMSG(99, "  rejecting because src=%s is not a local addr\n",
                 ccnl_addr2ascii(&from->peer));
        ccnl_mgmt_return_msg(ccnl, orig, from,
                             "refused: origin of mgmt cmd is not local");
        return -1;
    }

    if (!strcmp(cmd, "newface")) {
        DEBUGMSG(1, "ccnl_mgmt_newface msg\n");
        ccnl_mgmt_newface(ccnl, orig, prefix, from);
    }
    else if (!strcmp(cmd, "prefixreg")) {
        DEBUGMSG(1, "ccnl_mgmt_prefixreg msg\n");
        ccnl_mgmt_prefixreg(ccnl, orig, prefix, from);
    }
    else {
        DEBUGMSG(99, "unknown mgmt command %s\n", cmd);

        ccnl_mgmt_return_msg(ccnl, orig, from, "unknown mgmt command");
        return -1;
    }

    return 0;
}
#endif

// eof
