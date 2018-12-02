/*
 * Copyright (C) 2016 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_owsn_sock
 * @brief       Provides an implementation of the @ref net_sock by OpenWSN
 *
 * @{
 *
 * @file
 * @brief   OpenWSN-specific types and function definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef SOCK_TYPES_H
#define SOCK_TYPES_H

#include <stdbool.h>
#include <stdint.h>

#include "net/af.h"
#include "net/sock/ip.h"
#include "net/sock/udp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Raw IP sock type
 * @internal
 */
struct sock_ip {
    sock_ip_ep_t local;                 /**< local end-point */
    sock_ip_ep_t remote;                /**< remote end-point */
    uint16_t flags;                     /**< option flags */
};

/**
 * @brief   UDP sock type
 * @internal
 */
struct sock_udp {
    sock_udp_ep_t local;                /**< local end-point */
    sock_udp_ep_t remote;               /**< remote end-point */
    uint16_t flags;                     /**< option flags */
};

#ifdef __cplusplus
}
#endif

#endif /* SOCK_TYPES_H */
/** @} */
