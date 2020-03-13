/*
 * Copyright 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_fmt_table Table extension of the string formatting API (fmt_table)
 * @ingroup     sys_fmt
 * @brief       Provides utilities to print tables.
 *
 * \note The print functions in this library do not buffer any output.
 * Mixing calls to standard @c printf from stdio.h with the @c print_xxx
 * functions in fmt, especially on the same output line, may cause garbled
 * output.
 *
 * @{
 *
 * @file
 * @brief       Table extension of the string formatting API
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef FMT_TABLE_H
#define FMT_TABLE_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Print the given number and fill the output on the left to reach the
 *          given width
 *
 * @param   number      Number to print
 * @param   width       Number of characters to print
 * @param   fill        Character used to fill up (if needed) the output
 */
void print_u32_dec_fill(uint32_t number, size_t width, char fill);

/**
 * @brief   Print the given number and fill the output on the right to reach the
 *          given width
 *
 * @param   number      Number to print
 * @param   width       Number of characters to print
 * @param   fill        Character used to fill up (if needed) the output
 */
void print_u32_dec_fill_right(uint32_t number, size_t width, char fill);

/**
 * @brief   Print the given number and fill the output on the left to reach the
 *          given width
 *
 * @param   number      Number to print
 * @param   width       Number of characters to print
 * @param   fill        Character used to fill up (if needed) the output
 */
void print_s32_dec_fill(int32_t number, size_t width, char fill);

/**
 * @brief   Print the given number and fill the output on the right to reach the
 *          given width
 *
 * @param   number      Number to print
 * @param   width       Number of characters to print
 * @param   fill        Character used to fill up (if needed) the output
 */
void print_s32_dec_fill_right(int32_t number, size_t width, char fill);

/**
 * @brief   Print the given zero terminated string and fill the output on the
 *          left with the char @p fill to print @p width chars in total
 *
 * @param   str         String to print (terminated by a zero byte)
 * @param   width       Number of chars to print
 * @param   fill        Character to fill the output with
 */
void print_str_fill(const char *str, size_t width, char fill);

/**
 * @brief   Print the given zero terminated string and fill the output on the
 *          right with the char @p fill to print @p width chars in total
 *
 * @param   str         String to print (terminated by a zero byte)
 * @param   width       Number of chars to print
 * @param   fill        Character to fill the output with
 */
void print_str_fill_right(const char *str, size_t width, char fill);

/**
 * @brief   Print a table column with the given number as decimal
 *
 * @param   number    Number to print in the column
 * @param   width     Width of the column
 */
static inline void print_col_u32_dec(uint32_t number, size_t width)
{
    print_u32_dec_fill(number, width, ' ');
}

/**
 * @brief   Print a table column with the given number as decimal
 *
 * @param   number    Number to print in the column
 * @param   width     Width of the column
 */
static inline void print_col_s32_dec(int32_t number, size_t width)
{
    print_s32_dec_fill(number, width, ' ');
}

/**
 * @brief   Print the given numbers and add leading zeros when needed
 *
 * @param   number      Number to print
 * @param   width       Number of characters to print
 */
static inline void print_u32_dec_zeros(uint32_t number, size_t width)
{
    print_u32_dec_fill(number, width, '0');
}

/**
 * @brief   Print the given numbers and add leading zeros when needed
 *
 * @param   number      Number to print
 * @param   width       Number of characters to print
 */
static inline void print_s32_dec_zeros(int32_t number, size_t width)
{
    if (number < 0) {
        const char minus[1] = "-";
        print(minus, 1);
        width--;
        number = -number;
    }
    print_u32_dec_zeros((uint32_t)number, width);
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* FMT_TABLE_H */
