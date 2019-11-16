/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_l2util Stack-independent helpers for IPv6 over X
 * @ingroup     net
 * @brief       This implements some common helper functions for IPv6 over X
 *              implementations based on [network device types]
 *              (@ref net_netdev_type).
 *
 * @attention   If you add a new [network device type](@ref net_netdev_type)
 *              have at least a look at the implementation of these functions.
 * @{
 *
 * @file
 * @brief   Link-layer helper function definitions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_L2UTIL_H
#define NET_L2UTIL_H

#include <stdint.h>

#include "luid.h"
#include "net/eui64.h"
#include "net/ndp.h"

#ifdef __cplusplus
extern "C" {
#endif

#define L2UTIL_ADDR_MAX_LEN (8U)    /**< maximum expected length for addresses */

/**
 * @brief   Converts a given hardware address to an EUI-64.
 *
 * @attention When the link-layer of the interface has link-layer addresses, and
 *            `NDEBUG` is not defined, the node fails with an assertion instead
 *            returning `-ENOTSUP`.
 *
 * @param[in] dev_type  The network device type of the device @p addr came from
 *                      (either because it is the configured address of the
 *                      device or from a packet that came over it).
 * @param[in] addr      A hardware address.
 * @param[in] addr_len  Number of bytes in @p addr.
 * @param[out] eui64    The EUI-64 based on gnrc_netif_t::device_type
 *
 * @return  `sizeof(eui64_t)` on success.
 * @return  `-ENOTSUP`, when @p dev_type does not support EUI-64 conversion.
 * @return  `-EINVAL`, when @p addr_len is invalid for the @p dev_type.
 */
int l2util_eui64_from_addr(int dev_type, const uint8_t *addr, size_t addr_len,
                           eui64_t *eui64);

/**
 * @brief   Converts a given hardware address to an IPv6 IID.
 *
 * @attention When the link-layer of the interface has link-layer addresses, and
 *            `NDEBUG` is not defined, the node fails with an assertion instead
 *            returning `-ENOTSUP`.
 *
 * @param[in] dev_type  The network device type of the device @p addr came from
 *                      (either because it is the configured address of the
 *                      device or from a packet that came over it).
 * @param[in] addr      A hardware address.
 * @param[in] addr_len  Number of bytes in @p addr.
 * @param[out] iid      The IID based on gnrc_netif_t::device_type
 *
 * @return  `sizeof(eui64_t)` on success.
 * @return  `-ENOTSUP`, when @p dev_type does not support IID conversion.
 * @return  `-EINVAL`, when @p addr_len is invalid for the @p dev_type.
 */
int l2util_ipv6_iid_from_addr(int dev_type,
                              const uint8_t *addr, size_t addr_len,
                              eui64_t *iid);

/**
 * @brief   Converts an IPv6 IID to a hardware address
 *
 * @pre @p iid was based on a hardware address
 * @pre The number of bytes available at @p addr is less or equal to
 *      @ref L2UTIL_ADDR_MAX_LEN.
 *
 * @attention   When `NDEBUG` is not defined, the node fails with an assertion
 *              instead of returning `-ENOTSUP`
 *
 * @param[in] dev_type  The network device type of the device the @p iid came
 *                      from (either because it is based on the configured
 *                      address of the device or from a packet that came over
 *                      it).
 * @param[in] iid       An IID based on @p dev_type.
 * @param[out] addr     The hardware address. It is assumed that @p iid was
 *                      based on a hardware address and that the available
 *                      number of bytes in @p addr are greater or equal to
 *                      @ref L2UTIL_ADDR_MAX_LEN.
 *
 * @return  Length of resulting @p addr on success.
 * @return  `-ENOTSUP`, when @p dev_type does not support reverse IID
 *          conversion.
 */
int l2util_ipv6_iid_to_addr(int dev_type, const eui64_t *iid, uint8_t *addr);

/**
 * @brief   Derives the length of the link-layer address in an NDP link-layer
 *          address option from that option's length field and the given device
 *          type.
 *
 * @note    If an RFC exists that specifies how IPv6 operates over a link-layer,
 *          this function usually implements the section "Unicast Address
 *          Mapping".
 *
 * @see [RFC 4861, section 4.6.1](https://tools.ietf.org/html/rfc4861#section-4.6.1)
 *
 * @attention   When `NDEBUG` is not defined, the node fails with an assertion
 *              instead of returning `-ENOTSUP`
 *
 * @param[in] dev_type  The network device type of the device the @p opt came
 *                      over in an NDP message.
 * @param[in] opt       An NDP source/target link-layer address option.
 *
 * @return  Length of the link-layer address in @p opt on success
 * @return  `-ENOTSUP`, when implementation does not know how to derive the
 *          length of the link-layer address from @p opt's length field based
 *          on @p dev_type.
 * @return  `-EINVAL` if `opt->len` was an invalid value for the given
 *          @p dev_type.
 */
int l2util_ndp_addr_len_from_l2ao(int dev_type,
                                  const ndp_opt_t *opt);

/**
 * @brief Board-specific function to supply a short address to a netdev.
 *
 * @note  Implement this function in your board code if the board
 *        provides the means to supply a unique address to a netdev.
 *
 * @warning Don't call this function directly, use @ref l2util_generate_short_addr() instead.
 *
 * @param[in] driver    The driver name of the netdev
 * @param[in] idx       The index in the <driver>_params_t struct
 * @param[out] addr     The dedicated address for the netdev
 *
 * @return              The number of bytes copied. 0 if no address is available.
 */
size_t board_get_short_addr(const char *driver, unsigned idx, network_uint16_t *addr);

/**
 * @brief Generates an short address for the netdev interface.
 *
 * @note It is possible to supply a board-specific, constant addres
 *       by implementing @ref board_get_short_addr.
 *       If no such function is availiable, this will fall back to
 *       @ref luid_get_short.
 *
 * @param[in] driver    The driver name of the netdev
 *                      Will be passed on to @ref board_get_eui48.
 * @param[in] idx       The index in the <driver>_params_t struct
 *                      Will be passed on to @ref board_get_eui48.
 * @param[out] addr     The generated short address
 *
 */
static inline void l2util_generate_short_addr(const char *driver, unsigned idx, network_uint16_t *addr)
{
    if (board_get_short_addr(driver, idx, addr) == sizeof(*addr)) {
        return;
    }

    luid_get_short(addr);
}

/**
 * @brief Board-specific function to supply an EUI-48 to a netdev
 *
 * @note  Implement this function in your board code if the board
 *        provides the means to supply a unique address to a netdev.
 *
 * @warning Don't call this function directly, use @ref l2util_generate_eui48() instead.
 *
 * @param[in] driver    The driver name of the netdev
 * @param[in] idx       The index in the <driver>_params_t struct
 * @param[out] addr     The dedicated address for the netdev
 *
 * @return              The number of bytes copied. 0 if no address is available.
 */
size_t board_get_eui48(const char *driver, unsigned idx, eui48_t *addr);

/**
 * @brief Generates an EUI-48 address for the netdev interface.
 *
 * @note It is possible to supply a board-specific, constant addres
 *       by implementing @ref board_get_eui48.
 *       If no such function is availiable, this will fall back to
 *       @ref luid_get_eui48.
 *
 * @param[in] driver    The driver name of the netdev
 *                      Will be passed on to @ref board_get_eui48.
 * @param[in] idx       The index in the <driver>_params_t struct
 *                      Will be passed on to @ref board_get_eui48.
 * @param[out] addr     The generated EUI-48 address
 *
 */
static inline void l2util_generate_eui48(const char *driver, unsigned idx, eui48_t *addr)
{
    if (board_get_eui48(driver, idx, addr) == sizeof(*addr)) {
        return;
    }

    luid_get_eui48(addr);
}

/**
 * @brief Board-specific function to supply an EUI-64 to a netdev
 *
 * @note  Implement this function in your board code if the board
 *        provides the means to supply a unique address to a netdev.
 *
 * @warning Don't call this function directly, use @ref l2util_generate_eui64() instead.
 *
 * @param[in] driver    The driver name of the netdev
 * @param[in] idx       The index in the <driver>_params_t struct
 * @param[out] addr     The dedicated address for the netdev
 *
 * @return              The number of bytes copied. 0 if no address is available.
 */
size_t board_get_eui64(const char *driver, unsigned idx, eui64_t *addr);

/**
 * @brief Generates an EUI-64 address for the netdev interface.
 *
 * @note It is possible to supply a board-specific, constant addres
 *       by implementing @ref board_get_eui64.
 *       If no such function is availiable, this will fall back to
 *       @ref luid_get_eui64.
 *
 * @param[in] driver    The driver name of the netdev
 *                      Will be passed on to @ref board_get_eui64.
 * @param[in] idx       The index in the <driver>_params_t struct
 *                      Will be passed on to @ref board_get_eui64.
 * @param[out] addr     The generated EUI-64 address
 *
 */
static inline void l2util_generate_eui64(const char *driver, unsigned idx, eui64_t *addr)
{
    if (board_get_eui64(driver, idx, addr) == sizeof(*addr)) {
        return;
    }

    luid_get_eui64(addr);
}

#ifdef __cplusplus
}
#endif

#endif /* NET_L2UTIL_H */
/** @} */
