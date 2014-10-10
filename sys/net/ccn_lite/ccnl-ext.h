/*
 * @f ccnl-ext.h
 * @b header file for CCN lite extentions (forward declarations)
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
 * 2013-03-30 created
 */

// ----------------------------------------------------------------------

#ifndef CCNL_EXT_H__
#define CCNL_EXT_H__

#ifdef __cplusplus
extern "C" {
#endif

#define compute_ccnx_digest(buf) sha256(buf->data, buf->datalen, NULL)

#ifdef USE_FRAG

struct ccnl_frag_s *ccnl_frag_new(int protocol, int mtu);

void ccnl_frag_reset(struct ccnl_frag_s *e, struct ccnl_buf_s *buf,
                     int ifndx, sockunion *su);

int ccnl_frag_getfragcount(struct ccnl_frag_s *e, int origlen,
                           int *totallen);

struct ccnl_buf_s *ccnl_frag_getnext(struct ccnl_frag_s *e,
                                     int *ifndx, sockunion *su);

/*
struct ccnl_buf_s* ccnl_frag_handle_fragment(struct ccnl_relay_s *r,
        struct ccnl_face_s *f, unsigned char *data, int datalen);
*/

void ccnl_frag_destroy(struct ccnl_frag_s *e);

/*
struct ccnl_buf_s* ccnl_frag_fragment(struct ccnl_relay_s *ccnl,
                    struct ccnl_frag_s *frag,
                    struct ccnl_buf_s *buf);
*/

typedef int (RX_datagram)(struct ccnl_relay_s *, struct ccnl_face_s *,
                          unsigned char **, int *);

int ccnl_frag_RX_frag2012(RX_datagram callback, struct ccnl_relay_s *relay,
                          struct ccnl_face_s *from,
                          unsigned char **data, int *datalen);

int ccnl_frag_RX_CCNx2013(RX_datagram callback, struct ccnl_relay_s *relay,
                          struct ccnl_face_s *from,
                          unsigned char **data, int *datalen);

int ccnl_is_fragment(unsigned char *data, int datalen);
#else
# define ccnl_frag_new(e,u)   NULL
# define ccnl_frag_destroy(e) do{}while(0)
# define ccnl_frag_handle_fragment(r,f,data,len)    ccnl_buf_new(data,len)
# define ccnl_is_fragment(d,l)  0
#endif // USE_FRAG

// ----------------------------------------------------------------------

// ----------------------------------------------------------------------

int ccnl_mgmt(struct ccnl_relay_s *ccnl, struct ccnl_buf_s *buf,
              struct ccnl_prefix_s *prefix, struct ccnl_face_s *from);

struct ccnl_prefix_s *
ccnl_prefix_clone(struct ccnl_prefix_s *p);

struct ccnl_prefix_s *
ccnl_prefix_clone_strip(struct ccnl_prefix_s *p, int strip);

// ----------------------------------------------------------------------

# define ccnl_sched_CTS_done(S,C,L) do{}while(0)
# define ccnl_sched_destroy(S)      do{}while(0)

char *ccnl_prefix_to_path(struct ccnl_prefix_s *pr);

#ifdef __cplusplus
}
#endif

#endif /* CCNL_EXT_H__ */

// eof
