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

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#include "fmt.h"

void print_u32_dec_fill(uint32_t number, size_t width, char fill)
{
    char sbuf[10]; /* "4294967295" */
    size_t slen;

    slen = fmt_u32_dec(sbuf, number);
    while (width > slen) {
        print(&fill, 1);
        width--;
    }
    print(sbuf, slen);
}

void print_u32_dec_fill_right(uint32_t number, size_t width, char fill)
{
    char sbuf[10]; /* "4294967295" */
    size_t slen;

    slen = fmt_u32_dec(sbuf, number);
    print(sbuf, slen);
    while (width > slen) {
        print(&fill, 1);
        width--;
    }
}

void print_s32_dec_fill(int32_t number, size_t width, char fill)
{
    char sbuf[11]; /* "-2147483648" */
    size_t slen;

    slen = fmt_s32_dec(sbuf, number);
    while (width > slen) {
        print(&fill, 1);
        width--;
    }
    print(sbuf, slen);
}

void print_s32_dec_fill_right(int32_t number, size_t width, char fill)
{
    char sbuf[11]; /* "-2147483648" */
    size_t slen;

    slen = fmt_s32_dec(sbuf, number);
    print(sbuf, slen);
    while (width > slen) {
        print(&fill, 1);
        width--;
    }
}

void print_str_fill(const char *str, size_t width, char fill)
{
    size_t slen = strlen(str);
    while (width > slen) {
        print(&fill, 1);
        width--;
    }
    print(str, slen);
}

void print_str_fill_right(const char *str, size_t width, char fill)
{
    size_t slen = strlen(str);
    print(str, slen);
    while (width > slen) {
        print(&fill, 1);
        width--;
    }
}
