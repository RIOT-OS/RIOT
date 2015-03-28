/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_ng_ipv6 IPv6
 * @ingroup     net
 * @brief       New IPv6 implementation
 * @{
 *
 * @file
 * @brief       Definitions for IPv6
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */


#ifndef NG_IPV6_H_
#define NG_IPV6_H_

#include "net/ng_ipv6/addr.h"
#include "net/ng_ipv6/hdr.h"
#include "net/ng_ipv6/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default maximum transition unit
 *
 * @see <a href="https://tools.ietf.org/html/rfc2460#section-5">
 *          RFC 2460, section 5
 *      </a>
 */
#define NG_IPV6_DEFAULT_MTU (1280)

#ifdef __cplusplus
}
#endif

#endif /* NG_IPV6_H_ */
/**
 * @}
 */
