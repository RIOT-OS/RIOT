/*
 * Copyright 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_fmt_stdio   Partial compatibility with stdio.h
 * @ingroup     sys_fmt
 * @brief       Provides functions to easily replace users of stdio.h
 *
 * @{
 *
 * @file
 * @brief       APIs provied by @ref sys_fmt_stdio
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef FMT_STDIO_H
#define FMT_STDIO_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Print the output specified by the given format
 *
 * @param   format      String to print while processing format specifiers
 * @param   ...         Additional data to print as given by format specifiers
 *
 * Supported Format Specifiers
 * ===========================
 *
 * - `%s`: String (`char` array terminated by a zero byte)
 * - `%p`: Pointer address (hexadecimal without leading `0x`)
 * - `%[<LENGTH_SPECIFIER>]x`: Integer (printed hexadecimal)
 * - `%[<LENGTH_SPECIFIER>]d`: Signed integer of given length
 * - `%[<LENGTH_SPECIFIER>]i`: Signed integer of given length
 * - `%[<LENGTH_SPECIFIER>]u`: Unsigned integer of given length
 *
 * Supported Length Specifiers
 * ===========================
 * - Empty (e.g. `%d`): `int` / `unsigned int`
 * - `l` (e.g. `%ld`): `long int` / `unsigned long int`
 * - `h` (e.g. `%hd`): `short int` / `unsigned short int`
 * - `hh` (e.g. `%hhd`): `signed char` / `unsigned char`
 *
 * Limitations
 * ===========
 *
 * All other format specifiers and length are (intentionally) not supported.
 * Unlike the real `printf()`, the length of the printed string is
 * (intentionally) not returned. All additional format and length specifiers as
 * well as the return value are rarely useful, but increase ROM size.
 */
void printf_nano(const char *format, ...);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* FMT_STDIO_H */
