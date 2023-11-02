/*
 * Copyright (C) 2017-2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_cord_endpoint CoRE RD Endpoint
 * @ingroup     net_cord
 * @brief       Library for using RIOT as CoRE Resource Directory endpoint
 *
 * This module implements a CoRE Resource Directory endpoint library, that
 * allows RIOT nodes to register themselves with resource directories.
 * It implements the standard endpoint functionality as defined in
 * https://datatracker.ietf.org/doc/html/rfc9176
 *
 * # Design Decisions
 * - All operations are fully asynchronous and will not block. The module can be
 *   fully run within one of the event threads.
 * - While outside of the use cases of the rfc, the module allows for multiple
 *   registrations at different RDs
 *
 * @{
 *
 * @file
 * @brief       CoRE Resource Directory endpoint interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef NET_CORD_ENDPOINT_H
#define NET_CORD_ENDPOINT_H

#include "event.h"
#include "event/timeout.h"
#include "event/source.h"
#include "net/sock/udp.h"
#include "net/nanocoap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Buffer size allocated for the CoAP packet
 */
#ifndef CONFIG_CORD_ENDPOINT_BUFSIZE
#define CONFIG_CORD_ENDPOINT_BUFSIZE             (128U)
#endif

/**
 * @brief Number of seconds after which a CoAP failure should be retried
 */
#ifndef CONFIG_CORD_FAIL_RETRY_SEC
#define CONFIG_CORD_FAIL_RETRY_SEC              (60)
#endif

/**
 * @brief Maximum URI size allocated
 */
#define CORD_URI_MAX         (CONFIG_NANOCOAP_URI_MAX)

/**
 * @brief   Return values and error codes used by this module
 */
enum {
    CORD_ENDPOINT_OK        =  0,     /**< everything went as expected */
    CORD_ENDPOINT_TIMEOUT   = -1,     /**< no response from the network */
    CORD_ENDPOINT_ERR       = -2,     /**< internal error or invalid reply */
    CORD_ENDPOINT_NORD      = -3,     /**< not connected to an RD, not to be confused with north */
    CORD_ENDPOINT_NO_RD     = -3,     /**< not connected to an RD */
    CORD_ENDPOINT_OVERFLOW  = -4,     /**< internal buffers can not handle input */
    CORD_ENDPOINT_BUSY      = -5,     /**< Resource directory is busy  */
};

/**
 * @brief CORD endpoint state machine states
 */
typedef enum {
    CORD_STATE_IDLE,        /**< Idle, not active */
    CORD_STATE_DISCOVERY,   /**< Discovery of registration endpoint required */
    CORD_STATE_REGISTRY,    /**< Endpoint available, registration */
    CORD_STATE_REFRESH,     /**< Waiting until refresh is required */
    CORD_STATE_DELETE,      /**< Deleting registration */
} cord_state_t;

/**
 * @brief Core Resource Directory endpoint interface context
 */
typedef struct {
    mutex_t lock;                   /**< Synchronization mutex */
#ifdef MODULE_CORD_ENDPOINT
    char rd_loc[CORD_URI_MAX];      /**< Location path received from the RD */
    char rd_regif[CORD_URI_MAX];    /**< Registration interface discovered */
#endif
    uint8_t buf[CONFIG_CORD_ENDPOINT_BUFSIZE]; /**< Buffer size for CoAP packets */
    sock_udp_ep_t rd_remote;        /**< Remote sock endpoint */
    event_t state_update;           /**< State update event */
    event_timeout_t retry_timer;    /**< Retry timer event */
    event_queue_t *queue;           /**< Queue used by this CORD endpoint */
    event_source_t ev_source;       /**< Event system source */
    cord_state_t state;             /**< Current state of the endpoint */
} cord_endpoint_t;

/**
 * @brief   Initiate the endpoint registration with a resource directory
 *
 * @note    In case a multicast address is given, the @p regif parameter MUST be
 *          NULL. The first RD responding to the request will be chosen and all
 *          replies from other RD servers are ignored.
 *
 * @param   cord    Resource Directory context to use
 * @param   remote    remote endpoint of the target RD
 * @param   regif     registration interface resource of the RD, it will be
 *                      discovered automatically when set to NULL
 *
 * @return  CORD_ENDPOINT_OK on success
 * @return  CORD_ENDPOINT_OVERFLOW if @p regif does not fit into internal buffer
 * @return  CORD_ENDPOINT_BUSY when the endpoint is not idle
 */
int cord_endpoint_register(cord_endpoint_t *cord, const sock_udp_ep_t *remote, const char *regif);

/**
 * @brief   Force update our current entry at the RD
 *
 * @param   cord    Resource Directory context to use
 *
 * @return  CORD_ENDPOINT_OK on success
 * @return  CORD_ENDPOINT_BUSY if the update request can't be schedule now
 */
int cord_endpoint_update(cord_endpoint_t *cord);

/**
 * @brief   Unregister from a given RD server
 *
 * When simple registration is used, this does not cancel the registration with
 * the RD
 *
 * @param   cord    Resource Directory context to use
 */
void cord_endpoint_remove(cord_endpoint_t *cord);

/**
 * @brief   Start a resource directory endpoint and initiate with a resource
 *          directory if supplied.
 *
 * @see also @ref cord_endpoint_register
 *
 * @param   cord    Resource Directory context to use
 * @param   queue   Event queue to use for synchronization
 * @param   remote  Remote endpoint to register at, can be NULL to only
 *                  initialize
 * @param   regif   Endpoint to register at, can be NULL to use discovery
 *
 * @return  CORD_ENDPOINT_OK on success
 * @return  CORD_ENDPOINT_OVERFLOW when the registration interface is too long
 * to store
 */
int cord_endpoint_init(cord_endpoint_t *cord, event_queue_t *queue, const sock_udp_ep_t *remote, const char *regif);

/**
 * @brief   Copy the endpoint name into the provided buffer
 *
 * @param   buf     Buffer to copy into
 * @param   buf_len Length of the buffer
 *
 * @return  Negative on error
 * @return  Number of bytes copied
 */
ssize_t cord_endpoint_get_name(char *buf, size_t buf_len);

/**
 * @brief   Dump the current RD connection status to STDIO (for debugging)
 *
 * @param   cord    Resource Directory context to use
 */
void cord_endpoint_dump_status(const cord_endpoint_t *cord);

/**
 * @brief   Retrieve the current registration state of the endpoint
 *
 * @param   cord Resource Directory context to use
 *
 * @return  Current state of the RD endpoint
 */
cord_state_t cord_endpoint_get_state(cord_endpoint_t *cord);

/**
 * @brief   Retrieve the current registration location as supplied by the RD
 *          from the endpoint.
 *
 * @param   cord    Resource Directory context to use
 * @param   buf     Buffer to copy into
 * @param   buf_len Length of the buffer
 *
 * @return  Number of bytes copied
 * @return  CORD_ENDPOINT_OVERFLOW if the location does not fit in the provided
 *          buffer
 */
ssize_t cord_endpoint_get_location(cord_endpoint_t *cord, char *buf, size_t buf_len);

/**
 * @brief   Retrieve the current registration interface used with the RD from
 *          the endpoint.
 *
 * @param   cord    Resource Directory context to use
 * @param   buf     Buffer to copy into
 * @param   buf_len Length of the buffer
 *
 * @return  Number of bytes copied
 * @return  CORD_ENDPOINT_OVERFLOW if the interface name does not fit in the
 *          provided buffer
 */
ssize_t cord_endpoint_get_interface(cord_endpoint_t *cord, char *buf, size_t buf_len);

/**
 * @brief   Attach an event subscriber to the endpoint state changes
 *
 * @param   cord        Resource Directory context to use
 * @param   subscriber  Subscriber to attach
 */
static inline void cord_endpoint_event_source_attach(cord_endpoint_t *cord, event_source_subscriber_t *subscriber)
{
    event_source_attach(&cord->ev_source, subscriber);
}

/**
 * @brief   Detach an event subscriber from the endpoint state changes
 *
 * @param   cord        Resource Directory context to use
 * @param   subscriber  Subscriber to detach
 */
static inline void cord_endpoint_event_source_detach(cord_endpoint_t *cord, event_source_subscriber_t *subscriber)
{
    event_source_detach(&cord->ev_source, subscriber);
}

#ifdef __cplusplus
}
#endif

#endif /* NET_CORD_ENDPOINT_H */
/** @} */
