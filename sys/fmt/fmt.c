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
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "compiler_hints.h"
#include "container.h"
#include "fmt.h"
#include "modules.h"

extern ssize_t stdio_write(const void* buffer, size_t len);

NONSTRING
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

size_t fmt_bytes_hex_reverse(char *out, const uint8_t *ptr, size_t n)
{
    size_t len = n * 2;

    if (out) {
        while (n--) {
            out += fmt_byte_hex(out, ptr[n]);
        }
    }

    return len;
}

size_t fmt_strlen(const char *str)
{
    const char *tmp = str;

    while (*tmp) {
        tmp++;
    }
    return (tmp - str);
}

size_t fmt_strnlen(const char *str, size_t maxlen)
{
    const char *tmp = str;

    while (*tmp && maxlen--) {
        tmp++;
    }
    return (tmp - str);
}

size_t fmt_str(char *out, const char *str)
{
    int len = 0;

    if (!out) {
        len = fmt_strlen(str);
    }
    else {
        char c;
        while ((c = *str++)) {
            *out++ = c;
            len++;
        }
    }
    return len;
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
    return fmt_bytes_hex_reverse(out, (uint8_t *)&val, 2);
}

size_t fmt_u32_hex(char *out, uint32_t val)
{
    return fmt_bytes_hex_reverse(out, (uint8_t *)&val, 4);
}

size_t fmt_u64_hex(char *out, uint64_t val)
{
    return fmt_bytes_hex_reverse(out, (uint8_t *)&val, 8);
}

size_t fmt_u64_dec(char *out, uint64_t val)
{
    uint32_t d[5];
    uint32_t q;
    size_t len = 0;

    d[0] = val         & 0xFFFF;
    d[1] = (val >> 16) & 0xFFFF;
    d[2] = (val >> 32) & 0xFFFF;
    d[3] = (val >> 48) & 0xFFFF;

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
        while (first) {
            first--;
            if (d[first]) {
                size_t tmp = fmt_u32_dec(NULL, d[first]);
                fmt_u32_dec(out + (4 - tmp), d[first]);
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

size_t fmt_s16_dfp(char *out, int16_t val, int scale)
{
    return fmt_s32_dfp(out, val, scale);
}

size_t fmt_s32_dfp(char *out, int32_t val, int scale)
{
    unsigned pos = 0;

    if (scale == 0) {
        pos = fmt_s32_dec(out, val);
    }
    else if (scale > 0) {
        pos = fmt_s32_dec(out, val);
        if (out) {
            memset(&out[pos], '0', scale);
        }
        pos += scale;
    }
    else {
        scale = -scale;
        char buf[10]; /* "2147483648" */
        int negative = val < 0;
        uint32_t uval = negative ? -val : val;
        int len = fmt_u32_dec(buf, uval);
        if (negative) {
            if (out) {
                out[pos] = '-';
            }
            pos++;
        }
        if (len <= scale) {
            int zeroes = scale - len + 1;
            if (out) {
                memset(&out[pos], '0', zeroes);
            }
            pos += zeroes;
        }
        if (out) {
            memcpy(&out[pos], buf, len);
        }

        pos += len;

        if (out) {
            unsigned dot_pos = pos - scale;
            for (unsigned i = pos; i >= dot_pos; i--) {
                out[i] = out[i - 1];
            }
            out[dot_pos] = '.';
        }
        pos += 1;
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

    integer = (uint32_t)f;
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
            out += in_len - 1;

            while (in_len--) {
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

int fmt_time_tm_iso8601(char out[20], const struct tm *tm, char separator)
{
    assert(out);
    assert(tm);
    /* The lowest year allowed in ISO 8601 is 0000 (year zero), which represents 1 BCE */
    if ((tm->tm_year < -1900 || tm->tm_year > 9999 - 1900) ||
        (tm->tm_mon < -1 || tm->tm_mon > 99 - 1) ||
        (tm->tm_mday < 0 || tm->tm_mday > 99) ||
        (tm->tm_hour < 0 || tm->tm_hour > 99) ||
        (tm->tm_min < 0 || tm->tm_min > 99) ||
        (tm->tm_sec < 0 || tm->tm_sec > 99)) {
        return -EINVAL;
    }
    char *pos = out;
    int len;

    len =  fmt_u16_dec(pos, tm->tm_year + 1900);
    fmt_lpad(pos, len, 4, '0');
    pos += 4;
    *pos++ = '-';

    len = fmt_u16_dec(pos, tm->tm_mon + 1);
    fmt_lpad(pos, len, 2, '0');
    pos += 2;
    *pos++ = '-';

    len = fmt_u16_dec(pos, tm->tm_mday);
    fmt_lpad(pos, len, 2, '0');
    pos += 2;
    *pos++ = separator;

    len = fmt_u16_dec(pos, tm->tm_hour);
    fmt_lpad(pos, len, 2, '0');
    pos += 2;
    *pos++ = ':';

    len = fmt_u16_dec(pos, tm->tm_min);
    fmt_lpad(pos, len, 2, '0');
    pos += 2;
    *pos++ = ':';

    len = fmt_u16_dec(pos, tm->tm_sec);
    fmt_lpad(pos, len, 2, '0');
    pos += 2;
    *pos = '\0';

    return pos - out;
}

uint32_t scn_u32_dec(const char *str, size_t n)
{
    uint32_t res = 0;

    while (n--) {
        unsigned c = *str++;
        unsigned d = c - (unsigned)'0';
        if ( !( '0'<= c && c <= '9') ) {
            break;
        }
        res *= 10U;
        res += d;
    }
    return res;
}

uint32_t scn_u32_hex(const char *str, size_t n)
{
    uint32_t res = 0;

    while (n--) {
        unsigned c = *str++;
        unsigned d;
        if (('0'<= c) && (c <= '9')){
            d = c - (unsigned)'0';
        }
        else if (('A' <= c) && (c <= 'F')) {
            d = c - (unsigned)'A' + 0xaU;
        }
        else if (('a' <= c) && (c <= 'f')) {
            d = c - (unsigned)'a' + 0xaU;
        }
        else {
            break;
        }
        res <<= 4U;
        res |= d;

    }
    return res;
}

static bool _get_nibble(uint8_t *dest, char _c)
{
    uint8_t c = _c;
    if (((uint8_t)'0' <= c) && (c <= (uint8_t)'9')) {
        *dest = c - (uint8_t)'0';
        return true;
    }

    if (((uint8_t)'a' <= c) && (c <= (uint8_t)'f')) {
        *dest = c - (uint8_t)'a' + 10;
        return true;
    }

    if (((uint8_t)'A' <= c) && (c <= (uint8_t)'F')) {
        *dest = c - (uint8_t)'A' + 10;
        return true;
    }

    return false;
}

ssize_t scn_buf_hex(void *_dest, size_t dest_len, const char *hex, size_t hex_len)
{
    uint8_t *dest = _dest;
    assert((dest != NULL) || (dest_len == 0));
    assert((hex != NULL) || (hex_len == 0));

    if (hex_len & 1) {
        /* we need to chars per every byte, so odd inputs don't work */
        return -EINVAL;
    }

    size_t len = hex_len >> 1;
    if (len > dest_len) {
        return -EOVERFLOW;
    }

    for (size_t pos = 0; pos < len; pos++) {
        uint8_t high, low;
        if (!_get_nibble(&high, hex[pos << 1])) {
            return -EINVAL;
        }

        if (!_get_nibble(&low, hex[(pos << 1) + 1])) {
            return -EINVAL;
        }

        dest[pos] = (high << 4) | low;
    }

    return len;
}

int scn_time_tm_iso8601(struct tm *tm, const char *str, char separator)
{
    assert(tm);
    assert(str);
    memset(tm, 0, sizeof(*tm));
    tm->tm_isdst = -1; /* undefined */

    if (!fmt_is_digit(str[0]) || !fmt_is_digit(str[1]) ||
        !fmt_is_digit(str[2]) || !fmt_is_digit(str[3]) ||
        str[4] != '-' ||
        !fmt_is_digit(str[5]) || !fmt_is_digit(str[6]) ||
        str[7] != '-' ||
        !fmt_is_digit(str[8]) || !fmt_is_digit(str[9])) {
        return -EINVAL;
    }

    uint32_t num = scn_u32_dec(str, 4);
    tm->tm_year = num - 1900;
    num = scn_u32_dec(str + 5, 2);
    tm->tm_mon = num - 1;
    num = scn_u32_dec(str + 8, 2);
    tm->tm_mday = num;

    if (str[10] == '\0') {
        /* no time, just date */
        return 10;
    }
    if (str[10] != separator) {
        return -EBADF;
    }
    if (!fmt_is_digit(str[11]) || !fmt_is_digit(str[12]) ||
        str[13] != ':' ||
        !fmt_is_digit(str[14]) || !fmt_is_digit(str[15]) ||
        str[16] != ':' ||
        !fmt_is_digit(str[17]) || !fmt_is_digit(str[18])) {
        return -EINVAL;
    }

    num = scn_u32_dec(str + 11, 2);
    tm->tm_hour = num;
    num = scn_u32_dec(str + 14, 2);
    tm->tm_min = num;
    num = scn_u32_dec(str + 17, 2);
    tm->tm_sec = num;

    return 19;
}

/* native gets special treatment as native's stdio code is ... special.
 * And when not building for RIOT, there's no `stdio_write()`.
 * In those cases, just defer to `printf()`.
 */
#if IS_USED(MODULE_STDIO_NATIVE) || !defined(RIOT_OS)
void print(const char *s, size_t n)
{
    printf("%.*s", (int)n, s);
}
#else
void print(const char *s, size_t n)
{
    /* flush the libc's output buffer so output is not intermingled. */
    fflush(stdout);

    stdio_write(s, n);
}
#endif

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

void print_bytes_hex(const void *bytes, size_t num)
{
    const uint8_t *b = bytes;

    while (num--) {
        print_byte_hex(*b++);
    }
}

void print_u32_hex(uint32_t val)
{
    char buf[8];

    fmt_u32_hex(buf, val);
    print(buf, sizeof(buf));
}

void print_u64_hex(uint64_t val)
{
    print_u32_hex(val >> 32);
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

void print_str(const char *str)
{
    print(str, fmt_strlen(str));
}
