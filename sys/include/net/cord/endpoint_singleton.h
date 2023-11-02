/*
 * Copyright (C) 2017-2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_cord_endpoint_singleton CoRE RD Endpoint Standalone Extension
 * @ingroup     net_cord_endpoint
 * @brief       Run a single CoRE Resource Directory endpoint registration
 *
 * This sub-module instantiates a single CoRE RD endpoint and manages the
 * registration with the RD
 *
 * @{
 *
 * @file
 * @brief       CoRE Resource Directory endpoint singleton extension
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NET_CORD_ENDPOINT_SINGLETON_H
#define NET_CORD_ENDPOINT_SINGLETON_H

#include "net/sock/udp.h"
#include "event/source.h"
#include "net/cord/endpoint.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Event thread queue used for the singleton instance
 */
#ifndef CONFIG_CORD_ENDPOINT_SINGLETON_EVENT_QUEUE
#define CONFIG_CORD_ENDPOINT_SINGLETON_EVENT_QUEUE    EVENT_PRIO_LOWEST
#endif

/**
 * @brief Run the endpoint with compiled-in settings in the event thread
 *
 * @warning This function must only be called once (typically during system
 *          initialization)
 */
void cord_endpoint_singleton_run(void);

/**
 * @brief   Initiate the endpoint registration with a resource directory
 *
 * @note    In case a multicast address is given, the @p regif parameter MUST be
 *          NULL. The first RD responding to the request will be chosen and all
 *          replies from other RD servers are ignored.
 *
 * @param[in] remote    remote endpoint of the target RD
 * @param[in] regif     registration interface resource of the RD, it will be
 *                      discovered automatically when set to NULL
 *
 * @return  CORD_ENDPOINT_OK on success
 * @return  CORD_ENDPOINT_OVERFLOW if @p regif does not fit into internal buffer
 * @return  CORD_ENDPOINT_BUSY when the endpoint is not idle
 */
int cord_endpoint_singleton_register(const sock_udp_ep_t *remote, const char *regif);

/**
 * @brief   Force update our current entry at the RD
 *
 * @return  CORD_ENDPOINT_OK on success
 * @return  CORD_ENDPOINT_BUSY if the update request can't be schedule now
 */
int cord_endpoint_singleton_update(void);

/**
 * @brief   Unregister from a given RD server
 *
 * When simple registration is used, this does not cancel the registration with
 * the RD
 */
void cord_endpoint_singleton_remove(void);

/**
 * @brief   Dump the current RD connection status to STDIO (for debugging)
 */
void cord_endpoint_singleton_dump_status(void);

/**
 * @brief   Retrieve the current registration state of the endpoint
 *
 * @return  Current state of the RD endpoint
 */
cord_state_t cord_endpoint_singleton_get_state(void);

/**
 * @brief   Retrieve the current registration location as supplied by the RD
 *          from the endpoint.
 *
 * @param   buf     Buffer to copy into
 * @param   buf_len Length of the buffer
 *
 * @return  Number of bytes copied
 * @return  CORD_ENDPOINT_OVERFLOW if the location does not fit in the provided
 *          buffer
 */
ssize_t cord_endpoint_singleton_get_location(char *buf, size_t buf_len);

/**
 * @brief   Retrieve the current registration interface used with the RD from
 *          the endpoint.
 *
 * @param   buf     Buffer to copy into
 * @param   buf_len Length of the buffer
 *
 * @return  Number of bytes copied
 * @return  CORD_ENDPOINT_OVERFLOW if the interface name does not fit in the
 *          provided buffer
 */
ssize_t cord_endpoint_singleton_get_interface(char *buf, size_t buf_len);

/**
 * @brief   Attach an event subscriber to the endpoint state changes
 *
 * @param   subscriber  Subscriber to attach
 */
void cord_endpoint_singleton_event_source_attach(event_source_subscriber_t *subscriber);

/**
 * @brief   Detach an event subscriber from the endpoint state changes
 *
 * @param   subscriber  Subscriber to detach
 */
void cord_endpoint_singleton_event_source_detach(event_source_subscriber_t *subscriber);

#ifdef __cplusplus
}
#endif

#endif /* NET_CORD_ENDPOINT_SINGLETON_H */
/** @} */
