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

static const uint32_t _tenmap[] = {
    0,
    10LU,
    100LU,
    1000LU,
    10000LU,
    100000LU,
    1000000LU,
    10000000LU,
};

#define TENMAP_SIZE  ARRAY_SIZE(_tenmap)

static inline char _to_lower(char c)
{
    return 'a' + (c - 'A');
}

int fmt_is_number(const char *str)
{
    if (!str || !*str) {
        return 0;
    }
    for (; *str; str++) {
        if (!fmt_is_digit(*str)) {
            return 0;
        }
    }

    return 1;
}

size_t fmt_byte_hex(char *out, uint8_t byte)
{
    if (out) {
        *out++ = _hex_chars[byte >> 4];
        *out = _hex_chars[byte & 0x0F];
    }
    return 2;
}

size_t fmt_bytes_hex(char *out, const uint8_t *ptr, size_t n)
{
    size_t len = n * 2;
    if (out) {
        while (n--) {
            out += fmt_byte_hex(out, *ptr++);
        }
    }

    return len;
}

size_t fmt_strlen(const char *str)
{
    const char *tmp = str;
    while(*tmp) {
        tmp++;
    }
    return (tmp - str);
}

size_t fmt_strnlen(const char *str, size_t maxlen)
{
    const char *tmp = str;
    while(*tmp && maxlen--) {
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

static uint8_t _byte_mod25(uint8_t x)
{
    for (unsigned divisor = 200; divisor >= 25; divisor >>= 1) {
        if (x >= divisor) {
            x -= divisor;
        }
    }

    return x;
}

static uint8_t _hex_nib(uint8_t nib)
{
    return _byte_mod25((nib & 0x1f) + 9);
}

uint8_t fmt_hex_byte(const char *hex)
{
    return (_hex_nib(hex[0]) << 4) | _hex_nib(hex[1]);
}

size_t fmt_hex_bytes(uint8_t *out, const char *hex)
{
    size_t len = fmt_strlen(hex);

    if (len & 1) {
        return 0;
    }

    size_t final_len = len >> 1;

    if (out == NULL) {
        return final_len;
    }

    for (size_t i = 0, j = 0; j < final_len; i += 2, j++) {
        out[j] = fmt_hex_byte(hex + i);
    }

    return final_len;
}

size_t fmt_u16_hex(char *out, uint16_t val)
{
    return fmt_bytes_hex_reverse(out, (uint8_t*) &val, 2);
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
    /* avoid multiply overflow: uint32_t max len = 10 digits */
    if (val >= 1000000000ul) {
        len = 10;
    }
    else {
        for (uint32_t tmp = 10; tmp <= val; len++) {
            tmp *= 10;
        }
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

size_t fmt_s64_dec(char *out, int64_t val)
{
    unsigned negative = (val < 0);
    uint64_t sval;
    if (negative) {
        if (out) {
            *out++ = '-';
        }
        sval = -(uint64_t)(val);
    }
    else {
        sval = +(uint64_t)(val);
    }
    return fmt_u64_dec(out, sval) + negative;
}

size_t fmt_s32_dec(char *out, int32_t val)
{
    unsigned negative = (val < 0);
    uint32_t sval;
    if (negative) {
        if (out) {
            *out++ = '-';
        }
        sval = -((uint32_t)(val));
    }
    else {
        sval = +((uint32_t)(val));
    }
    return fmt_u32_dec(out, sval) + negative;
}

size_t fmt_s16_dec(char *out, int16_t val)
{
    return fmt_s32_dec(out, val);
}

size_t fmt_s16_dfp(char *out, int16_t val, int fp_digits)
{
    return fmt_s32_dfp(out, val, fp_digits);
}

size_t fmt_s32_dfp(char *out, int32_t val, int fp_digits)
{
    assert(fp_digits > -(int)TENMAP_SIZE);

    unsigned  pos = 0;

    if (fp_digits == 0) {
        pos = fmt_s32_dec(out, val);
    }
    else if (fp_digits > 0) {
        pos = fmt_s32_dec(out, val);
        if (out) {
            memset(&out[pos], '0', fp_digits);
        }
        pos += fp_digits;
    }
    else {
        fp_digits *= -1;
        uint32_t e = _tenmap[fp_digits];
        int32_t abs = (val / (int32_t)e);
        int32_t div = val - (abs * e);

        /* the divisor should never be negative */
        if (div < 0) {
            div *= -1;
        }
        /* handle special case for negative number with zero as absolute value */
        if ((abs == 0) && (val < 0)) {
            if (out) {
                out[pos] = '-';
            }
            pos++;
        }

        if (!out) {
            /* compensate for the decimal point character... */
            pos += fmt_s32_dec(NULL, abs) + 1;
        }
        else {
            pos += fmt_s32_dec(&out[pos], abs);
            out[pos++] = '.';
            unsigned div_len = fmt_s32_dec(&out[pos], div);
            fmt_lpad(&out[pos], div_len, (size_t)fp_digits, '0');
        }
        pos += fp_digits;
    }

    return pos;
}

/* this is very probably not the most efficient implementation, as it at least
 * pulls in floating point math.  But it works, and it's always nice to have
 * low hanging fruits when optimizing. (Kaspar)
 */
size_t fmt_float(char *out, float f, unsigned precision)
{
    assert(precision < TENMAP_SIZE);

    unsigned negative = (f < 0);
    uint32_t integer;

    if (negative) {
        f = -f;
    }

    integer = (uint32_t) f;
    f -= integer;

    uint32_t fraction = f * _tenmap[precision];

    if (negative && out) {
        *out++ = '-';
    }

    size_t res = fmt_u32_dec(out, integer);
    if (precision && fraction) {
        if (out) {
            out += res;
            *out++ = '.';
            size_t tmp = fmt_u32_dec(out, fraction);
            fmt_lpad(out, tmp, precision, '0');
        }
        res += (1 + precision);
    }
    res += negative;

    return res;
}

size_t fmt_lpad(char *out, size_t in_len, size_t pad_len, char pad_char)
{
    if (in_len >= pad_len) {
        return in_len;
    }

    if (out) {
        size_t n = pad_len - in_len;

        if (FMT_USE_MEMMOVE) {
            memmove(out + n, out, in_len);
            memset(out, pad_char, n);
        }
        else {
            char *pos = out + pad_len - 1;
            out += in_len -1;

            while(in_len--) {
                *pos-- = *out--;
            }

            while (n--) {
                *pos-- = pad_char;
            }
        }
    }

    return pad_len;
}

size_t fmt_char(char *out, char c)
{
    if (out) {
        *out = c;
    }

    return 1;
}

size_t fmt_to_lower(char *out, const char *str)
{
    size_t len = 0;

    while (str && *str) {
        if (fmt_is_upper(*str)) {
            if (out) {
                *out++ = _to_lower(*str);
            }
        }
        else if (out) {
            *out++ = *str;
        }
        str++;
        len++;
    }

    return len;
}

uint32_t scn_u32_dec(const char *str, size_t n)
{
    uint32_t res = 0;
    while(n--) {
        char c = *str++;
        if (!fmt_is_digit(c)) {
            break;
        }
        else {
            res *= 10;
            res += (c - '0');
        }
    }
    return res;
}

uint32_t scn_u32_hex(const char *str, size_t n)
{
    uint32_t res = 0;

    while (n--) {
        char c = *str++;
        if (!fmt_is_digit(c)) {
            if (fmt_is_upper(c)) {
                c = _to_lower(c);
            }
            if (c == '\0' || c > 'f') {
                break;
            }
            res <<= 4;
            res |= c - 'a' + 0xa;
        }
        else {
            res <<= 4;
            res |= c - '0';
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
    char buf[10]; /* "4294967295" */
    size_t len = fmt_u32_dec(buf, val);
    print(buf, len);
}

void print_s32_dec(int32_t val)
{
    char buf[11]; /* "-2147483648" */
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
    char buf[20]; /* "18446744073709551615" */
    size_t len = fmt_u64_dec(buf, val);
    print(buf, len);
}

void print_s64_dec(uint64_t val)
{
    char buf[20]; /* "-9223372036854775808" */
    size_t len = fmt_s64_dec(buf, val);
    print(buf, len);
}

void print_float(float f, unsigned precision)
{
    char buf[19];
    size_t len = fmt_float(buf, f, precision);
    print(buf, len);
}

void print_str(const char* str)
{
    print(str, fmt_strlen(str));
}
