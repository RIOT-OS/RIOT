/*
 * SPDX-FileCopyrightText: 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    net_af  UNIX address families
 * @ingroup     net
 * @brief       Global UNIX address family definitions
 * @{
 *
 * @file
 * @brief   Global UNIX address family definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   UNIX address family definitions
 */
typedef enum {
    AF_UNSPEC = 0,              /**< unspecified address family */
#define AF_UNSPEC   AF_UNSPEC   /**< unspecified address family (as macro) */
    AF_UNIX,                    /**< local to host (pipes, portals) address family. */
#define AF_UNIX     AF_UNIX     /**< local to host (pipes, portals) address family. (as macro) */
    AF_PACKET,                  /**< packet family */
#define AF_PACKET   AF_PACKET   /**< packet family (as macro) */
    AF_INET,                    /**< internetwork address family: UDP, TCP, etc. */
#define AF_INET     AF_INET     /**< internetwork address family: UDP, TCP, etc. (as macro) */
    AF_INET6,                   /**< internetwork address family with IPv6: UDP, TCP, etc. */
#define AF_INET6    AF_INET6    /**< internetwork address family with IPv6: UDP, TCP, etc.
                                 *    (as macro) */
    AF_NUMOF,                   /**< maximum number of address families on this system */
#define AF_NUMOF    AF_NUMOF    /**< maximum number of address families on this system (as macro) */
#define AF_MAX      AF_NUMOF    /**< alias for @ref AF_NUMOF */
} unix_af_t;

#ifdef __cplusplus
}
#endif

/** @} */
