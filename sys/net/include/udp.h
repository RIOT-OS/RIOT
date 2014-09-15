/*
 * Copyright (C) 2013, 2014 INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup tcp    UDP implementation
 * @ingroup net
 * @brief   This module implements the transport layer protocol UDP.
 *          This includes 6LoWPAN UDP header compression.
 * @see     <a href="http://tools.ietf.org/html/rfc6282#section-4.3">
 *              RFC 6282 - Compression Format for IPv6 Datagrams over
 *                  IEEE 802.15.4-Based Networks - UDP Header Compression
 *          </a>
 * @{
 * @file
 * @brief   udp functions
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author  Cenk Gündoğan <cnkgndgn@gmail.com>
 */

#ifndef UDP_H
#define UDP_H

#include "socket_base/in.h"
#include "socket_base/socket.h"
#include "socket_base/types.h"

/**
 * Initializes udp.
 *
 * @return 0 on success, other else.
 */
int udp_init_transport_layer(void);

#endif /* UDP_H */
