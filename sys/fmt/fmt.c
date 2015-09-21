/**
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup sys_fmt
 * @{
 * @file
 * @brief String formatting library implementation
 * @author Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>

#include "div.h"
#include "fmt.h"

static const char _hex_chars[16] = "0123456789ABCDEF";

static inline int _is_digit(char c)
{
    return (c >= '0' && c <= '9');
}

size_t fmt_byte_hex(char *out, uint8_t byte)
{
    if (out) {
        *out++ = _hex_chars[byte >> 4];
        *out = _hex_chars[byte & 0x0F];
    }
    return 2;
}

size_t fmt_strlen(const char *str)
{
    const char *tmp = str;
    while(*tmp) {
        tmp++;
    }
    return (tmp - str);
}

size_t fmt_str(char *out, const char *str)
{
    int len = 0;
    if (!out) {
        len = fmt_strlen(str);
    } else {
        char c;
        while ((c = *str++)) {
            *out++ = c;
            len++;
        }
    }
    return len;
}

size_t fmt_bytes_hex_reverse(char *out, const uint8_t *ptr, size_t n)
{
    size_t i = n;
    while (i--) {
        out += fmt_byte_hex(out, ptr[i]);
    }
    return (n<<1);
}

size_t fmt_u32_hex(char *out, uint32_t val)
{
    return fmt_bytes_hex_reverse(out, (uint8_t*) &val, 4);
}

size_t fmt_u64_hex(char *out, uint64_t val)
{
    return fmt_bytes_hex_reverse(out, (uint8_t*) &val, 8);
}

size_t fmt_u32_dec(char *out, uint32_t val)
{
    size_t len = 1;

    /* count needed characters */
    for (uint32_t tmp = val; (tmp > 9); len++) {
        tmp = div_u32_by_10(tmp);
    }

    if (out) {
        char *ptr = out + len;
        while(val) {
            *--ptr = div_u32_mod_10(val) + '0';
            val = div_u32_by_10(val);
        }
    }

    return len;
}

size_t fmt_u16_dec(char *out, uint16_t val)
{
    return fmt_u32_dec(out, val);
}

size_t fmt_s32_dec(char *out, int32_t val)
{
    int negative = (val < 0);
    if (negative) {
        if (out) {
            *out++ = '-';
        }
        val *= -1;
    }
    return fmt_u32_dec(out, val) + negative;
}

uint32_t scn_u32_dec(const char *str, size_t n)
{
    uint32_t res = 0;
    while(n--) {
        char c = *str++;
        if (!_is_digit(c)) {
            break;
        }
        else {
            res *= 10;
            res += (c - '0');
        }
    }
    return res;
}

void print(const char *s, size_t n)
{
    while (n > 0) {
        ssize_t written = write(STDOUT_FILENO, s, n);
        if (written < 0) {
            break;
        }
        n -= written;
        s += written;
    }
}

void print_u32_dec(uint32_t val)
{
    char buf[10];
    size_t len = fmt_u32_dec(buf, val);
    print(buf, len);
}

void print_s32_dec(int32_t val)
{
    char buf[11];
    size_t len = fmt_s32_dec(buf, val);
    print(buf, len);
}

void print_u32_hex(uint32_t val)
{
    char buf[8];
    fmt_u32_hex(buf, val);
    print(buf, sizeof(buf));
}

void print_u64_hex(uint64_t val)
{
    print_u32_hex(val>>32);
    print_u32_hex(val);
}

void print_str(const char* str)
{
    print(str, fmt_strlen(str));
}
