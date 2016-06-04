/*
 * Copyright (C) 2016 Freie Univesität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_sock_flags   Common flags for @ref net_sock
 * @ingroup     net_sock
 * @brief       Common flags for usage with @ref net_sock
 * @{
 *
 * @file
 * @brief   Common flags definitions for usage with @ref net_sock
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_SOCK_FLAGS_H_
#define NET_SOCK_FLAGS_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Common flags for @ref net_conn
 * @{
 */
#define SOCK_FLAGS_REUSE_EP     (0x00000001)    /**< allow to reuse end point on bind */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NET_SOCK_FLAGS_H_ */
/** @} */
