/*
 * @f ccnl-core.h
 * @b CCN lite (CCNL), core header file (internal data structures)
 *
 * Copyright (C) 2011-13, Christian Tschudin, University of Basel
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
 * 2013-03-19 updated (ms): modified struct ccnl_relay_s for 'aux' field
 */

#ifndef CCNL_CORE_H__
#define CCNL_CORE_H__

#define EXACT_MATCH 1
#define PREFIX_MATCH 0

#define CMP_EXACT   0 // used to compare interests among themselves
#define CMP_MATCH   1 // used to match interest and content
#define CMP_LONGEST 2 // used to lookup the FIB

#define CCNL_FACE_FLAGS_STATIC  1
#define CCNL_FACE_FLAGS_REFLECT 2
#define CCNL_FACE_FLAGS_SERVED  4
#define CCNL_FACE_FLAGS_FWDALLI 8 // forward all interests, also known ones
#define CCNL_FACE_FLAGS_BROADCAST  16

#define CCNL_FRAG_NONE      0
#define CCNL_FRAG_SEQUENCED2012 1
#define CCNL_FRAG_CCNx2013  2

#define CCNL_CONTENT_FLAGS_STATIC  0x01
#define CCNL_CONTENT_FLAGS_STALE   0x02

#define CCNL_FORWARD_FLAGS_STATIC  0x01

#include <inttypes.h>
#include <time.h>
#include <sys/time.h>

#include "mutex.h"

#include "ccnl.h"

// ----------------------------------------------------------------------

typedef union {
    uint16_t id;
} sockunion;

struct ccnl_txrequest_s {
    struct ccnl_buf_s *buf;
    sockunion dst;
    void (*txdone)(void *, int, int);
    struct ccnl_face_s *txdone_face;
};

struct ccnl_if_s { // interface for packet IO
    sockunion addr;
    int sock;
    int (*sendfunc)(uint8_t *, uint16_t, uint16_t);
    int reflect; // whether to reflect I packets on this interface
    int fwdalli; // whether to forward all I packets rcvd on this interface
    int mtu;

    int qlen;  // number of pending sends
    int qfront; // index of next packet to send
    struct ccnl_txrequest_s queue[CCNL_MAX_IF_QLEN];
    struct ccnl_sched_s *sched;
    struct ccnl_face_s *broadcast_face;
};

struct ccnl_relay_s {
    struct timeval startup_time;
    int id;
    struct ccnl_face_s *faces;
    struct ccnl_forward_s *fib;
    struct ccnl_interest_s *pit;
    struct ccnl_content_s *contents; //, *contentsend;
    struct ccnl_nonce_s *nonces;
    int contentcnt;     // number of cached items
    int max_cache_entries;  // -1: unlimited
    struct ccnl_if_s ifs[CCNL_MAX_INTERFACES];
    int ifcount;        // number of active interfaces
    char halt_flag;
    struct ccnl_sched_s *(*defaultFaceScheduler)(struct ccnl_relay_s *,
            void(*cts_done)(void *, void *));
    struct ccnl_sched_s *(*defaultInterfaceScheduler)(struct ccnl_relay_s *,
            void(*cts_done)(void *, void *));
    struct ccnl_http_s *http;
    struct ccnl_stats_s *stats;
    void *aux;
    int fib_threshold_prefix; /* how may name components should be considdered as dynamic */
    int fib_threshold_aggregate;
    int riot_pid;
    int riot_helper_pid;
    mutex_t global_lock;
    mutex_t stop_lock;
};

struct ccnl_buf_s {
    struct ccnl_buf_s *next;
    unsigned int datalen;
    unsigned char data[1];
};

struct ccnl_nonce_s {
    struct ccnl_nonce_s *next, *prev;
    struct timeval created;
    struct ccnl_buf_s *buf;
};

struct ccnl_prefix_s {
    unsigned char **comp;
    int *complen;
    int compcnt;
    unsigned char *path; // memory for name component copies
};

struct ccnl_stat_s {
    int send_interest[CCNL_MAX_INTEREST_RETRANSMIT];
    int send_content[CCNL_MAX_CONTENT_SERVED_STAT];
    int received_interest;
    int received_content;
};

struct ccnl_frag_s {
    int protocol; // (0=plain CCNx)
    int mtu;
    sockunion dest;
    struct ccnl_buf_s *bigpkt;
    unsigned int sendoffs;
    // transport state, if present:
    int ifndx;

    struct ccnl_buf_s *defrag;

    unsigned int sendseq;
    unsigned int losscount;
    unsigned int recvseq;
    unsigned char flagwidth;
    unsigned char sendseqwidth;
    unsigned char losscountwidth;
    unsigned char recvseqwidth;
};

struct ccnl_face_s {
    struct ccnl_face_s *next, *prev;
    int faceid;
    int ifndx;
    sockunion peer;
    int flags;
    struct timeval last_used; // updated when we receive a packet
    struct ccnl_buf_s *outq, *outqend; // queue of packets to send
    struct ccnl_frag_s *frag;  // which special datagram armoring
    struct ccnl_sched_s *sched;

    struct ccnl_stat_s stat;
};

struct ccnl_forward_s {
    struct ccnl_forward_s *next, *prev;
    struct ccnl_prefix_s *prefix;
    struct ccnl_face_s *face;
    int flags;
    struct timeval last_used; // updated when we use this fib entry
};

struct ccnl_interest_s {
    struct ccnl_interest_s *next, *prev;
    struct ccnl_face_s *from;
    struct ccnl_pendint_s *pending; // linked list of faces wanting that content
    struct ccnl_prefix_s *prefix;
    int minsuffix, maxsuffix;
    struct ccnl_buf_s *ppkd;       // publisher public key digest
    struct ccnl_buf_s *pkt;    // full datagram
    struct timeval last_used;
    int retries;
    struct ccnl_forward_s *forwarded_over;
};

struct ccnl_pendint_s { // pending interest
    struct ccnl_pendint_s *next; // , *prev;
    struct ccnl_face_s *face;
    struct timeval last_used;
};

struct ccnl_content_s {
    struct ccnl_content_s *next, *prev;
    struct ccnl_prefix_s *name;
    struct ccnl_buf_s *ppkd; // publisher public key digest
    struct ccnl_buf_s *pkt; // full datagram
    int flags;
    unsigned char *content; // pointer into the data buffer
    int contentlen;
    // NON-CONFORM: "The [ContentSTore] MUST also implement the Staleness Bit."
    // >> CCNL: currently no stale bit, old content is fully removed <<
    struct timeval last_used;
    int served_cnt;
};

// ----------------------------------------------------------------------
// macros for double linked lists (these double linked lists are not rings)

#define DBL_LINKED_LIST_ADD(l,e) \
  do { if ((l)) (l)->prev = (e); \
       (e)->next = (l); \
       (l) = (e); \
  } while(0)

#define DBL_LINKED_LIST_REMOVE(l,e) \
  do { if ((l) == (e)) (l) = (e)->next; \
       if ((e)->prev) (e)->prev->next = (e)->next; \
       if ((e)->next) (e)->next->prev = (e)->prev; \
  } while(0)

// ----------------------------------------------------------------------
// collect the USE_* macros in a string
const char *compile_string(void);

int consume(int typ, int num, unsigned char **buf, int *len,
            unsigned char **valptr, int *vallen);

void
ccnl_core_RX(struct ccnl_relay_s *relay, int ifndx, unsigned char *data,
             int datalen, uint16_t sender_id);

void ccnl_core_cleanup(struct ccnl_relay_s *ccnl);

struct ccnl_buf_s *
ccnl_buf_new(void *data, int len);

struct ccnl_content_s *
ccnl_content_new(struct ccnl_relay_s *ccnl, struct ccnl_buf_s **pkt,
                 struct ccnl_prefix_s **prefix, struct ccnl_buf_s **ppkd,
                 unsigned char *content, int contlen);

struct ccnl_content_s *
ccnl_content_add2cache(struct ccnl_relay_s *ccnl, struct ccnl_content_s *c);

void ccnl_content_learn_name_route(struct ccnl_relay_s *ccnl, struct ccnl_prefix_s *p,
                                   struct ccnl_face_s *f, int threshold_prefix, int flags);

struct ccnl_face_s *
ccnl_get_face_or_create(struct ccnl_relay_s *ccnl, int ifndx, uint16_t sender_id);

int ccnl_face_enqueue(struct ccnl_relay_s *ccnl, struct ccnl_face_s *to,
                      struct ccnl_buf_s *buf);

struct ccnl_face_s *
ccnl_face_remove(struct ccnl_relay_s *ccnl, struct ccnl_face_s *f);

struct ccnl_forward_s *
ccnl_forward_remove(struct ccnl_relay_s *ccnl, struct ccnl_forward_s *fwd);

struct ccnl_buf_s *
ccnl_extract_prefix_nonce_ppkd(unsigned char **data, int *datalen, int *scope,
                               int *aok, int *min, int *max, struct ccnl_prefix_s **prefix,
                               struct ccnl_buf_s **nonce, struct ccnl_buf_s **ppkd,
                               unsigned char **content, int *contlen);

void ccnl_do_retransmit(void *ptr, void *dummy);
void ccnl_do_ageing(void *ptr, void *dummy);
void ccnl_do_nonce_timeout(void *ptr, void *dummy);

void ccnl_interface_CTS(void *aux1, void *aux2);

void ccnl_face_print_stat(struct ccnl_face_s *f);

#define ccnl_malloc(s)  malloc(s)
#define ccnl_calloc(n,s)    calloc(n,s)
#define ccnl_realloc(p,s)   realloc(p,s)
#define ccnl_free(p)        free(p)

void free_2ptr_list(void *a, void *b);
void free_3ptr_list(void *a, void *b, void *c);
void free_4ptr_list(void *a, void *b, void *c, void *d);
void free_prefix(struct ccnl_prefix_s *p);
void free_content(struct ccnl_content_s *c);

#endif /*CCNL_CORE_H__*/
// eof
