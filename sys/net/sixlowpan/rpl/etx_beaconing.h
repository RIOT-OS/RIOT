/*
 * Header for the ETX-beaconing module
 * etx_beaconing.h
 *
 *  Created on: Feb 26, 2013
 *      Author: stephan
 */

#ifndef ETX_BEACONING_H_
#define ETX_BEACONING_H_

#include "sys/net/sixlowpan/sixlowip.h"

#define ETX_BEACON_STACKSIZE    2000 //TODO debug stacksize, set for production
#define ETX_RADIO_STACKSIZE     2000 //TODO debug stacksize, set for production
#define ETX_UPDT_STACKSIZE      800 //TODO debug stacksize, set for production


//[option|length|ipaddr.|packetcount] with up to 15 ipaddr|packetcount pairs
// 1 Byte 1 Byte  1 Byte  1 Byte
#define ETX_BUF_SIZE   (32)

#define ETX_RCV_QUEUE_SIZE     (128)

//Constants for packets

//ETX beaconing type (XXX ATTENTION! this is non-standard)
#define ETX_BEACON          0x20//Non-standard way of saying this is an etx-pkt.

//ETX Interval parameters
#define MS  1000

/*
 * ETX_INTERVAL
 *
 * Given in ms, the default is 1 second.
 * Should be divisible through 2 (For ETX_DEF_JIT_CORRECT)
 * and 5 (For ETX_MAX_JITTER) unless those values are adjusted too.
 */
#define ETX_INTERVAL        (1000)
#define ETX_ROUNDS          (10)                    //10 is the default value
#define ETX_PKT_HDR_LEN     (2)                     //Option type + Length (1 Byte each)
#define ETX_TUPLE_SIZE      (2)                     //1 Byte for Addr, 1 Byte for packets rec.
#define ETX_PKT_REC_OFFSET  (ETX_TUPLE_SIZE - 1)    //Offset in a tuple of (addr,pkt_rec), will always be the last byte
#define ETX_IPV6_LAST_BYTE  (15)                    //The last byte for an ipv6 address
#define ETX_MAX_JITTER      (ETX_INTERVAL / 5)      //The default value is 20% of ETX_INTERVAL
#define ETX_JITTER_MOD      (ETX_MAX_JITTER + 1)    //The modulo value for jitter computation
#define ETX_DEF_JIT_CORRECT (ETX_MAX_JITTER / 2)    //Default Jitter correction value (normally ETX_MAX_JITTER / 2)

//prototypes
void etx_init_beaconing(ipv6_addr_t * address);
void etx_beacon(void);
double etx_get_metric(ipv6_addr_t * address);
void etx_update(void);
void etx_radio(void);

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
 */
typedef struct __attribute__((packed)) {
    uint8_t code;
    uint8_t length;
    uint8_t* data;
} etx_probe_t;


#endif /* ETX_BEACONING_H_ */
