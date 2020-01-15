/*
 * Copyright (C) 2017-2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef NET_CORD_EP_STANDALONE_H
#define NET_CORD_EP_STANDALONE_H

#include "net/cord/ep.h"

#ifdef __cplusplus
extern "C" {
#endif

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
 * @param[in] cb        callback to execute on RD endpoint state changes, my be
 *                      NULL to disable event notifications
 * @param[in] evt_mask  mask for selecting the events that trigger notifications
 */
void cord_ep_standalone_event_cb(cord_ep_cb_t cb, uint16_t evt_mask);

#ifdef __cplusplus
}
#endif

#endif /* NET_CORD_EP_STANDALONE_H */
/** @} */
