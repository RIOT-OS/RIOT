/*
 * Copyright (C) 2016 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_sock   GNRC-specific implementation of the sock API
 * @ingroup     net_gnrc
 * @brief       Provides an implementation of the @ref net_sock by the
 *              @ref net_gnrc
 *
 * @{
 *
 * @file
 * @brief   GNRC-specific types and function definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef SOCK_TYPES_H
#define SOCK_TYPES_H

#include <stdbool.h>
#include <stdint.h>

#include "mbox.h"
#include "net/af.h"
#include "net/gnrc.h"
#include "net/gnrc/tcp.h"
#include "net/gnrc/netreg.h"
#ifdef SOCK_HAS_ASYNC
#include "net/sock/async/types.h"
#endif
#include "net/sock/ip.h"
#include "net/sock/udp.h"
#include "net/sock/tcp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup net_gnrc_sock_conf  GNRC sock implementation compile configurations
 * @ingroup  net_gnrc_conf
 * @{
 */
/**
 * @brief   Default size for gnrc_sock_reg_t::mbox_queue (as exponent of 2^n).
 *
 *          As the queue size ALWAYS needs to be power of two, this option
 *          represents the exponent of 2^n, which will be used as the size of
 *          the queue.
 */
#ifndef CONFIG_GNRC_SOCK_MBOX_SIZE_EXP
#define CONFIG_GNRC_SOCK_MBOX_SIZE_EXP      (3)
#endif
/** @} */

/**
 * @brief Size for gnrc_sock_reg_t::mbox_queue
 */
#ifndef GNRC_SOCK_MBOX_SIZE
#define GNRC_SOCK_MBOX_SIZE  (1 << CONFIG_GNRC_SOCK_MBOX_SIZE_EXP)
#endif

/**
 * @brief   Forward declaration
 * @internal
 */
typedef struct gnrc_sock_reg gnrc_sock_reg_t;

#ifdef SOCK_HAS_ASYNC
/**
 * @brief   Event callback for @ref gnrc_sock_reg_t
 * @internal
 */
typedef void (*gnrc_sock_reg_cb_t)(gnrc_sock_reg_t *sock,
                                   sock_async_flags_t flags,
                                   void *arg);
#endif  /* SOCK_HAS_ASYNC */

/**
 * @brief   sock @ref net_gnrc_netreg info
 * @internal
 */
struct gnrc_sock_reg {
#ifdef MODULE_GNRC_SOCK_CHECK_REUSE
    struct gnrc_sock_reg *next;            /**< list-like for internal storage */
#endif
    gnrc_netreg_entry_t entry;             /**< @ref net_gnrc_netreg entry for mbox */
    mbox_t mbox;                           /**< @ref core_mbox target for the sock */
    msg_t mbox_queue[GNRC_SOCK_MBOX_SIZE]; /**< queue for gnrc_sock_reg_t::mbox */
#ifdef SOCK_HAS_ASYNC
    gnrc_netreg_entry_cbd_t netreg_cb;     /**< netreg callback */
    /**
     * @brief   asynchronous upper layer callback
     *
     * @note    All have void return value and a (sock pointer, sock_async_flags_t)
     *          pair, so casting between these function pointers is okay.
     */
    union {
        gnrc_sock_reg_cb_t generic;        /**< generic version */
#ifdef MODULE_SOCK_IP
        sock_ip_cb_t ip;                   /**< IP version */
#endif
#ifdef MODULE_SOCK_UDP
        sock_udp_cb_t udp;                 /**< UDP version */
#endif
    } async_cb;
    void *async_cb_arg;                    /**< asynchronous callback argument */
#ifdef SOCK_HAS_ASYNC_CTX
    sock_async_ctx_t async_ctx;            /**< asynchronous event context */
#endif
#endif  /* SOCK_HAS_ASYNC */
};

/**
 * @brief   Raw IP sock type
 * @internal
 */
struct sock_ip {
    gnrc_sock_reg_t reg;                   /**< netreg info */
    sock_ip_ep_t local;                    /**< local end-point */
    sock_ip_ep_t remote;                   /**< remote end-point */
    uint16_t flags;                        /**< option flags */
};

/**
 * @brief   UDP sock type
 * @internal
 */
struct sock_udp {
    gnrc_sock_reg_t reg;                   /**< netreg info */
    sock_udp_ep_t local;                   /**< local end-point */
    sock_udp_ep_t remote;                  /**< remote end-point */
    uint16_t flags;                        /**< option flags */
};

#ifdef __cplusplus
}
#endif

#endif /* SOCK_TYPES_H */
/** @} */
