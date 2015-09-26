/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  posix_sockets
 */
/**
 * @{
 *
 * @file
 * @brief   System-internal byte operations.
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef BYTES_H_
#define BYTES_H_

#include "byteorder.h"

#ifdef __cplusplus
extern "C" {
#endif

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
#ifndef htonl
#define htonl(hostlong)     HTONL(hostlong)
#endif

/**
 * @brief   Convert values between host and network byte order.
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/htons.html">
 *          The Open Group Base Specification Issue 7, htons
 *      </a>
 *
 * @param[in] hostshort A 16 bit number.
 * @return              The argument value converted from host to network byte
 *                      order.
 */
#ifndef htons
#define htons(hostshort)    HTONS(hostshort)
#endif

/**
 * @brief   Convert values between host and network byte order.
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/ntohl.html">
 *          The Open Group Base Specification Issue 7, ntohl
 *      </a>
 *
 * @param[in] netlong   A 32-bit integer number.
 * @return              The argument value converted from network to host byte
 *                      order.
 */
#ifndef ntohl
#define ntohl(netlong)      NTOHL(netlong)
#endif

/**
 * @brief   Convert values between host and network byte order.
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/ntohs.html">
 *          The Open Group Base Specification Issue 7, ntohs
 *      </a>
 *
 * @param[in] netshort  A 16-bit integer number.
 * @return              The argument value converted from network to host byte
 *                      order.
 */
#ifndef ntohs
#define ntohs(netshort)     NTOHS(netshort)
#endif

typedef size_t socklen_t;           /**< socket address length */

#ifdef __cplusplus
}
#endif

#endif /* BYTES_H_ */
/** @} */
