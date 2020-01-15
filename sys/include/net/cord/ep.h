/*
 * Copyright (C) 2017-2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_cord_ep CoRE RD Endpoint
 * @ingroup     net_cord
 * @brief       Library for using RIOT as CoRE Resource Directory endpoint
 *
 * This module implements a CoRE Resource Directory endpoint library, that
 * allows RIOT nodes to register themselves with resource directories.
 * It implements the standard endpoint functionality as defined in
 * draft-ietf-core-resource-directory-15.
 * @see https://tools.ietf.org/html/draft-ietf-core-resource-directory-15
 *
 * # Design Decisions
 * - all operations provided by this module are fully synchronous, meaning that
 *   the functions will block until an operation is successful or will time out
 * - the implementation limits the endpoint to be registered with a single RD at
 *   any point in time
 *
 * @{
 *
 * @file
 * @brief       CoRE Resource Directory endpoint interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NET_CORD_EP_H
#define NET_CORD_EP_H

#include "net/sock/udp.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CORD_EP_EVENT_NONE      (0x0000)
#define CORD_EP_EVENT_ALL       (0xffff)

/**
 * @brief   Return values and error codes used by this module
 */
enum {
    CORD_EP_OK        =  0,     /**< everything went as expected */
    CORD_EP_TIMEOUT   = -1,     /**< no response from the network */
    CORD_EP_ERR       = -2,     /**< internal error or invalid reply */
    CORD_EP_NORD      = -3,     /**< not connected to an RD */
    CORD_EP_OVERFLOW  = -4,     /**< internal buffers can not handle input */
};

/**
 * @brief   Events that can be signaled via the event callback function
 */
enum {
    CORD_EP_REGISTERED    = 0x0001,
    CORD_EP_DEREGISTERED  = 0x0002,
    CORD_EP_UPDATE_OK     = 0x0004,
    CORD_EP_UPDATE_FAILED = 0x0008,
};

/**
 * @brief   Callback function signature for RD endpoint state synchronization
 *
 * The registered callback function is executed in the context of the dedicated
 * standalone RD endpoint's thread.
 *
 * @param[in] t         type of event
 */
typedef void(*cord_ep_cb_t)(uint16_t event);

/**
 * @brief   Discover the registration interface resource of a RD
 *
 * @param[in] remote    remote endpoint of the target RD
 * @param[out] regif    the registration interface is written to this buffer
 * @param[in] maxlen    size of @p regif
 *
 * @return  CORD_EP_OK on success
 * @return  CORD_EP_TIMEOUT if the discovery request times out
 * @return  CORD_EP_NORD if addressed endpoint is not a RD
 * @return  CORD_EP_ERR on any other internal error
 */
int cord_ep_discover_regif(const sock_udp_ep_t *remote,
                           char *regif, size_t maxlen);

/**
 * @brief   Initiate the node registration by sending an empty push
 *
 * - if registration fails (e.g. timeout), we are not associated with any RD
 *   anymore (even if we have been before we called cord_ep_register)
 *
 * @note    In case a multicast address is given, the @p regif parameter MUST be
 *          NULL. The first RD responding to the request will be chosen and all
 *          replies from other RD servers are ignored.
 *
 * @param[in] remote    remote endpoint of the target RD
 * @param[in] regif     registration interface resource of the RD, it will be
 *                      discovered automatically when set to NULL
 *
 * @return  CORD_EP_OK on success
 * @return  CORD_EP_TIMEOUT on registration timeout
 * @return  CORD_EP_NORD if addressed endpoint is not a RD
 * @return  CORD_EP_OVERFLOW if @p regif does not fit into internal buffer
 * @return  CORD_EP_ERR on any other internal error
 */
int cord_ep_register(const sock_udp_ep_t *remote, const char *regif);

/**
 * @brief   Update our current entry at the RD
 *
 * @return  CORD_EP_OK on success
 * @return  CORD_EP_TIMEOUT if the update request times out
 * @return  CORD_EP_ERR on any other internal error
 */
int cord_ep_update(void);

/**
 * @brief   Unregister from a given RD server
 *
 * @return  CORD_EP_OK on success
 * @return  CORD_EP_TIMEOUT if the remove request times out
 * @return  CORD_EP_ERR on any other internal error
 */
int cord_ep_remove(void);

/**
 * @brief   Register a callback to be notified about RD endpoint state changes
 *
 * Only a single callback can be active at any point in time, so setting a new
 * callback will override the existing one.
 *
 * @warning Do not register an event callback when using cord_ep_standalone. Use
 *          cord_ep_standalone_event_cb() instead.
 *
 * @param[in] cb        callback to execute on RD endpoint state changes, may be
 *                      NULL to disable notifications
 * @param[in] evt_mask
 */
void cord_ep_event_cb(cord_ep_cb_t cb, uint16_t evt_mask);

/**
 * @brief   Dump the current RD connection status to STDIO (for debugging)
 */
void cord_ep_dump_status(void);


#ifdef __cplusplus
}
#endif

#endif /* NET_CORD_EP_H */
/** @} */
