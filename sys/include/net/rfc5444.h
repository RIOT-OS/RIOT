/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_rfc5444 RFC 5444
 * @ingroup     net
 * @brief       Provides types and helper functions related to the Generalized
 *              Mobile Ad Hoc Network (MANET) Packet/Message Format (RFC 5444).
 * @see [RFC 5444](https://tools.ietf.org/html/rfc5444)
 *
 * @{
 *
 * @file
 * @brief   RFC 5444 include gathering header.
 *
 * @author  Jean Pierre Dudey <jeandudey@hotmail.com>
 */
#ifndef NET_RFC5444_H
#define NET_RFC5444_H

#include "net/rfc5444/addr.h"
#include "net/rfc5444/hdr.h"
#include "net/rfc5444/msg.h"
#include "net/rfc5444/tlv.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   RFC 5444 packet format version
 *
 * @see [RFC 5444, Section 5.1]
 *      (https://tools.ietf.org/html/rfc5444#section-5.1)
 */
#define RFC5444_VERSION (0U)

#ifdef __cplusplus
}
#endif

#endif /* NET_RFC5444_H */
/** @} */
