/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  net_sock_async_event
 * @{
 *
 * @file
 * @brief   Type definitions for asynchronous socks with @ref sys_event
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef SOCK_ASYNC_CTX_H
#define SOCK_ASYNC_CTX_H

#include "event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Generalized callback type
 */
typedef union {
    /**
     * @brief   anything goes
     */
    void (*generic)(void *, sock_async_flags_t, void *);
#ifdef MODULE_SOCK_DTLS
    sock_dtls_cb_t dtls;                            /**< DTLS callback */
#endif
#ifdef MODULE_SOCK_IP
    sock_ip_cb_t ip;                                /**< IP callback */
#endif
#ifdef MODULE_SOCK_TCP
    sock_tcp_cb_t tcp;                              /**< TCP callback */
    sock_tcp_queue_cb_t tcp_queue;                  /**< TCP queue callback */
#endif
#ifdef MODULE_SOCK_UDP
    sock_udp_cb_t udp;                              /**< UDP callback */
#endif
} sock_event_cb_t;

/**
 * @brief   Event definition for context scope
 */
typedef struct {
    event_t super;              /**< event structure that gets extended */
    sock_event_cb_t cb;         /**< callback */
    void *sock;                 /**< generic pointer to a @ref net_sock object */
    void *cb_arg;               /**< callback argument */
    sock_async_flags_t type;    /**< types of the event */
} sock_event_t;

/**
 * @brief   Asynchronous context for @ref net_sock_async_event
 */
typedef struct {
    sock_event_t event;     /**< event storage */
    event_queue_t *queue;   /**< event queue to post socket events to */
} sock_async_ctx_t;

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* SOCK_ASYNC_CTX_H */
