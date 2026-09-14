/*
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    net_eui64   IEEE EUI-64 identifier
 * @ingroup     net
 * @brief       Type definition of the IEEE EUI-64 identifier
 * @see         <a href="https://web.archive.org/web/20170730083229/http://standards.ieee.org:80/develop/regauth/tut/eui64.pdf">
 *                  IEEE, "Guidelines for 64-bit Global Identifier (EUI-64)"
 *              </a>
 * @{
 *
 * @file
 * @brief   EUI-64 data type definition
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 */

#include <stdint.h>
#include "byteorder.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name EUI-64 bit flags contained in the first octet
 *
 * @see IEEE 802-2001 section 9.2
 * @{
 */

/**
 * @brief Locally administered address.
 */
#define EUI64_LOCAL_FLAG        0x02

/**
 * @brief Group type address.
 */
#define EUI64_GROUP_FLAG        0x01
/** @} */

/**
 * @brief Data type to represent an EUI-64.
 */
typedef union {
    network_uint64_t uint64;     /**< represented as 64 bit value */
    uint8_t uint8[8];            /**< split into 8 8-bit words.   */
    network_uint16_t uint16[4];  /**< split into 4 16-bit words.  */
    network_uint32_t uint32[2];  /**< split into 2 32-bit words.  */
} eui64_t;

/**
 * @brief Set the locally administrated bit in the EUI-64 address.
 *
 * @see IEEE 802-2001 section 9.2
 *
 * @param   addr    eui64 address
 */
static inline void eui64_set_local(eui64_t *addr)
{
    addr->uint8[0] |= EUI64_LOCAL_FLAG;
}

/**
 * @brief Clear the group address bit to signal the address as individual
 * address
 *
 * @see IEEE 802-2001 section 9.2
 *
 * @param   addr    eui64 address
 */
static inline void eui64_clear_group(eui64_t *addr)
{
    addr->uint8[0] &= ~EUI64_GROUP_FLAG;
}

#ifdef __cplusplus
}
#endif

/** @} */
