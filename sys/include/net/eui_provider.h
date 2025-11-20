/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    net_eui_provider    IEEE EUI-48/64 provider
 * @ingroup     net
 * @brief       MAC address management
 *
 * About
 * =====
 *
 * An EUI provider provides an Extended Unique Identifier, that is
 * a hardware address for a network device.
 *
 * A board may have an EEPROM with a unique ID (e.g. @ref drivers_at24mac) that is used
 * to store an address, a unique address is stored in a config
 * area of the board's flash during production, etc.
 *
 * The EUI provider is connected to a network device that will then use
 * this address.
 *
 * How it works
 * ============
 *
 * If there is only one EUI provider on the board, the configuration
 * is done through three defines:
 *
 * - `EUIxx_PROVIDER_TYPE` to specify which device the EUI should be used for.
 *   That is to prevent it from matching to 'virtual' devices like ethos or SLIP.
 *   It is of course also possible to provide a fixed address for 'virtual' devices
 *   this way.

 * - `EUIxx_PROVIDER_INDEX` to specify which interface the EUI should be used for if
 *   there are multiple interfaces.
 *
 * - `EUIxx_PROVIDER_FUNC` the function that provides the EUI. It can use an optional
 *   `EUIxx_PROVIDER_ARG` argument that is passed unmodified.
 *
 * If more than one EUI provider is present on the board, an array of `euiXX_conf_t`
 * has to be provided to `EUI64_PROVIDER_PARAMS`:
 *
 * ```C
 * #define EUI64_PROVIDER_PARAMS {              \
 *              .provider = _board_get_eui,     \
 *              .arg      = BOARD_ADDR_SUBGHZ,  \
 *              .type     = NETDEV_AT86RF215,   \
 *              .index    = 0,   },             \
 *                               {              \
 *              .provider = _board_get_eui,     \
 *              .arg      = BOARD_ADDR_24GHZ,   \
 *              .type     = NETDEV_AT86RF215,   \
 *              .index    = 1,   }
 * ```
 *
 * This also assumes a `_board_get_eui()` function to provide the EUI.
 * For example, it could read an EUI from a config region on the flash
 * that is provided with the memory-mapped addresses `BOARD_ADDR_SUBGHZ` and
 * `BOARD_ADDR_24GHZ`.
 * The function would then do
 *
 * ```C
 * static inline int _board_get_eui(const void *src, eui64_t *addr)
 * {
 *     memcpy(addr, src, sizeof(*addr));
 *     return 0;
 * }
 *
 * ```
 *
 * Recommendations
 * ===============
 *
 * Do not use `NETDEV_ANY` as EUI device type. Otherwise if you have two
 * interfaces both will match the same EUI.
 *
 * It is however possible to use `NETDEV_INDEX_ANY` if you have multiple
 * interfaces of the same type and your EUI provider function takes the index
 * into account (or returns error if the index is out of bounds with the
 * available ids).
 *
 * Fixed addresses are only guaranteed if the network devices are also fixed.
 * E.g. if you usually have two netdevs and disable the first one at compile-time
 * the second interface will now use the first slot / index and therefore also
 * use the EUI provider that was previously used by interface in the first slot.
 *
 * @{
 *
 * @file
 * @brief   EUI-48 and EUI-64 address provider
 *
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include "net/eui48.h"
#include "net/eui64.h"
#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Function for providing a EUI-48 to a device
 *
 * @param[in]   index   index of the netdev
 * @param[out]  addr    Destination pointer for the EUI-48 address
 *
 * @return      0 on success, next provider in eui48_conf_t will be
 *              used otherwise.
 *              Will fall back to @see luid_get_eui48 eventually.
 */
typedef int (*netdev_get_eui48_cb_t)(uint8_t index, eui48_t *addr);

/**
 * @brief   Function for providing a EUI-64 to a device
 *
 * @param[in]   index   index of the netdev
 * @param[out]  addr    Destination pointer for the EUI-64 address
 *
 * @return      0 on success, next provider in eui64_conf_t will be
 *              used otherwise.
 *              Will fall back to @see luid_get_eui64 eventually.
 */
typedef int (*netdev_get_eui64_cb_t)(uint8_t index, eui64_t *addr);

/**
 * @brief Structure to hold providers for EUI-48 addresses
 */
typedef struct {
    netdev_get_eui48_cb_t provider; /**< function to provide an EUI-48                  */
    netdev_type_t type;             /**< device type to match                           */
    uint8_t index;                  /**< device index to match or `NETDEV_INDEX_ANY`    */
} eui48_conf_t;

/**
 * @brief Structure to hold providers for EUI-64 addresses
 */
typedef struct {
    netdev_get_eui64_cb_t provider; /**< function to provide an EUI-64                  */
    netdev_type_t type;             /**< device type to match                           */
    uint8_t index;                  /**< device index to match or `NETDEV_INDEX_ANY`    */
} eui64_conf_t;

/**
 * @brief Generates an EUI-48 address for the netdev interface.
 *
 * @note It is possible to supply a board-specific, constant address
 *       by implementing a EUI-48 provider function.
 *       If no such function is available, this will fall back to
 *       @ref luid_get_eui48.
 *
 * @param[in] netdev    The network device for which the address is
 *                      generated.
 * @param[out] addr     The generated EUI-48 address
 *
 */
void netdev_eui48_get(netdev_t *netdev, eui48_t *addr);

/**
 * @brief Generates an EUI-64 address for the netdev interface.
 *
 * @note It is possible to supply a board-specific, constant address
 *       by implementing a EUI-64 provider function.
 *       If no such function is available, this will fall back to
 *       @ref luid_get_eui64.
 *
 * @param[in] netdev    The network device for which the address is
 *                      generated.
 * @param[out] addr     The generated EUI-64 address
 *
 */
void netdev_eui64_get(netdev_t *netdev, eui64_t *addr);

/**
 * @brief   Get a short unicast address from an EUI-64
 *
 * The resulting address is built from the provided long address.
 * The last two bytes of the long address will be used as the short
 * address with the first bit cleared.
 *
 * @param[in]  addr_long    the address to base the short address on
 * @param[out] addr_short   memory location to copy the address into.
 */
static inline void eui_short_from_eui64(eui64_t *addr_long,
                                        network_uint16_t *addr_short)
{
    /* https://tools.ietf.org/html/rfc4944#section-12 requires the first bit to
     * 0 for unicast addresses */
    addr_short->u8[0] = addr_long->uint8[6] & 0x7F;
    addr_short->u8[1] = addr_long->uint8[7];
}

#ifdef __cplusplus
}
#endif

/** @} */
