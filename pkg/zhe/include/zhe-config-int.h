/* -*- mode: c; c-basic-offset: 4; fill-column: 95; -*- */
#ifndef ZHE_CONFIG_INT_H
#define ZHE_CONFIG_INT_H

#include "zhe-config.h"

/**********************************************************************/
#ifndef TCP
/**********************************************************************/

/* Maximum number of peers one node can have (that is, the network may consist of at most MAX_PEERS+1 nodes). If MAX_PEERS is 0, it becomes a client rather than a peer, and scouts for a broker instead */
#define MAX_PEERS 16

/* Number of input conduits, that is, the highest conduit id for which it can receive data from any peer/broker is N_IN_CONDUITS-1. The input conduit state is per-peer, per-conduit id, and hence which ones are used and what those are used for is determined on the sending side. It also means that peers may have different configurations for the maximum number of conduits. Input conduits have very little state. */
#define N_IN_CONDUITS 3

/* Number of output conduits, that is, the highest conduit id over which data can be sent by this peer/client is N_OUT_CONDUITS-1. */
#define N_OUT_CONDUITS 3

/* Set to 0 if a unicast conduit is not desired (in which case all N_OUT_CONDUITS are multicast), or to 1 if a unicast one is desired (in which case there are multicast ones only N_OUT_CONDUITS >= 2, in which case they are 0 .. N_OUT_CONDUITS-2). A unicast conduit is one that uses the same CID (N_OUT_CONDUITS-1) for each peer, even though each peer has its own independent one. It is recommended that unicast be used if at most one 1 peer is supported. */
#define HAVE_UNICAST_CONDUIT 1

/* The peer joins a number of multicast groups on startup (using transport_ops.join; the transport can define them any way they like, but on the provided UDP/IP transport implementation they have the obvious meaning). The number of these is limited by MAX_MULTICAST_GROUPS, but fewer is allowed, too. These addresses are exchanged during session establishment and used by the peers to determine from which of their output conduits the data will reach the peer */
#define MAX_MULTICAST_GROUPS 5

#include "platform-udp.h"

/**********************************************************************/
#else
/**********************************************************************/

#define MAX_PEERS 16
#define N_IN_CONDUITS 2
#define N_OUT_CONDUITS 2
#define HAVE_UNICAST_CONDUIT 1
#define MAX_MULTICAST_GROUPS 1
#define ZHE_TCPOPEN_MAXWAIT 4000
#define ZHE_TCPOPEN_THROTTLE 8000

#include "platform-tcp.h"

/**********************************************************************/
#endif
/**********************************************************************/

/* Maximum number of subscriptions a peer may register */
#define ZHE_MAX_SUBSCRIPTIONS_PER_PEER ZHE_MAX_SUBSCRIPTIONS

/* Transmit window size for multicast conduits (XMITW_BYTES) and for unicast conduits (XMITW_BYTES_UNICAST). Neither type of conduit need be enabled, and no sizes needs to be given for the one that is not configured. Each reliable message is stored in the window prefixed by its size in represented as a "zhe_msgsize_t" (for which, see below). */
#define XMITW_BYTES 16384u
#define XMITW_BYTES_UNICAST 384u
#define XMITW_SAMPLES 1600u
#define XMITW_SAMPLES_UNICAST 63u

/* Whether or not to maintain a index of samples in the transmit windows that maps sequence number to byte position */
#define XMITW_SAMPLE_INDEX 1

/* Constraints on storing URIs -- if MAX_URISPACE is set to 0, no URIs will be stored and resource declarations will be ignored */
#define ZHE_MAX_URISPACE 3072
#define ZHE_MAX_RESOURCES 20
#define ZHE_MAX_URILENGTH 100

/* Whether or not to enable tracing */
#define ENABLE_TRACING 1

/* Setting a latency budget globally for now, though it could be done per-publisher as well. Packets will go out when full or when LATENCY_BUDGET milliseconds passed since we started filling it. Setting it to 0 will disable packing of data messages, setting to INF only stops packing when the MTU is reached and generally requires explicit flushing. Both edge cases eliminate the latency budget handling and state from the code, saving a whopping 4 bytes of RAM!  */
#define LATENCY_BUDGET_INF      (4294967295u)
#define LATENCY_BUDGET         10 /* units, see ZHE_TIMEBASE */

/* Send a SYNCH message set every MSYNCH_INTERVAL ms when unack'd messages are present in the transmit window. Ideally this would be based on a measured round-trip time, but instead it is based on an estimate of the round-trip time. */
#define MSYNCH_INTERVAL        10 /* units, see ZHE_TIMEBASE */
#define ROUNDTRIP_TIME_ESTIMATE 1 /* units, see ZHE_TIMEBASE */

/* Scouts are sent periodically by a peer; by a client only when not connected to, or trying to connect to, a broker. The interval is configurable. Scouts are always multicasted (however implemented by the transport). */
#define SCOUT_INTERVAL       3000 /* units, see ZHE_TIMEBASE */
#define SCOUT_COUNT             0 /* send 10 scouts then stop scouting (0: unlimited; MAX_PEERS == 0 requires 0) */

/* Once new peer/a broker has been discovered, a number of attempts at establishing a connection take place. The interval between these attempts is OPEN_INTERVAL, the number of attempts before giving up and relying on scouting again is OPEN_RETRIES. */
#define OPEN_INTERVAL        1000 /* units, see ZHE_TIMEBASE */
#define OPEN_RETRIES           10 /* limited by OPENING_MIN .. _MAX */

/* Lease duration should be greater than SCOUT_INTERVAL (or 0, for infinite lease) */
#define LEASE_DURATION       5000 /* units, see ZHE_TIMEBASE */

/* Peer IDs are non-empty byte vectors of at most PEERID_SIZE. Peers that provide a peer id that does not meet these requirements are ignored */
#define PEERID_SIZE            16

/* Sequence numbers are represented on the wire as VLE, but internally as a fixed size unsigned integer, but counting only 7 bits per byte (there is no point in having mostly unused bytes for the sequence number most of the time). So, uint16_t is a 14-bit sequence number, uint8_t a 7-bit one, uint32_t a 28-bit one and uint64_t a 56-bit one. The transmit window size and the sequence number are related: the window must be full before the window contains as many samples as half the range can represent (i.e., for a 14-bit sequence number the window must be smaller than 8192 messages, worst-case). */
#define SEQNUM_LEN              14u

/* We're pretty dependent on making no typos in HAVE_UNICAST_CONDUIT, so it seems sensible to
   enable warnings for the use of undefined macros */
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6) || __clang__
#pragma GCC diagnostic warning "-Wundef"
#endif

#endif
