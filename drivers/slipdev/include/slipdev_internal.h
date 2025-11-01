/*
 * SPDX-FileCopyrightText: 2018 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup drivers_slipdev
 * @{
 *
 * @file
 * @internal
 * @brief   Internal SLIP device definitions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <stddef.h>
#include <stdint.h>

#include "isrpipe.h"
#include "periph/uart.h"
#include "mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    SLIP marker bytes
 * @see     [RFC 1055](https://tools.ietf.org/html/rfc1055)
 * @{
 */
#define SLIPDEV_END         (0xc0U)
#define SLIPDEV_ESC         (0xdbU)
#define SLIPDEV_END_ESC     (0xdcU)
#define SLIPDEV_ESC_ESC     (0xddU)

/**
 * @brief   Marker byte for beginning of stdio
 * @see     taken from diagnostic transfer from
 *          [SLIPMUX](https://tools.ietf.org/html/draft-bormann-t2trg-slipmux-03#section-4)
 */
#define SLIPDEV_STDIO_START (0x0aU)

/**
 * @brief   Marker byte for beginning of configuration/CoAP
 * @see     taken from configuration from
 *          [SLIPMUX](https://tools.ietf.org/html/draft-bormann-t2trg-slipmux-03#section-5)
 */
#define SLIPDEV_CONFIG_START (0xa9U)
/** @} */

/**
 * @brief   ISR pipe to hand read bytes to stdin
 */
extern isrpipe_t slipdev_stdio_isrpipe;

/**
 * @brief   Mutex to synchronize write operations to the UART between stdio
 *          sub-module and normal SLIP.
 */
extern mutex_t slipdev_mutex;

/**
 * @brief   Writes one byte to UART
 *
 * @param[in] uart  The UART device to write to.
 * @param[in] byte  The byte to write to @p uart.
 */
static inline void slipdev_write_byte(uart_t uart, uint8_t byte)
{
    uart_write(uart, &byte, 1U);
}

/**
 * @brief   Write multiple bytes SLIP-escaped to UART
 *
 * @param[in] uart  The UART device to write to.
 * @param[in] data  The bytes to write SLIP-escaped to @p uart.
 * @param[in] len   Number of bytes in @p data.
 */
void slipdev_write_bytes(uart_t uart, const uint8_t *data, size_t len);

#ifdef __cplusplus
}
#endif

/** @} */
