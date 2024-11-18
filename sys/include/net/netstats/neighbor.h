/*
 * Copyright (C) 2017 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

#pragma once

/**
 * @ingroup     net_netstats
 * @brief       Records statistics about link layer neighbors
 * @{
 *
 * @file
 * @brief       Neighbor stats definitions
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#include <string.h>
#include "net/netif.h"
#include "timex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Result of the transmission
 * @{
 */
typedef enum {
    NETSTATS_NB_SUCCESS = 0,    /**< Successful transmission */
    NETSTATS_NB_NOACK,          /**< Failed due to no ack received */
    NETSTATS_NB_BUSY,           /**< Failed due to medium busy */
} netstats_nb_result_t;
/** @} */

/**
 * @name @ref EWMA parameters
 * @{
 */
/**
 * @brief Multiplication factor of the EWMA
 */
#define NETSTATS_NB_EWMA_SCALE            100

/**
 * @brief Alpha factor of the EWMA
 */
#define NETSTATS_NB_EWMA_ALPHA             15

/**
 * @brief Alpha factor of the EWMA when stats are not fresh
 */
#define NETSTATS_NB_EWMA_ALPHA_RAMP        30
/** @} */

/**
 * @name @ref ETX parameters
 * @{
 */
/**
 * @brief ETX penalty for not receiving any ACK
 */
#define NETSTATS_NB_ETX_NOACK_PENALTY       6
/**
 * @brief ETX fixed point divisor (rfc 6551)
 */
#define NETSTATS_NB_ETX_DIVISOR           128
/**
 * @brief Initial ETX, assume a mediocre link
 */
#define NETSTATS_NB_ETX_INIT                2
/** @} */

/**
 * @name @ref Freshness parameters
 * @{
 */
/**
 * @brief seconds after the freshness counter is halved
 */
#define NETSTATS_NB_FRESHNESS_HALF        600
/**
 * @brief freshness count needed before considering the statistics fresh
 */
#define NETSTATS_NB_FRESHNESS_TARGET        4
/**
 * @brief Maximum freshness
 */
#define NETSTATS_NB_FRESHNESS_MAX          16
/**
 * @brief seconds after statistics have expired
 */
#define NETSTATS_NB_FRESHNESS_EXPIRATION 1200
/** @} */
/**
 * @name @ref Timeout Parameters
 * @{
 */
/**
 * @brief milliseconds without TX done notification after which
 *        a TX event is discarded
 */
#define NETSTATS_NB_TX_TIMEOUT_MS         100
/** @} */

/**
 * @brief Initialize the neighbor stats
 *
 * @param[in] netif     network interface descriptor
 *
 */
void netstats_nb_init(netif_t *netif);

/**
 * @brief Find a neighbor stat by the mac address.
 *
 * @param[in] netif     network interface descriptor
 * @param[in] l2_addr   pointer to the L2 address
 * @param[in] len       length of the L2 address
 * @param[out] out      destination for the matching neighbor entry
 *
 * @return true if a matching peer was found, false otherwise
 */
bool netstats_nb_get(netif_t *netif, const uint8_t *l2_addr, uint8_t len, netstats_nb_t *out);

/**
 * @brief Store this neighbor as next in the transmission queue.
 *
 * Set @p len to zero if a nop record is needed, for example if the
 * transmission has a multicast address as a destination.
 *
 * @param[in] netif     network interface descriptor
 * @param[in] l2_addr   pointer to the L2 address
 * @param[in] len       length of the L2 address
 *
 */
void netstats_nb_record(netif_t *netif, const uint8_t *l2_addr, uint8_t len);

/**
 * @brief Update the next recorded neighbor with the provided numbers
 *
 * This only increments the statistics if the length of the l2-address of the retrieved record
 * is non-zero. See also @ref netstats_nb_record. The numbers indicate the number of transmissions
 * the radio had to perform before a successful transmission was performed. For example: in the case
 * of a single send operation needing 3 tries before an ACK was received, there are 2 failed
 * transmissions and 1 successful transmission.
 *
 * @param[in] netif    network interface descriptor
 * @param[in] result   Result of the transmission
 * @param[in] transmissions  Number of times the packet was sent over the air
 *
 * @return pointer to the record
 */
netstats_nb_t *netstats_nb_update_tx(netif_t *netif, netstats_nb_result_t result,
                                     uint8_t transmissions);

/**
 * @brief Record rx stats for the l2_addr
 *
 * @param[in] netif        network interface descriptor
 * @param[in] l2_addr pointer  to the L2 address
 * @param[in] l2_addr_len  length of the L2 address
 * @param[in] rssi         RSSI of the received transmission in abs([dBm])
 * @param[in] lqi          Link Quality Indication provided by the radio
 *
 * @return pointer to the updated record
 */
netstats_nb_t *netstats_nb_update_rx(netif_t *netif, const uint8_t *l2_addr,
                                     uint8_t l2_addr_len, uint8_t rssi, uint8_t lqi);

/**
 * @brief Check if a record is fresh
 *
 * Freshness half time is checked and updated before verifying freshness.
 *
 * @param[in] netif  network interface the statistic belongs to
 * @param[in] stats  pointer to the statistic
 */
bool netstats_nb_isfresh(netif_t *netif, netstats_nb_t *stats);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
