/*
 * SPDX-FileCopyrightText: 2019 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup        net_sock_async  Sock extension for asynchronous access
 * @ingroup         net_sock
 * @brief           Provides backend functionality for asynchronous sock access.
 * @experimental    This API extension is still under development and should
 *                  not be used in production yet.
 *
 * @{
 *
 * @file
 * @brief   Definitions for sock extension for asynchronous access
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include "net/sock/async/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(SOCK_HAS_ASYNC) || defined(DOXYGEN)
#if defined(MODULE_SOCK_DTLS) || defined(DOXYGEN)

/**
 * @brief Gets the asynchronous event session from sock object
 *
 * @pre `(sock != NULL) && (session != NULL)`
 *
 * @note Only available with @ref SOCK_HAS_ASYNC defined.
 *       Should only be called within a DTLS event and session is only available
 *       for the event types @ref SOCK_ASYNC_CONN_RDY and @ref SOCK_ASYNC_CONN_FIN.
 *       For other event types use @ref sock_dtls_recv() to get the session.
 *
 * @param[in]  sock       The DTLS sock object of the current event.
 * @param[out] session    Session object of the current event.
 *
 * @return true if the event session is available, false otherwise.
 */
bool sock_dtls_get_event_session(sock_dtls_t *sock,
                                 sock_dtls_session_t *session);

/**
 * @brief   Sets the asynchronous event context from sock object
 *
 * @pre `(sock != NULL)`
 *
 * @warning Never handle the callback in the caller's context! You might block
 *          the operation of the network stack or the DTLS library with that.
 *
 * @note    Only available with @ref SOCK_HAS_ASYNC defined.
 *
 * @param[in] sock      A DTLS sock object.
 * @param[in] cb        An event callback. May be NULL to unset event callback.
 * @param[in] cb_arg    Argument to provide to @p cb. May be NULL.
 */
void sock_dtls_set_cb(sock_dtls_t *sock, sock_dtls_cb_t cb, void *cb_arg);
#endif  /* defined(MODULE_SOCK_DTLS) || defined(DOXYGEN) */

#if defined(MODULE_SOCK_IP) || defined(DOXYGEN)
/**
 * @brief   Sets event callback for @ref sock_ip_t
 *
 * @pre `(sock != NULL)`
 *
 * @warning Never handle the callback in the caller's context! You might block
 *          the operation of the network stack with that.
 *
 * @note    Only available with @ref SOCK_HAS_ASYNC defined.
 *
 * @param[in] sock  A raw IPv4/IPv6 sock object.
 * @param[in] cb    An event callback. May be NULL to unset event callback.
 * @param[in] cb_arg    Argument to provide to @p cb. May be NULL.
 */
void sock_ip_set_cb(sock_ip_t *sock, sock_ip_cb_t cb, void *cb_arg);
#endif  /* defined(MODULE_SOCK_IP) || defined(DOXYGEN) */

#if defined(MODULE_SOCK_TCP) || defined(DOXYGEN)
/**
 * @brief   Sets event callback for @ref sock_tcp_t
 *
 * @pre `(sock != NULL)`
 *
 * @warning Never handle the callback in the caller's context! You might block
 *          the operation of the network stack with that.
 *
 * @note    Only available with @ref SOCK_HAS_ASYNC defined.
 *
 * @param[in] sock      A TCP sock object.
 * @param[in] cb        An event callback. May be NULL to unset event callback.
 * @param[in] cb_arg    Argument to provide to @p cb. May be NULL.
 */
void sock_tcp_set_cb(sock_tcp_t *sock, sock_tcp_cb_t cb, void *cb_arg);

/**
 * @brief   Sets event callback for @ref sock_tcp_queue_t
 *
 * @pre `(sock != NULL)`
 *
 * @warning Never handle the callback in the caller's context! You might block
 *          the operation of the network stack with that.
 *
 * @note    Only available with @ref SOCK_HAS_ASYNC defined.
 *
 * @param[in] queue     A TCP listening queue.
 * @param[in] cb        An event callback. May be NULL to unset event callback.
 * @param[in] cb_arg    Argument to provide to @p cb. May be NULL.
 */
void sock_tcp_queue_set_cb(sock_tcp_queue_t *queue, sock_tcp_queue_cb_t cb,
                           void *cb_arg);
#endif  /* defined(MODULE_SOCK_TCP) || defined(DOXYGEN) */

#if defined(MODULE_SOCK_UDP) || defined(DOXYGEN)
/**
 * @brief   Gets the asynchronous event context from sock object
 *
 * @pre `(sock != NULL)`
 *
 * @warning Never handle the callback in the caller's context! You might block
 *          the operation of the network stack with that.
 *
 * @note    Only available with @ref SOCK_HAS_ASYNC defined.
 *
 * @param[in] sock      A UDP sock object.
 * @param[in] cb        An event callback. May be NULL to unset event callback.
 * @param[in] cb_arg    Argument to provide to @p cb
 */
void sock_udp_set_cb(sock_udp_t *sock, sock_udp_cb_t cb, void *cb_arg);
#endif  /* defined(MODULE_SOCK_UDP) || defined(DOXYGEN) */

#if defined(SOCK_HAS_ASYNC_CTX) || defined(DOXYGEN)
#include "sock_async_ctx.h"     /* defines sock_async_ctx_t */

#if defined(MODULE_SOCK_DTLS) || defined(DOXYGEN)
/**
 * @brief   Gets the asynchronous event context from sock object
 *
 * @pre `(sock != NULL)`
 *
 * @note    Only available with @ref SOCK_HAS_ASYNC and @ref SOCK_HAS_ASYNC_CTX
 *          defined.
 *
 * @see @ref SOCK_HAS_ASYNC_CTX
 *
 * @param[in] sock  A DTLS sock object.
 *
 * @return  The asynchronous event context
 */
sock_async_ctx_t *sock_dtls_get_async_ctx(sock_dtls_t *sock);
#endif  /* defined(MODULE_SOCK_DTLS) || defined(DOXYGEN) */

#if defined(MODULE_SOCK_IP) || defined(DOXYGEN)
/**
 * @brief   Gets the asynchronous event context from sock object
 *
 * @pre `(sock != NULL)`
 *
 * @note    Only available with @ref SOCK_HAS_ASYNC and @ref SOCK_HAS_ASYNC_CTX
 *          defined.
 *
 * @see @ref SOCK_HAS_ASYNC_CTX
 *
 * @param[in] sock  A raw IPv4/IPv6 sock object.
 *
 * @return  The asynchronous event context
 */
sock_async_ctx_t *sock_ip_get_async_ctx(sock_ip_t *sock);
#endif  /* defined(MODULE_SOCK_IP) || defined(DOXYGEN) */

#if defined(MODULE_SOCK_TCP) || defined(DOXYGEN)
/**
 * @brief   Gets the asynchronous event context from sock object
 *
 * @pre `(sock != NULL)`
 *
 * @note    Only available with @ref SOCK_HAS_ASYNC and @ref SOCK_HAS_ASYNC_CTX
 *          defined.
 *
 * @see @ref SOCK_HAS_ASYNC_CTX
 *
 * @param[in] sock  A TCP sock object.
 *
 * @return  The asynchronous event context
 */
sock_async_ctx_t *sock_tcp_get_async_ctx(sock_tcp_t *sock);

/**
 * @brief   Gets the asynchronous event context from TCP listening queue
 *
 * @pre `(queue != NULL)`
 *
 * @note    Only available with @ref SOCK_HAS_ASYNC and @ref SOCK_HAS_ASYNC_CTX
 *          defined.
 *
 * @see @ref SOCK_HAS_ASYNC_CTX
 *
 * @param[in] queue A TCP listening queue.
 *
 * @return  The asynchronous event context
 */
sock_async_ctx_t *sock_tcp_queue_get_async_ctx(sock_tcp_queue_t *queue);
#endif  /* defined(MODULE_SOCK_TCP) || defined(DOXYGEN) */

#if defined(MODULE_SOCK_UDP) || defined(DOXYGEN)
/**
 * @brief   Gets the asynchronous event context from sock object
 *
 * @pre `(sock != NULL)`
 *
 * @note    Only available with @ref SOCK_HAS_ASYNC and @ref SOCK_HAS_ASYNC_CTX
 *          defined.
 *
 * @see @ref SOCK_HAS_ASYNC_CTX
 *
 * @param[in] sock  A UDP sock object.
 *
 * @return  The asynchronous event context
 */
sock_async_ctx_t *sock_udp_get_async_ctx(sock_udp_t *sock);
#endif  /* defined(MODULE_SOCK_UDP) || defined(DOXYGEN) */
#endif  /* defined(SOCK_HAS_ASYNC_CTX) || defined(DOXYGEN) */
#endif  /* defined(SOCK_HAS_ASYNC) || defined(DOXYGEN) */

#ifdef __cplusplus
}
#endif

/** @} */
