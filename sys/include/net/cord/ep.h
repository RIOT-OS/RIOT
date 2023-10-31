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
 * draft-ietf-core-resource-directory-27.
 * @see https://datatracker.ietf.org/doc/html/rfc9176
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
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef NET_CORD_EP_H
#define NET_CORD_EP_H

#include "event.h"
#include "event/timeout.h"
#include "net/sock/udp.h"
#include "net/nanocoap.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MODULE_CORD_EP
#define CONFIG_CORD_EP_BUFSIZE             (128U)
#else
#define CONFIG_CORD_EP_BUFSIZE             (64U)
#endif

#define CONFIG_CORD_FAIL_RETRY_SEC          (5)
#define CONFIG_CORD_URI_MAX         (CONFIG_NANOCOAP_URI_MAX)

/**
 * @brief   Return values and error codes used by this module
 */
enum {
    CORD_EP_OK        =  0,     /**< everything went as expected */
    CORD_EP_TIMEOUT   = -1,     /**< no response from the network */
    CORD_EP_ERR       = -2,     /**< internal error or invalid reply */
    CORD_EP_NORD      = -3,     /**< not connected to an RD, not to be confused with north */
    CORD_EP_NO_RD     = -3,     /**< not connected to an RD */
    CORD_EP_OVERFLOW  = -4,     /**< internal buffers can not handle input */
    CORD_EP_BUSY      = -5,     /**< Resource directory is busy  */
};

typedef enum {
    CORD_STATE_IDLE = 0, /**< Idle, not active */
    CORD_STATE_DISCOVERY, /**< Discovery of registration endpoint required */
    CORD_STATE_DISCOVERING, /**< Waiting for discovery reply */
    CORD_STATE_REGISTRY,    /**< Endpoint available, registration */
    CORD_STATE_REGISTERING,  /**< Waiting for registration reply */
    CORD_STATE_WAIT_REFRESH, /**< Waiting until refresh is required */
    CORD_STATE_REFRESHING,   /**< Waiting for refresh reply */
    CORD_STATE_DELETE,    /**< Deleting registration */
    CORD_STATE_DELETING,    /**< Deleting registration */
    CORD_STATE_WAIT_RETRY,  /**< Failure happened, waiting to retry */
} cord_state_t;

/**
 * @brief Core Resource Directory endpoint interface context
 */
typedef struct {
    mutex_t lock;
#ifdef MODULE_CORD_EP
    char rd_loc[CONFIG_CORD_URI_MAX];
#endif
    char rd_regif[CONFIG_CORD_URI_MAX];
    uint8_t buf[CONFIG_CORD_EP_BUFSIZE];
    sock_udp_ep_t rd_remote;
    event_t state_update;
    event_timeout_t retry_timer;
    event_queue_t *queue;
    cord_state_t state;
} cord_ep_ctx_t;

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
int cord_ep_discover_regif(cord_ep_ctx_t *cord, const sock_udp_ep_t *remote,
                           char *regif, size_t maxlen);

/**
 * @brief   Initiate the node registration by sending an empty push
 *
 * - if registration fails (e.g. timeout), we are not associated with any RD
 *   anymore (even if we have been before we called cord_endpoint_register)
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
int cord_ep_register(cord_ep_ctx_t *cord, const sock_udp_ep_t *remote, const char *regif);

/**
 * @brief   Force update our current entry at the RD
 *
 * @param   cord    Resource Directory context to use
 *
 * @return  CORD_EP_OK on success
 * @return  CORD_EP_TIMEOUT if the update request times out
 * @return  CORD_EP_ERR on any other internal error
 */
int cord_ep_update(cord_ep_ctx_t *cord);

/**
 * @brief   Unregister from a given RD server
 *
 * @param   cord    Resource Directory context to use
 *
 * @return  CORD_EP_OK on success
 * @return  CORD_EP_TIMEOUT if the remove request times out
 * @return  CORD_EP_ERR on any other internal error
 */
int cord_ep_remove(cord_ep_ctx_t *cord);

/**
 * @brief   Start a resource directory registration
 *
 * @param   cord    Resource Directory context to use
 * @param   queue   Event queue to use for synchronization
 * @param   remote  Remote endpoint to register at
 * @param   regif   Endpoint to register at, can be NULL to use discovery
 *
 * @return  CORD_EP_OK on success
 */
int cord_ep_init(cord_ep_ctx_t *cord, event_queue_t *queue, const sock_udp_ep_t *remote, const char *regif);

/**
 * @brief   Copy the endpoint name into the provided buffer
 *
 * @param   cord    Resource Directory context to use
 * @param   buf     Buffer to copy into
 * @param   len     Length of the buffer
 *
 * @return  Negative on error
 * @return  Number of bytes copied
 */
ssize_t cord_ep_get_ep(char *buf, size_t buf_len);

/**
 * @brief   Dump the current RD connection status to STDIO (for debugging)
 *
 * @param   cord    Resource Directory context to use
 */
void cord_ep_dump_status(const cord_ep_ctx_t *cord);

#ifdef __cplusplus
}
#endif

#endif /* NET_CORD_EP_H */
/** @} */
