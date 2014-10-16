/*
 * @f ccnl-core.c
 * @b CCN lite, core CCNx protocol logic
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
 * 2011-04-09 created
 */

#define CCNL_VERSION "2013-07-27"

#include <string.h>
#include <stdio.h>

#include "ccnl.h"
#include "ccnl-core.h"
#include "ccnl-pdu.h"
#include "ccnx.h"
#include "ccnl-ext.h"
#include "ccnl-platform.h"

#include "ccnl-includes.h"

#include "ccnl-riot-compat.h"

#define CCNL_DYNAMIC_FIB (0)

static struct ccnl_interest_s *ccnl_interest_remove(struct ccnl_relay_s *ccnl,
        struct ccnl_interest_s *i);

void ccnl_ll_TX(struct ccnl_relay_s *ccnl, struct ccnl_if_s *ifc,
                sockunion *dest, struct ccnl_buf_s *buf);

void free_2ptr_list(void *a, void *b)
{
    ccnl_free(a);
    ccnl_free(b);
}
void free_3ptr_list(void *a, void *b, void *c)
{
    ccnl_free(a);
    ccnl_free(b);
    ccnl_free(c);
}
void free_4ptr_list(void *a, void *b, void *c, void *d)
{
    ccnl_free(a);
    ccnl_free(b);
    ccnl_free(c);
    ccnl_free(d);
}

void free_prefix(struct ccnl_prefix_s *p)
{
    if (p) {
        free_4ptr_list(p->path, p->comp, p->complen, p);
    }
}

void free_content(struct ccnl_content_s *c)
{
    free_prefix(c->name);
    free_2ptr_list(c->pkt, c);
}

void free_forward(struct ccnl_forward_s *fwd)
{
    free_prefix(fwd->prefix);
    ccnl_free(fwd);
}

// ----------------------------------------------------------------------
// datastructure support functions

struct ccnl_buf_s *
ccnl_buf_new(void *data, int len)
{
    struct ccnl_buf_s *b = (struct ccnl_buf_s *) ccnl_malloc(sizeof(*b) + len);

    if (!b) {
        return NULL;
    }

    b->next = NULL;
    b->datalen = len;

    if (data) {
        memcpy(b->data, data, len);
    }

    return b;
}

struct ccnl_buf_s *buf_dup(struct ccnl_buf_s *B)
{
    return (B) ? ccnl_buf_new(B->data, B->datalen) : NULL;
}

int buf_equal(struct ccnl_buf_s *X, struct ccnl_buf_s *Y)
{
    return ((X) && (Y) && (X->datalen == Y->datalen) && !memcmp(X->data, Y->data, X->datalen));
}

int ccnl_prefix_cmp(struct ccnl_prefix_s *name, unsigned char *md,
                    struct ccnl_prefix_s *p, int mode)
/* returns -1 if no match at all (all modes) or exact match failed
 returns  0 if full match (CMP_EXACT)
 returns n>0 for matched components (CMP_MATCH, CMP_LONGEST) */
{
    int i, clen, nlen = name->compcnt + (md ? 1 : 0), rc = -1;
    unsigned char *comp;

    if (mode == CMP_EXACT && nlen != p->compcnt) {
        goto done;
    }

    for (i = 0; i < nlen && i < p->compcnt; i++) {
        comp = i < name->compcnt ? name->comp[i] : md;
        clen = i < name->compcnt ? name->complen[i] : 32; // SHA256_DIGEST_LEN

        if (clen != p->complen[i] || memcmp(comp, p->comp[i], p->complen[i])) {
            rc = mode == CMP_EXACT ? -1 : i;
            goto done;
        }
    }

    rc = (mode == CMP_EXACT) ? 0 : i;
done:
    DEBUGMSG(49,
             "ccnl_prefix_cmp (mode=%d, nlen=%d, plen=%d, %d), name=%s: %d (%p)\n",
             mode, nlen, p->compcnt, name->compcnt, ccnl_prefix_to_path(name),
             rc, md);
    return rc;
}

// ----------------------------------------------------------------------
// ccnb parsing support

static int hunt_for_end(unsigned char **buf, int *len, unsigned char **valptr,
                        int *vallen)
{
    int typ, num;

    while (dehead(buf, len, &num, &typ) == 0) {
        if (num == 0 && typ == 0) {
            return 0;
        }

        if (consume(typ, num, buf, len, valptr, vallen) < 0) {
            return -1;
        }
    }

    return -1;
}

int consume(int typ, int num, unsigned char **buf, int *len,
            unsigned char **valptr, int *vallen)
{
    if (typ == CCN_TT_BLOB || typ == CCN_TT_UDATA) {
        if (valptr) {
            *valptr = *buf;
        }

        if (vallen) {
            *vallen = num;
        }

        *buf += num, *len -= num;
        return 0;
    }

    if (typ == CCN_TT_DTAG || typ == CCN_TT_DATTR) {
        return hunt_for_end(buf, len, valptr, vallen);
    }

    //  case CCN_TT_TAG, CCN_TT_ATTR:
    //  case DTAG, DATTR:
    return -1;
}

static int data2uint(unsigned char *cp, int len)
{
    int i, val;

    for (i = 0, val = 0; i < len; i++)
        if (isdigit(cp[i])) {
            val = 10 * val + cp[i] - '0';
        }
        else {
            return -1;
        }

    return val;
}

struct ccnl_nonce_s *ccnl_nonce_new(struct ccnl_buf_s *that)
{
    struct ccnl_nonce_s *n = (struct ccnl_nonce_s *) ccnl_malloc(sizeof(struct ccnl_nonce_s));

    n->buf = buf_dup(that);
    ccnl_get_timeval(&n->created);

    n->next = NULL;
    n->prev = NULL;

    return n;
}

struct ccnl_nonce_s *ccnl_nonce_remove(struct ccnl_relay_s *ccnl, struct ccnl_nonce_s *nonce)
{
    DEBUGMSG(99, "ccnl_nonce_remove: %u:%u:%u:%u\n",
             nonce->buf->data[0], nonce->buf->data[1], nonce->buf->data[2], nonce->buf->data[3]);

    struct ccnl_nonce_s *next = nonce->next;
    DBL_LINKED_LIST_REMOVE(ccnl->nonces, nonce);

    free(nonce->buf);
    free(nonce);

    return next;
}

struct ccnl_buf_s *
ccnl_extract_prefix_nonce_ppkd(unsigned char **data, int *datalen, int *scope,
                               int *aok, int *min, int *max, struct ccnl_prefix_s **prefix,
                               struct ccnl_buf_s **nonce, struct ccnl_buf_s **ppkd,
                               unsigned char **content, int *contlen)
{
    unsigned char *start = *data - 2, *cp;
    int num, typ, len;
    struct ccnl_prefix_s *p;
    struct ccnl_buf_s *buf, *n = 0, *pub = 0;
    DEBUGMSG(99, "ccnl_extract_prefix\n");

    p = (struct ccnl_prefix_s *) ccnl_calloc(1, sizeof(struct ccnl_prefix_s));

    if (!p) {
        puts("can't get more memory from malloc, dropping ccn msg...");
        return NULL;
    }

    p->comp = (unsigned char **) ccnl_malloc(
                  CCNL_MAX_NAME_COMP * sizeof(unsigned char **));
    p->complen = (int *) ccnl_malloc(CCNL_MAX_NAME_COMP * sizeof(int));

    if (!p->comp || !p->complen) {
        puts("can't get more memory from malloc, dropping ccn msg...");
        goto Bail;
    }

    while (dehead(data, datalen, &num, &typ) == 0) {
        if (num == 0 && typ == 0) {
            break;    // end
        }

        if (typ == CCN_TT_DTAG) {
            if (num == CCN_DTAG_NAME) {
                while (1) {
                    if (dehead(data, datalen, &num, &typ) != 0) {
                        goto Bail;
                    }

                    if (num == 0 && typ == 0) {
                        break;
                    }

                    if (typ == CCN_TT_DTAG && num == CCN_DTAG_COMPONENT
                        && p->compcnt < CCNL_MAX_NAME_COMP) {
                        if (hunt_for_end(data, datalen, p->comp + p->compcnt,
                                         p->complen + p->compcnt) < 0) {
                            goto Bail;
                        }

                        p->compcnt++;
                    }
                    else {
                        if (consume(typ, num, data, datalen, 0, 0) < 0) {
                            goto Bail;
                        }
                    }
                }

                continue;
            }

            if (num == CCN_DTAG_SCOPE || num == CCN_DTAG_NONCE
                || num == CCN_DTAG_MINSUFFCOMP
                || num == CCN_DTAG_MAXSUFFCOMP
                || num == CCN_DTAG_PUBPUBKDIGEST) {
                if (hunt_for_end(data, datalen, &cp, &len) < 0) {
                    goto Bail;
                }

                if (num == CCN_DTAG_SCOPE && len == 1 && scope) {
                    *scope = isdigit(*cp) && (*cp < '3') ? *cp - '0' : -1;
                }

                if (num == CCN_DTAG_ANSWERORIGKIND && aok) {
                    *aok = data2uint(cp, len);
                }

                if (num == CCN_DTAG_MINSUFFCOMP && min) {
                    *min = data2uint(cp, len);
                }

                if (num == CCN_DTAG_MAXSUFFCOMP && max) {
                    *max = data2uint(cp, len);
                }

                if (num == CCN_DTAG_NONCE && !n) {
                    n = ccnl_buf_new(cp, len);
                }

                if (num == CCN_DTAG_PUBPUBKDIGEST && !pub) {
                    pub = ccnl_buf_new(cp, len);
                }

                if (num == CCN_DTAG_EXCLUDE) {
                    DEBUGMSG(49, "warning: 'exclude' field ignored\n");
                }
                else {
                    continue;
                }
            }

            if (num == CCN_DTAG_CONTENT || num == CCN_DTAG_CONTENTOBJ) {
                if (consume(typ, num, data, datalen, content, contlen) < 0) {
                    goto Bail;
                }

                continue;
            }
        }

        if (consume(typ, num, data, datalen, 0, 0) < 0) {
            goto Bail;
        }
    }

    if (prefix) {
        p->comp[p->compcnt] = NULL;
        *prefix = p;
    }
    else {
        free_prefix(p);
    }

    if (nonce) {
        *nonce = n;
    }
    else {
        ccnl_free(n);
    }

    if (ppkd) {
        *ppkd = pub;
    }
    else {
        ccnl_free(pub);
    }

    buf = ccnl_buf_new(start, *data - start);

    if (!buf) {
        puts("can't get more memory from malloc, dropping ccn msg...");
        goto Bail;
    }

    // carefully rebase ptrs to new buf because of 64bit pointers:
    if (content) {
        *content = buf->data + (*content - start);
    }

    for (num = 0; num < p->compcnt; num++) {
        p->comp[num] = buf->data + (p->comp[num] - start);
    }

    return buf;
Bail:
    free_prefix(p);
    free_2ptr_list(n, pub);
    return NULL;
}

// ----------------------------------------------------------------------
// addresses, interfaces and faces

static void ccnl_face_CTS(struct ccnl_relay_s *ccnl, struct ccnl_face_s *f);

int ccnl_addr_cmp(sockunion *s1, sockunion *s2)
{
    return (s1->id == s2->id);
}

struct ccnl_face_s *
ccnl_get_face_or_create(struct ccnl_relay_s *ccnl, int ifndx, uint16_t sender_id)
{
    struct ccnl_face_s *f;

    for (f = ccnl->faces; f; f = f->next) {
        if (ifndx == f->ifndx && (f->faceid == sender_id)) {
            DEBUGMSG(1, "face found! ifidx=%d sender_id=%d faceid=%d\n", ifndx, sender_id, f->faceid);
            ccnl_get_timeval(&f->last_used);
            return f;
        }
    }

    f = (struct ccnl_face_s *) ccnl_calloc(1, sizeof(struct ccnl_face_s));
    if (!f) {
        return NULL;
    }

    f->faceid = sender_id;
    f->ifndx = ifndx;

    if (sender_id == RIOT_BROADCAST) {
        f->flags |= CCNL_FACE_FLAGS_BROADCAST;
    }

    if (ifndx >= 0) {
        if (ccnl->defaultFaceScheduler)
            f->sched = ccnl->defaultFaceScheduler(ccnl,
                                                  (void ( *)(void *, void *)) ccnl_face_CTS);

        if (ccnl->ifs[ifndx].reflect) {
            f->flags |= CCNL_FACE_FLAGS_REFLECT;
        }

        if (ccnl->ifs[ifndx].fwdalli) {
            f->flags |= CCNL_FACE_FLAGS_FWDALLI;
        }
    }

    f->peer.id = sender_id;

#ifdef USE_FRAG

    if (ifndx == RIOT_TRANS_IDX) {
        // if newly created face, no fragment policy is defined yet
        // turning on fragmentation for riot trans dev based faces
        int flagval = CCNL_FACE_FLAGS_STATIC;
        f->flags = flagval &
                   (CCNL_FACE_FLAGS_STATIC | CCNL_FACE_FLAGS_REFLECT);

        if (f->frag) {
            ccnl_frag_destroy(f->frag);
            f->frag = NULL;
        }

        int mtu = ccnl->ifs[f->ifndx].mtu;
        f->frag = ccnl_frag_new(CCNL_FRAG_CCNx2013, mtu); //TODO
    }

#endif

    ccnl_get_timeval(&f->last_used);
    DBL_LINKED_LIST_ADD(ccnl->faces, f);

    return f;
}

struct ccnl_face_s *
ccnl_face_remove(struct ccnl_relay_s *ccnl, struct ccnl_face_s *f)
{
    struct ccnl_face_s *f2;
    struct ccnl_interest_s *pit;
    struct ccnl_forward_s **ppfwd;
    DEBUGMSG(1, "ccnl_face_remove relay=%p face=%p\n", (void *) ccnl, (void *) f);

    ccnl_sched_destroy(f->sched);
    ccnl_frag_destroy(f->frag);

    for (pit = ccnl->pit; pit;) {
        struct ccnl_pendint_s **ppend, *pend;

        if (pit->from == f) {
            pit->from = NULL;
        }

        for (ppend = &pit->pending; *ppend;) {
            if ((*ppend)->face == f) {
                pend = *ppend;
                *ppend = pend->next;
                ccnl_free(pend);
            }
            else {
                ppend = &(*ppend)->next;
            }
        }

        if (pit->pending) {
            pit = pit->next;
        }
        else {
            pit = ccnl_interest_remove(ccnl, pit);
        }
    }

    for (ppfwd = &ccnl->fib; *ppfwd;) {
        if ((*ppfwd)->face == f) {
            struct ccnl_forward_s *pfwd = *ppfwd;
            *ppfwd = pfwd->next;

            ccnl_forward_remove(ccnl, pfwd);
        }
        else {
            ppfwd = &(*ppfwd)->next;
        }
    }

    while (f->outq) {
        struct ccnl_buf_s *tmp = f->outq->next;
        ccnl_free(f->outq);
        f->outq = tmp;
    }

#if ENABLE_DEBUG
    ccnl_face_print_stat(f);
#endif

    f2 = f->next;
    DBL_LINKED_LIST_REMOVE(ccnl->faces, f);
    ccnl_free(f);
    return f2;
}

#if ENABLE_DEBUG
void ccnl_face_print_stat(struct ccnl_face_s *f)
{
    DEBUGMSG(1, "ccnl_face_print_stat: faceid=%d ifndx=%d\n", f->faceid, f->ifndx);
    DEBUGMSG(1, "  STAT interest send=%d:%d:%d:%d:%d\n", f->stat.send_interest[0],
             f->stat.send_interest[1], f->stat.send_interest[2],
             f->stat.send_interest[3], f->stat.send_interest[4]);

    DEBUGMSG(1, "  STAT content  send=%d:%d:%d:%d:%d\n", f->stat.send_content[0],
             f->stat.send_content[1], f->stat.send_content[2],
             f->stat.send_content[3], f->stat.send_content[4]);

    DEBUGMSG(1, "  STAT interest received=%d\n", f->stat.received_interest);

    DEBUGMSG(1, "  STAT content  received=%d\n", f->stat.received_content);
}
#endif

void ccnl_interface_cleanup(struct ccnl_if_s *i)
{
    int j;
    DEBUGMSG(99, "ccnl_interface_cleanup\n");

    ccnl_sched_destroy(i->sched);

    for (j = 0; j < i->qlen; j++) {
        struct ccnl_txrequest_s *r = i->queue
                                     + (i->qfront + j) % CCNL_MAX_IF_QLEN;
        ccnl_free(r->buf);
    }
}

// ----------------------------------------------------------------------
// face and interface queues, scheduling

void ccnl_interface_CTS(void *aux1, void *aux2)
{
    struct ccnl_relay_s *ccnl = (struct ccnl_relay_s *) aux1;
    struct ccnl_if_s *ifc = (struct ccnl_if_s *) aux2;
    struct ccnl_txrequest_s *r, req;
    DEBUGMSG(25, "ccnl_interface_CTS interface=%p, qlen=%d, sched=%p\n",
             (void *) ifc, ifc->qlen, (void *) ifc->sched);

    if (ifc->qlen <= 0) {
        return;
    }

    r = ifc->queue + ifc->qfront;
    memcpy(&req, r, sizeof(req));
    ifc->qfront = (ifc->qfront + 1) % CCNL_MAX_IF_QLEN;
    ifc->qlen--;

    ccnl_ll_TX(ccnl, ifc, &req.dst, req.buf);
    ccnl_free(req.buf);
}

void ccnl_interface_enqueue(void (tx_done)(void *, int, int),
                            struct ccnl_face_s *f, struct ccnl_relay_s *ccnl, struct ccnl_if_s *ifc,
                            struct ccnl_buf_s *buf, sockunion *dest)
{
    struct ccnl_txrequest_s *r;
    DEBUGMSG(25, "ccnl_interface_enqueue interface=%p buf=%p (qlen=%d)\n",
             (void *) ifc, (void *) buf, ifc->qlen);

    if (ifc->qlen >= CCNL_MAX_IF_QLEN) {
        DEBUGMSG(2, "  DROPPING buf=%p\n", (void *) buf);
        ccnl_free(buf);
        return;
    }

    r = ifc->queue + ((ifc->qfront + ifc->qlen) % CCNL_MAX_IF_QLEN);
    r->buf = buf;
    memcpy(&r->dst, dest, sizeof(sockunion));
    r->txdone = tx_done;
    r->txdone_face = f;
    ifc->qlen++;

    ccnl_interface_CTS(ccnl, ifc);
}

struct ccnl_buf_s *
ccnl_face_dequeue(struct ccnl_relay_s *ccnl, struct ccnl_face_s *f)
{
    (void) ccnl; /* unused */

    struct ccnl_buf_s *pkt;
    DEBUGMSG(20, "ccnl_face_dequeue face=%p (id=%d.%d)\n", (void *) f, ccnl->id,
             f->faceid);

    if (!f->outq) {
        return NULL;
    }

    pkt = f->outq;
    f->outq = pkt->next;

    if (!pkt->next) {
        f->outqend = NULL;
    }

    pkt->next = NULL;
    return pkt;
}

void ccnl_face_CTS_done(void *ptr, int cnt, int len)
{
    (void) ptr;
    (void) cnt;
    (void) len;
    DEBUGMSG(99, "ccnl_face_CTS_done face=%p cnt=%d len=%d\n", ptr, cnt, len);
}

void ccnl_face_CTS(struct ccnl_relay_s *ccnl, struct ccnl_face_s *f)
{
    DEBUGMSG(99, "ccnl_face_CTS face=%p sched=%p\n", (void *) f,
             (void *) f->sched);

    if (!f->frag || f->frag->protocol == CCNL_FRAG_NONE) {
        struct ccnl_buf_s *buf = ccnl_face_dequeue(ccnl, f);

        if (buf)
            ccnl_interface_enqueue(ccnl_face_CTS_done, f, ccnl,
                                   ccnl->ifs + f->ifndx, buf, &f->peer);
    }

#ifdef USE_FRAG
    else {
        sockunion dst;
        int ifndx = f->ifndx;
        buf = ccnl_frag_getnext(f->frag, &ifndx, &dst);

        if (!buf) {
            buf = ccnl_face_dequeue(ccnl, f);
            ccnl_frag_reset(f->frag, buf, f->ifndx, &f->peer);
            buf = ccnl_frag_getnext(f->frag, &ifndx, &dst);
        }

        if (buf) {
            ccnl_interface_enqueue(ccnl_face_CTS_done, f,
                                   ccnl, ccnl->ifs + ifndx, buf, &dst);
        }
    }

#endif
}

int ccnl_face_enqueue(struct ccnl_relay_s *ccnl, struct ccnl_face_s *to,
                      struct ccnl_buf_s *buf)
{
    struct ccnl_buf_s *msg;
    DEBUGMSG(20, "ccnl_face_enqueue face=%p (id=%d.%d) buf=%p len=%d\n",
             (void *) to, ccnl->id, to->faceid, (void *) buf, buf->datalen);

    for (msg = to->outq; msg; msg = msg->next) // already in the queue?
        if (buf_equal(msg, buf)) {
            DEBUGMSG(31, "    not enqueued because already there\n");
            ccnl_free(buf);
            return -1;
        }

    buf->next = NULL;

    if (to->outqend) {
        to->outqend->next = buf;
    }
    else {
        to->outq = buf;
    }

    to->outqend = buf;
    ccnl_face_CTS(ccnl, to);
    return 0;
}

// ----------------------------------------------------------------------
// handling of interest messages

int ccnl_nonce_find_or_append(struct ccnl_relay_s *ccnl,
                              struct ccnl_buf_s *nonce)
{
    struct ccnl_nonce_s *n, *last;
    int i;
    DEBUGMSG(99, "ccnl_nonce_find_or_append: %u:%u:%u:%u\n",
             nonce->data[0], nonce->data[1], nonce->data[2], nonce->data[3]);

    /* test for noce in nonce cache */

    for (n = ccnl->nonces, i = 0; n; n = n->next, i++) {
        DEBUGMSG(1, "known: %u:%u:%u:%u\n",
                 n->buf->data[0], n->buf->data[1], n->buf->data[2], n->buf->data[3]);
        if (buf_equal(n->buf, nonce)) {
            /* nonce in cache -> known */
            return -1;
        }
        if (n->next) {
            last = n->next;
        }
    }

    /* nonce not in local cache, add it */
    n = ccnl_nonce_new(nonce);
    DBL_LINKED_LIST_ADD(ccnl->nonces, n);

    /* nonce chache full? */
    if (i >= CCNL_MAX_NONCES) {
        /* cache is full, drop oldest nonce: its the last element in the list */
        ccnl_nonce_remove(ccnl, last);
    }

    return 0;
}

struct ccnl_interest_s *
ccnl_interest_new(struct ccnl_relay_s *ccnl, struct ccnl_face_s *from,
                  struct ccnl_buf_s **pkt, struct ccnl_prefix_s **prefix, int minsuffix,
                  int maxsuffix, struct ccnl_buf_s **ppkd)
{
    struct ccnl_interest_s *i = (struct ccnl_interest_s *) ccnl_calloc(1,
                                sizeof(struct ccnl_interest_s));
    DEBUGMSG(99, "ccnl_new_interest\n");

    if (!i) {
        puts("can't get more memory from malloc, dropping ccn msg...");
        return NULL;
    }

    i->from = from;
    i->prefix = *prefix;
    *prefix = 0;
    i->pkt = *pkt;
    *pkt = 0;
    i->ppkd = *ppkd;
    *ppkd = 0;
    i->minsuffix = minsuffix;
    i->maxsuffix = maxsuffix;
    ccnl_get_timeval(&i->last_used);
    DBL_LINKED_LIST_ADD(ccnl->pit, i);
    return i;
}

int ccnl_interest_append_pending(struct ccnl_interest_s *i,
                                 struct ccnl_face_s *from)
{
    struct ccnl_pendint_s *pi, *last = NULL;
    DEBUGMSG(99, "ccnl_append_pending\n");

    for (pi = i->pending; pi; pi = pi->next) { // check whether already listed
        if (pi->face == from) {
            DEBUGMSG(40, "  we found a matching interest, updating time\n");
            ccnl_get_timeval(&pi->last_used);
            return 0;
        }

        last = pi;
    }

    pi = (struct ccnl_pendint_s *) ccnl_calloc(1,
            sizeof(struct ccnl_pendint_s));
    DEBUGMSG(40, "  appending a new pendint entry %p\n", (void *) pi);

    if (!pi) {
        return -1;
    }

    pi->face = from;
    ccnl_get_timeval(&pi->last_used);

    if (last) {
        last->next = pi;
    }
    else {
        i->pending = pi;
    }

    return 0;
}

void ccnl_interest_propagate(struct ccnl_relay_s *ccnl,
                             struct ccnl_interest_s *i)
{
    struct ccnl_forward_s *fwd;
    DEBUGMSG(99, "ccnl_interest_propagate\n");

    // CONFORM: "A node MUST implement some strategy rule, even if it is only to
    // transmit an Interest Message on all listed dest faces in sequence."
    // CCNL strategy: we forward on all FWD entries with a prefix match
    int forward_cnt = 0;
    for (fwd = ccnl->fib; fwd; fwd = fwd->next) {
        int rc = ccnl_prefix_cmp(fwd->prefix, NULL, i->prefix, CMP_LONGEST);
        DEBUGMSG(40, "  ccnl_interest_propagate, rc=%d/%d\n", rc,
                 fwd->prefix->compcnt);

        if (rc < fwd->prefix->compcnt) {
            continue;
        }

        DEBUGMSG(40, "  ccnl_interest_propagate, fwd==%p\n", (void *) fwd);

        // suppress forwarding to origin of interest, except wireless
        if (!i->from || fwd->face != i->from
            || (i->from->flags & CCNL_FACE_FLAGS_REFLECT)) {

            i->forwarded_over = fwd;
            fwd->face->stat.send_interest[i->retries]++;
            ccnl_get_timeval(&i->last_used);
            ccnl_face_enqueue(ccnl, fwd->face, buf_dup(i->pkt));
            ccnl_get_timeval(&fwd->last_used);
            forward_cnt++;
        }
    }

    if (forward_cnt == 0) {
        DEBUGMSG(40, "  ccnl_interest_propagate: using broadcast face!\n");
        ccnl->ifs[RIOT_TRANS_IDX].broadcast_face->stat.send_interest[i->retries]++;
        ccnl_get_timeval(&i->last_used);
        ccnl_face_enqueue(ccnl, ccnl->ifs[RIOT_TRANS_IDX].broadcast_face, buf_dup(i->pkt));
    }

    return;
}

struct ccnl_interest_s *
ccnl_interest_remove(struct ccnl_relay_s *ccnl, struct ccnl_interest_s *i)
{
    struct ccnl_interest_s *i2;
    DEBUGMSG(40, "ccnl_interest_remove %p\n", (void *) i);

    while (i->pending) {
        struct ccnl_pendint_s *tmp = i->pending->next;
        ccnl_free(i->pending);
        i->pending = tmp;
    }

    i2 = i->next;
    DBL_LINKED_LIST_REMOVE(ccnl->pit, i);
    free_prefix(i->prefix);
    free_3ptr_list(i->ppkd, i->pkt, i);
    return i2;
}

// ----------------------------------------------------------------------
// handling of content messages

int ccnl_i_prefixof_c(struct ccnl_prefix_s *prefix, struct ccnl_buf_s *ppkd,
                      int minsuffix, int maxsuffix, struct ccnl_content_s *c)
{
    unsigned char *md;
    DEBUGMSG(99, "ccnl_i_prefixof_c prefix=%s min=%d max=%d\n",
             ccnl_prefix_to_path(prefix), minsuffix, maxsuffix);

    // CONFORM: we do prefix match, honour min. and maxsuffix,
    // and check the PublisherPublicKeyDigest if present

    // NON-CONFORM: "Note that to match a ContentObject must satisfy
    // all of the specifications given in the Interest Message."
    // >> CCNL does not honour the exclusion filtering

    if ((ppkd && !buf_equal(ppkd, c->ppkd))
        || (prefix->compcnt + minsuffix) > (c->name->compcnt + 1)
        || (prefix->compcnt + maxsuffix) < (c->name->compcnt + 1)) {
        return 0;
    }

    md = NULL;

    if ((prefix->compcnt - c->name->compcnt) == 1) {
        md = compute_ccnx_digest(c->pkt);
    }

    return ccnl_prefix_cmp(c->name, md, prefix, CMP_MATCH) == prefix->compcnt;
}

struct ccnl_content_s *
ccnl_content_new(struct ccnl_relay_s *ccnl, struct ccnl_buf_s **pkt,
                 struct ccnl_prefix_s **prefix, struct ccnl_buf_s **ppkd,
                 unsigned char *content, int contlen)
{

    (void) ccnl; /* unused */

    struct ccnl_content_s *c;
    //    DEBUGMSG(99, "ccnl_content_new <%s>\n",
    //            prefix == NULL ? NULL : ccnl_prefix_to_path(*prefix));

    c = (struct ccnl_content_s *) ccnl_calloc(1, sizeof(struct ccnl_content_s));

    if (!c) {
        return NULL;
    }

    ccnl_get_timeval(&c->last_used);
    c->content = content;
    c->contentlen = contlen;
    c->pkt = *pkt;
    *pkt = NULL;
    c->name = *prefix;
    *prefix = NULL;

    if (ppkd) {
        c->ppkd = *ppkd;
        *ppkd = NULL;
    }

    return c;
}

struct ccnl_content_s *
ccnl_content_remove(struct ccnl_relay_s *ccnl, struct ccnl_content_s *c)
{
    struct ccnl_content_s *c2;
    DEBUGMSG(99, "ccnl_content_remove: %s\n", ccnl_prefix_to_path(c->name));

    c2 = c->next;
    DBL_LINKED_LIST_REMOVE(ccnl->contents, c);
    free_content(c);
    ccnl->contentcnt--;
    return c2;
}

struct ccnl_content_s *
ccnl_content_add2cache(struct ccnl_relay_s *ccnl, struct ccnl_content_s *c)
{
    DEBUGMSG(99, "ccnl_content_add2cache (%d/%d)\n", ccnl->contentcnt,
             ccnl->max_cache_entries);

    if (ccnl->max_cache_entries <= 0) {
        DEBUGMSG(1, "  content store disabled...\n");
        return NULL;
    }

    while (ccnl->max_cache_entries <= ccnl->contentcnt) {
        DEBUGMSG(1, "  remove Least Recently Used content...\n");
        struct ccnl_content_s *c2, *lru = NULL;

        for (c2 = ccnl->contents; c2; c2 = c2->next) {
            DEBUGMSG(1, "    '%s' -> %ld:%ld\n", ccnl_prefix_to_path(c2->name), c2->last_used.tv_sec, c2->last_used.tv_usec);
            if (!(c2->flags & CCNL_CONTENT_FLAGS_STATIC)
                && ((!lru) || timevaldelta(&c2->last_used, &lru->last_used) < 0)) {
                lru = c2;
            }
        }

        if (lru) {
            DEBUGMSG(1, "   replaced: '%s'\n", ccnl_prefix_to_path(lru->name));
            ccnl_content_remove(ccnl, lru);
        }
        else {
            DEBUGMSG(1, "   no dynamic content to remove...\n");
            break;
        }
    }

    DEBUGMSG(1, "  add new content to store: '%s'\n", ccnl_prefix_to_path(c->name));
    DBL_LINKED_LIST_ADD(ccnl->contents, c);
    ccnl->contentcnt++;
    return c;
}

// deliver new content c to all clients with (loosely) matching interest,
// but only one copy per face
// returns: number of forwards
int ccnl_content_serve_pending(struct ccnl_relay_s *ccnl,
                               struct ccnl_content_s *c,
                               struct ccnl_face_s *from)
{
    struct ccnl_interest_s *i;
    struct ccnl_face_s *f;
    int cnt = 0;
    DEBUGMSG(99, "ccnl_content_serve_pending\n");

    for (f = ccnl->faces; f; f = f->next) {
        f->flags &= ~CCNL_FACE_FLAGS_SERVED;    // reply on a face only once
    }

    for (i = ccnl->pit; i;) {
        struct ccnl_pendint_s *pi;

        if (!ccnl_i_prefixof_c(i->prefix, i->ppkd, i->minsuffix, i->maxsuffix,
                               c)) {
            i = i->next;
            continue;
        }

        // CONFORM: "Data MUST only be transmitted in response to
        // an Interest that matches the Data."
        for (pi = i->pending; pi; pi = pi->next) {
            if (pi->face->flags & CCNL_FACE_FLAGS_SERVED) {
                continue;
            }

            if (pi->face == from) {
                // the existing pending interest is from the same face
                // as the newly arrived content is...no need to send content back
                DEBUGMSG(1, "  detected looping content, before loop could happen\n");
                continue;
            }

            pi->face->flags |= CCNL_FACE_FLAGS_SERVED;

            DEBUGMSG(6, "  forwarding content <%s>\n",
                     ccnl_prefix_to_path(c->name));
            pi->face->stat.send_content[c->served_cnt % CCNL_MAX_CONTENT_SERVED_STAT]++;
            ccnl_face_enqueue(ccnl, pi->face, buf_dup(c->pkt));

            c->served_cnt++;
            ccnl_get_timeval(&c->last_used);
            cnt++;
        }

        i = ccnl_interest_remove(ccnl, i);
    }

    return cnt;
}

/**
 * returns an entry from the fib which has a common prefix with p.
 * only returns that entry if it fullfils the aggregate threshold!
 */
struct ccnl_forward_s *ccn_forward_find_common_prefix_to_aggregate(struct ccnl_relay_s *ccnl, struct ccnl_prefix_s *p, int *match_len)
{
    if (!ccnl->fib) {
        DEBUGMSG(999, "ccn_forward_find_common_prefix: fib was empty\n");
        return NULL;
    }

    /* fib as at least one enty */
    struct ccnl_forward_s *fwd2 = ccnl->fib;
    while (fwd2) {
        DEBUGMSG(1, "ccn_forward_find_common_prefix: '%s' vs. '%s'\n", ccnl_prefix_to_path(p), ccnl_prefix_to_path(fwd2->prefix));
        *match_len = ccnl_prefix_cmp(fwd2->prefix, 0, p, CMP_LONGEST);

        /* check for threshold and never date up a static enty */
        if ((ccnl->fib_threshold_aggregate <= *match_len) && !(fwd2->flags & CCNL_FORWARD_FLAGS_STATIC)) {
            return fwd2;
        }

        fwd2 = fwd2->next;
    }

    return NULL;
}

static struct ccnl_forward_s *ccnl_forward_new(struct ccnl_prefix_s *p, struct ccnl_face_s *f, int threshold_prefix, int flags)
{
    struct ccnl_forward_s *fwd = ccnl_calloc(1, sizeof(struct ccnl_forward_s));
    fwd->prefix = ccnl_prefix_clone_strip(p, threshold_prefix);
    fwd->face = f;
    fwd->flags = flags;
    return fwd;
}

void ccnl_content_learn_name_route(struct ccnl_relay_s *ccnl, struct ccnl_prefix_s *p, struct ccnl_face_s *f, int threshold_prefix, int flags)
{
    /*
     * we want to insert a new dynamic route in the fib, let's try to aggregate!
     * for aggregation we ask the fib for a prefix match
     */
    int match_len;
    struct ccnl_forward_s *fwd = ccn_forward_find_common_prefix_to_aggregate(ccnl, p, &match_len);
    if (!fwd) {
        /* there was no prefix match with the user defined creteria. */

        /* create a new fib entry */
        fwd = ccnl_forward_new(p, f, threshold_prefix, flags);
        DBL_LINKED_LIST_ADD(ccnl->fib, fwd);
        DEBUGMSG(999, "ccnl_content_learn_name_route: new route '%s' on face %d learned\n", ccnl_prefix_to_path(fwd->prefix), f->faceid);
    }
    else {
        /* there was a prefix match with the user defined creteria. */

        /* if the new entry has shorter prefix */
        if (p->compcnt < fwd->prefix->compcnt) {
            /* we need to aggregate! */
            DBL_LINKED_LIST_REMOVE(ccnl->fib, fwd);

            /* create a new fib entry */
            fwd = ccnl_forward_new(p, f, (p->compcnt - match_len), flags);
            DBL_LINKED_LIST_ADD(ccnl->fib, fwd);
            DEBUGMSG(999, "ccnl_content_learn_name_route: route '%s' on face %d replaced\n", ccnl_prefix_to_path(fwd->prefix), f->faceid);
        }
        else {
            /* we don't need to do an update, because we know a shorter prefix already */
        }
    }

    /* refresh fwd entry */
    DEBUGMSG(999, "ccnl_content_learn_name_route refresh route '%s' on face %d\n", ccnl_prefix_to_path(fwd->prefix), f->faceid);
    ccnl_get_timeval(&fwd->last_used);
}

struct ccnl_forward_s *
ccnl_forward_remove(struct ccnl_relay_s *ccnl, struct ccnl_forward_s *fwd)
{
    struct ccnl_forward_s *fwd2;
    DEBUGMSG(40, "ccnl_forward_remove %p\n", (void *) fwd);

    fwd2 = fwd->next;
    DBL_LINKED_LIST_REMOVE(ccnl->fib, fwd);

    for (struct ccnl_interest_s *p = ccnl->pit; p; p = p->next) {
        if (p->forwarded_over == fwd) {
            p->forwarded_over = NULL;
        }
    }
    DEBUGMSG(40, "  ccnl_forward_remove next=%p\n", (void *) fwd2);

    free_forward(fwd);
    return fwd2;
}

bool ccnl_is_timed_out(struct timeval *now, struct timeval *last_used, uint32_t timeout_s, uint32_t timeout_us)
{
    timex_t time = timex_set(timeout_s, timeout_us);
    timex_normalize(&time);
    struct timeval abs_timeout = { last_used->tv_sec + time.seconds, last_used->tv_usec + time.microseconds };
    return timevaldelta(now, &abs_timeout) > 0;
}

void ccnl_do_retransmit(void *ptr, void *dummy)
{
    (void) dummy; /* unused */

    struct ccnl_relay_s *relay = (struct ccnl_relay_s *) ptr;

    for (struct ccnl_interest_s *i = relay->pit; i; i = i->next) {
        // CONFORM: "Entries in the PIT MUST timeout rather
        // than being held indefinitely."
        if(i->retries <= CCNL_MAX_INTEREST_RETRANSMIT) {
            // CONFORM: "A node MUST retransmit Interest Messages
            // periodically for pending PIT entries."
            DEBUGMSG(7, " retransmit %d <%s>\n", i->retries,
                     ccnl_prefix_to_path(i->prefix));

            if (i->forwarded_over
                && !(i->forwarded_over->flags & CCNL_FORWARD_FLAGS_STATIC)
                && (i->retries >= CCNL_MAX_INTEREST_OPTIMISTIC)) {
                DEBUGMSG(1, "  removed dynamic forward %p\n", (void *) i->forwarded_over);
                ccnl_forward_remove(relay, i->forwarded_over);
            }

            i->retries++;
            ccnl_interest_propagate(relay, i);
        }
    }
}

void ccnl_do_nonce_timeout(void *ptr, void *dummy)
{
    (void) dummy; /* unused */

    struct ccnl_relay_s *relay = (struct ccnl_relay_s *) ptr;

    struct timeval now;
    ccnl_get_timeval(&now);
    //DEBUGMSG(99, "ccnl_do_nonce_timeout: %lu:%lu\n", now.tv_sec, now.tv_usec);

    for (struct ccnl_nonce_s *n = relay->nonces; n;) {
        if (ccnl_is_timed_out(&now, &n->created, CCNL_NONCE_TIMEOUT_SEC, CCNL_NONCE_TIMEOUT_USEC)) {
            n = ccnl_nonce_remove(relay, n);
        } else {
            n = n->next;
        }
    }
}

void ccnl_do_ageing(void *ptr, void *dummy)
{

    (void) dummy; /* unused */

    struct ccnl_relay_s *relay = (struct ccnl_relay_s *) ptr;
    struct ccnl_interest_s *i = relay->pit;
    struct ccnl_content_s *c = relay->contents;

    struct ccnl_face_s *f = relay->faces;
    struct timeval now;
    ccnl_get_timeval(&now);
    //DEBUGMSG(999, "ccnl_do_ageing %ld:%ld\n", now.tv_sec, now.tv_usec);

    while (i) {
        if (ccnl_is_timed_out(&now, &i->last_used, CCNL_INTEREST_TIMEOUT_SEC,
                CCNL_INTEREST_TIMEOUT_USEC)) {
            if (i->from->ifndx == RIOT_MSG_IDX) {
                /* this interest was requested by an app from this node */
                /* inform this app about this problem */
                riot_send_nack(i->from->faceid);
            }
            i = ccnl_interest_remove(relay, i);
        }
        else {
            i = i->next;
        }
    }

    while (c) {
        if (ccnl_is_timed_out(&now, &c->last_used, CCNL_CONTENT_TIMEOUT_SEC, CCNL_CONTENT_TIMEOUT_USEC)
            && !(c->flags & CCNL_CONTENT_FLAGS_STATIC)) {
            c = ccnl_content_remove(relay, c);
        }
        else {
            c = c->next;
        }
    }

    while (f) {
        if (!(f->flags & CCNL_FACE_FLAGS_STATIC)
            && ccnl_is_timed_out(&now, &f->last_used, CCNL_FACE_TIMEOUT_SEC, CCNL_FACE_TIMEOUT_USEC)) {
            f = ccnl_face_remove(relay, f);
        }
        else {
            f = f->next;
        }
    }

    struct ccnl_forward_s *fwd = relay->fib;
    while (fwd) {
        if (!(fwd->flags & CCNL_FORWARD_FLAGS_STATIC)
            && ccnl_is_timed_out(&now, &fwd->last_used, CCNL_FWD_TIMEOUT_SEC, CCNL_FWD_TIMEOUT_USEC)) {
            fwd = ccnl_forward_remove(relay, fwd);
        }
        else {
            fwd = fwd->next;
        }
    }
}

void ccnl_core_cleanup(struct ccnl_relay_s *ccnl)
{
    int k;
    DEBUGMSG(99, "ccnl_core_cleanup %p\n", (void *) ccnl);

    while (ccnl->pit) {
        ccnl_interest_remove(ccnl, ccnl->pit);
    }

    while (ccnl->faces) {
        ccnl_face_remove(ccnl, ccnl->faces);    // also removes all FWD entries
    }

    while (ccnl->contents) {
        ccnl_content_remove(ccnl, ccnl->contents);
    }

    while (ccnl->nonces) {
        ccnl_nonce_remove(ccnl, ccnl->nonces);
    }

    for (k = 0; k < ccnl->ifcount; k++) {
        ccnl_interface_cleanup(ccnl->ifs + k);
    }
}

// ----------------------------------------------------------------------
// the core logic of CCN:

int ccnl_core_RX_i_or_c(struct ccnl_relay_s *relay, struct ccnl_face_s *from,
                        unsigned char **data, int *datalen)
{
    int rc = -1, scope = 3, aok = 3, minsfx = 0, maxsfx = CCNL_MAX_NAME_COMP,
        contlen;
    struct ccnl_buf_s *buf = 0, *nonce = 0, *ppkd = 0;
    struct ccnl_interest_s *i = 0;
    struct ccnl_content_s *c = 0;
    struct ccnl_prefix_s *p = 0;
    unsigned char *content = 0;
    DEBUGMSG(1, "ccnl_core_RX_i_or_c: (%d bytes left)\n", *datalen);

    buf = ccnl_extract_prefix_nonce_ppkd(data, datalen, &scope, &aok, &minsfx,
                                         &maxsfx, &p, &nonce, &ppkd, &content, &contlen);

    if (!buf) {
        DEBUGMSG(6, "  parsing error or no prefix\n");
        goto Done;
    }

    if (nonce && ccnl_nonce_find_or_append(relay, nonce)) {
        DEBUGMSG(6, "  dropped because of duplicate nonce\n");
        goto Skip;
    }

    if (buf->data[0] == 0x01 && buf->data[1] == 0xd2) { // interest
        DEBUGMSG(1, "ccnl_core_RX_i_or_c: interest=<%s>\n", ccnl_prefix_to_path(p));
        from->stat.received_interest++;

        if (p->compcnt > 0 && p->comp[0][0] == (unsigned char) 0xc1) {
            goto Skip;
        }

        if (p->compcnt == 4 && !memcmp(p->comp[0], "ccnx", 4)) {
            DEBUGMSG(1, "it's a mgnt msg!\n");
            rc = ccnl_mgmt(relay, buf, p, from);
            DEBUGMSG(1, "mgnt processing done!\n");
            goto Done;
        }

        // CONFORM: Step 1:
        if (aok & 0x01) { // honor "answer-from-existing-content-store" flag
            for (c = relay->contents; c; c = c->next) {
                if (!ccnl_i_prefixof_c(p, ppkd, minsfx, maxsfx, c)) {
                    continue;
                }

                // FIXME: should check stale bit in aok here
                DEBUGMSG(7, "  matching content for interest, content %p\n",
                         (void *) c);
                from->stat.send_content[c->served_cnt % CCNL_MAX_CONTENT_SERVED_STAT]++;
                c->served_cnt++;

                if (from->ifndx >= 0) {
                    ccnl_face_enqueue(relay, from, buf_dup(c->pkt));
                }

                goto Skip;
            }
        }

        // CONFORM: Step 2: check whether interest is already known
        for (i = relay->pit; i; i = i->next) {
            if (!ccnl_prefix_cmp(i->prefix, NULL, p, CMP_EXACT)
                && i->minsuffix == minsfx && i->maxsuffix == maxsfx
                && ((!ppkd && !i->ppkd) || buf_equal(ppkd, i->ppkd))) {
                break;
            }
        }

        if (!i) { // this is a new/unknown I request: create and propagate
            i = ccnl_interest_new(relay, from, &buf, &p, minsfx, maxsfx, &ppkd);

            if (i) { // CONFORM: Step 3 (and 4)
                DEBUGMSG(7, "  created new interest entry %p\n", (void *) i);

                if (scope > 2) {
                    ccnl_interest_propagate(relay, i);
                }
            }
        }
        else if (scope > 2 && (from->flags & CCNL_FACE_FLAGS_FWDALLI)) {
            DEBUGMSG(7, "  old interest, nevertheless propagated %p\n",
                     (void *) i);
            ccnl_interest_propagate(relay, i);
        }

        if (i) { // store the I request, for the incoming face (Step 3)
            DEBUGMSG(7, "  appending interest entry %p\n", (void *) i);
            ccnl_interest_append_pending(i, from);
        }
    }
    else {   // content
        DEBUGMSG(6, "  content=<%s>\n", ccnl_prefix_to_path(p));
        from->stat.received_content++;

        // CONFORM: Step 1:
        for (c = relay->contents; c; c = c->next) {
            if (buf_equal(c->pkt, buf)) {
                DEBUGMSG(1, "content is dup: skip\n");
                goto Skip;
            }
        }

        c = ccnl_content_new(relay, &buf, &p, &ppkd, content, contlen);

        if (c) { // CONFORM: Step 2 (and 3)
            if (!ccnl_content_serve_pending(relay, c, from)) { // unsolicited content
                // CONFORM: "A node MUST NOT forward unsolicited data [...]"
                DEBUGMSG(7, "  removed because no matching interest\n");
                free_content(c);
                goto Skip;
            }
#if CCNL_DYNAMIC_FIB
            else {
                /* content has matched an interest, we consider this name as available on this face */
                ccnl_content_learn_name_route(relay, c->name, from, relay->fib_threshold_prefix, 0);
            }
#endif

            if (relay->max_cache_entries != 0) { // it's set to -1 or a limit
                DEBUGMSG(7, "  adding content to cache\n");
                ccnl_content_add2cache(relay, c);
            }
            else {
                DEBUGMSG(7, "  content not added to cache\n");
                free_content(c);
            }
        }
    }

Skip:
    rc = 0;
Done:
    free_prefix(p);
    free_3ptr_list(buf, nonce, ppkd);
    DEBUGMSG(1, "leaving\n");
    return rc;
}

int ccnl_core_RX_datagram(struct ccnl_relay_s *relay, struct ccnl_face_s *from,
                          unsigned char **data, int *datalen)
{
    int rc = 0, num = 0, typ = 0;
    DEBUGMSG(1, "ccnl_core_RX_datagram: %d bytes from face=%p (id=%d.%d)\n",
             *datalen, (void *) from, relay->id, from ? from->faceid : -1);

    while (rc >= 0 && *datalen > 0) {
        if (dehead(data, datalen, &num, &typ) || typ != CCN_TT_DTAG) {
            return -1;
        }

        switch (num) {
            case CCN_DTAG_INTEREST:
                /* no break */

            case CCN_DTAG_CONTENTOBJ:
                rc = ccnl_core_RX_i_or_c(relay, from, data, datalen);
                continue;
#ifdef USE_FRAG

            case CCNL_DTAG_FRAGMENT2012:
                rc = ccnl_frag_RX_frag2012(ccnl_core_RX_datagram, relay, from, data, datalen);
                continue;

            case CCNL_DTAG_FRAGMENT:
                rc = ccnl_frag_RX_CCNx2013(ccnl_core_RX_datagram, relay, from, data, datalen);
                continue;
#endif

            default:
                DEBUGMSG(15, "  unknown datagram type %d\n", num);
                return -1;
        }
    }

    return rc;
}

void
ccnl_core_RX(struct ccnl_relay_s *relay, int ifndx, unsigned char *data,
             int datalen, uint16_t sender_id)
{
    struct ccnl_face_s *from;
    DEBUGMSG(14, "ccnl_core_RX ifndx=%d, %d bytes\n", ifndx, datalen);

    from = ccnl_get_face_or_create(relay, ifndx, sender_id);

    if (!from) {
        return;
    }

    DEBUGMSG(1, "ccnl_core_RX: faceid=%d frag=%p\n", from->faceid, (void *) from->frag);

    ccnl_core_RX_datagram(relay, from, &data, &datalen);
}

// eof
