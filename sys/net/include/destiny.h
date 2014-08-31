/**
 * destiny.h - Wraps all API types, constants and functions of the transport
 *             layer implementation.
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup destiny    DESTiny - Transport layer implementation
 * @ingroup net
 * @brief   DESTiny module implements the transport layer. This includes
 *          6LoWPAN UDP header compression and (experimental) 6LoWPAN TCP header
 *          compression.
 * @see     <a href="http://tools.ietf.org/html/rfc6282#section-4.3">
 *              RFC 6282 - Compression Format for IPv6 Datagrams over
 *                  IEEE 802.15.4-Based Networks - UDP Header Compression
 *          </a>
 * @see     <a href="http://tools.ietf.org/html/draft-aayadi-6lowpan-tcphc-01">
 *              RFC draft-aayadi-6lowpan-tcphc-01 - TCP header compression for
 *                  6LoWPAN
 *          </a>
 * @{
 * @file
 * @brief   destiny functions
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 */

#ifndef DESTINY_H
#define DESTINY_H

#include "destiny/in.h"
#include "destiny/socket.h"
#include "destiny/types.h"

/**
 * Initializes transport layer.
 *
 * @return 0 on success, other else.
 */
int destiny_init_transport_layer(void);

#endif /* DESTINY_H */
