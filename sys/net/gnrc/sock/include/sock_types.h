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
#include "net/gnrc/netreg.h"
#include "net/sock/ip.h"
#include "net/sock/udp.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SOCK_MBOX_SIZE
#define SOCK_MBOX_SIZE      (8)         /**< Size for gnrc_sock_reg_t::mbox_queue */
#endif

/**
 * @brief   sock @ref net_gnrc_netreg info
 * @internal
 */
typedef struct gnrc_sock_reg {
#ifdef MODULE_GNRC_SOCK_CHECK_REUSE
    struct gnrc_sock_reg *next;         /**< list-like for internal storage */
#endif
    gnrc_netreg_entry_t entry;          /**< @ref net_gnrc_netreg entry for mbox */
    mbox_t mbox;                        /**< @ref core_mbox target for the sock */
    msg_t mbox_queue[SOCK_MBOX_SIZE];   /**< queue for gnrc_sock_reg_t::mbox */
} gnrc_sock_reg_t;

/**
 * @brief   Raw IP sock type
 * @internal
 */
struct sock_ip {
    gnrc_sock_reg_t reg;                /**< netreg info */
    sock_ip_ep_t local;                 /**< local end-point */
    sock_ip_ep_t remote;                /**< remote end-point */
    uint16_t flags;                     /**< option flags */
};

/**
 * @brief   UDP sock type
 * @internal
 */
struct sock_udp {
    gnrc_sock_reg_t reg;                /**< netreg info */
    sock_udp_ep_t local;                /**< local end-point */
    sock_udp_ep_t remote;               /**< remote end-point */
    uint16_t flags;                     /**< option flags */
};

#ifdef __cplusplus
}
#endif

#endif /* SOCK_TYPES_H */
/** @} */
