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
#ifndef NET_EUI64_H
#define NET_EUI64_H

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
    uint8_t uint8[8];            /**< split into 8 8-bit words. */
    network_uint16_t uint16[4];  /**< split into 4 16-bit words. */
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

/**
 * @brief Generates an short address for the netdev interface.
 *
 * @note It is possible to supply a board-specific, constant addres
 *       by implementing @ref board_get_eui_short.
 *       If no such function is availiable, this will fall back to
 *       @ref luid_get_short.
 *
 * @param[in] netdev    The network device for which the address is
 *                      generated.
 *                      Will be passed on to @ref board_get_eui48.
 * @param[out] addr     The generated short address
 *
 */
void eui_short_get(netdev_t *netdev, uint16_t *addr);

/**
 * @brief Generates an EUI-64 address for the netdev interface.
 *
 * @note It is possible to supply a board-specific, constant addres
 *       by implementing @ref board_get_eui64.
 *       If no such function is availiable, this will fall back to
 *       @ref luid_get_eui64.
 *
 * @param[in] netdev    The network device for which the address is
 *                      generated.
 *                      Will be passed on to @ref board_get_eui64.
 * @param[out] addr     The generated EUI-64 address
 *
 */
void eui64_get(netdev_t *netdev, eui64_t *addr);

/**
 * @brief Board-specific function to supply a short address to a netdev
 *
 * @note  Implement this function in your board code if the board
 *        provides the means to supply a unique address to a netdev.
 *
 * @see eui_short_get
 *
 * @param[in] netdev    The network device that requested the address
 * @param[out] addr     The dedicated address for the netdev
 *
 */
size_t board_get_eui_short(netdev_t *netdev, uint16_t *addr);

/**
 * @brief Board-specific function to supply an EUI-64 to a netdev
 *
 * @note  Implement this function in your board code if the board
 *        provides the means to supply a unique address to a netdev.
 *
 * @see eui64_get
 *
 * @param[in] netdev    The network device that requested the address
 * @param[out] addr     The dedicated address for the netdev
 *
 */
size_t board_get_eui64(netdev_t *netdev, eui64_t *addr);

#ifdef __cplusplus
}
#endif

#endif /* NET_EUI64_H */
/** @} */
