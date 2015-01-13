/*
 * Copyright 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     rpl
 * @{
 * @file        etx_beaconing.h
 *
 * Header-file, which includes all constants and functions used for ETX-based beaconing.
 *
 * @author  Stephan Arndt <arndtste@zedat.fu-berlin.de>
 * @}
 */

#ifndef ETX_BEACONING_H_
#define ETX_BEACONING_H_
#include <stdint.h>
#include "sixlowpan_legacy.h"

#ifdef __cplusplus
extern "C" {
#endif

//[option|length|ipaddr.|packetcount] with up to 15 ipaddr|packetcount pairs
// 1 Byte 1 Byte  1 Byte  1 Byte
#define ETX_BUF_SIZE   (32)

#define ETX_RCV_QUEUE_SIZE     (128)

/*
 * Default 40, should be enough to get all messages for neighbors.
 * In my tests, the maximum count of neighbors was around 30-something
 */
#if ENABLE_DEBUG
#define ETX_MAX_CANDIDATE_NEIGHBORS (15) //Stacksizes are huge in debug mode, so memory is rare
#else
#define ETX_MAX_CANDIDATE_NEIGHBORS (40)
#endif
//ETX Interval parameters
#define MS  (1000)

/*
 * ETX_INTERVAL
 *
 * Given in ms, the default is 1 second.
 * Should be divisible through 2 (For ETX_DEF_JIT_CORRECT)
 * and 5 (For ETX_MAX_JITTER) unless those values are adjusted too.
 */
#define ETX_INTERVAL        (1000)
#define ETX_WINDOW          (10)                    //10 is the default value
#define ETX_BEST_CANDIDATES (15)                    //Sent only 15 candidates in a beaconing packet
#define ETX_TUPLE_SIZE      (2)                     //1 Byte for Addr, 1 Byte for packets rec.
#define ETX_PKT_REC_OFFSET  (ETX_TUPLE_SIZE - 1)    //Offset in a tuple of (addr,pkt_rec), will always be the last byte
#define ETX_IPV6_LAST_BYTE  (15)                    //The last byte for an ipv6 address
#define ETX_MAX_JITTER      (ETX_INTERVAL / 5)      //The default value is 20% of ETX_INTERVAL
#define ETX_JITTER_MOD      (ETX_MAX_JITTER + 1)    //The modulo value for jitter computation
#define ETX_DEF_JIT_CORRECT (ETX_MAX_JITTER / 2)    //Default Jitter correction value (normally ETX_MAX_JITTER / 2)
#define ETX_CLOCK_ADJUST    (52500)                 //Adjustment for clockthread computations to stay close/near ETX_INTERVAL

/*
 * The ETX beaconing packet consists of:
 *
 *      0                   1                   2
 *      0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+- - - - - - - -
 *     |  Option Type  | Option Length | Data
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+- - - - - - - -
 * Option type:     Set to 0x20
 *
 * Option Length:   The length of the Data sent with this packet
 *
 * Option Data:     2-Octet Pairs of 8 bit for addresses and a positive integer
 *                  denoting the amount of packets received from that IP address
 *
 * We only need 1 octet for the ip address since RPL for now only allows for
 * 255 different addresses.
 *
 * If the length of this packet says 0, it has received no other beaconing
 * packets itself so far.
 *
 * The packet is always 32bytes long, but may contain varying amounts of
 * information.
 * The information processed shall not exceed the value set in Option Length.
 */
typedef struct __attribute__((packed)) etx_probe_t {
    uint8_t code;
    uint8_t length;
    uint8_t data[30];
} etx_probe_t;

typedef struct etx_neighbor_t {
    ipv6_legacy_addr_t addr;           //The address of this node
    uint8_t     tx_cur_round;   //The indicator for receiving a packet from this candidate this round
    uint8_t     packets_tx[ETX_WINDOW]; //The packets this node has transmitted TO ME
    uint8_t     packets_rx;     //The packets this node has received FROM ME
    double      cur_etx;        //The currently calculated ETX-value
    uint8_t     used;           //The indicator if this node is active or not
} etx_neighbor_t;

//prototypes
void etx_init_beaconing(ipv6_addr_t *address);
double etx_get_metric(ipv6_addr_t *address);
void etx_update(etx_neighbor_t *neighbor);

#define ETX_PKT_OPT         (0)     //Position of Option-Type-Byte
#define ETX_PKT_OPTVAL      (0x20)  //Non-standard way of saying this is an ETX-Packet.
#define ETX_PKT_LEN         (1)     //Position of Length-Byte
#define ETX_DATA_MAXLEN     (30)    //max length of the data
#define ETX_PKT_HDR_LEN     (2)     //Option type + Length (1 Byte each)
#define ETX_PKT_DATA        (2)     //Begin of Data Bytes

#ifdef __cplusplus
}
#endif

#endif /* ETX_BEACONING_H_ */
