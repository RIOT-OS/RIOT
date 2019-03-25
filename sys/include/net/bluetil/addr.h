/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef NET_BLUETIL_ADDR_H
#define NET_BLUETIL_ADDR_H

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
 * @brief   Convert the given BLE address to a human readable string
 *
 * @param[out] out      '\0' terminated address string, *must* be able to hold
 *                      BLUETIL_ADDR_STRLEN bytes
 * @param[in] addr      address buffer, *must* hold BLE_ADDR_LEN bytes
 */
void bluetil_addr_sprint(char *out, const uint8_t *addr);

/**
 * @brief   Print the given BLE address to STDOUT
 *
 * @param[in] addr      address to print, is expected to hold BLE_ADDR_LEN bytes
 */
void bluetil_addr_print(const uint8_t *addr);

/**
 * @brief   Get a string representation of the given BLE addresses IID-based
 *          link local address
 *
 * @param[out] out      '\0' terminated string, *must* be able to hold
 *                      BLUETIL_IPV6_IID_STRLEN bytes
 * @param[in]  addr     address to convert, , *must* hold BLE_ADDR_LEN bytes
 */
void bluetil_addr_ipv6_l2ll_sprint(char *out, const uint8_t *addr);

/**
 * @brief   Dump the given BLE addresses IPv6 IID-based link local address to
 *          STDIO
 *
 * @param[in] addr      generate IID for this address
 */
void bluetil_addr_ipv6_l2ll_print(const uint8_t *addr);

#ifdef __cplusplus
}
#endif

#endif /* NET_BLUETIL_ADDR_H */
/** @} */
