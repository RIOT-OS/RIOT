/*
 * Copyright (C) 2017-2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef NET_CORD_EP_STANDALONE_H
#define NET_CORD_EP_STANDALONE_H

/**
 * @defgroup    net_cord_ep_standalone CoRE RD Endpoint Standalone Extension
 * @ingroup     net_cord_ep
 * @brief       Run a CoRE Resource Directory endpoint standalone
 *
 * This sub-module enables a CoRE RD endpoint to manage is registration state
 * with a RD autonomously by periodically running the update procedure. This
 * is implemented by running a dedicated thread.
 *
 * @{
 *
 * @file
 * @brief       CoRE Resource Directory endpoint standalone extension
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Possible types of events triggered by the cord_ep_standalone module
 */
typedef enum {
    CORD_EP_REGISTERED,
    CORD_EP_DEREGISTERED,
    CORD_EP_UPDATED,
} cord_ep_standalone_event_t;

/**
 * @brief   Callback function signature for RD endpoint state synchronization
 *
 * The registered callback function is executed in the context of the dedicated
 * standalone RD endpoint's thread.
 *
 * @param[in] event     type of event
 */
typedef void(*cord_ep_standalone_cb_t)(cord_ep_standalone_event_t event);

/**
 * @brief   Spawn a new thread that takes care of sending periodic updates to an
 *          active RD entry
 *
 * @warning This function must only be called once (typically during system
 *          initialization)
 */
void cord_ep_standalone_run(void);

/**
 * @brief   Register a callback to be notified about RD endpoint state changes
 *
 * Only a single callback can be active at any point in time, so setting a new
 * callback will override the existing one.
 *
 * @pre                     @p cb != NULL
 *
 * @param[in] cb            callback to execute on RD endpoint state changes
 */
void cord_ep_standalone_reg_cb(cord_ep_standalone_cb_t cb);

/**
 * @brief   Signal the cord_ep thread about connection status change
 *
 * @note    This function should not be called by a user, but it is called from
 *          within the cord_ep implementation
 *
 * @param[in] connected     set to true if we are connected to a RD
 */
void cord_ep_standalone_signal(bool connected);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_CORD_EP_STANDALONE_H */
