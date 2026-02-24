/*
 * SPDX-FileCopyrightText: 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    net_ipv6    IPv6
 * @ingroup     net
 * @brief       Provides types and helper functions related to Internet Protocol
 *              version 6 (IPv6)
 * @see [RFC 8200](http://tools.ietf.org/html/rfc8200) et al.
 * @{
 *
 * @file
 * @brief   IPv6 include gathering header.
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include "net/ipv6/addr.h"
#include "net/ipv6/ext.h"
#include "net/ipv6/hdr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   minimum **M**aximum **T**ransition **U**nit
 *
 * @see [RFC 8200, section 5](https://tools.ietf.org/html/rfc8200#section-5)
 */
#define IPV6_MIN_MTU    (1280)

#ifdef __cplusplus
}
#endif

/** @} */
