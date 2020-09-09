/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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
#ifndef SLIPDEV_INTERNAL_H
#define SLIPDEV_INTERNAL_H

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
 *          [SLIPMUX](https://tools.ietf.org/html/draft-bormann-t2trg-slipmux-02#section-4)
 */
#define SLIPDEV_STDIO_START (0x0aU)
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

/**
 * @brief   Unstuffs a (SLIP-escaped) byte.
 *
 * @param[out] buf          The buffer to write to. It must at least be able to
 *                          receive 1 byte.
 * @param[in] byte          The byte to unstuff.
 * @param[in,out] escaped   When set to `false` on in, @p byte will be read as
 *                          though it was not escaped, when set to `true` it
 *                          will be read as though it was escaped. On out it
 *                          will be `false` unless @p byte was `SLIPDEV_ESC`.
 *
 * @return  0, when @p byte did not resolve to an actual byte
 * @return  1, when @p byte resolves to an actual byte (or @p escaped was set to
 *          true on in and resolves to a byte that was previously escaped).
 */
unsigned slipdev_unstuff_readbyte(uint8_t *buf, uint8_t byte, bool *escaped);

#ifdef __cplusplus
}
#endif

#endif /* SLIPDEV_INTERNAL_H */
/** @} */
