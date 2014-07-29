/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @addtogroup  pnet
 * @{
 */

/**
 * @file    arpa/inet.h
 * @brief   Definitions for internet operations
 * @see     <a href="http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/arpa_inet.h.html">
 *              The Open Group Base Specifications Issue 7, <arpa/inet.h>
 *          </a>
 *
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef ARPA_INET_H
#define ARPA_INET_H

#include <inttypes.h>

#include "net_help.h"
#include "inet_ntop.h"
#include "inet_pton.h"

typedef uint16_t in_port_t;     ///< Internet port type
typedef uint32_t in_addr_t;     ///< IPv4 address type

#define INET_ADDRSTRLEN 16      ///< Length of the string form for IPv4.
#define INET6_ADDRSTRLEN 46     ///< Length of the string form for IPv6.

/**
 * IPv4 address structure type.
 */
struct in_addr {
    in_addr_t s_addr;       ///< an IPv4 address
};

/**
 * @brief   Convert values between host and network byte order.
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/htonl.html">
 *          The Open Group Base Specification Issue 7, htonl
 *      </a>
 *
 * @param[in] hostlong  A 32 bit number.
 * @return              The argument value converted from host to network byte
 *                      order.
 */
#define htonl(hostlong)     HTONL(hostlong)

/**
 * @brief   Convert values between host and network byte order.
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/htons.html">
 *          The Open Group Base Specification Issue 7, htons
 *      </a>
 *
 * @param[in] hostlong  A 16 bit number.
 * @return              The argument value converted from host to network byte
 *                      order.
 */
#define htons(hostshort)    HTONS(hostshort)

/**
 * @brief   Convert values between host and network byte order.
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/ntohl.html">
 *          The Open Group Base Specification Issue 7, ntohl
 *      </a>
 *
 * @param[in] hostlong  A 32-bit integer number.
 * @return              The argument value converted from network to host byte
 *                      order.
 */
#define ntohl(netlong)      NTOHL(netlong)

/**
 * @brief   Convert values between host and network byte order.
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/ntohs.html">
 *          The Open Group Base Specification Issue 7, ntohs
 *      </a>
 *
 * @param[in] hostlong  A 16-bit integer number.
 * @return              The argument value converted from network to host byte
 *                      order.
 */
#define ntohs(netshort)     NTOHS(netshort)

/**
 * @}
 */
#endif /* ARPA_INET_H */
