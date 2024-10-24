/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    pkg_nimble_addr Address Helper
 * @ingroup     pkg_nimble
 * @brief       NimBLE specific helper functions for handling addresses
 * @{
 *
 * @file
 * @brief       Interface for NimBLE specific address helper functions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NIMBLE_ADDR_H
#define NIMBLE_ADDR_H

#include "nimble/ble.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Length of an address string in byte, including `\0` termination
 */
#define NIMBLE_ADDR_STRLEN          (28U)

/**
 * @brief   Print the given address on STDIO
 *
 * @param[in] addr      address to print
 */
void nimble_addr_print(const ble_addr_t *addr);

/**
 * @brief   Write the string representation of the given address into a buffer
 *
 * The resulting string written to @p buf is `\0` terminated and is always
 * 28 bytes (NIMBLE_ADDR_STRLEN) long.
 *
 * @param[out] buf      buffer
 * @param[in] addr      addre to convert
 */
void nimble_addr_sprint(char *buf, const ble_addr_t *addr);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NIMBLE_ADDR_H */
