/*
 * Copyright (C) 2017 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_gnrc_netdev_stats  Netdev stats group
 * @ingroup     net_gnrc_netdev
 * @brief       Records statistics about link layer neighbors
 * @{
 *
 * @file
 * @brief       Neighbor stats definitions
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */
#ifndef NET_NETSTATS_NEIGHBOR_H
#define NET_NETSTATS_NEIGHBOR_H

#include <string.h>
#include "net/netdev.h"
#include "xtimer.h"
#include "timex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Result of the transmission
 * @{
 */
typedef enum {
    NETSTATS_NB_BUSY,       /**< Failed due to medium busy */
    NETSTATS_NB_NOACK,      /**< Failed due to no ack received */
    NETSTATS_NB_SUCCESS,    /**< Succesful transmission */
} netstats_nb_result_t;
/** @} */

/**
 * @brief Max length of a L2 address
 */
#define NETSTATS_NB_L2_ADDR_MAX_SIZE      (8)

/**
 * @name @ref EWMA parameters
 * @{
 */
#define NETSTATS_NB_EWMA_SCALE            100   /**< Multiplication factor of the EWMA */
#define NETSTATS_NB_EWMA_ALPHA             15   /**< Alpha factor of the EWMA */
#define NETSTATS_NB_EWMA_ALPHA_RAMP        30   /**< Alpha factor of the EWMA when stats are not fresh */
/** @} */

/**
 * @name @ref ETX parameters
 * @{
 */
#define NETSTATS_NB_ETX_NOACK_PENALTY       6   /**< ETX penalty for not receiving any ACK */
#define NETSTATS_NB_ETX_DIVISOR           128   /**< ETX fixed point divisor (rfc 6551) */
#define NETSTATS_NB_ETX_INIT                2   /**< Initial ETX, assume a mediocre link */
/** @} */

/**
 * @name @ref Freshness parameters
 * @{
 */
#define NETSTATS_NB_FRESHNESS_HALF        600   /**< seconds after the freshness counter is halved */
#define NETSTATS_NB_FRESHNESS_TARGET        4   /**< freshness count needed before considering the statistics fresh */
#define NETSTATS_NB_FRESHNESS_MAX          16   /**< Maximum freshness */
#define NETSTATS_NB_FRESHNESS_EXPIRATION 1200   /**< seconds after statistics have expired */
/** @} */

/**
 * @brief Initialize the neighbor stats
 *
 * @param[in] dev  ptr to netdev device
 *
 */
void netstats_nb_init(netdev_t *dev);

/**
 * @brief Find or create a neighbor stat by the mac address.
 *
 * Find the entry by the mac address. if there is no statistics entry of the
 * mac address, an entry is created, potentially replacing the oldest nonfresh
 * statistics entry. NULL is only returned if all entries are fresh and no
 * matching entry is found.
 *
 * @param[in] dev       ptr to netdev device
 * @param[in] l2_addr   ptr to the L2 address
 * @param[in] len       length of the L2 address
 *
 * @return ptr to the statistics record
 */
netstats_nb_t *netstats_nb_get_or_create(netdev_t *dev, const uint8_t *l2_addr, uint8_t len);

/**
 * @brief Iterator over the recorded neighbors, returns the next non-zero record. NULL if none remaining
 *
 * @param[in] first     ptr to the first record
 * @param[in] prev      ptr to the previous record
 *
 * @return ptr to the record
 * @return NULL if no records remain
 */
netstats_nb_t *netstats_nb_get_next(netstats_nb_t *first, netstats_nb_t *prev);

/**
 * @brief Store this neighbor as next in the transmission queue.
 *
 * Set len to zero if a nop record is needed, for example if the
 * transmission has a multicast address as a destination.
 *
 * @param[in] dev       ptr to netdev device
 * @param[in] l2_addr   ptr to the L2 address
 * @param[in] len       length of the L2 address
 *
 */
void netstats_nb_record(netdev_t *dev, const uint8_t *l2_addr, uint8_t len);

/**
 * @brief Get the first available neighbor in the transmission queue and increment pointer.
 *
 * @param[in] dev       ptr to netdev device
 *
 * @return ptr to the record
 */
netstats_nb_t *netstats_nb_get_recorded(netdev_t *dev);

/**
 * @brief Update the next recorded neighbor with the provided numbers
 *
 * This only increments the statistics if the length of the l2-address of the retrieved record
 * is non-zero. see also @ref netstats_nb_record. The numbers indicate the number of transmissions
 * the radio had to perform before a successful transmission was performed. For example: in the case
 * of a single send operation needing 3 tries before an ACK was received, there are 2 failed
 * transmissions and 1 successful transmission.
 *
 * @param[in] dev      ptr to netdev device
 * @param[in] result   Result of the transmission
 * @param[in] retries  Number of retries necessary for the transmission
 *
 * @return ptr to the record
 */
netstats_nb_t *netstats_nb_update_tx(netdev_t *dev, netstats_nb_result_t result, uint8_t retries);

/**
 * @brief Update the ETX value of the statistic.
 *
 * @param[in] stats    ptr to the statistic
 * @param[in] result   Result of the transmission
 * @param[in] retries  Number of retries necessary for the transmission
 */
void netstats_nb_update_etx(netstats_nb_t *stats, netstats_nb_result_t result, uint8_t retries);

/**
 * @brief Record rx stats for the l2_addr
 *
 * @param[in] dev          ptr to netdev device
 * @param[in] l2_addr ptr  to the L2 address
 * @param[in] l2_addr_len  length of the L2 address
 * @param[in] rssi         RSSI of the received transmission in abs([dBm])
 * @param[in] lqi          Link Quality Indication provided by the radio
 *
 * @return ptr to the updated record
 */
netstats_nb_t *netstats_nb_update_rx(netdev_t *dev, const uint8_t *l2_addr, uint8_t l2_addr_len, uint8_t rssi, uint8_t lqi);

/**
 * @brief Increase the freshness of the record
 *
 * Freshness half time is checked before incrementing the freshness.
 *
 * @param[in] stats  ptr to the statistic
 */
void netstats_nb_incr_freshness(netstats_nb_t *stats);

/**
 * @brief Check if a record is fresh
 *
 * Freshness half time is checked and updated before verifying freshness.
 *
 * @param[in] stats  ptr to the statistic
 */
bool netstats_nb_isfresh(netstats_nb_t *stats);

/**
 * @brief Reduce freshness by the amount of half time periods passed
 *
 * @param[in] stats  ptr to the statistic
 * @param[in] cur    ptr to the current time
 */
void netstats_nb_half_freshness(netstats_nb_t *stats, timex_t *cur);


/**
 * @brief Compare the freshness of two records
 *
 * @param[in] a  ptr to the first record
 * @param[in] b  ptr to the second record
 *
 * @return       ptr to the least fresh record
 */
static inline netstats_nb_t *netstats_nb_comp(const netstats_nb_t *a,
                                              const netstats_nb_t *b)
{
    return (netstats_nb_t *)((a->last_updated < b->last_updated) ? a : b);
}

#ifdef __cplusplus
}
#endif

#endif /* NET_NETSTATS_NEIGHBOR_H */
/**
 * @}
 */
