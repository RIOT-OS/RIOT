/**
 * 6LoWPAN data types
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup sixlowpan
 * @{
 * @file    sixlowpan/types.h
 * @brief   6LoWPAN data types
 * @author  Stephan Zeisberg <zeisberg@mi.fu-berlin.de>
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author  Eric Engel <eric.engel@fu-berlin.de>
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 * @}
 */

#ifndef SIXLOWPAN_TYPES_H
#define SIXLOWPAN_TYPES_H

/**
 * @brief Data type to represent an EUI-64.
 */
typedef union __attribute__((packed)) {
    uint8_t uint8[8];   ///< devided by 8 8-bit words.
    uint16_t uint16[4]; ///< devided by 4 16-bit words.
} ieee_802154_long_t;

/**
 * @brief Data type to represent IEEE 802.15.4 short address.
 */
typedef union __attribute__((packed)) {
    uint8_t uint8[2];   ///< devided by 2 8-bit words.
    uint16_t uint16[1]; ///< as a 16-bit value.
} ieee_802154_short_t;

/**
 * @brief Data type to represent an IPv6 address.
 */
typedef union __attribute__((packed)) {
    uint8_t uint8[16];  ///< devided by 16 8-bit words.
    uint16_t uint16[8]; ///< devided by 8 16-bit words.
    uint32_t uint32[4]; ///< devided by 4 32-bit words.
} ipv6_addr_t;


#endif /* SIXLOWPAN_TYPES_H */
