/*
 * Copyright (C) 2013-15 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  posix_sockets
 * @{
 */

/**
 * @file
 * @brief   Definitions for internet operations
 * @see     <a href="http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/arpa_inet.h.html">
 *              The Open Group Base Specifications Issue 7, <arpa/inet.h>
 *          </a>
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef ARPA_INET_H
#define ARPA_INET_H

#include "net/af.h"
#include "sys/bytes.h"
#include "netinet/in.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Size in byte of an IPv4 address
 */
#ifndef INADDRSZ
#define INADDRSZ            (4)
#endif

/**
 * @brief   Size in byte of an IPv6 address
 */
#ifndef IN6ADDRSZ
#define IN6ADDRSZ           (16)
#endif

/**
 * @brief   Converts an IP address to its string representation
 *
 * @pre (af == AF_INET) || (af == AF_INET6)
 *
 * @param[in] af    Address family of @p src. Must be @ref AF_INET or @ref AF_INET6.
 * @param[in] src   An IP address.
 * @param[out] dst  The resulting string representation.
 * @param[in] size  Length of @p result.
 *
 * @return  @p dst, on success
 * @return  NULL, if @p size was smaller than needed
 * @return  NULL, if @p src or @p dst was NULL
 */
const char *inet_ntop(int af, const void *__restrict src, char *__restrict dst,
                      socklen_t size);

/**
 * @brief   Converts an IP address string representation to a byte-represented
 *          IP address
 *
 * @pre (af == AF_INET) || (af == AF_INET6)
 *
 * @param[in] af    Address family of @p src. Must be @ref AF_INET or @ref AF_INET6.
 * @param[in] src   An IP address string representation
 * @param[out] dst   The resulting byte representation
 *
 * @return  1, on success.
 * @return  0, if @p src was malformed or input pointers were NULL.
 * @return  -1, if @p af is not supported.
 */
int inet_pton(int af, const char *src, void *dst);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
#endif /* ARPA_INET_H */
