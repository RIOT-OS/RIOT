/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_ipv6    IPv6
 * @ingroup     net
 * @brief       Provides types and helper functions related to Internet Protocol
 *              version 6 (IPv6)
 * @see <a href="http://tools.ietf.org/html/rfc2460">
 *          RFC 2460
 *      </a> et al.
 * @{
 *
 * @file
 * @brief   IPv6 include gathering header.
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef IPV6_H_
#define IPV6_H_

#include "net/ipv6/addr.h"
#include "net/ipv6/ext.h"
#include "net/ipv6/hdr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   minimum **M**aximum **T**ransition **U**nit
 *
 * @see <a href="https://tools.ietf.org/html/rfc4944#section-5.3">
 *          RFC 2460, section 5.3
 *      </a>
 */
#define IPV6_MIN_MTU    (1280)

#ifdef __cplusplus
}
#endif


#endif /* IPV6_H_ */
/** @} */
