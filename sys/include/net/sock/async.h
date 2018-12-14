/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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
#ifndef NET_SOCK_ASYNC_H
#define NET_SOCK_ASYNC_H

#ifdef SOCK_DTLS
#include "sock/dtls.h"
#endif

#ifdef SOCK_IP
#include "sock/ip.h"
#endif

#ifdef SOCK_TCP
#include "sock/tcp.h"
#endif

#ifdef SOCK_UDP
#include "sock/udp"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOCK_HAS_ASYNC || defined(DOXYGEN)
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

#if defined(SOCK_HAS_ASYNC) && defined(SOCK_HAS_ASYNC_CTX)
#include "sock_async_ctx.h"     /* defines sock_async_ctx_t */
#endif

#if defined(SOCK_DTLS) || defined(DOXYGEN)
/**
 * @brief   Event callback for @ref sock_dtls_t
 *
 * @pre `(sock != NULL)`
 *
 * @note    Only applicable with @ref SOCK_HAS_ASYNC defined.
 *
 * @param[in] sock  The sock the event happened on
 * @param[in] flags The event flags. Expected values are
 *                  - @ref SOCK_ASYNC_CONN_RDY (if a session you created becomes ready),
 *                  - @ref SOCK_ASYNC_CONN_FIN (if a created session was destroyed),
 *                  - @ref SOCK_ASYNC_CONN_RECV (if a peer tries to create a session),
 *                  - @ref SOCK_ASYNC_MSG_RECV,
 *                  - @ref SOCK_ASYNC_MSG_SENT,
 *                  - @ref SOCK_ASYNC_PATH_PROP, or
 *                  - a combination of them.
 */
typedef void (*sock_dtls_cb_t)(sock_dtls_t *sock, sock_async_flags_t flags);

/**
 * @brief   Gets the asynchronous event context from sock object
 *
 * @pre `(sock != NULL)`
 *
 * @warning Never handle the callback in the caller's context! You might block
 *          the operation of the network stack or the DTLS library with that.
 *
 * @note    Only available with @ref SOCK_HAS_ASYNC defined.
 *
 * @param[in] sock  A DTLS sock object.
 * @param[in] cb    An event callback. May be NULL to unset event callback.
 */
void sock_dtls_set_cb(sock_dtls_t *sock, sock_dtls_cb_t cb);
#endif  /* defined(SOCK_DTLS) || defined(DOXYGEN) */

#if defined(SOCK_IP) || defined(DOXYGEN)
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
 */
typedef void (*sock_ip_cb_t)(sock_ip_t *sock, sock_async_flags_t flags);

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
 */
void sock_ip_set_cb(sock_ip_t *sock, sock_ip_cb_t cb);
#endif  /* defined(SOCK_IP) || defined(DOXYGEN) */

#if defined(SOCK_TCP) || defined(DOXYGEN)
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
 */
typedef void (*sock_tcp_cb_t)(sock_tcp_t *sock, sock_async_flags_t flags);

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
 */
typedef void (*sock_tcp_queue_cb_t)(sock_tcp_queue_t *queue,
                                    sock_async_flags_t flags);

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
 * @param[in] sock  A TCP sock object.
 * @param[in] cb    An event callback. May be NULL to unset event callback.
 */
void sock_tcp_set_cb(sock_tcp_t *sock, sock_tcp_cb_t cb);

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
 * @param[in] queue A TCP listening queue.
 * @param[in] cb    An event callback. May be NULL to unset event callback.
 */
void sock_tcp_queue_set_cb(sock_tcp_queue_t *queue, sock_tcp_queue_cb_t cb);
#endif  /* defined(SOCK_TCP) || defined(DOXYGEN) */

#if defined(SOCK_UDP) || defined(DOXYGEN)
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
 */
typedef void (*sock_udp_cb_t)(sock_udp_t *sock, sock_async_flags_t type);

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
 * @param[in] sock  A UDP sock object.
 * @param[in] cb    An event callback. May be NULL to unset event callback.
 */
void sock_udp_set_cb(sock_udp_t *sock, sock_udp_cb_t cb);
#endif  /* defined(SOCK_UDP) || defined(DOXYGEN) */

#if SOCK_HAS_ASYNC_CTX || defined(DOXYGEN)
#if defined(SOCK_DTLS) || defined(DOXYGEN)
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
#endif  /* defined(SOCK_DTLS) || defined(DOXYGEN) */

#if defined(SOCK_IP) || defined(DOXYGEN)
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
#endif  /* defined(SOCK_IP) || defined(DOXYGEN) */

#if defined(SOCK_TCP) || defined(DOXYGEN)
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
#endif  /* defined(SOCK_TCP) || defined(DOXYGEN) */

#if defined(SOCK_UDP) || defined(DOXYGEN)
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
#endif  /* defined(SOCK_UDP) || defined(DOXYGEN) */
#endif /* SOCK_HAS_ASYNC_CTX */

#ifdef __cplusplus
}
#endif

#endif /* NET_SOCK_ASYNC_H */
/** @} */
