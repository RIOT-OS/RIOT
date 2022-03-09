/*
 * Copyright (C) 2019-20 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      net_sock_async  Sock extension for asynchronous access
 *
 * @{
 *
 * @file
 * @brief   Definitions for sock extension for asynchronous access
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_SOCK_ASYNC_TYPES_H
#define NET_SOCK_ASYNC_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/* This header needs to re-typedef the sock types to prevent cyclic includes */
#if defined (__clang__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wtypedef-redefinition"
#endif

#if defined(SOCK_HAS_ASYNC) || defined(DOXYGEN)
/**
 * @brief   Flag types to signify asynchronous sock events
 * @note    Only applicable with @ref SOCK_HAS_ASYNC defined.
 */
typedef enum {
    SOCK_ASYNC_CONN_RDY  = 0x0001,  /**< Connection ready event */
    SOCK_ASYNC_CONN_FIN  = 0x0002,  /**< Connection finished event */
    SOCK_ASYNC_CONN_RECV = 0x0004,  /**< Listener received connection event */
    SOCK_ASYNC_MSG_RECV  = 0x0010,  /**< Message received event */
    SOCK_ASYNC_MSG_SENT  = 0x0020,  /**< Message sent event */
    SOCK_ASYNC_PATH_PROP = 0x0040,  /**< Path property changed event */
} sock_async_flags_t;

#if defined(MODULE_SOCK_DTLS) || defined(DOXYGEN)
typedef struct sock_dtls sock_dtls_t;       /**< forward declare for async */

/**
 * @brief   Event callback for @ref sock_dtls_t
 *
 * @pre `(sock != NULL)`
 *
 * @note    Only applicable with @ref SOCK_HAS_ASYNC defined.
 *
 * @param[in] sock  The sock the event happened on
 * @param[in] flags The event flags. Expected values are
 *                  - @ref SOCK_ASYNC_CONN_RDY (if a session becomes ready),
 *                  - @ref SOCK_ASYNC_CONN_FIN (if a created session was destroyed),
 *                  - @ref SOCK_ASYNC_CONN_RECV (if a handshake message needs to be completed)
 *                  - @ref SOCK_ASYNC_MSG_RECV,
 *                  - @ref SOCK_ASYNC_MSG_SENT,
 *                  - @ref SOCK_ASYNC_PATH_PROP, or
 *                  - a combination of them.
 * @param[in] arg   Argument provided when setting the callback using
 *                  @ref sock_dtls_set_cb(). May be NULL.
 */
typedef void (*sock_dtls_cb_t)(sock_dtls_t *sock, sock_async_flags_t flags,
                               void *arg);
#endif  /* defined(MODULE_SOCK_DTLS) || defined(DOXYGEN) */

#if defined(MODULE_SOCK_IP) || defined(DOXYGEN)
typedef struct sock_ip sock_ip_t;       /**< forward declare for async */

/**
 * @brief   Event callback for @ref sock_ip_t
 *
 * @pre `(sock != NULL)`
 *
 * @note    Only applicable with @ref SOCK_HAS_ASYNC defined.
 *
 * @param[in] sock  The sock the event happened on
 * @param[in] flags The event flags. Expected values are
 *                  - @ref SOCK_ASYNC_MSG_RECV,
 *                  - @ref SOCK_ASYNC_MSG_SENT,
 *                  - @ref SOCK_ASYNC_PATH_PROP, or
 *                  - a combination of them.
 * @param[in] arg   Argument provided when setting the callback using
 *                  @ref sock_ip_set_cb(). May be NULL.
 */
typedef void (*sock_ip_cb_t)(sock_ip_t *sock, sock_async_flags_t flags,
                             void *arg);
#endif  /* defined(MODULE_SOCK_IP) || defined(DOXYGEN) */

#if defined(MODULE_SOCK_TCP) || defined(DOXYGEN)
typedef struct sock_tcp sock_tcp_t;            /**< forward declare for async */
typedef struct sock_tcp_queue sock_tcp_queue_t;/**< forward declare for async */

/**
 * @brief   Event callback for @ref sock_tcp_t
 *
 * @pre `(sock != NULL)`
 *
 * @note    Only applicable with @ref SOCK_HAS_ASYNC defined.
 *
 * @param[in] sock  The sock the event happened on
 * @param[in] flags The event flags. Expected values are
 *                  - @ref SOCK_ASYNC_CONN_RDY,
 *                  - @ref SOCK_ASYNC_CONN_FIN,
 *                  - @ref SOCK_ASYNC_MSG_RECV,
 *                  - @ref SOCK_ASYNC_MSG_SENT,
 *                  - @ref SOCK_ASYNC_PATH_PROP, or
 *                  - a combination of them.
 * @param[in] arg   Argument provided when setting the callback using
 *                  @ref sock_tcp_set_cb(). May be NULL.
 */
typedef void (*sock_tcp_cb_t)(sock_tcp_t *sock, sock_async_flags_t flags,
                              void *arg);

/**
 * @brief   Event callback for @ref sock_tcp_queue_t
 *
 * @pre `(queue != NULL)`
 *
 * @note    Only applicable with @ref SOCK_HAS_ASYNC defined.
 *
 * @param[in] queue The TCP listening queue the event happened on
 * @param[in] flags The event flags. The only expected value is @ref
 *                  SOCK_ASYNC_CONN_RECV.
 * @param[in] arg   Argument provided when setting the callback using
 *                  @ref sock_tcp_queue_set_cb(). May be NULL.
 */
typedef void (*sock_tcp_queue_cb_t)(sock_tcp_queue_t *queue,
                                    sock_async_flags_t flags,
                                    void *arg);
#endif  /* defined(MODULE_SOCK_TCP) || defined(DOXYGEN) */

#if defined(MODULE_SOCK_UDP) || defined(DOXYGEN)
typedef struct sock_udp sock_udp_t;     /**< forward declare for async */

/**
 * @brief   Event callback for @ref sock_udp_t
 *
 * @pre `(sock != NULL)`
 *
 * @note    Only applicable with @ref SOCK_HAS_ASYNC defined.
 *
 * @param[in] sock  The sock the event happened on
 * @param[in] flags The event flags. Expected values are
 *                  - @ref SOCK_ASYNC_MSG_RECV,
 *                  - @ref SOCK_ASYNC_MSG_SENT,
 *                  - @ref SOCK_ASYNC_PATH_PROP, or
 *                  - a combination of them.
 * @param[in] arg   Argument provided when setting the callback using
 *                  @ref sock_udp_set_cb(). May be NULL.
 */
typedef void (*sock_udp_cb_t)(sock_udp_t *sock, sock_async_flags_t flags,
                              void *arg);
#endif  /* defined(MODULE_SOCK_UDP) || defined(DOXYGEN) */

#ifdef SOCK_HAS_ASYNC_CTX
#include "sock_async_ctx.h"
#endif

#include "sock_types.h"
#ifdef MODULE_SOCK_DTLS
#include "sock_dtls_types.h"
#endif  /* MODULE_SOCK_DTLS */
#endif  /* defined(SOCK_HAS_ASYNC) || defined(DOXYGEN) */

#if defined (__clang__)
# pragma clang diagnostic pop
#endif

#ifdef __cplusplus
}
#endif

#endif /* NET_SOCK_ASYNC_TYPES_H */
/** @} */
