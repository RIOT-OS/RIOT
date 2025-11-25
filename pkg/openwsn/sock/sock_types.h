/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup pkg_openwsn_sock OpenWSN-specific implementation of the sock API
 * @ingroup  pkg_openwsn
 * @brief    Provides an implementation of the @ref net_sock by the
 *           @ref pkg_openwsn
 * @{
 *
 * @file
 * @brief OpenWSN-specific types
 *
 * @author  Timothy Claeys <timothy.claeys@inria.fr>
 * @author  Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifndef SOCK_TYPES_H
#define SOCK_TYPES_H

#include "mbox.h"
#include "net/af.h"
#ifdef SOCK_HAS_ASYNC
#include "net/sock/async/types.h"
#endif
#include "net/sock/udp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default size for OpenWSN sock_udp_t::mbox_queue (as exponent of 2^n).
 *
 *          As the queue size ALWAYS needs to be power of two, this option
 *          represents the exponent of 2^n, which will be used as the size of
 *          the queue.
 */
#ifndef CONFIG_OPENWSN_SOCK_MBOX_SIZE_EXP
#define CONFIG_OPENWSN_SOCK_MBOX_SIZE_EXP      (3)
#endif

/**
 * @brief Size for OpenWSN sock_udp_t::mbox_queue
 */
#ifndef OPENWSN_SOCK_MBOX_SIZE
#define OPENWSN_SOCK_MBOX_SIZE  (1 << CONFIG_OPENWSN_SOCK_MBOX_SIZE_EXP)
#endif

/**
 * @brief   Forward declaration
 * @internal
 */
typedef struct openwsn_gen_sock openwsn_gen_sock_t;

/**
 * @brief   Generic openwsn sock type
 * @warning For network stack internal purposes only. Do not access members
 *          externally.
 * @internal
 */
struct openwsn_gen_sock {
    sock_udp_ep_t local;         /**< local end-point */
    sock_udp_ep_t remote;        /**< remote end-point */
    uint16_t flags;              /**< option flags */
};

/**
 * @brief   UDP sock type
 * @warning For network stack internal purposes only. Do not access members
 *          externally.
 * @internal
 */
struct sock_udp {
    struct sock_udp *next;                      /**< sock liked list */
    openwsn_gen_sock_t gen_sock;                /**< Generic socket */
    mbox_t mbox;                                /**< @ref core_mbox target for the sock */
    msg_t mbox_queue[OPENWSN_SOCK_MBOX_SIZE];   /**< queue for gnrc_sock_reg_t::mbox */
#ifdef MODULE_SOCK_ASYNC_EVENT
    sock_async_ctx_t async_ctx;                 /**< asynchronous event context */
#endif
#ifdef SOCK_HAS_ASYNC
    sock_udp_cb_t async_cb;                     /**< asynchronous callback */
    void* async_cb_arg;
#endif
};

#ifdef __cplusplus
}
#endif

#endif /* SOCK_TYPES_H */
/** @} */
