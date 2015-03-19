/*
 * Copyright 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @defgroup etx_beaconing
 * @ingroup     rpl
 * @{
 * @file
 *
 * Header-file, which includes all constants and functions used for ETX-based beaconing.
 *
 * @author  Stephan Arndt <arndtste@zedat.fu-berlin.de>
 */

#ifndef ETX_BEACONING_H_
#define ETX_BEACONING_H_
#include <stdint.h>
#include "sixlowpan.h"

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * @brief [option|length|ipaddr|packetcount] with up to 15 ipaddr|packetcount pairs
 *        | option | length | ipaddr  | packetcount |
 *        | ----:  | :----: | :----:  | :----:      |
 *        | 1 Byte | 1 Byte | 1 Byte  | 1 Byte      |
 */
#define ETX_BUF_SIZE   (32)
#define ETX_RCV_QUEUE_SIZE     (128)

/**  
 * @brief defining the number of neighbours
 */
#if ENABLE_DEBUG
 /**Defines the maximum number of neighbours in debug mode, we fix it at 15
  */
#define ETX_MAX_CANDIDATE_NEIGHBORS (15)
/**Defines the maximum number of neighbours in normal mode default is 40
 */
#else
#define ETX_MAX_CANDIDATE_NEIGHBORS (40)
#endif
#define MS  (1000)

/**
 * @{
 * @name ETX Interval parameters
 * 
 * @brief Defines the ETX_INTERVAL Given in ms, the default is 1 second
 * 
 * Should be divisible through 2 (For ETX_DEF_JIT_CORRECT)
 * and 5 (For ETX_MAX_JITTER) unless those values are adjusted too.
 */
#define ETX_INTERVAL        (1000)
/** @brief Defines the ETX_WINDOW , 10 is the default value
 */
#define ETX_WINDOW          (10)
 /** @brief Defines the best candidates to send the beaconing packet
  */
#define ETX_BEST_CANDIDATES (15)
/** @brief Defines the Tuple size , 1 Byte for Addr, 1 Byte for packets rec
 */
#define ETX_TUPLE_SIZE      (2)
/** @brief Defines the Offset in a tuple of (addr,pkt_rec)
 */
#define ETX_PKT_REC_OFFSET  (ETX_TUPLE_SIZE - 1)
/**@brief Defines the last byte for an ipv6 address
 */
#define ETX_IPV6_LAST_BYTE  (15)
/** @brief Defines the maximum jitter , default value is 20% of ETX_INTERVAL
 */
#define ETX_MAX_JITTER      (ETX_INTERVAL / 5)
/** @brief Defines the modulo value for jitter computation
 */
#define ETX_JITTER_MOD      (ETX_MAX_JITTER + 1)
/** @brief Defines the Default Jitter correction value (normally ETX_MAX_JITTER / 2)
 */
#define ETX_DEF_JIT_CORRECT (ETX_MAX_JITTER / 2)
/** @brief Defines the Adjustment for clockthread computations to stay close/near ETX_INTERVAL
 */
#define ETX_CLOCK_ADJUST    (52500)
/** @} */
/**
 * @brief contents of ETX beaconing packet
 * 
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
 * 255 different addresses
 *
 * If the length of this packet says 0, it has received no other beaconing
 * packets itself so far
 *
 * The packet is always 32bytes long, but may contain varying amounts of
 * information
 * The information processed shall not exceed the value set in Option Length
 */
 typedef struct __attribute__((packed)) etx_probe_t {
    uint8_t code;                       /**< Option type */
    uint8_t length;                     /**< Option Length*/
    uint8_t data[30];                   /**< Option Data */
} etx_probe_t;
/**
 * @brief etx neighbour data structure
 */ 
typedef struct etx_neighbor_t {
    /**The address of this node
     */
    ipv6_addr_t addr;
    /**The indicator for receiving a packet from this candidate this round
     */
    uint8_t     tx_cur_round;
    /**The packets this node has transmitted TO ME
     */
    uint8_t     packets_tx[ETX_WINDOW];
    /**The packets this node has received FROM ME
     */
    uint8_t     packets_rx;
    /**The currently calculated ETX-value
     */
    double      cur_etx;
     /**The indicator if this node is active or not
      */
    uint8_t     used;
} etx_neighbor_t;

 /**
  * @brief initializes the module
  * @param[in] address a ipv6_addr_t pointer
  */
void etx_init_beaconing(ipv6_addr_t *address);

 /**
  * @brief calculates the ETX-value
  * @param[in] address a ipv6_addr_t pointer
  * @return the calculated ETX-value
  */
double etx_get_metric(ipv6_addr_t *address);

 /**
  * @brief Updates the current ETX value of a candidate
  * @param[in] neighbour an etx_neighbour_t pointer
  */
void etx_update(etx_neighbor_t *neighbor);

#define ETX_PKT_OPT         (0)     /**< Position of Option-Type-Byte */
#define ETX_PKT_OPTVAL      (0x20)  /**< Non-standard way of saying this is an ETX-Packet. */
#define ETX_PKT_LEN         (1)     /**< Position of Length-Byte */
#define ETX_DATA_MAXLEN     (30)    /**< max length of the data */
#define ETX_PKT_HDR_LEN     (2)     /**< Option type + Length (1 Byte each) */
#define ETX_PKT_DATA        (2)     /**< Begin of Data Bytes */


/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ETX_BEACONING_H_ */
