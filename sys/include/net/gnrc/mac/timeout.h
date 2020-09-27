/*
 * Copyright (C) 2015 Daniel Krebs
 *               2017 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_mac
 * @{
 *
 * @file
 * @brief       Timeout APIs used by GNRC_MAC
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Shuguo Zhuo  <shuguo.zhuo@inria.fr>
 */

#ifndef NET_GNRC_MAC_TIMEOUT_H
#define NET_GNRC_MAC_TIMEOUT_H

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

#include "evtimer_msg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief definition for GNRC_MAC timeout event type
 */
#define GNRC_MAC_EVENT_TIMEOUT_TYPE        (0x4400)

/**
 * @brief Definitions of GNRC_MAC timeout types.
 *
 * This structure can be extended to contain more needed
 * timeout types of different MAC protocols. Please guard
 * them by appropriate \#ifdef directives when applicable.
 */
typedef enum {
    GNRC_MAC_TIMEOUT_DISABLED = 0,    /**< Timeout is disabled, not in used. */
} gnrc_mac_timeout_type_t;

/**
 * @brief Structure of the GNRC_MAC timeout event.
 */
typedef struct {
    evtimer_msg_event_t msg_event;      /**< The timeout message event. */
    gnrc_mac_timeout_type_t type;       /**< GNRC_MAC timeout type. */
} gnrc_mac_timeout_event_t;

/**
 * @brief Structure holding the GNRC_MAC timeouts.
 */
typedef struct {
    evtimer_t evtimer;                                          /**< evtimer entity which
                                                                     stores the timeout list. */
    gnrc_mac_timeout_event_t *timeouts;                         /**< The gnrc_mac timeout
                                                                     unites. */
    uint8_t timeout_num;                                        /**< Timeout number. */
} gnrc_mac_timeout_t;

/**
 * @brief   Initialize the MAC timeout module of gnrc_mac before using,
 *          it also sets the timeout callback function.
 *
 * @param[in,out] mac_timeout  gnrc_mac timeout management unit
 * @param[in] timeouts         gnrc_mac timeouts
 * @param[in] num              timeout number
 */
void gnrc_mac_init_timeouts(gnrc_mac_timeout_t *mac_timeout,
                            gnrc_mac_timeout_event_t timeouts[],
                            uint8_t num);

/**
 * @brief   Set a MAC timeout of @p type.
 *
 * @param[in,out] mac_timeout  gnrc_mac timeout management unit
 * @param[in]     type         the MAC timeout type
 * @param[in]     offset       the timeout offset
 * @param[in]     pid          the targeted thread pid
 */
void gnrc_mac_set_timeout(gnrc_mac_timeout_t *mac_timeout, gnrc_mac_timeout_type_t type,
                          uint32_t offset, kernel_pid_t pid);

/**
 * @brief   Find a MAC timeout of @p type.
 *
 * @param[in] mac_timeout  gnrc_mac timeout management unit
 * @param[in] type         the MAC timeout type
 *
 * @return                 Return index >= 0 if found timeout, -ENONENT if not found
 */
int gnrc_mac_find_timeout(gnrc_mac_timeout_t *mac_timeout, gnrc_mac_timeout_type_t type);

/**
 * @brief   Clear a MAC timeout of @p type.
 *
 * @param[in,out] mac_timeout  gnrc_mac timeout management unit
 * @param[in]     type         the MAC timeout type
 */
void gnrc_mac_clear_timeout(gnrc_mac_timeout_t *mac_timeout, gnrc_mac_timeout_type_t type);

/**
 * @brief   Check whether a MAC timeout of @p type is running or not.
 *
 * @param[in] mac_timeout  gnrc_mac timeout management unit
 * @param[in] type         the MAC timeout type
 *
 * @return                 true, if the time of @p type is running
 * @return                 false, if the time of @p type is not running, or not exist
 */
static inline bool gnrc_mac_timeout_is_running(gnrc_mac_timeout_t *mac_timeout,
                                               gnrc_mac_timeout_type_t type)
{
    assert(mac_timeout);
    return (gnrc_mac_find_timeout(mac_timeout, type) >= 0);
}

/**
 * @brief   Check whether a MAC timeout of @p type has expired or not.
 *
 * @param[in,out] mac_timeout  gnrc_mac timeout management unit
 * @param[in]     type         the MAC timeout type
 *
 * @return                     true, if the MAC time of @p type is expired
 * @return                     false, if the MAC time of @p type is not expired, or not exist
 */
bool gnrc_mac_timeout_is_expired(gnrc_mac_timeout_t *mac_timeout, gnrc_mac_timeout_type_t type);

/**
 * @brief   Reset all the MAC timeouts.
 *
 * @param[in,out] mac_timeout  gnrc_mac timeout management unit
 */
void gnrc_mac_reset_timeouts(gnrc_mac_timeout_t *mac_timeout);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_MAC_TIMEOUT_H */
