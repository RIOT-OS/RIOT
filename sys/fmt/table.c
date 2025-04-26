/*
 * Copyright 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_fmt_table
 * @{
 *
 * @file
 * @brief       Implementation of the table extensions of the string formatting
 *              library
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#include "compiler_hints.h"
#include "fmt.h"
#include "fmt_table.h"

NONSTRING
static const char fmt_table_spaces[16] = "                ";

/**
 * @brief Prints @p fill_size bytes of the given pattern, repeating the
 *        pattern if needed
 * @param pat       Pattern to print
 * @param pat_size  Size of the pattern in bytes
 * @param fill_size Number of bytes to print (if bigger than @p pat_size, the
 *                  pattern will be repeated)
 *
 * E.g. `print_pattern("ab", 2, 5);` will print `ababa` to the console.
 * This can be used to fill table columns with spaces, draw lines, etc.
 */
static void print_pattern(const char *pat, size_t pat_size, size_t fill_size)
{
    while (fill_size > pat_size) {
        print(pat, pat_size);
        fill_size -= pat_size;
    }

    print(pat, fill_size);
}

void print_col_u32_dec(uint32_t number, size_t width)
{
    char sbuf[10]; /* "4294967295" */
    size_t slen;

    slen = fmt_u32_dec(sbuf, number);
    if (width > slen) {
        print_pattern(fmt_table_spaces, sizeof(fmt_table_spaces), width - slen);
    }
    print(sbuf, slen);
}

void print_col_s32_dec(int32_t number, size_t width)
{
    char sbuf[11]; /* "-2147483648" */
    size_t slen;

    slen = fmt_s32_dec(sbuf, number);
    if (width > slen) {
        print_pattern(fmt_table_spaces, sizeof(fmt_table_spaces), width - slen);
    }
    print(sbuf, slen);
}
