/*
 * @f ccnl.h
 * @b header file for CCN lite (CCNL)
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
 * 2011-03-30 created
 */

#ifdef __cplusplus
extern "C" {
#endif

#define CCNL_MAX_INTERFACES             2 /* transceiver and msg interfaces */

#define CCNL_INTEREST_TIMEOUT_SEC       0
#define CCNL_INTEREST_TIMEOUT_USEC      ((CCNL_CHECK_RETRANSMIT_USEC) * ((CCNL_MAX_INTEREST_RETRANSMIT) + 1))

#define CCNL_CONTENT_TIMEOUT_SEC        2
#define CCNL_CONTENT_TIMEOUT_USEC       0

#define CCNL_NONCE_TIMEOUT_SEC          0
#define CCNL_NONCE_TIMEOUT_USEC         (3 * (CCNL_CHECK_RETRANSMIT_USEC))

#define CCNL_MAX_CONTENT_SERVED_STAT    10

#define CCNL_MAX_INTEREST_RETRANSMIT    5
#define CCNL_MAX_INTEREST_OPTIMISTIC    2

#define CCNL_FACE_TIMEOUT_SEC           10
#define CCNL_FACE_TIMEOUT_USEC          0

#define CCNL_FWD_TIMEOUT_SEC            10
#define CCNL_FWD_TIMEOUT_USEC           0

#define CCNL_CHECK_TIMEOUT_SEC          1
#define CCNL_CHECK_TIMEOUT_USEC         0

#define CCNL_CHECK_RETRANSMIT_SEC       0
#define CCNL_CHECK_RETRANSMIT_USEC      (300 * 1000)

#define CCNL_MAX_NAME_COMP              16
#define CCNL_MAX_IF_QLEN                64

#define CCNL_MAX_NONCES                 256 // for detected dups

#define TIMEOUT_TO_US(SEC, USEC) ((SEC)*1000*1000 + (USEC))

// ----------------------------------------------------------------------
// our own CCN-lite extensions for the ccnb encoding:

// management protocol: (ccnl-ext-mgmt.c)
#define CCNL_DTAG_MACSRC    99001 // newface: which L2 interface
#define CCNL_DTAG_IP4SRC    99002 // newface: which L3 interface
#define CCNL_DTAG_UNIXSRC   99003 // newface: which UNIX path
#define CCNL_DTAG_FRAG      99004 // fragmentation protocol, see core.h
#define CCNL_DTAG_FACEFLAGS 99005 //
#define CCNL_DTAG_DEVINSTANCE   99006 // adding/removing a device/interface
#define CCNL_DTAG_DEVNAME   99007 // name of interface (eth0, wlan0)
#define CCNL_DTAG_DEVFLAGS  99008 //
#define CCNL_DTAG_MTU       99009 //

#define CCNL_DTAG_DEBUGREQUEST  99100 //
#define CCNL_DTAG_DEBUGACTION   99101 // dump, halt, dump+halt

//FOR THE DEBUG_REPLY MSG
#define CCNL_DTAG_DEBUGREPLY    99201 // dump reply
#define CCNL_DTAG_INTERFACE     99202 // interface list
#define CCNL_DTAG_NEXT          99203 // next pointer e.g. for faceinstance
#define CCNL_DTAG_PREV          99204 // prev pointer e.g. for faceinstance
#define CCNL_DTAG_IFNDX         99205
#define CCNL_DTAG_IP            99206
#define CCNL_DTAG_ETH           99207
#define CCNL_DTAG_UNIX          99208
#define CCNL_DTAG_PEER          99209
#define CCNL_DTAG_FWD           99210
#define CCNL_DTAG_FACE          99211
#define CCNL_DTAG_ADDRESS       99212
#define CCNL_DTAG_SOCK          99213
#define CCNL_DTAG_REFLECT       99214
#define CCNL_DTAG_PREFIX        99215
#define CCNL_DTAG_INTERESTPTR   99216
#define CCNL_DTAG_LAST          99217
#define CCNL_DTAG_MIN           99218
#define CCNL_DTAG_MAX           99219
#define CCNL_DTAG_RETRIES       99220
#define CCNL_DTAG_PUBLISHER     99221
#define CCNL_DTAG_CONTENTPTR    99222
#define CCNL_DTAG_LASTUSE       99223
#define CCNL_DTAG_SERVEDCTN     99224

// fragmentation protocol: (ccnl-ext-frag.c, FRAG_SEQUENCED2012)
#define CCNL_DTAG_FRAGMENT2012  144144 // http://redmine.ccnx.org/issues/100803
#define CCNL_DTAG_FRAGMENT  144199 // pending request (2013-07-24)

#define CCNL_DTAG_FRAG_TYPE (CCNL_DTAG_FRAGMENT+1)
#define CCNL_DTAG_FRAG_FLAGS    (CCNL_DTAG_FRAGMENT+2)
#define CCNL_DTAG_FRAG_SEQNR    (CCNL_DTAG_FRAGMENT+3)  // our seq number

#define CCNL_DTAG_FRAG_OLOSS    (CCNL_DTAG_FRAGMENT+5)  // our loss count
#define CCNL_DTAG_FRAG_YSEQN    (CCNL_DTAG_FRAGMENT+6)  // your (highest) seq no
/*
#define CCNL_DTAG_FRAG_YSEQN16  (CCNL_DTAG_FRAGMENT+4)
#define CCNL_DTAG_FRAG_YSEQN32  (CCNL_DTAG_FRAGMENT+5)
*/
#define CCNL_DTAG_FRAG_FLAG_MASK    0x03
#define CCNL_DTAG_FRAG_FLAG_FIRST   0x01
#define CCNL_DTAG_FRAG_FLAG_MID     0x00
#define CCNL_DTAG_FRAG_FLAG_LAST    0x02
#define CCNL_DTAG_FRAG_FLAG_SINGLE  0x03

#define CCNL_DTAG_FRAG_FLAG_STATUSREQ   0x04

// echo "FHBH" | base64 -d | hexdump -v -e '/1 "@x%02x"'| tr @ '\\'; echo
#define CCNL_FRAG_TYPE_CCNx2013_VAL     "\x14\x70\x47"

// fragmentation protocol: (ccnl-ext-frag.c, FRAG_WIRE2013)
#define CCNL_DTAG_WIRE      333333
#define CCNL_DTAG_WFRAG_FLAGS   (CCNL_DTAG_WIRE+1)
#define CCNL_DTAG_WFRAG_OSEQN   (CCNL_DTAG_WIRE+2)  // our seq number
#define CCNL_DTAG_WFRAG_OLOSS   (CCNL_DTAG_WIRE+3)  // our loss count
#define CCNL_DTAG_WFRAG_YSEQN   (CCNL_DTAG_WIRE+4)  // your (highest) seq no
#define CCNL_DTAG_WFRAG_FLAG_FIRST  0x01
#define CCNL_DTAG_WFRAG_FLAG_LAST   0x02

#include "debug.h"

// function alias for RIOTs debug infrastructure
#define DEBUGMSG(LVL, ...) DEBUG(__VA_ARGS__)

#ifdef __cplusplus
}
#endif

// eof
