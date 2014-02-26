/**
 * tl_socket.h - Wraps all API types, constants and functions of the transport
 *               layer implementation.
 *
 * Copyright (C) 2014  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup tl_socket Transport layer socket functionality and UDP/TCP implementation
 * @ingroup net
 * @brief   This module implements the transport layer. This includes
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
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 */

#ifndef TL_SOCKET_H
#define TL_SOCKET_H

#include "tl_socket/in.h"
#include "tl_socket/socket.h"
#include "tl_socket/types.h"

/**
 * Initializes TCP.
 *
 * @return 0 on success, other else.
 */
int tcp_init(void);

/**
 * Initializes UDP.
 *
 * @return 0 on success, other else.
 */
int udp_init(void);

#endif /* TL_SOCKET_H */
