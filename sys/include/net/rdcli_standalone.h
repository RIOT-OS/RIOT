/*
 * Copyright (C) 2017-2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_rdcli_standalone CoRE RD Standalone Extension
 * @ingroup     net_rdcli
 * @brief       Run CoRE Resource Directory client in standalone configuration
 *
 * This sub-module enables the CoRE RD client to manage is registration state
 * with a server autonomously by periodically running the update procedure. This
 * is implemented by running a dedicated thread.
 *
 * @{
 *
 * @file
 * @brief       CoRE Resource Directory client standalone extension
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NET_RDCLI_STANDALONE_H
#define NET_RDCLI_STANDALONE_H

#include "net/sock/udp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Possible types of events triggered by the standalone rdcli module
 */
typedef enum {
    RDCLI_REGISTERED,
    RDCLI_DEREGISTERED,
    RDCLI_UPDATED,
} rdcli_standalone_event_t;

/**
 * @brief   Callback function signature for RD client state synchronization
 *
 * The registered callback function is executed in the context of the dedicated
 * standalone RD client's thread.
 *
 * @param[in] t         type of event
 */
typedef void(*rdcli_standalone_cb_t)(rdcli_standalone_event_t event);

/**
 * @brief   Spawn a new thread that takes care of sending periodic updates to an
 *          active RD entry
 *
 * @note    This function must only be called once (typically during system
 *          initialization)
 */
void rdcli_standalone_run(void);

/**
 * @brief   Register a callback to be notified about RD client state changes
 *
 * Only a single callback can be active at any point in time, so setting a new
 * callback will override the existing one.
 *
 * @pre                     @p cb != NULL
 *
 * @param[in] cb            callback to execute on RD client state changes
 */
void rdcli_standalone_reg_cb(rdcli_standalone_cb_t cb);

/**
 * @brief   Signal the rdcli thread about connection status change
 *
 * @note    This function should not be called by a user, but it is called from
 *          withing the rdcli implementation
 *
 * @param[in] connected     set to true if we are connected to a RD
 */
void rdcli_standalone_signal(bool connected);

#ifdef __cplusplus
}
#endif

#endif /* NET_RDCLI_STANDALONE_H */
/** @} */
