/*
 * Copyright (C) 2014, INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

#ifndef L2_PING_H
#define L2_PING_H

/**
 * @defgroup    net_l2_ping Link Layer Ping
 * @ingroup     net
 * @brief       Link Layer Ping
 *
 * @{
 * @file
 * @brief       Link Layer Ping public interface, data structs, and defines
 *
 * @author Oliver Hahm <oliver.hahm@inria.fr>
 */

#include "radio/radio.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief the bitmask of the first link layer payload byte defining the type
 */
#define L2_PAYLOAD_TYPE     (0xF0)

/**
 * @brief link layer payload type
 *
 * @note  assure not to interfere with valid network layer values like the
 *        6LoWPAN dispatch field
 */
#define L2_PAYLOAD_PING     (0x10)

/**
 * @brief the bitmask for the payload identifier, defining the actual l2 ping type
 *
 * @see l2_ping_type_t
 */
#define L2_PING_TYPE        (0x0F)

/**
 * @brief the default interval between two ping packets are sent in microseconds
 */
#define L2_PING_DEFAULT_INTERVAL       (50 * 1000u)

/**
 * @brief maximum number of bytes for payload within a l2 ping packet
 */
#define L2_PING_PAYLOAD_SIZE        (8)

/**
 * @brief data type for the l2 ping type
 */
typedef enum {
    L2_PING  = 0x01, /**< ping request packet */
    L2_PONG  = 0x02, /**< pong response packet */
    L2_PROBE = 0x04  /**< probe packet */
} l2_ping_type_t;

/**
 * @brief l2 ping statistics data structure
 */
typedef struct {
    radio_address_t dst;    /**< the destination address for the last ping request sent */
    uint16_t ping_count;    /**< the amount of ping requests sent to this destination */
    uint16_t pong_count;    /**< the amount of pong respnses received from this node */
    timex_t last_rtt;       /**< the round trip time for the last received pong response */
    timex_t avg_rtt;        /**< the average round trip time to this node */
    timex_t max_rtt;        /**< the maximum round trip time to this node */
    timex_t min_rtt;        /**< the minimum round trip time to this node */
} l2_ping_stats_t;

/**
 * @brief l2 probe statistics entry
 */
typedef struct {
    radio_address_t src;    /**< link layer address of the probe's origin */
    uint16_t count;         /**< number of received probes from this node */
} l2_probe_stat_entry_t;

/**
 * @brief payload data structure for l2 ping packets
 */
typedef struct ping_payload {
    uint8_t type;                           /**< the data type */
    uint16_t seq;                           /**< sequence number */
    /* cppcheck-suppress unusedStructMember */
    char payload[L2_PING_PAYLOAD_SIZE];     /**< the actual payload */
    uint8_t payload_len;                    /**< number of payload bytes */
} l2_ping_payload_t;

/**
 * @brief the current l2 ping statistics about the current communication
 *        partner
 */
extern l2_ping_stats_t l2_ping_stats;

/**
 * @brief initializes the l2 ping module
 *
 * @note this function gets usually called by auto_init
 */
void l2_ping_init(void);

/**
 * @brief send a ping request
 *
 * @param[in] addr          the destination address
 * @param[in] count         the number of requests to send, 0 means infinite
 * @param[in] interval      the interval between sending ping requests,
 *                          if 0 the default interval (#L2_PING_DEFAULT_INTERVAL)
 * @param[in] payload       optional payload data, may be NULL
 * @param[in] payload_len   the length of the payload data, must be smaller than 
 *                          #L2_PING_PAYLOAD_SIZE
 * @param[in] probe_only    do not request a pong response
 */
void l2_ping(radio_address_t addr, uint16_t count, uint32_t interval,
             const char *payload, uint8_t payload_len, uint8_t probe_only);

/**
 * @brief get l2 probe statistics to check how many probes have been received
 *
 * @param[out] l2_probe_stats   pointer to an array of ::l2_probe_stat_entry_t
 * @param[out] count            pointer to the number of entries in l2_probe_stats
 */
void l2_probe_stats(l2_probe_stat_entry_t *l2_probe_stats[], uint16_t *count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* L2_PING_H */
