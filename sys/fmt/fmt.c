/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_fmt
 * @{
 *
 * @file
 * @brief       String formatting library implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#if defined(__WITH_AVRLIBC__) || defined(__mips__)
#include <stdio.h>  /* for fwrite() */
#else
/* work around broken sys/posix/unistd.h */
ssize_t write(int fildes, const void *buf, size_t nbyte);
#endif

#include "fmt.h"

static const char _hex_chars[16] = "0123456789ABCDEF";

static inline int _is_digit(char c)
{
    return (c >= '0' && c <= '9');
}

static inline unsigned pwr(unsigned val, unsigned exp)
{
    unsigned res = 1;

    for (unsigned i = 0; i < exp; i++) {
        res *= val;
    }

    return res;
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

size_t fmt_u64_dec(char *out, uint64_t val)
{
    uint32_t d[5];
    uint32_t q;
    size_t len = 0;

    d[0] = val       & 0xFFFF;
    d[1] = (val>>16) & 0xFFFF;
    d[2] = (val>>32) & 0xFFFF;
    d[3] = (val>>48) & 0xFFFF;

    d[0] = 656 * d[3] + 7296 * d[2] + 5536 * d[1] + d[0];
    q = d[0] / 10000;
    d[0] = d[0] % 10000;

    d[1] = q + 7671 * d[3] + 9496 * d[2] + 6 * d[1];
    q = d[1] / 10000;
    d[1] = d[1] % 10000;

    d[2] = q + 4749 * d[3] + 42 * d[2];
    q = d[2] / 10000;
    d[2] = d[2] % 10000;

    d[3] = q + 281 * d[3];
    q = d[3] / 10000;
    d[3] = d[3] % 10000;

    d[4] = q;

    int first = 4;

    while (!d[first] && first) {
        first--;
    }

    len = fmt_u32_dec(out, d[first]);
    int total_len = len + (first * 4);

    if (out) {
        out += len;
        memset(out, '0', total_len - len);
        while(first) {
            first--;
            if (d[first]) {
                size_t tmp = fmt_u32_dec(NULL, d[first]);
                fmt_u32_dec(out+(4-tmp), d[first]);
            }
            out += 4;
        }
    }

    return total_len;
}

size_t fmt_u32_dec(char *out, uint32_t val)
{
    size_t len = 1;

    /* count needed characters */
    for (uint32_t tmp = val; (tmp > 9); len++) {
        tmp /= 10;
    }

    if (out) {
        char *ptr = out + len;
        do {
            *--ptr = (val % 10) + '0';
        } while ((val /= 10));
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

size_t fmt_s16_dec(char *out, int16_t val)
{
    return fmt_s32_dec(out, val);
}

size_t fmt_s16_dfp(char *out, int16_t val, unsigned fp_digits)
{
    int16_t absolute, divider;
    size_t pos = 0;
    size_t div_len, len;
    unsigned e;
    char tmp[4];

    if (fp_digits > 4) {
        return 0;
    }
    if (fp_digits == 0) {
        return fmt_s16_dec(out, val);
    }
    if (val < 0) {
        if (out) {
            out[pos++] = '-';
        }
        val *= -1;
    }

    e = pwr(10, fp_digits);
    absolute = (val / (int)e);
    divider = val - (absolute * e);

    pos += fmt_s16_dec(&out[pos], absolute);

    if (!out) {
        return pos + 1 + fp_digits;     /* abs len + decimal point + divider */
    }

    out[pos++] = '.';
    len = pos + fp_digits;
    div_len = fmt_s16_dec(tmp, divider);

    while (pos < (len - div_len)) {
        out[pos++] = '0';
    }
    for (size_t i = 0; i < div_len; i++) {
        out[pos++] = tmp[i];
    }

    return pos;
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
#ifdef __WITH_AVRLIBC__
    /* AVR's libc doesn't offer write(), so use fwrite() instead */
    fwrite(s, n, 1, stdout);
#else
    while (n > 0) {
        ssize_t written = write(STDOUT_FILENO, s, n);
        if (written < 0) {
            break;
        }
        n -= written;
        s += written;
    }
#endif /* __WITH_AVRLIBC__ */
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

void print_byte_hex(uint8_t byte)
{
    char buf[2];
    fmt_byte_hex(buf, byte);
    print(buf, sizeof(buf));
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

void print_u64_dec(uint64_t val)
{
    char buf[18];
    size_t len = fmt_u64_dec(buf, val);
    print(buf, len);
}

void print_str(const char* str)
{
    print(str, fmt_strlen(str));
}
