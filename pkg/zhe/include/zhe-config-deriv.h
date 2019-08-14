#ifndef ZHE_CONFIG_DERIV_H
#define ZHE_CONFIG_DERIV_H

#include <limits.h>
#include <inttypes.h>
#include "zhe-config-int.h"
#include "zhe-rid.h"

#define MAX_PEERS_1 (MAX_PEERS == 0 ? 1 : MAX_PEERS)

#if ! HAVE_UNICAST_CONDUIT
#  define N_OUT_MCONDUITS N_OUT_CONDUITS
#  define MAX_MULTICAST_CID (N_OUT_CONDUITS - 1)
#else
#  define UNICAST_CID (N_OUT_CONDUITS - 1)
#  define N_OUT_MCONDUITS (N_OUT_CONDUITS - 1)
#  if N_OUT_CONDUITS > 1
#    define MAX_MULTICAST_CID (N_OUT_MCONDUITS - 2)
#  endif
#endif

#if TRANSPORT_MODE != TRANSPORT_STREAM && TRANSPORT_MODE != TRANSPORT_PACKET
#  error "transport configuration did not set MODE properly"
#endif

/* There is some lower limit that really won't work anymore, but I actually know what that is, so the 16 is just a placeholder (but it is roughly correct); 16-bit unsigned indices are used to index a packet, with the maximum value used as an exceptional value, so larger than 2^16-2 is also no good; and finally, the return type of zhe_input is an int, and so the number of consumed bytes must fit in an int */
#if TRANSPORT_MTU < 16 || TRANSPORT_MTU > 65534 || TRANSPORT_MTU > INT_MAX
#  error "transport configuration did not set MTU properly"
#endif

#if MAX_PEERS > 1 && N_OUT_MCONDUITS == 0
#  error "MAX_PEERS > 1 requires presence of multicasting conduit"
#endif

#if MAX_PEERS <= 1 && ! HAVE_UNICAST_CONDUIT
#  warning "should use a unicast conduit in a client or if there can be at most one peer"
#endif

#if MAX_MULTICAST_GROUPS == 0 && N_OUT_MCONDUITS > 0
#  error "MAX_MULTICAST_GROUPS must be at least 1 for multicast group handling to work"
#endif

#if 0 < LEASE_DURATION && LEASE_DURATION <= SCOUT_INTERVAL
#  warning "scout interval should be shorter than lease duration"
#endif

/* Limiting number of scouts sent can make sense in a peer-to-peer setting, but it doesn't make sense for a client: the client only scouts when it is looking for a broker, and it may not ever find one if it doesn't keep looking */
#if MAX_PEERS == 0 && SCOUT_COUNT > 0
#  error "max scout count must be 0 in client mode"
#endif

#if MAX_PEERS < UINT8_MAX
typedef uint8_t peeridx_t;
#elif MAX_PEERS < UINT16_MAX
typedef uint16_t peeridx_t;
#else
#  error "MAX_PEERS is too large for 16-bit peer idx"
#endif
#define PEERIDX_INVALID ((peeridx_t)-1)

#if TRANSPORT_MTU < (UINT8_MAX - 2)
typedef uint8_t zhe_msgsize_t; /* type used for representing the size of an XRCE message */
#else
typedef uint16_t zhe_msgsize_t;
#endif

/* zhe_msgsize_t is the type capable of representing the maximum size of a message and may not
 be larger than the zhe_paysize_t, the type capable of representing the maximum size of a
 payload (fragmentation in principle allows for larger payload (components) than message);
 type conversions occur under the assumption that a zhe_paysize_t can always hold a zhe_msgsize_t. */
struct msgsize_leq_paysize_t {
    char req[sizeof(zhe_msgsize_t) <= sizeof(zhe_paysize_t) ? 1 : -1];
};

/* There is not a fundamental limit on the number of conduits, but there are some places
 where a conduit id is assumed to fit in a single byte in message processing, and there
 are some places where a non-negative integer is used to index a multicast conduit, and a
 negative one a unicast conduit as (-(cid_t)peeridx-1). */
#if N_OUT_CONDUITS <= 127 && N_IN_CONDUITS <= 127 && MAX_PEERS <= 127
typedef int8_t cid_t;
#  define MAX_CID_T 127
#elif N_OUT_CONDUITS <= 127 && N_IN_CONDUITS <= 127 && MAX_PEERS <= 32767
typedef int16_t cid_t;
#  define MAX_CID_T 32767
#else
#  error "Conduits are limited to 127 because the VLE encoding is short-circuited for CIDs"
#endif

#if N_IN_CONDUITS < N_OUT_CONDUITS
#  error "Validation in MCONDUIT checks against N_IN_CONDUIT, but for MACKNACK the conduit id really refers to the output conduit, so N_IN >= N_OUT unless the case for MACKNACK gets special treatment"
#endif

/* Size of sequence number in bits is "negotiated" -- that is, determined by the client, so we
 get to choose.  Sequence numbers are VLE on the wire (to allow decoding messages without
 knowing the sequence number size), a multiple of 7 bits avoids spending a byte of which only
 a few bits will be used. */
#if SEQNUM_LEN <= 8
typedef uint8_t seq_t;    /* type internally used for representing sequence numbers */
typedef int8_t sseq_t;    /* signed version of seq_t */
#define PRIuSEQ PRIu8
#elif SEQNUM_LEN <= 16
typedef uint16_t seq_t;
typedef int16_t sseq_t;
#define PRIuSEQ PRIu16
#elif SEQNUM_LEN <= 32
typedef uint32_t seq_t;
typedef int32_t sseq_t;
#define PRIuSEQ PRIu32
#elif SEQNUM_LEN <= 64
typedef uint64_t seq_t;
typedef int64_t sseq_t;
#define PRIuSEQ PRIu64
#else
#  error "SEQNUM_LEN out of range"
#endif
#define SEQNUM_SHIFT        (CHAR_BIT * sizeof(seq_t) - SEQNUM_LEN)
#define SEQNUM_UNIT         ((seq_t)((seq_t)1 << SEQNUM_SHIFT))

#if (defined(XMITW_SAMPLES) && XMITW_SAMPLES >= (1 << (SEQNUM_LEN-1))) || (defined(XMITW_SAMPLES_UNICAST) && XMITW_SAMPLES_UNICAST >= (1 << (SEQNUM_LEN-1)))
#  error "XMITW_SAMPLES or XMITW_SAMPLES_UNICAST too large for SEQNUM_LEN"
#endif

#define ZHE_NEED_ICGCB (ZHE_MAX_URISPACE > 0)

#if ZHE_TIMEBASE != 1000000
#warning "better get the time conversions correct first ..."
#endif
#define ZTIME_TO_SECu32(zt) ((uint32_t)((zt) / (1000000000 / ZHE_TIMEBASE)))
#define ZTIME_TO_MSECu32(zt) ((uint32_t)((zt) / (1000000 / ZHE_TIMEBASE)) % 1000u)

#ifdef __GNUC__
#define ZHE_NONNULL_ALL __attribute__((__nonnull__))
#define ZHE_NONNULL(l) __attribute__((__nonnull__ l))
#else
#define ZHE_NONNULL_ALL
#define ZHE_NONNULL(l)
#endif

#endif
