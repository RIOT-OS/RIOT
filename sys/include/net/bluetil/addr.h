/*
 * SPDX-FileCopyrightText: 2019 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    ble_bluetil_addr BLE Address Helper
 * @ingroup     ble_bluetil
 * @brief       Generic BLE address handling functions
 * @{
 *
 * @file
 * @brief       Interface for the generic BLE address helper functions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include <stdint.h>

#include "net/ble.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The length of a BLE address string in bytes (including '\0')
 */
#define BLUETIL_ADDR_STRLEN         (18U)

/**
 * @brief   The length of an IPv6 IID string in bytes (including '\0')
 */
#define BLUETIL_IPV6_IID_STRLEN     (28U)

/**
 * @brief   Copy address and swap the byte order in the target buffer
 *
 * @param[in] src       buffer with source address, *must* hold
 *                      @ref BLE_ADDR_LEN bytes
 * @param[out] dst      target buffer, *must* be able to hold @ref BLE_ADDR_LEN
 *                      bytes
 */
void bluetil_addr_swapped_cp(const uint8_t *src, uint8_t *dst);

/**
 * @brief   Convert the given BLE address to a human readable string
 *
 * @note    The address is expected to be in network byte order
 *
 * @param[out] out      '\0' terminated address string, *must* be able to hold
 *                      @ref BLUETIL_ADDR_STRLEN bytes
 * @param[in] addr      address buffer, *must* hold @ref BLE_ADDR_LEN bytes
 */
void bluetil_addr_sprint(char *out, const uint8_t *addr);

/**
 * @brief   Print the given BLE address to STDOUT
 *
 * @note    The address is expected to be in network byte order
 *
 * @param[in] addr      address to print, is expected to hold @ref BLE_ADDR_LEN
 *                      bytes
 */
void bluetil_addr_print(const uint8_t *addr);

/**
 * @brief   Parse a BLE address from the given string
 *
 * @note    The address is expected most significant byte first and is written
 *          to @p addr in network byte order
 *
 * @param[out] addr     buffer to write the BLE address, *must* be able to hold
 *                      @ref BLE_ADDR_LEN bytes
 * @param[in] addr_str  address string, must be at least of length
 *                      (@ref BLUETIL_ADDR_STRLEN - 1)
 *
 * @return  a pointer to the resulting address on success
 * @return  NULL on parsing error
 */
uint8_t *bluetil_addr_from_str(uint8_t *addr, const char *addr_str);

/**
 * @brief   Get a string representation of the given BLE addresses IID-based
 *          link local address
 *
 * @note    The address is expected to be in network byte order
 *
 * @param[out] out      '\0' terminated string, *must* be able to hold
 *                      @ref BLUETIL_IPV6_IID_STRLEN bytes
 * @param[in]  addr     address to convert, , *must* hold @ref BLE_ADDR_LEN bytes
 */
void bluetil_addr_ipv6_l2ll_sprint(char *out, const uint8_t *addr);

/**
 * @brief   Dump the given BLE addresses IPv6 IID-based link local address to
 *          STDIO
 *
 * @note    The address is expected to be in network byte order
 *
 * @param[in] addr      generate IID for this address
 */
void bluetil_addr_ipv6_l2ll_print(const uint8_t *addr);

#ifdef __cplusplus
}
#endif

/** @} */
