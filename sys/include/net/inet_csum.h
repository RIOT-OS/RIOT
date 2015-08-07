/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_inet_csum    Internet Checksum
 * @ingroup     net
 * @brief   Provides a function to calculate the Internet Checksum
 * @{
 *
 * @file
 * @brief   Internet Checksum definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef INET_CSUM_H_
#define INET_CSUM_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Calculates the unnormalized Internet Checksum of @p buf.
 *
 * @see <a href="https://tools.ietf.org/html/rfc1071">
 *          RFC 1071
 *      </a>
 *
 * @details The Internet Checksum is not normalized (i. e. its 1's complement
 *          was not taken of the result) to use it for further calculation.
 *
 * @param[in] sum   An initial value for the checksum.
 * @param[in] buf   A buffer.
 * @param[in] len   Length of @p buf in byte.
 *
 * @return  The unnormalized Internet Checksum of @p buf.
 */
uint16_t inet_csum(uint16_t sum, const uint8_t *buf, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* INET_CSUM_H_ */
/** @} */
