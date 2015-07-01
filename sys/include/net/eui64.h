/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_eui64   IEEE EUI-64 identifier
 * @ingroup     net
 * @brief       Type definiton of the IEEE EUI-64 identifier
 * @see         <a href="http://standards.ieee.org/regauth/oui/tutorials/EUI64.html">
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
#ifndef EUI64_H_
#define EUI64_H_

#include <stdint.h>
#include "byteorder.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Data type to represent an EUI-64.
 */
typedef union {
    network_uint64_t uint64;     /**< represented as 64 bit value */
    uint8_t uint8[8];            /**< split into 8 8-bit words. */
    network_uint16_t uint16[4];  /**< split into 4 16-bit words. */
} eui64_t;

#ifdef __cplusplus
}
#endif

#endif /* EUI64_H_ */
/** @} */
