/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 Kevin Funk <kfunk@kde.org>
 * Copyright (C) 2014 Jana Cavojska <jana.cavojska9@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @author      Kevin Funk <kfunk@kde.org>
 * @author      Jana Cavojska <jana.cavojska9@gmail.com>
 */

#include "cbor.h"

#include "byteorder.h"

#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef MODULE_CBOR_CTIME
#include <time.h>
#endif
#define ENABLE_DEBUG (0)
#include "debug.h"

#define CBOR_TYPE_MASK          0xE0    /* top 3 bits */
#define CBOR_INFO_MASK          0x1F    /* low 5 bits */

#define CBOR_BYTE_FOLLOWS       24      /* indicator that the next byte is part of this item */

/* Jump Table for Initial Byte (cf. table 5) */
#define CBOR_UINT       0x00            /* type 0 */
#define CBOR_NEGINT     0x20            /* type 1 */
#define CBOR_BYTES      0x40            /* type 2 */
#define CBOR_TEXT       0x60            /* type 3 */
#define CBOR_ARRAY      0x80            /* type 4 */
#define CBOR_MAP        0xA0            /* type 5 */
#define CBOR_TAG        0xC0            /* type 6 */
#define CBOR_7          0xE0            /* type 7 (float and other types) */

/* Major types (cf. section 2.1) */
/* Major type 0: Unsigned integers */
#define CBOR_UINT8_FOLLOWS      24      /* 0x18 */
#define CBOR_UINT16_FOLLOWS     25      /* 0x19 */
#define CBOR_UINT32_FOLLOWS     26      /* 0x1a */
#define CBOR_UINT64_FOLLOWS     27      /* 0x1b */

/* Indefinite Lengths for Some Major types (cf. section 2.2) */
#define CBOR_VAR_FOLLOWS        31      /* 0x1f */

/* Major type 6: Semantic tagging */
#define CBOR_DATETIME_STRING_FOLLOWS        0
#define CBOR_DATETIME_EPOCH_FOLLOWS         1

/* Major type 7: Float and other types */
#define CBOR_FALSE      (CBOR_7 | 20)
#define CBOR_TRUE       (CBOR_7 | 21)
#define CBOR_NULL       (CBOR_7 | 22)
#define CBOR_UNDEFINED  (CBOR_7 | 23)
/* CBOR_BYTE_FOLLOWS == 24 */
#define CBOR_FLOAT16    (CBOR_7 | 25)
#define CBOR_FLOAT32    (CBOR_7 | 26)
#define CBOR_FLOAT64    (CBOR_7 | 27)
#define CBOR_BREAK      (CBOR_7 | 31)

#define CBOR_TYPE(stream, offset) (stream->data[offset] & CBOR_TYPE_MASK)
#define CBOR_ADDITIONAL_INFO(stream, offset) (stream->data[offset] & CBOR_INFO_MASK)

/* Ensure that @p stream is big enough to fit @p bytes bytes, otherwise return 0 */
#define CBOR_ENSURE_SIZE(stream, bytes) do { \
    if (stream->pos + bytes >= stream->size) { return 0; } \
} while(0)

#define CBOR_ENSURE_SIZE_READ(stream, bytes) do { \
    if (bytes > stream->size) { return 0; } \
} while(0)

/* Extra defines not related to the protocol itself */
#define CBOR_STREAM_PRINT_BUFFERSIZE 1024 /* bytes */

/* Array size */
#define MAX_TIMESTRING_LENGTH   (21)
#define TIMESTRING_FORMAT "%Y-%m-%dT%H:%M:%SZ"

#ifndef INFINITY
#define INFINITY (1.0/0.0)
#endif
#ifndef NAN
#define NAN (0.0/0.0)
#endif


/* pack to force aligned access on ARMv7 (buggy GCC) */
#pragma GCC diagnostic error "-Wcast-align"
typedef struct __attribute__((packed)) {
    unsigned char u8;
    union {
        uint16_t u16;
        uint32_t u32;
        uint64_t u64;
    } u;
} cast_align_u8_t;

#ifdef MODULE_CBOR_FLOAT

/**
 * Convert float @p x to network format
 */
static uint32_t htonf(float x)
{
    union u {
        float f;
        uint32_t i;
    } u = { .f = x };
    return htonl(u.i);
}

/**
 * Convert float @p x to host format
 */
static float ntohf(uint32_t x)
{
    union u {
        float f;
        uint32_t i;
    } u = { .i = ntohl(x) };
    return u.f;
}

/**
 * Convert double @p x to network format
 */
static uint64_t htond(double x)
{
    union u {
        double d;
        uint64_t i;
    } u = { .d = x };
    return htonll(u.i);
}

/**
 * Convert double @p x to host format
 */
static double ntohd(uint64_t x)
{
    union u {
        double d;
        uint64_t i;
    } u = { .i = htonll(x) };
    return u.d;
}

/**
 * Source: CBOR RFC reference implementation
 */
double decode_float_half(unsigned char *halfp)
{
    int half = (halfp[0] << 8) + halfp[1];
    int exp = (half >> 10) & 0x1f;
    int mant = half & 0x3ff;
    double val;

    if (exp == 0) {
        val = ldexp(mant, -24);
    }
    else if (exp != 31) {
        val = ldexp(mant + 1024, exp - 25);
    }
    else {
        val = mant == 0 ? INFINITY : NAN;
    }

    return (half & 0x8000) ? -val : val;
}

/**
 * Source: According to http://gamedev.stackexchange.com/questions/17326/conversion-of-a-number-from-single-precision-floating-point-representation-to-a
 */
static uint16_t encode_float_half(float x)
{
    union u {
        float f;
        uint32_t i;
    } u = { .f = x };

    uint16_t bits = (u.i >> 16) & 0x8000; /* Get the sign */
    uint16_t m = (u.i >> 12) & 0x07ff; /* Keep one extra bit for rounding */
    unsigned int e = (u.i >> 23) & 0xff; /* Using int is faster here */

    /* If zero, or denormal, or exponent underflows too much for a denormal
     * half, return signed zero. */
    if (e < 103) {
        return bits;
    }

    /* If NaN, return NaN. If Inf or exponent overflow, return Inf. */
    if (e > 142) {
        bits |= 0x7c00u;
        /* If exponent was 0xff and one mantissa bit was set, it means NaN,
         * not Inf, so make sure we set one mantissa bit too. */
        bits |= (e == 255) && (u.i & 0x007fffffu);
        return bits;
    }

    /* If exponent underflows but not too much, return a denormal */
    if (e < 113) {
        m |= 0x0800u;
        /* Extra rounding may overflow and set mantissa to 0 and exponent
         * to 1, which is OK. */
        bits |= (m >> (114 - e)) + ((m >> (113 - e)) & 1);
        return bits;
    }

    bits |= ((e - 112) << 10) | (m >> 1);
    /* Extra rounding. An overflow will set mantissa to 0 and increment
     * the exponent, which is OK. */
    bits += m & 1;
    return bits;
}
#endif /* MODULE_CBOR_FLOAT */

/**
 * Print @p size bytes at @p data in hexadecimal display format
 */
void dump_memory(const unsigned char *data, size_t size)
{
    if (!data || !size) {
        return;
    }

    printf("0x");

    for (size_t i = 0; i < size; ++i) {
        printf("%02X", data[i]);
    }
    puts("");
}

void cbor_init(cbor_stream_t *stream, unsigned char *buffer, size_t size)
{
    if (!stream) {
        return;
    }

    stream->data = buffer;
    stream->size = size;
    stream->pos = 0;
}

void cbor_clear(cbor_stream_t *stream)
{
    if (!stream) {
        return;
    }

    stream->pos = 0;
}

void cbor_destroy(cbor_stream_t *stream)
{
    if (!stream) {
        return;
    }

    stream->data = 0;
    stream->size = 0;
    stream->pos = 0;
}

/**
 * Return additional info field value for input value @p val
 *
 * @return Byte with the additional info bits set
 */
static unsigned char uint_additional_info(uint64_t val)
{
    if (val < CBOR_UINT8_FOLLOWS) {
        return val;
    }
    else if (val <= 0xff) {
        return CBOR_UINT8_FOLLOWS;
    }
    else if (val <= 0xffff) {
        return CBOR_UINT16_FOLLOWS;
    }
    else if (val <= 0xffffffffL) {
        return CBOR_UINT32_FOLLOWS;
    }

    return CBOR_UINT64_FOLLOWS;
}

/**
 * Return the number of bytes that would follow the additional info field @p additional_info
 *
 * @param additional_info Must be in the range [CBOR_UINT8_FOLLOWS, CBOR_UINT64_FOLLOWS]
 */
static unsigned char uint_bytes_follow(unsigned char additional_info)
{
    if (additional_info < CBOR_UINT8_FOLLOWS || additional_info > CBOR_UINT64_FOLLOWS) {
        return 0;
    }

    static const unsigned char BYTES_FOLLOW[] = {1, 2, 4, 8};
    return BYTES_FOLLOW[additional_info - CBOR_UINT8_FOLLOWS];
}

static size_t encode_int(unsigned char major_type, cbor_stream_t *s, uint64_t val)
{
    if (!s) {
        return 0;
    }

    unsigned char additional_info = uint_additional_info(val);
    unsigned char bytes_follow = uint_bytes_follow(additional_info);
    CBOR_ENSURE_SIZE(s, bytes_follow + 1);
    s->data[s->pos++] = major_type | additional_info;

    for (int i = bytes_follow - 1; i >= 0; --i) {
        s->data[s->pos++] = (val >> (8 * i)) & 0xff;
    }

    return bytes_follow + 1;
}

static size_t decode_int(const cbor_stream_t *s, size_t offset, uint64_t *val)
{
    if (!s) {
        return 0;
    }

    *val = 0; /* clear val first */

    CBOR_ENSURE_SIZE_READ(s, offset + 1);

    unsigned char *in = &s->data[offset];
    unsigned char additional_info = CBOR_ADDITIONAL_INFO(s, offset);
    unsigned char bytes_follow = uint_bytes_follow(additional_info);

    CBOR_ENSURE_SIZE_READ(s, offset + 1 + bytes_follow);

    switch (bytes_follow) {
        case 0:
            *val = (in[0] & CBOR_INFO_MASK);
            break;

        case 1:
            *val = in[1];
            break;

        case 2:
            *val = htons(((cast_align_u8_t *)in)->u.u16);
            break;

        case 4:
            *val = htonl(((cast_align_u8_t *)in)->u.u32);
            break;

        default:
            *val = htonll(((cast_align_u8_t *)in)->u.u64);
            break;
    }

    return bytes_follow + 1;
}

static size_t encode_bytes(unsigned char major_type, cbor_stream_t *s, const char *data,
                           size_t length)
{
    size_t length_field_size = uint_bytes_follow(uint_additional_info(length)) + 1;
    CBOR_ENSURE_SIZE(s, length_field_size + length);

    size_t bytes_start = encode_int(major_type, s, (uint64_t) length);

    if (!bytes_start) {
        return 0;
    }

    memcpy(&(s->data[s->pos]), data, length); /* copy byte string into our cbor struct */
    s->pos += length;
    return (bytes_start + length);
}

static size_t decode_bytes(const cbor_stream_t *s, size_t offset, char *out, size_t length)
{
    CBOR_ENSURE_SIZE_READ(s, offset + 1);

    if ((CBOR_TYPE(s, offset) != CBOR_BYTES && CBOR_TYPE(s, offset) != CBOR_TEXT) || !out) {
        return 0;
    }

    uint64_t bytes_length;
    size_t bytes_start = decode_int(s, offset, &bytes_length);

    if (!bytes_start) {
        return 0;
    }

    if (bytes_length == SIZE_MAX || length < bytes_length + 1) {
        return 0;
    }

    CBOR_ENSURE_SIZE_READ(s, offset + bytes_start + bytes_length);

    memcpy(out, &s->data[offset + bytes_start], bytes_length);
    out[bytes_length] = '\0';
    return (bytes_start + bytes_length);
}

/* A zero copy version of decode_bytes.
   Will not null termiante input, but will tell you the size of what you read.
   Great for reading byte strings which could contain nulls inside of unknown size
   without forced copies.
*/
static size_t decode_bytes_no_copy(const cbor_stream_t *s, size_t offset, unsigned char **out, size_t *length)
{
    CBOR_ENSURE_SIZE_READ(s, offset + 1);

    if ((CBOR_TYPE(s, offset) != CBOR_BYTES && CBOR_TYPE(s, offset) != CBOR_TEXT) || !out) {
        return 0;
    }

    uint64_t bytes_length;
    size_t bytes_start = decode_int(s, offset, &bytes_length);

    if (!bytes_start) {
        return 0;
    }

    CBOR_ENSURE_SIZE_READ(s, offset + bytes_start + bytes_length);
    *out = &(s->data[offset + bytes_start]);
    *length = bytes_length;
    return (bytes_start + bytes_length);
}

size_t cbor_deserialize_int(const cbor_stream_t *stream, size_t offset, int *val)
{
    CBOR_ENSURE_SIZE_READ(stream, offset + 1);

    if ((CBOR_TYPE(stream, offset) != CBOR_UINT && CBOR_TYPE(stream, offset) != CBOR_NEGINT) || !val) {
        return 0;
    }

    uint64_t buf;
    size_t read_bytes = decode_int(stream, offset, &buf);

    if (!read_bytes) {
        return 0;
    }

    if (CBOR_TYPE(stream, offset) == CBOR_UINT) {
        *val = buf; /* resolve as CBOR_UINT */
    }
    else {
        *val = -1 - buf; /* resolve as CBOR_NEGINT */
    }

    return read_bytes;
}

size_t cbor_serialize_int(cbor_stream_t *s, int val)
{
    if (val >= 0) {
        /* Major type 0: an unsigned integer */
        return encode_int(CBOR_UINT, s, val);
    }
    else {
        /* Major type 1: an negative integer */
        return encode_int(CBOR_NEGINT, s, -1 - val);
    }
}

size_t cbor_deserialize_uint64_t(const cbor_stream_t *stream, size_t offset, uint64_t *val)
{
    if (CBOR_TYPE(stream, offset) != CBOR_UINT || !val) {
        return 0;
    }

    return decode_int(stream, offset, val);
}

size_t cbor_serialize_uint64_t(cbor_stream_t *s, uint64_t val)
{
    return encode_int(CBOR_UINT, s, val);
}

size_t cbor_deserialize_int64_t(const cbor_stream_t *stream, size_t offset, int64_t *val)
{
    if ((CBOR_TYPE(stream, offset) != CBOR_UINT && CBOR_TYPE(stream, offset) != CBOR_NEGINT) || !val) {
        return 0;
    }

    uint64_t buf;
    size_t read_bytes = decode_int(stream, offset, &buf);

    if (CBOR_TYPE(stream, offset) == CBOR_UINT) {
        *val = buf; /* resolve as CBOR_UINT */
    }
    else {
        *val = -1 - buf; /* resolve as CBOR_NEGINT */
    }

    return read_bytes;
}

size_t cbor_serialize_int64_t(cbor_stream_t *s, int64_t val)
{
    if (val >= 0) {
        /* Major type 0: an unsigned integer */
        return encode_int(CBOR_UINT, s, val);
    }
    else {
        /* Major type 1: an negative integer */
        return encode_int(CBOR_NEGINT, s, -1 - val);
    }
}

size_t cbor_deserialize_bool(const cbor_stream_t *stream, size_t offset, bool *val)
{
    if (CBOR_TYPE(stream, offset) != CBOR_7 || !val) {
        return 0;
    }

    unsigned char byte = stream->data[offset];
    *val = (byte == CBOR_TRUE);
    return 1;
}

size_t cbor_serialize_bool(cbor_stream_t *s, bool val)
{
    CBOR_ENSURE_SIZE(s, 1);
    s->data[s->pos++] = val ? CBOR_TRUE : CBOR_FALSE;
    return 1;
}

#ifdef MODULE_CBOR_FLOAT
size_t cbor_deserialize_float_half(const cbor_stream_t *stream, size_t offset, float *val)
{
    if (CBOR_TYPE(stream, offset) != CBOR_7 || !val) {
        return 0;
    }

    unsigned char *data = &stream->data[offset];

    if (*data == CBOR_FLOAT16) {
        *val = (float)decode_float_half(data + 1);
        return 3;
    }

    return 0;
}

size_t cbor_serialize_float_half(cbor_stream_t *s, float val)
{
    CBOR_ENSURE_SIZE(s, 3);
    s->data[s->pos++] = CBOR_FLOAT16;
    uint16_t encoded_val = htons(encode_float_half(val));
    memcpy(s->data + s->pos, &encoded_val, 2);
    s->pos += 2;
    return 3;
}

size_t cbor_deserialize_float(const cbor_stream_t *stream, size_t offset, float *val)
{
    if (CBOR_TYPE(stream, offset) != CBOR_7 || !val) {
        return 0;
    }

    unsigned char *data = &stream->data[offset];

    if (*data == CBOR_FLOAT32) {
        *val = ntohf(((cast_align_u8_t *)data)->u.u32);
        return 5;
    }

    return 0;
}

size_t cbor_serialize_float(cbor_stream_t *s, float val)
{
    CBOR_ENSURE_SIZE(s, 5);
    s->data[s->pos++] = CBOR_FLOAT32;
    uint32_t encoded_val = htonf(val);
    memcpy(s->data + s->pos, &encoded_val, 4);
    s->pos += 4;
    return 5;
}

size_t cbor_deserialize_double(const cbor_stream_t *stream, size_t offset, double *val)
{
    CBOR_ENSURE_SIZE_READ(stream, offset + 1);

    if (CBOR_TYPE(stream, offset) != CBOR_7 || !val) {
        return 0;
    }

    unsigned char *data = &stream->data[offset];

    if (*data == CBOR_FLOAT64) {
        CBOR_ENSURE_SIZE_READ(stream, offset + 9);
        *val = ntohd(((cast_align_u8_t *)data)->u.u64);
        return 9;
    }

    return 0;
}

size_t cbor_serialize_double(cbor_stream_t *s, double val)
{
    CBOR_ENSURE_SIZE(s, 9);
    s->data[s->pos++] = CBOR_FLOAT64;
    uint64_t encoded_val = htond(val);
    memcpy(s->data + s->pos, &encoded_val, 8);
    s->pos += 8;
    return 9;
}
#endif /* MODULE_CBOR_FLOAT */

size_t cbor_deserialize_byte_string(const cbor_stream_t *stream, size_t offset, char *val,
                                    size_t length)
{
    CBOR_ENSURE_SIZE_READ(stream, offset + 1);

    if (CBOR_TYPE(stream, offset) != CBOR_BYTES) {
        return 0;
    }

    return decode_bytes(stream, offset, val, length);
}

size_t cbor_deserialize_byte_string_no_copy(const cbor_stream_t *stream, size_t offset, unsigned char **val,
                                    size_t *length)
{
    CBOR_ENSURE_SIZE_READ(stream, offset + 1);

    if (CBOR_TYPE(stream, offset) != CBOR_BYTES) {
        return 0;
    }

    return decode_bytes_no_copy(stream, offset, val, length);
}

size_t cbor_serialize_byte_string(cbor_stream_t *stream, const char *val)
{
    return encode_bytes(CBOR_BYTES, stream, val, strlen(val));
}

size_t cbor_serialize_byte_stringl(cbor_stream_t *stream, const char *val, size_t length)
{
    return encode_bytes(CBOR_BYTES, stream, val, length);
}

size_t cbor_deserialize_unicode_string(const cbor_stream_t *stream, size_t offset, char *val,
                                       size_t length)
{
    CBOR_ENSURE_SIZE_READ(stream, offset + 1);

    if (CBOR_TYPE(stream, offset) != CBOR_TEXT) {
        return 0;
    }

    return decode_bytes(stream, offset, val, length);
}

size_t cbor_deserialize_unicode_string_no_copy(const cbor_stream_t *stream, size_t offset, unsigned char **val,
                                    size_t *length)
{
    CBOR_ENSURE_SIZE_READ(stream, offset + 1);

    if (CBOR_TYPE(stream, offset) != CBOR_TEXT) {
        return 0;
    }

    return decode_bytes_no_copy(stream, offset, val, length);
}

size_t cbor_serialize_unicode_string(cbor_stream_t *stream, const char *val)
{
    return encode_bytes(CBOR_TEXT, stream, val, strlen(val));
}

size_t cbor_deserialize_array(const cbor_stream_t *s, size_t offset, size_t *array_length)
{
    if (CBOR_TYPE(s, offset) != CBOR_ARRAY || !array_length) {
        return 0;
    }

    uint64_t val;
    size_t read_bytes = decode_int(s, offset, &val);
    *array_length = (size_t)val;
    return read_bytes;
}

size_t cbor_serialize_array(cbor_stream_t *s, size_t array_length)
{
    /* serialize number of array items */
    return encode_int(CBOR_ARRAY, s, array_length);
}

size_t cbor_serialize_array_indefinite(cbor_stream_t *s)
{
    CBOR_ENSURE_SIZE(s, 1);
    s->data[s->pos++] = CBOR_ARRAY | CBOR_VAR_FOLLOWS;
    return 1;

}

size_t cbor_deserialize_array_indefinite(const cbor_stream_t *s, size_t offset)
{
    if (s->data[offset] != (CBOR_ARRAY | CBOR_VAR_FOLLOWS)) {
        return 0;
    }

    return 1;
}

size_t cbor_serialize_map_indefinite(cbor_stream_t *s)
{
    CBOR_ENSURE_SIZE(s, 1);
    s->data[s->pos++] = CBOR_MAP | CBOR_VAR_FOLLOWS;
    return 1;
}

size_t cbor_deserialize_map_indefinite(const cbor_stream_t *s, size_t offset)
{
    if (s->data[offset] != (CBOR_MAP | CBOR_VAR_FOLLOWS)) {
        return 0;
    }

    return 1;
}

size_t cbor_deserialize_map(const cbor_stream_t *s, size_t offset, size_t *map_length)
{
    if (CBOR_TYPE(s, offset) != CBOR_MAP || !map_length) {
        return 0;
    }

    uint64_t val;
    size_t read_bytes = decode_int(s, offset, &val);
    *map_length = (size_t)val;
    return read_bytes;
}

size_t cbor_serialize_map(cbor_stream_t *s, size_t map_length)
{
    /* serialize number of item key-value pairs */
    return encode_int(CBOR_MAP, s, map_length);
}

#ifdef MODULE_CBOR_SEMANTIC_TAGGING
#ifdef MODULE_CBOR_CTIME
size_t cbor_deserialize_date_time(const cbor_stream_t *stream, size_t offset, struct tm *val)
{
    if ((CBOR_TYPE(stream, offset) != CBOR_TAG)
        || (CBOR_ADDITIONAL_INFO(stream, offset) != CBOR_DATETIME_STRING_FOLLOWS)) {
        return 0;
    }

    char buffer[21];
    offset++;  /* skip tag byte to decode date_time */
    size_t read_bytes = cbor_deserialize_unicode_string(stream, offset, buffer, sizeof(buffer));

    if (strptime(buffer, TIMESTRING_FORMAT, val) == 0) {
        return 0;
    }

    val->tm_isdst = -1; /* not set by strptime(), undefined in CBOR */

    if (mktime(val) == -1) {
        return 0;
    }

    return read_bytes + 1;  /* + 1 tag byte */
}

size_t cbor_serialize_date_time(cbor_stream_t *stream, struct tm *val)
{
    CBOR_ENSURE_SIZE(stream, MAX_TIMESTRING_LENGTH + 1); /* + 1 tag byte */

    char time_str[MAX_TIMESTRING_LENGTH];

    if (strftime(time_str, sizeof(time_str), TIMESTRING_FORMAT, val) == 0) { /* struct tm to string */
        return 0;
    }

    if (!cbor_write_tag(stream, CBOR_DATETIME_STRING_FOLLOWS)) {
        return 0;
    }

    size_t written_bytes = cbor_serialize_unicode_string(stream, time_str);
    return written_bytes + 1; /* utf8 time string length + tag length */
}

size_t cbor_deserialize_date_time_epoch(const cbor_stream_t *stream, size_t offset, time_t *val)
{
    if ((CBOR_TYPE(stream, offset) != CBOR_TAG)
        || (CBOR_ADDITIONAL_INFO(stream, offset) != CBOR_DATETIME_EPOCH_FOLLOWS)) {
        return 0;
    }

    offset++; /* skip tag byte */
    uint64_t epoch;
    size_t read_bytes = cbor_deserialize_uint64_t(stream, offset, &epoch);

    if (!read_bytes) {
        return 0;
    }

    *val = (time_t)epoch;
    return read_bytes + 1; /* + 1 tag byte */
}

size_t cbor_serialize_date_time_epoch(cbor_stream_t *stream, time_t val)
{
    /* we need at least 2 bytes (tag byte + at least 1 byte for the integer) */
    CBOR_ENSURE_SIZE(stream, 2);

    if (val < 0) {
        return 0; /* we currently don't support negative values for the time_t object */
    }

    if (!cbor_write_tag(stream, CBOR_DATETIME_EPOCH_FOLLOWS)) {
        return 0;
    }


    uint64_t time = (uint64_t)val;
    size_t written_bytes = encode_int(CBOR_UINT, stream, time);
    return written_bytes + 1; /* + 1 tag byte */
}
#endif /* MODULE_CBOR_CTIME */


size_t cbor_write_tag(cbor_stream_t *s, unsigned char tag)
{
    CBOR_ENSURE_SIZE(s, 1);
    s->data[s->pos++] = CBOR_TAG | tag;
    return 1;
}

bool cbor_at_tag(const cbor_stream_t *s, size_t offset)
{
    return cbor_at_end(s, offset) || CBOR_TYPE(s, offset) == CBOR_TAG;
}
#endif /* MODULE_CBOR_SEMANTIC_TAGGING */

size_t cbor_write_break(cbor_stream_t *s)
{
    CBOR_ENSURE_SIZE(s, 1);
    s->data[s->pos++] = CBOR_BREAK;
    return 1;
}

bool cbor_at_break(const cbor_stream_t *s, size_t offset)
{
    return cbor_at_end(s, offset) || s->data[offset] == CBOR_BREAK;
}

bool cbor_at_end(const cbor_stream_t *s, size_t offset)
{
    /* cbor_stream_t::pos points at the next *free* byte, hence the -1 */
    return s ? offset >= s->pos - 1 : true;
}

/* BEGIN: Printers */
void cbor_stream_print(const cbor_stream_t *stream)
{
    dump_memory(stream->data, stream->pos);
}

/**
 * Skip byte(s) at offset @p offset in stream @p stream
 *
 * This function can be used as fallback, in case we cannot deserialize the
 * current byte
 */
static size_t cbor_stream_decode_skip(cbor_stream_t *stream, size_t offset)
{
    size_t consume_bytes = 0;

    switch (CBOR_ADDITIONAL_INFO(stream, offset)) {
        case CBOR_BYTE_FOLLOWS:
            consume_bytes = 2;
            break;

        default:
            consume_bytes = 1;
            break;
    }

    printf("(unsupported, ");
    dump_memory(stream->data + offset, consume_bytes);
    puts(")");
    return consume_bytes;
}

/**
 * Decode CBOR data item from @p stream at position @p offset
 *
 * @return Amount of bytes consumed
 */
static size_t cbor_stream_decode_at(cbor_stream_t *stream, size_t offset, int indent)
{
#define DESERIALIZE_AND_PRINT(type, suffix, format_string) { \
        type val; \
        size_t read_bytes = cbor_deserialize_##suffix(stream, offset, &val); \
        printf("("#type", "format_string")\n", val); \
        return read_bytes; \
    }

    printf("%*s", indent, "");

    switch (CBOR_TYPE(stream, offset)) {
        case CBOR_UINT:
            DESERIALIZE_AND_PRINT(uint64_t, uint64_t, "%" PRIu64)
        case CBOR_NEGINT:
            DESERIALIZE_AND_PRINT(int64_t, int64_t, "%" PRId64)
        case CBOR_BYTES: {
            char buffer[CBOR_STREAM_PRINT_BUFFERSIZE];
            size_t read_bytes = cbor_deserialize_byte_string(stream, offset, buffer, sizeof(buffer));
            printf("(byte string, \"%s\")\n", buffer);
            return read_bytes;
        }

        case CBOR_TEXT: {
            char buffer[CBOR_STREAM_PRINT_BUFFERSIZE];
            size_t read_bytes = cbor_deserialize_unicode_string(stream, offset, buffer, sizeof(buffer));
            printf("(unicode string, \"%s\")\n", buffer);
            return read_bytes;
        }

        case CBOR_ARRAY: {
            const bool is_indefinite = (stream->data[offset] == (CBOR_ARRAY | CBOR_VAR_FOLLOWS));
            uint64_t array_length = 0;
            size_t read_bytes;

            if (is_indefinite) {
                offset += read_bytes = cbor_deserialize_array_indefinite(stream, offset);
                puts("(array, length: [indefinite])");
            }
            else {
                offset += read_bytes = decode_int(stream, offset, &array_length);
                printf("(array, length: %"PRIu64")\n", array_length);
            }

            size_t i = 0;

            while (is_indefinite ? !cbor_at_break(stream, offset) : i < array_length) {
                size_t inner_read_bytes;
                offset += inner_read_bytes = cbor_stream_decode_at(stream, offset, indent + 2);

                if (inner_read_bytes == 0) {
                    DEBUG("Failed to read array item at position %u\n", (unsigned)i);
                    break;
                }

                read_bytes += inner_read_bytes;
                ++i;
            }

            read_bytes += cbor_at_break(stream, offset);
            return read_bytes;
        }

        case CBOR_MAP: {
            const bool is_indefinite = (stream->data[offset] == (CBOR_MAP | CBOR_VAR_FOLLOWS));
            uint64_t map_length = 0;
            size_t read_bytes;

            if (is_indefinite) {
                offset += read_bytes = cbor_deserialize_map_indefinite(stream, offset);
                puts("(map, length: [indefinite])");
            }
            else {
                offset += read_bytes = decode_int(stream, offset, &map_length);
                printf("(map, length: %"PRIu64")\n", map_length);
            }

            size_t i = 0;

            while (is_indefinite ? !cbor_at_break(stream, offset) : i < map_length) {
                size_t key_read_bytes, value_read_bytes;
                offset += key_read_bytes = cbor_stream_decode_at(stream, offset, indent + 1); /* key */
                offset += value_read_bytes = cbor_stream_decode_at(stream, offset, indent + 2); /* value */

                if (key_read_bytes == 0 || value_read_bytes == 0) {
                    DEBUG("Failed to read key-value pair at position %u\n", (unsigned)i);
                    break;
                }

                read_bytes += key_read_bytes + value_read_bytes;
                ++i;
            }

            read_bytes += cbor_at_break(stream, offset);
            return read_bytes;
        }
#ifdef MODULE_CBOR_SEMANTIC_TAGGING
        case CBOR_TAG: {
            unsigned char tag = CBOR_ADDITIONAL_INFO(stream, offset);

            switch (tag) {
                    /* Non-native builds likely don't have support for ctime (hence disable it there)
                     * TODO: Better check for availability of ctime functions? */
#ifdef MODULE_CBOR_CTIME
                case CBOR_DATETIME_STRING_FOLLOWS: {
                    char buf[64];
                    struct tm timeinfo;
                    size_t read_bytes = cbor_deserialize_date_time(stream, offset, &timeinfo);
                    strftime(buf, sizeof(buf), TIMESTRING_FORMAT, &timeinfo);
                    printf("(tag: %u, date/time string: \"%s\")\n", tag, buf);
                    return read_bytes;
                }

                case CBOR_DATETIME_EPOCH_FOLLOWS: {
                    time_t time;
                    size_t read_bytes = cbor_deserialize_date_time_epoch(stream, offset, &time);
                    printf("(tag: %u, date/time epoch: %d)\n", tag, (int)time);
                    return read_bytes;
                }

#endif /* MODULE_CBOR_CTIME */

                default:
                    break;
            }
            break;
        }
#endif /* MODULE_CBOR_SEMANTIC_TAGGING */

        case CBOR_7: {
            switch (stream->data[offset]) {
                case CBOR_FALSE:
                case CBOR_TRUE:
                    DESERIALIZE_AND_PRINT(bool, bool, "%d")
#ifdef MODULE_CBOR_FLOAT
                case CBOR_FLOAT16:
                    DESERIALIZE_AND_PRINT(float, float_half, "%f")
                case CBOR_FLOAT32:
                    DESERIALIZE_AND_PRINT(float, float, "%f")
                case CBOR_FLOAT64:
                    DESERIALIZE_AND_PRINT(double, double, "%lf")
#endif /* MODULE_CBOR_FLOAT */
                default:
                    break;
            }
        }
    }

    /* if we end up here, we weren't able to parse the current byte
     * let's just skip this (and the next one as well if required) */
    return cbor_stream_decode_skip(stream, offset);

#undef DESERIALIZE_AND_PRINT
}

void cbor_stream_decode(cbor_stream_t *stream)
{
    puts("Data:");
    size_t offset = 0;

    while (offset < stream->pos) {
        size_t read_bytes = cbor_stream_decode_at(stream, offset, 0);

        if (read_bytes == 0) {
            DEBUG("Failed to read from stream at offset %u, start byte 0x%02X\n", (unsigned)offset, stream->data[offset]);
            cbor_stream_print(stream);
            return;
        }

        offset += read_bytes;
    }

    puts("");
}
/* END: Printers */
