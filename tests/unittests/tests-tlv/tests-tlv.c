/*
 * SPDX-FileCopyrightText: 2026 Carl Seifert <carl.seifert@tu-dresden.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "tests-tlv.h"

#include <string.h>
#include "errno.h"
#include "tlv.h"

static uint8_t long_buf[640];
static uint8_t long_ref[300];
static uint8_t long_copy[640];

static void test_tlv_decode_short(void)
{
    const uint8_t raw[] = { 0x01, 0x03, 'a', 'b', 'c' };
    uint8_t tag = 0;
    size_t length = 0;
    const uint8_t *value = NULL;

    ssize_t decoded = tlv_decode(raw, sizeof(raw), &tag, &length, &value);

    TEST_ASSERT_EQUAL_INT(sizeof(raw), decoded);
    TEST_ASSERT_EQUAL_INT(0x01, tag);
    TEST_ASSERT_EQUAL_INT(3, length);
    TEST_ASSERT(value == &raw[2]);
}

static void test_tlv_decode_sequence(void)
{
    /* tag 1 "abc", tag 2 (empty), tag 3 "z" */
    const uint8_t raw[] = { 0x01, 0x03, 'a', 'b', 'c',
                            0x02, 0x00,
                            0x03, 0x01, 'z' };
    uint8_t tag = 0;
    size_t length = 0;
    const uint8_t *value = NULL;
    ssize_t decoded;

    decoded = tlv_decode(raw, sizeof(raw), &tag, &length, &value);
    TEST_ASSERT_EQUAL_INT(5, decoded);
    TEST_ASSERT_EQUAL_INT(0x01, tag);
    TEST_ASSERT_EQUAL_INT(3, length);
    TEST_ASSERT(value == &raw[2]);

    decoded = tlv_decode(&raw[5], 5, &tag, &length, &value);
    TEST_ASSERT_EQUAL_INT(2, decoded);
    TEST_ASSERT_EQUAL_INT(0x02, tag);
    TEST_ASSERT_EQUAL_INT(0, length);

    decoded = tlv_decode(&raw[7], 3, &tag, &length, &value);
    TEST_ASSERT_EQUAL_INT(3, decoded); /* last element: nothing remaining */
    TEST_ASSERT_EQUAL_INT(0x03, tag);
    TEST_ASSERT_EQUAL_INT(1, length);
    TEST_ASSERT(value == &raw[9]);
}

static void test_tlv_decode_long_form(void)
{
    uint8_t tag = 0;
    size_t length = 0;
    const uint8_t *value = NULL;
    ssize_t decoded;

    /* tag 9 with 300 (0x012C) bytes of 0xA5, then tag 7 with "x" */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x09;
    long_buf[1] = 0xFF;
    long_buf[2] = 0x01;
    long_buf[3] = 0x2C;
    memset(&long_buf[4], 0xA5, 300);
    long_buf[304] = 0x07;
    long_buf[305] = 0x01;
    long_buf[306] = 'x';

    decoded = tlv_decode(long_buf, 307, &tag, &length, &value);
    TEST_ASSERT_EQUAL_INT(304, decoded);
    TEST_ASSERT_EQUAL_INT(0x09, tag);
    TEST_ASSERT_EQUAL_INT(300, length);
    TEST_ASSERT(value == &long_buf[4]);

    decoded = tlv_decode(&long_buf[304], 3, &tag, &length, &value);
    TEST_ASSERT_EQUAL_INT(3, decoded);
    TEST_ASSERT_EQUAL_INT(0x07, tag);
    TEST_ASSERT_EQUAL_INT(1, length);
    TEST_ASSERT(value == &long_buf[306]);
}

static void test_tlv_decode_longest_short(void)
{
    uint8_t tag = 0;
    size_t length = 0;
    const uint8_t *value = NULL;

    /* 254 (0xFE) is the longest lengthgth that fits the single lengthgth byte */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x01;
    long_buf[1] = 0xFE;
    memset(&long_buf[2], 'a', 254);

    ssize_t decoded = tlv_decode(long_buf, 256, &tag, &length, &value);

    TEST_ASSERT_EQUAL_INT(256, decoded);
    TEST_ASSERT_EQUAL_INT(0x01, tag);
    TEST_ASSERT_EQUAL_INT(254, length);
    TEST_ASSERT(value == &long_buf[2]);
}

static void test_tlv_decode_shortest_long(void)
{
    uint8_t tag = 0;
    size_t length = 0;
    const uint8_t *value = NULL;

    /* 255 (0xFF) is the shortest lengthgth that needs the long form */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x02;
    long_buf[1] = 0xFF;
    long_buf[2] = 0x00;
    long_buf[3] = 0xFF;
    memset(&long_buf[4], 'b', 255);

    ssize_t decoded = tlv_decode(long_buf, 259, &tag, &length, &value);

    TEST_ASSERT_EQUAL_INT(259, decoded);
    TEST_ASSERT_EQUAL_INT(0x02, tag);
    TEST_ASSERT_EQUAL_INT(255, length);
    TEST_ASSERT(value == &long_buf[4]);
}

static void test_tlv_decode_too_short(void)
{
    const uint8_t raw[] = { 0x01 };
    uint8_t tag = 0;
    size_t length = 0;
    const uint8_t *value = NULL;

    /* nothing at all */
    TEST_ASSERT_EQUAL_INT(-EBADMSG, tlv_decode(raw, 0, &tag, &length, &value));

    /* a tag without a lengthgth */
    TEST_ASSERT_EQUAL_INT(-EBADMSG, tlv_decode(raw, sizeof(raw), &tag, &length, &value));
}

static void test_tlv_decode_truncated(void)
{
    const uint8_t short_value[] = { 0x01, 0x05, 'a' };        /* wants 5, has 1 */
    const uint8_t long_marker[] = { 0x01, 0xFF };             /* no lengthgth */
    const uint8_t long_hdr[] = { 0x01, 0xFF, 0x01 };          /* half a lengthgth */
    const uint8_t long_nodata[] = { 0x01, 0xFF, 0x01, 0x00 }; /* 256, has 0 */
    uint8_t tag = 0;
    size_t length = 0;
    const uint8_t *value = NULL;

    TEST_ASSERT_EQUAL_INT(-EBADMSG, tlv_decode(short_value, sizeof(short_value), &tag, &length, &value));

    TEST_ASSERT_EQUAL_INT(-EBADMSG, tlv_decode(long_marker, sizeof(long_marker), &tag, &length, &value));

    TEST_ASSERT_EQUAL_INT(-EBADMSG, tlv_decode(long_hdr, sizeof(long_hdr), &tag, &length, &value));

    TEST_ASSERT_EQUAL_INT(-EBADMSG, tlv_decode(long_nodata, sizeof(long_nodata), &tag, &length, &value));

    /* long form, 300 bytes announced, 299 present */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x01;
    long_buf[1] = 0xFF;
    long_buf[2] = 0x01;
    long_buf[3] = 0x2C;
    memset(&long_buf[4], 0x42, 299);
    TEST_ASSERT_EQUAL_INT(-EBADMSG, tlv_decode(long_buf, 303, &tag, &length, &value));

    /* short form, 254 bytes announced, 253 present */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x01;
    long_buf[1] = 0xFE;
    memset(&long_buf[2], 0x42, 253);
    TEST_ASSERT_EQUAL_INT(-EBADMSG, tlv_decode(long_buf, 255, &tag, &length, &value));
}

static void test_tlv_decode_noncanonical(void)
{
    /* the long form must not be used for lengthgths below 0xFF */
    const uint8_t length5[] = { 0x01, 0xFF, 0x00, 0x05, 'a', 'b', 'c', 'd', 'e' };
    const uint8_t length0[] = { 0x01, 0xFF, 0x00, 0x00 };
    uint8_t tag = 0;
    size_t length = 0;
    const uint8_t *value = NULL;

    TEST_ASSERT_EQUAL_INT(-EBADMSG, tlv_decode(length5, sizeof(length5), &tag, &length, &value));

    TEST_ASSERT_EQUAL_INT(-EBADMSG, tlv_decode(length0, sizeof(length0), &tag, &length, &value));

    /* 254 = 0x00FE in long form, with all 254 bytes present */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x01;
    long_buf[1] = 0xFF;
    long_buf[2] = 0x00;
    long_buf[3] = 0xFE;
    memset(&long_buf[4], 0x42, 254);
    TEST_ASSERT_EQUAL_INT(-EBADMSG, tlv_decode(long_buf, 258, &tag, &length, &value));
}

static void test_tlv_decode_error_untouched(void)
{
    const uint8_t short_value[] = { 0x01, 0x05, 'a' };
    const uint8_t long_hdr[] = { 0x01, 0xFF, 0x01 };
    const uint8_t long_nodata[] = { 0x01, 0xFF, 0x01, 0x00 };
    const uint8_t noncanon[] = { 0x01, 0xFF, 0x00, 0x05, 'a', 'b', 'c', 'd', 'e' };
    uint8_t tag;
    size_t length;
    const uint8_t *value;

    /* on -EBADMSG all outputs keep their old values */
    tag = 0x55;
    length = 0x1234;
    value = NULL;
    TEST_ASSERT_EQUAL_INT(-EBADMSG, tlv_decode(short_value, sizeof(short_value), &tag, &length, &value));
    TEST_ASSERT_EQUAL_INT(0x55, tag);
    TEST_ASSERT_EQUAL_INT(0x1234, length);
    TEST_ASSERT(value == NULL);

    tag = 0x55;
    length = 0x1234;
    value = NULL;
    TEST_ASSERT_EQUAL_INT(-EBADMSG, tlv_decode(long_hdr, sizeof(long_hdr), &tag, &length, &value));
    TEST_ASSERT_EQUAL_INT(0x55, tag);
    TEST_ASSERT_EQUAL_INT(0x1234, length);
    TEST_ASSERT(value == NULL);

    tag = 0x55;
    length = 0x1234;
    value = NULL;
    TEST_ASSERT_EQUAL_INT(-EBADMSG, tlv_decode(long_nodata, sizeof(long_nodata), &tag, &length, &value));
    TEST_ASSERT_EQUAL_INT(0x55, tag);
    TEST_ASSERT_EQUAL_INT(0x1234, length);
    TEST_ASSERT(value == NULL);

    tag = 0x55;
    length = 0x1234;
    value = NULL;
    TEST_ASSERT_EQUAL_INT(-EBADMSG, tlv_decode(noncanon, sizeof(noncanon), &tag, &length, &value));
    TEST_ASSERT_EQUAL_INT(0x55, tag);
    TEST_ASSERT_EQUAL_INT(0x1234, length);
    TEST_ASSERT(value == NULL);
}

static void test_tlv_find_hit(void)
{
    /* tag 1 "abc", tag 2 (empty), tag 3 "z", tag 1 "xy" */
    const uint8_t raw[] = { 0x01, 0x03, 'a', 'b', 'c',
                            0x02, 0x00,
                            0x03, 0x01, 'z',
                            0x01, 0x02, 'x', 'y' };
    const uint8_t *value = NULL;
    ssize_t r;

    /* tag 1 occurs twice: the first one is found */
    r = tlv_find(raw, sizeof(raw), 0x01, &value);
    TEST_ASSERT_EQUAL_INT(3, r);
    TEST_ASSERT(value == &raw[2]);

    /* an element in the middle */
    value = NULL;
    r = tlv_find(raw, sizeof(raw), 0x03, &value);
    TEST_ASSERT_EQUAL_INT(1, r);
    TEST_ASSERT(value == &raw[9]);

    /* an empty value is a hit, not an error */
    r = tlv_find(raw, sizeof(raw), 0x02, &value);
    TEST_ASSERT_EQUAL_INT(0, r);
}

static void test_tlv_find_last_element(void)
{
    /* the match is the very last element of the buffer */
    const uint8_t raw[] = { 0x01, 0x01, 'a',
                            0x04, 0x02, 'x', 'y' };
    const uint8_t *value = NULL;

    ssize_t r = tlv_find(raw, sizeof(raw), 0x04, &value);

    TEST_ASSERT_EQUAL_INT(2, r);
    TEST_ASSERT(value == &raw[5]);
}

static void test_tlv_find_missing(void)
{
    const uint8_t raw[] = { 0x01, 0x03, 'a', 'b', 'c',
                            0x02, 0x00,
                            0x03, 0x01, 'z',
                            0x01, 0x02, 'x', 'y' };
    const uint8_t *value = NULL;

    TEST_ASSERT_EQUAL_INT(-ENOENT, tlv_find(raw, sizeof(raw), 0x09, &value));
    TEST_ASSERT_EQUAL_INT(-ENOENT, tlv_find(raw, sizeof(raw), 0x00, &value));

    /* only the first element (5 bytes) is in range */
    TEST_ASSERT_EQUAL_INT(3, tlv_find(raw, 5, 0x01, &value));
    TEST_ASSERT_EQUAL_INT(-ENOENT, tlv_find(raw, 5, 0x02, &value));

    /* only the first two elements (7 bytes) are in range */
    TEST_ASSERT_EQUAL_INT(-ENOENT, tlv_find(raw, 7, 0x03, &value));
}

static void test_tlv_find_empty(void)
{
    const uint8_t raw[] = { 0x01, 0x01, 'a' };
    const uint8_t *value = NULL;

    TEST_ASSERT_EQUAL_INT(-ENOENT, tlv_find(raw, 0, 0x00, &value));
    TEST_ASSERT_EQUAL_INT(-ENOENT, tlv_find(raw, 0, 0x01, &value));
}

static void test_tlv_find_long_value(void)
{
    const uint8_t *value = NULL;
    ssize_t r;

    /* tag 5, 300 bytes (long form) */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x05;
    long_buf[1] = 0xFF;
    long_buf[2] = 0x01;
    long_buf[3] = 0x2C;
    memset(&long_buf[4], 'x', 300);
    r = tlv_find(long_buf, 304, 0x05, &value);
    TEST_ASSERT_EQUAL_INT(300, r);
    TEST_ASSERT(value == &long_buf[4]);
    TEST_ASSERT_EQUAL_INT(-ENOENT, tlv_find(long_buf, 304, 0x06, &value));

    /* tag 5, 255 bytes (shortest long form) */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x05;
    long_buf[1] = 0xFF;
    long_buf[2] = 0x00;
    long_buf[3] = 0xFF;
    memset(&long_buf[4], 'x', 255);
    value = NULL;
    r = tlv_find(long_buf, 259, 0x05, &value);
    TEST_ASSERT_EQUAL_INT(255, r);
    TEST_ASSERT(value == &long_buf[4]);

    /* tag 5, 254 bytes (longest short form) */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x05;
    long_buf[1] = 0xFE;
    memset(&long_buf[2], 'x', 254);
    value = NULL;
    r = tlv_find(long_buf, 256, 0x05, &value);
    TEST_ASSERT_EQUAL_INT(254, r);
    TEST_ASSERT(value == &long_buf[2]);
}

static void test_tlv_find_after_long_element(void)
{
    const uint8_t *value = NULL;

    /* tag 5 with 300 bytes, then tag 6 with "z": must skip the long element */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x05;
    long_buf[1] = 0xFF;
    long_buf[2] = 0x01;
    long_buf[3] = 0x2C;
    memset(&long_buf[4], 'x', 300);
    long_buf[304] = 0x06;
    long_buf[305] = 0x01;
    long_buf[306] = 'z';

    ssize_t r = tlv_find(long_buf, 307, 0x06, &value);

    TEST_ASSERT_EQUAL_INT(1, r);
    TEST_ASSERT(value == &long_buf[306]);
}

static void test_tlv_find_malformed(void)
{
    const uint8_t trailing[] = { 0x01, 0x01, 'a', 0x02 };     /* element + lone byte */
    const uint8_t truncated[] = { 0x01, 0x05, 'a' };
    const uint8_t noncanon[] = { 0x01, 0xFF, 0x00, 0x05, 'a', 'b', 'c', 'd', 'e' };
    const uint8_t *value = NULL;

    /* a match before the damage is still found */
    TEST_ASSERT_EQUAL_INT(1, tlv_find(trailing, sizeof(trailing), 0x01, &value));
    /* damage reached before a match */
    TEST_ASSERT_EQUAL_INT(-EBADMSG, tlv_find(trailing, sizeof(trailing), 0x02, &value));
    TEST_ASSERT_EQUAL_INT(-EBADMSG, tlv_find(trailing, sizeof(trailing), 0x09, &value));

    TEST_ASSERT_EQUAL_INT(-EBADMSG, tlv_find(truncated, sizeof(truncated), 0x01, &value));
    TEST_ASSERT_EQUAL_INT(-EBADMSG, tlv_find(truncated, sizeof(truncated), 0x09, &value));

    TEST_ASSERT_EQUAL_INT(-EBADMSG, tlv_find(noncanon, sizeof(noncanon), 0x01, &value));
}

/* ------------------------------------------------------------------------ */
/* tlv_encode                                                                */
/* ------------------------------------------------------------------------ */

static void test_tlv_encode_short(void)
{
    uint8_t buf[8] = { 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE };
    const uint8_t expected[5] = { 0x01, 0x03, 'a', 'b', 'c' };

    /* capacity is exactly the encoded size */
    ssize_t r = tlv_encode(buf, sizeof(expected), 0x01, 3, (const uint8_t *)"abc");

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(expected)]);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(buf) - 1]);
}

static void test_tlv_encode_zero_lengthgth(void)
{
    uint8_t buf[4] = { 0xEE, 0xEE, 0xEE, 0xEE };

    ssize_t r = tlv_encode(buf, 2, 0x03, 0, NULL);

    TEST_ASSERT_EQUAL_INT(2, r);
    TEST_ASSERT_EQUAL_INT(0x03, buf[0]);
    TEST_ASSERT_EQUAL_INT(0x00, buf[1]);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[2]);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(buf) - 1]);
}

static void test_tlv_encode_sequence(void)
{
    uint8_t buf[16] = { 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE,
                        0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE };
    const uint8_t expected[10] = { 0x01, 0x03, 'a', 'b', 'c',
                                   0x02, 0x00,
                                   0x03, 0x01, 'z' };
    ssize_t r;

    r = tlv_encode(buf, sizeof(buf), 0x01, 3, (const uint8_t *)"abc");
    TEST_ASSERT_EQUAL_INT(5, r);

    r = tlv_encode(&buf[5], 11, 0x02, 0, NULL);
    TEST_ASSERT_EQUAL_INT(2, r);

    r = tlv_encode(&buf[7], 9, 0x03, 1, (const uint8_t *)"z");
    TEST_ASSERT_EQUAL_INT(3, r);

    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(expected)]);
}

static void test_tlv_encode_longest_short(void)
{

    memset(long_buf, 0xEE, sizeof(long_buf));
    memset(long_ref, 'a', 254);

    /* 254 bytes: single lengthgth byte 0xFE, exact fit */
    ssize_t r = tlv_encode(long_buf, 256, 0x01, 254, long_ref);

    TEST_ASSERT_EQUAL_INT(256, r);
    TEST_ASSERT_EQUAL_INT(0x01, long_buf[0]);
    TEST_ASSERT_EQUAL_INT(0xFE, long_buf[1]);
    TEST_ASSERT(memcmp(&long_buf[2], long_ref, 254) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, long_buf[256]);
}

static void test_tlv_encode_shortest_long(void)
{

    memset(long_buf, 0xEE, sizeof(long_buf));
    memset(long_ref, 'b', 255);

    /* 255 bytes: 0xFF marker + 16 bit big endian lengthgth 0x00FF, exact fit */
    ssize_t r = tlv_encode(long_buf, 259, 0x02, 255, long_ref);

    TEST_ASSERT_EQUAL_INT(259, r);
    TEST_ASSERT_EQUAL_INT(0x02, long_buf[0]);
    TEST_ASSERT_EQUAL_INT(0xFF, long_buf[1]);
    TEST_ASSERT_EQUAL_INT(0x00, long_buf[2]);
    TEST_ASSERT_EQUAL_INT(0xFF, long_buf[3]);
    TEST_ASSERT(memcmp(&long_buf[4], long_ref, 255) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, long_buf[259]);
}

static void test_tlv_encode_long(void)
{

    memset(long_buf, 0xEE, sizeof(long_buf));
    memset(long_ref, 0xAB, sizeof(long_ref));

    /* 300 = 0x012C */
    ssize_t r = tlv_encode(long_buf, 304, 0x02, sizeof(long_ref), long_ref);

    TEST_ASSERT_EQUAL_INT(304, r);
    TEST_ASSERT_EQUAL_INT(0x02, long_buf[0]);
    TEST_ASSERT_EQUAL_INT(0xFF, long_buf[1]);
    TEST_ASSERT_EQUAL_INT(0x01, long_buf[2]);
    TEST_ASSERT_EQUAL_INT(0x2C, long_buf[3]);
    TEST_ASSERT(memcmp(&long_buf[4], long_ref, sizeof(long_ref)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, long_buf[304]);
}

static void test_tlv_encode_nobufs_short(void)
{
    uint8_t buf[8];
    const uint8_t untouched[8] = { 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE };
    ssize_t r;

    /* needs 5, one byte short */
    memset(buf, 0xEE, sizeof(buf));
    r = tlv_encode(buf, 4, 0x01, 3, (const uint8_t *)"abc");
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, r);
    TEST_ASSERT(memcmp(buf, untouched, sizeof(untouched)) == 0);

    /* needs 2, one byte short */
    memset(buf, 0xEE, sizeof(buf));
    r = tlv_encode(buf, 1, 0x01, 0, NULL);
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, r);
    TEST_ASSERT(memcmp(buf, untouched, sizeof(untouched)) == 0);

    /* needs 5, much too small */
    memset(buf, 0xEE, sizeof(buf));
    r = tlv_encode(buf, 0, 0x01, 3, (const uint8_t *)"abc");
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, r);
    TEST_ASSERT(memcmp(buf, untouched, sizeof(untouched)) == 0);

    memset(buf, 0xEE, sizeof(buf));
    r = tlv_encode(buf, 1, 0x01, 3, (const uint8_t *)"abc");
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, r);
    TEST_ASSERT(memcmp(buf, untouched, sizeof(untouched)) == 0);

    memset(buf, 0xEE, sizeof(buf));
    r = tlv_encode(buf, 3, 0x01, 3, (const uint8_t *)"abc");
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, r);
    TEST_ASSERT(memcmp(buf, untouched, sizeof(untouched)) == 0);
}

static void test_tlv_encode_nobufs_long(void)
{
    ssize_t r;
    int bad;

    memset(long_ref, 'x', sizeof(long_ref));

    /* 255 bytes need 259: one byte short */
    memset(long_buf, 0xEE, sizeof(long_buf));
    r = tlv_encode(long_buf, 258, 0x01, 255, long_ref);
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, r);
    bad = 0;
    for (size_t i = 0; i < sizeof(long_buf); i++) {
        if (long_buf[i] != 0xEE) {
            bad++;
        }
    }
    TEST_ASSERT_EQUAL_INT(0, bad);

    /* the long header only partly fits */
    memset(long_buf, 0xEE, sizeof(long_buf));
    r = tlv_encode(long_buf, 3, 0x01, 255, long_ref);
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, r);
    bad = 0;
    for (size_t i = 0; i < sizeof(long_buf); i++) {
        if (long_buf[i] != 0xEE) {
            bad++;
        }
    }
    TEST_ASSERT_EQUAL_INT(0, bad);

    /* 300 bytes need 304: one byte short */
    memset(long_buf, 0xEE, sizeof(long_buf));
    r = tlv_encode(long_buf, 303, 0x01, sizeof(long_ref), long_ref);
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, r);
    bad = 0;
    for (size_t i = 0; i < sizeof(long_buf); i++) {
        if (long_buf[i] != 0xEE) {
            bad++;
        }
    }
    TEST_ASSERT_EQUAL_INT(0, bad);

    memset(long_buf, 0xEE, sizeof(long_buf));
    r = tlv_encode(long_buf, 0, 0x01, sizeof(long_ref), long_ref);
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, r);
    bad = 0;
    for (size_t i = 0; i < sizeof(long_buf); i++) {
        if (long_buf[i] != 0xEE) {
            bad++;
        }
    }
    TEST_ASSERT_EQUAL_INT(0, bad);
}

/* ------------------------------------------------------------------------ */
/* tlv_update                                                                */
/* ------------------------------------------------------------------------ */
#include <stdio.h>
static void test_tlv_update_same_size(void)
{
    uint8_t buf[16] = { 0x01, 0x03, 'a', 'a', 'a',
                        0x02, 0x02, 'b', 'b',
                        0x03, 0x01, 'c',
                        0xEE, 0xEE, 0xEE, 0xEE };
    const uint8_t value[] = { 'n', 'n' };
    const uint8_t expected[12] = { 0x01, 0x03, 'a', 'a', 'a',
                                   0x02, 0x02, 'n', 'n',
                                   0x03, 0x01, 'c' };

    /* tag 2: "bb" -> "nn"; capacity == size */
    ssize_t r = tlv_update(buf, sizeof(expected), sizeof(expected), 0x02, sizeof(value), value);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    for (size_t i = 0; i < sizeof(expected); i += 1) { printf("%02x", buf[i]); }
    printf("\n");
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(expected)]);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(buf) - 1]);
}

static void test_tlv_update_shrink_middle(void)
{
    uint8_t buf[24] = { 0x01, 0x03, 'a', 'a', 'a',
                        0x02, 0x0A, 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b',
                        0x03, 0x01, 'c',
                        0xEE, 0xEE, 0xEE, 0xEE };
    const uint8_t value[] = { 'n' };
    const uint8_t expected[11] = { 0x01, 0x03, 'a', 'a', 'a',
                                   0x02, 0x01, 'n',
                                   0x03, 0x01, 'c' };

    /* tag 2: 10 bytes -> 1 byte, the tail moves down */
    ssize_t r = tlv_update(buf, 20, 20, 0x02, sizeof(value), value);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[20]);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(buf) - 1]);
}

static void test_tlv_update_grow_middle(void)
{
    /* 0xDD is spare room inside the capacity, 0xEE is guard after it */
    uint8_t buf[18] = { 0x01, 0x03, 'a', 'a', 'a',
                        0x02, 0x02, 'b', 'b',
                        0x03, 0x01, 'c',
                        0xDD, 0xDD,
                        0xEE, 0xEE, 0xEE, 0xEE };
    const uint8_t value[] = { 'n', 'n', 'n', 'n' };
    const uint8_t expected[14] = { 0x01, 0x03, 'a', 'a', 'a',
                                   0x02, 0x04, 'n', 'n', 'n', 'n',
                                   0x03, 0x01, 'c' };

    /* tag 2: 2 -> 4 bytes, capacity is exactly the new size */
    ssize_t r = tlv_update(buf, 12, sizeof(expected), 0x02, sizeof(value), value);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(expected)]);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(buf) - 1]);
}

static void test_tlv_update_grow_first(void)
{
    uint8_t buf[21] = { 0x01, 0x03, 'a', 'a', 'a',
                        0x02, 0x02, 'b', 'b',
                        0x03, 0x01, 'c',
                        0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
                        0xEE, 0xEE, 0xEE, 0xEE };
    const uint8_t value[] = { 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n' };
    const uint8_t expected[17] = { 0x01, 0x08, 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n',
                                   0x02, 0x02, 'b', 'b',
                                   0x03, 0x01, 'c' };

    /* tag 1: 3 -> 8 bytes */
    ssize_t r = tlv_update(buf, 12, sizeof(expected), 0x01, sizeof(value), value);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(expected)]);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(buf) - 1]);
}

static void test_tlv_update_grow_last(void)
{
    uint8_t buf[24] = { 0x01, 0x03, 'a', 'a', 'a',
                        0x02, 0x02, 'b', 'b',
                        0x03, 0x01, 'c',
                        0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
                        0xEE, 0xEE, 0xEE, 0xEE };
    const uint8_t value[] = { 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n' };
    const uint8_t expected[20] = { 0x01, 0x03, 'a', 'a', 'a',
                                   0x02, 0x02, 'b', 'b',
                                   0x03, 0x09, 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n', 'n' };

    /* tag 3 is the last element: 1 -> 9 bytes, nothing behind it to move */
    ssize_t r = tlv_update(buf, 12, sizeof(expected), 0x03, sizeof(value), value);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(expected)]);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(buf) - 1]);
}

static void test_tlv_update_shrink_first(void)
{
    uint8_t buf[19] = { 0x01, 0x09, 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
                        0x02, 0x02, 'b', 'b',
                        0xEE, 0xEE, 0xEE, 0xEE };
    const uint8_t value[] = { 'n' };
    const uint8_t expected[7] = { 0x01, 0x01, 'n',
                                  0x02, 0x02, 'b', 'b' };

    ssize_t r = tlv_update(buf, 15, 15, 0x01, sizeof(value), value);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[15]);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(buf) - 1]);
}

static void test_tlv_update_only_element(void)
{
    uint8_t buf[11] = { 0x04, 0x05, 'q', 'q', 'q', 'q', 'q',
                        0xEE, 0xEE, 0xEE, 0xEE };
    const uint8_t expected[2] = { 0x04, 0x00 };

    /* 5 bytes -> empty value */
    ssize_t r = tlv_update(buf, 7, 7, 0x04, 0, NULL);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[7]);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(buf) - 1]);
}

static void test_tlv_update_to_zero_lengthgth(void)
{
    uint8_t buf[17] = { 0x01, 0x03, 'a', 'a', 'a',
                        0x02, 0x03, 'b', 'b', 'b',
                        0x03, 0x01, 'c',
                        0xEE, 0xEE, 0xEE, 0xEE };
    const uint8_t expected[10] = { 0x01, 0x03, 'a', 'a', 'a',
                                   0x02, 0x00,
                                   0x03, 0x01, 'c' };

    ssize_t r = tlv_update(buf, 13, 13, 0x02, 0, NULL);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[13]);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(buf) - 1]);
}

static void test_tlv_update_from_zero_lengthgth(void)
{
    uint8_t buf[17] = { 0x01, 0x03, 'a', 'a', 'a',
                        0x02, 0x00,
                        0x03, 0x01, 'c',
                        0xDD, 0xDD, 0xDD,
                        0xEE, 0xEE, 0xEE, 0xEE };
    const uint8_t value[] = { 'n', 'n', 'n' };
    const uint8_t expected[13] = { 0x01, 0x03, 'a', 'a', 'a',
                                   0x02, 0x03, 'n', 'n', 'n',
                                   0x03, 0x01, 'c' };

    ssize_t r = tlv_update(buf, 10, sizeof(expected), 0x02, sizeof(value), value);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(expected)]);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(buf) - 1]);
}

static void test_tlv_update_first_of_duplicates(void)
{
    /* grow: only the first tag 5 changes */
    uint8_t grow[22] = { 0x01, 0x03, 'a', 'a', 'a',
                         0x05, 0x02, 'x', 'x',
                         0x05, 0x02, 'y', 'y',
                         0x03, 0x01, 'c',
                         0xDD, 0xDD,
                         0xEE, 0xEE, 0xEE, 0xEE };
    const uint8_t grow_value[] = { 'n', 'n', 'n', 'n' };
    const uint8_t grow_expected[18] = { 0x01, 0x03, 'a', 'a', 'a',
                                        0x05, 0x04, 'n', 'n', 'n', 'n',
                                        0x05, 0x02, 'y', 'y',
                                        0x03, 0x01, 'c' };
    /* shrink */
    uint8_t shrink[21] = { 0x01, 0x03, 'a', 'a', 'a',
                           0x05, 0x06, 'x', 'x', 'x', 'x', 'x', 'x',
                           0x05, 0x02, 'y', 'y',
                           0xEE, 0xEE, 0xEE, 0xEE };
    const uint8_t shrink_value[] = { 'n' };
    const uint8_t shrink_expected[12] = { 0x01, 0x03, 'a', 'a', 'a',
                                          0x05, 0x01, 'n',
                                          0x05, 0x02, 'y', 'y' };
    ssize_t r;

    r = tlv_update(grow, 16, sizeof(grow_expected), 0x05, sizeof(grow_value), grow_value);
    TEST_ASSERT_EQUAL_INT(sizeof(grow_expected), r);
    TEST_ASSERT(memcmp(grow, grow_expected, sizeof(grow_expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, grow[sizeof(grow_expected)]);

    r = tlv_update(shrink, 17, 17, 0x05, sizeof(shrink_value), shrink_value);
    TEST_ASSERT_EQUAL_INT(sizeof(shrink_expected), r);
    TEST_ASSERT(memcmp(shrink, shrink_expected, sizeof(shrink_expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, shrink[17]);
}

static void test_tlv_update_append_missing(void)
{
    uint8_t buf[15] = { 0x01, 0x03, 'a', 'a', 'a',
                        0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
                        0xEE, 0xEE, 0xEE, 0xEE };
    const uint8_t value[] = { 'n', 'n', 'n', 'n' };
    const uint8_t expected[11] = { 0x01, 0x03, 'a', 'a', 'a',
                                   0x09, 0x04, 'n', 'n', 'n', 'n' };

    /* tag 9 does not exist: it is appended behind the existing data */
    ssize_t r = tlv_update(buf, 5, sizeof(expected), 0x09, sizeof(value), value);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(expected)]);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(buf) - 1]);
}

static void test_tlv_update_append_to_empty(void)
{
    uint8_t buf[8] = { 0xDD, 0xDD, 0xDD, 0xDD,
                       0xEE, 0xEE, 0xEE, 0xEE };
    const uint8_t value[] = { 'n', 'n' };
    const uint8_t expected[4] = { 0x04, 0x02, 'n', 'n' };

    ssize_t r = tlv_update(buf, 0, sizeof(expected), 0x04, sizeof(value), value);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(expected)]);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(buf) - 1]);
}

static void test_tlv_update_tag_zero(void)
{
    const uint8_t value2[] = { 'n', 'n' };
    const uint8_t value3[] = { 'n', 'n', 'n' };
    const uint8_t value1[] = { 'n' };
    ssize_t r;

    /* tag 0 into an empty buffer */
    uint8_t empty[8] = { 0xDD, 0xDD, 0xDD, 0xDD, 0xEE, 0xEE, 0xEE, 0xEE };
    const uint8_t empty_expected[4] = { 0x00, 0x02, 'n', 'n' };
    r = tlv_update(empty, 0, sizeof(empty_expected), 0x00, sizeof(value2), value2);
    TEST_ASSERT_EQUAL_INT(sizeof(empty_expected), r);
    TEST_ASSERT(memcmp(empty, empty_expected, sizeof(empty_expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, empty[sizeof(empty_expected)]);

    /* tag 0 is not there: appended */
    uint8_t missing[18] = { 0x01, 0x03, 'a', 'a', 'a',
                            0x02, 0x02, 'b', 'b',
                            0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
                            0xEE, 0xEE, 0xEE, 0xEE };
    const uint8_t missing_expected[14] = { 0x01, 0x03, 'a', 'a', 'a',
                                           0x02, 0x02, 'b', 'b',
                                           0x00, 0x03, 'n', 'n', 'n' };
    r = tlv_update(missing, 9, sizeof(missing_expected), 0x00, sizeof(value3), value3);
    TEST_ASSERT_EQUAL_INT(sizeof(missing_expected), r);
    TEST_ASSERT(memcmp(missing, missing_expected, sizeof(missing_expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, missing[sizeof(missing_expected)]);

    /* tag 0 exists: replaced */
    uint8_t existing[16] = { 0x01, 0x03, 'a', 'a', 'a',
                             0x00, 0x02, 'z', 'z',
                             0x03, 0x01, 'c',
                             0xEE, 0xEE, 0xEE, 0xEE };
    const uint8_t existing_expected[11] = { 0x01, 0x03, 'a', 'a', 'a',
                                            0x00, 0x01, 'n',
                                            0x03, 0x01, 'c' };
    r = tlv_update(existing, 12, 12, 0x00, sizeof(value1), value1);
    TEST_ASSERT_EQUAL_INT(sizeof(existing_expected), r);
    TEST_ASSERT(memcmp(existing, existing_expected, sizeof(existing_expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, existing[12]);
}

static void test_tlv_update_short_to_long(void)
{
    ssize_t r;

    /* tag 1 "a", tag 2 with 10 bytes, tag 3 "c"  ->  tag 2 with 300 bytes */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x01;
    long_buf[1] = 0x01;
    long_buf[2] = 'a';
    long_buf[3] = 0x02;
    long_buf[4] = 0x0A;
    memset(&long_buf[5], 'b', 10);
    long_buf[15] = 0x03;
    long_buf[16] = 0x01;
    long_buf[17] = 'c';
    memset(&long_buf[18], 0xDD, 292);       /* spare room up to the new size, 310 */
    memset(long_ref, 'n', sizeof(long_ref));

    r = tlv_update(long_buf, 18, 310, 0x02, sizeof(long_ref), long_ref);

    TEST_ASSERT_EQUAL_INT(310, r);
    TEST_ASSERT_EQUAL_INT(0x01, long_buf[0]);
    TEST_ASSERT_EQUAL_INT(0x01, long_buf[1]);
    TEST_ASSERT_EQUAL_INT('a', long_buf[2]);
    TEST_ASSERT_EQUAL_INT(0x02, long_buf[3]);
    TEST_ASSERT_EQUAL_INT(0xFF, long_buf[4]);
    TEST_ASSERT_EQUAL_INT(0x01, long_buf[5]);
    TEST_ASSERT_EQUAL_INT(0x2C, long_buf[6]);
    TEST_ASSERT(memcmp(&long_buf[7], long_ref, sizeof(long_ref)) == 0);
    TEST_ASSERT_EQUAL_INT(0x03, long_buf[307]);
    TEST_ASSERT_EQUAL_INT(0x01, long_buf[308]);
    TEST_ASSERT_EQUAL_INT('c', long_buf[309]);
    TEST_ASSERT_EQUAL_INT(0xEE, long_buf[310]);
}

static void test_tlv_update_long_to_short(void)
{
    const uint8_t value[] = { 'n', 'n', 'n' };
    const uint8_t expected[11] = { 0x01, 0x01, 'a',
                                   0x02, 0x03, 'n', 'n', 'n',
                                   0x03, 0x01, 'c' };
    ssize_t r;

    /* tag 1 "a", tag 2 with 300 bytes, tag 3 "c"  ->  tag 2 "nnn" */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x01;
    long_buf[1] = 0x01;
    long_buf[2] = 'a';
    long_buf[3] = 0x02;
    long_buf[4] = 0xFF;
    long_buf[5] = 0x01;
    long_buf[6] = 0x2C;
    memset(&long_buf[7], 'b', 300);
    long_buf[307] = 0x03;
    long_buf[308] = 0x01;
    long_buf[309] = 'c';

    r = tlv_update(long_buf, 310, 310, 0x02, sizeof(value), value);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT(memcmp(long_buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, long_buf[310]);
}

static void test_tlv_update_254_to_255(void)
{
    ssize_t r;

    /* the value crosses from the short to the long form: 254 -> 255 bytes */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x01;
    long_buf[1] = 0x01;
    long_buf[2] = 'a';
    long_buf[3] = 0x02;
    long_buf[4] = 0xFE;
    memset(&long_buf[5], 'b', 254);
    long_buf[259] = 0x03;
    long_buf[260] = 0x01;
    long_buf[261] = 'c';
    memset(&long_buf[262], 0xDD, 3);        /* spare room up to the new size, 265 */
    memset(long_ref, 'n', 255);

    r = tlv_update(long_buf, 262, 265, 0x02, 255, long_ref);

    TEST_ASSERT_EQUAL_INT(265, r);
    TEST_ASSERT_EQUAL_INT(0x02, long_buf[3]);
    TEST_ASSERT_EQUAL_INT(0xFF, long_buf[4]);
    TEST_ASSERT_EQUAL_INT(0x00, long_buf[5]);
    TEST_ASSERT_EQUAL_INT(0xFF, long_buf[6]);
    TEST_ASSERT(memcmp(&long_buf[7], long_ref, 255) == 0);
    TEST_ASSERT_EQUAL_INT(0x03, long_buf[262]);
    TEST_ASSERT_EQUAL_INT(0x01, long_buf[263]);
    TEST_ASSERT_EQUAL_INT('c', long_buf[264]);
    TEST_ASSERT_EQUAL_INT(0xEE, long_buf[265]);
}

static void test_tlv_update_255_to_254(void)
{
    ssize_t r;

    /* and back: 255 -> 254 bytes */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x01;
    long_buf[1] = 0x01;
    long_buf[2] = 'a';
    long_buf[3] = 0x02;
    long_buf[4] = 0xFF;
    long_buf[5] = 0x00;
    long_buf[6] = 0xFF;
    memset(&long_buf[7], 'b', 255);
    long_buf[262] = 0x03;
    long_buf[263] = 0x01;
    long_buf[264] = 'c';
    memset(long_ref, 'n', 254);

    r = tlv_update(long_buf, 265, 265, 0x02, 254, long_ref);

    TEST_ASSERT_EQUAL_INT(262, r);
    TEST_ASSERT_EQUAL_INT(0x02, long_buf[3]);
    TEST_ASSERT_EQUAL_INT(0xFE, long_buf[4]);
    TEST_ASSERT(memcmp(&long_buf[5], long_ref, 254) == 0);
    TEST_ASSERT_EQUAL_INT(0x03, long_buf[259]);
    TEST_ASSERT_EQUAL_INT(0x01, long_buf[260]);
    TEST_ASSERT_EQUAL_INT('c', long_buf[261]);
    TEST_ASSERT_EQUAL_INT(0xEE, long_buf[265]);
}

static void test_tlv_update_nobufs_grow(void)
{
    uint8_t buf[17] = { 0x01, 0x03, 'a', 'a', 'a',
                        0x02, 0x02, 'b', 'b',
                        0x03, 0x01, 'c',
                        0xDD,
                        0xEE, 0xEE, 0xEE, 0xEE };
    const uint8_t unchanged[17] = { 0x01, 0x03, 'a', 'a', 'a',
                                    0x02, 0x02, 'b', 'b',
                                    0x03, 0x01, 'c',
                                    0xDD,
                                    0xEE, 0xEE, 0xEE, 0xEE };
    const uint8_t value[] = { 'n', 'n', 'n', 'n' };

    /* tag 2: 2 -> 4 bytes needs 14, capacity is 13 */
    ssize_t r = tlv_update(buf, 12, 13, 0x02, sizeof(value), value);

    TEST_ASSERT_EQUAL_INT(-ENOBUFS, r);
    TEST_ASSERT(memcmp(buf, unchanged, sizeof(unchanged)) == 0);
}

static void test_tlv_update_nobufs_append(void)
{
    uint8_t buf[18] = { 0x01, 0x03, 'a', 'a', 'a',
                        0x02, 0x02, 'b', 'b',
                        0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
                        0xEE, 0xEE, 0xEE, 0xEE };
    const uint8_t unchanged[18] = { 0x01, 0x03, 'a', 'a', 'a',
                                    0x02, 0x02, 'b', 'b',
                                    0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
                                    0xEE, 0xEE, 0xEE, 0xEE };
    const uint8_t value[] = { 'n', 'n', 'n', 'n' };
    ssize_t r;

    /* appending tag 9 with 4 bytes needs 15, capacity is 14 */
    r = tlv_update(buf, 9, 14, 0x09, sizeof(value), value);
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, r);
    TEST_ASSERT(memcmp(buf, unchanged, sizeof(unchanged)) == 0);

    /* an empty element needs 2 bytes: capacity == size, then one byte more */
    r = tlv_update(buf, 9, 9, 0x09, 0, NULL);
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, r);
    TEST_ASSERT(memcmp(buf, unchanged, sizeof(unchanged)) == 0);

    r = tlv_update(buf, 9, 10, 0x09, 0, NULL);
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, r);
    TEST_ASSERT(memcmp(buf, unchanged, sizeof(unchanged)) == 0);
}

static void test_tlv_update_nobufs_long(void)
{
    ssize_t r;

    /* tag 1 "a", tag 2 with 254 bytes (259 in total); growing tag 2 to 255
     * bytes needs 262 but the capacity is 261 */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x01;
    long_buf[1] = 0x01;
    long_buf[2] = 'a';
    long_buf[3] = 0x02;
    long_buf[4] = 0xFE;
    memset(&long_buf[5], 'b', 254);
    memset(&long_buf[259], 0xDD, 2);
    memcpy(long_copy, long_buf, sizeof(long_buf));
    memset(long_ref, 'n', 255);

    r = tlv_update(long_buf, 259, 261, 0x02, 255, long_ref);

    TEST_ASSERT_EQUAL_INT(-ENOBUFS, r);
    TEST_ASSERT(memcmp(long_buf, long_copy, sizeof(long_buf)) == 0);

    /* appending 300 bytes needs 304 more, only 303 are left */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x01;
    long_buf[1] = 0x01;
    long_buf[2] = 'a';
    memset(&long_buf[3], 0xDD, 303);
    memcpy(long_copy, long_buf, sizeof(long_buf));
    memset(long_ref, 'n', sizeof(long_ref));

    r = tlv_update(long_buf, 3, 306, 0x09, sizeof(long_ref), long_ref);

    TEST_ASSERT_EQUAL_INT(-ENOBUFS, r);
    TEST_ASSERT(memcmp(long_buf, long_copy, sizeof(long_buf)) == 0);
}

static void test_tlv_update_malformed(void)
{
    const uint8_t value[] = { 'n', 'n' };
    ssize_t r;

    /* truncated value, tag present and absent */
    uint8_t truncated[16] = { 0x01, 0x05, 'a',
                              0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE,
                              0xEE, 0xEE, 0xEE, 0xEE };
    const uint8_t truncated_orig[16] = { 0x01, 0x05, 'a',
                                         0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE,
                                         0xEE, 0xEE, 0xEE, 0xEE };
    r = tlv_update(truncated, 3, 12, 0x01, sizeof(value), value);
    TEST_ASSERT_EQUAL_INT(-EBADMSG, r);
    TEST_ASSERT(memcmp(truncated, truncated_orig, sizeof(truncated_orig)) == 0);
    r = tlv_update(truncated, 3, 12, 0x09, sizeof(value), value);
    TEST_ASSERT_EQUAL_INT(-EBADMSG, r);
    TEST_ASSERT(memcmp(truncated, truncated_orig, sizeof(truncated_orig)) == 0);

    /* a lone byte after a valid element, tag absent */
    uint8_t trailing[16] = { 0x01, 0x01, 'a', 0x02,
                             0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE,
                             0xEE, 0xEE, 0xEE, 0xEE };
    const uint8_t trailing_orig[16] = { 0x01, 0x01, 'a', 0x02,
                                        0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE,
                                        0xEE, 0xEE, 0xEE, 0xEE };
    r = tlv_update(trailing, 4, 12, 0x09, sizeof(value), value);
    TEST_ASSERT_EQUAL_INT(-EBADMSG, r);
    TEST_ASSERT(memcmp(trailing, trailing_orig, sizeof(trailing_orig)) == 0);

    /* non-canonical long form */
    uint8_t noncanon[16] = { 0x01, 0xFF, 0x00, 0x05, 'a', 'b', 'c', 'd', 'e',
                             0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE };
    const uint8_t noncanon_orig[16] = { 0x01, 0xFF, 0x00, 0x05, 'a', 'b', 'c', 'd', 'e',
                                        0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE };
    r = tlv_update(noncanon, 9, sizeof(noncanon), 0x01, sizeof(value), value);
    TEST_ASSERT_EQUAL_INT(-EBADMSG, r);
    TEST_ASSERT(memcmp(noncanon, noncanon_orig, sizeof(noncanon_orig)) == 0);
}

static void test_tlv_update_value_untouched(void)
{
    /* the value passed in is only read, never written */
    uint8_t value[16] = { 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                          0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F };
    const uint8_t original[16] = { 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                                   0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F };
    uint8_t replace[40] = { 0x01, 0x03, 'a', 'a', 'a',
                            0x02, 0x02, 'b', 'b',
                            0x03, 0x01, 'c' };
    uint8_t append[40] = { 0x01, 0x03, 'a', 'a', 'a',
                           0x02, 0x02, 'b', 'b',
                           0x03, 0x01, 'c' };
    ssize_t r;

    /* replace tag 2: new size 5 + 18 + 3 = 26 */
    r = tlv_update(replace, 12, sizeof(replace), 0x02, sizeof(value), value);
    TEST_ASSERT_EQUAL_INT(26, r);
    TEST_ASSERT(memcmp(value, original, sizeof(original)) == 0);
    TEST_ASSERT_EQUAL_INT(0x02, replace[5]);
    TEST_ASSERT_EQUAL_INT(sizeof(value), replace[6]);
    TEST_ASSERT(memcmp(&replace[7], original, sizeof(original)) == 0);
    TEST_ASSERT_EQUAL_INT(0x03, replace[23]);
    TEST_ASSERT_EQUAL_INT(0x01, replace[24]);
    TEST_ASSERT_EQUAL_INT('c', replace[25]);

    /* append tag 9: new size 12 + 18 = 30 */
    r = tlv_update(append, 12, sizeof(append), 0x09, sizeof(value), value);
    TEST_ASSERT_EQUAL_INT(30, r);
    TEST_ASSERT(memcmp(value, original, sizeof(original)) == 0);
    TEST_ASSERT_EQUAL_INT(0x09, append[12]);
    TEST_ASSERT_EQUAL_INT(sizeof(value), append[13]);
    TEST_ASSERT(memcmp(&append[14], original, sizeof(original)) == 0);
}

/* ------------------------------------------------------------------------ */
/* tlv_remove (removes all elements with the tag, returns the new size)      */
/* ------------------------------------------------------------------------ */

static void test_tlv_remove_middle(void)
{
    uint8_t buf[12] = { 0x01, 0x03, 'a', 'a', 'a',
                        0x02, 0x02, 'b', 'b',
                        0x03, 0x01, 'c' };
    const uint8_t expected[8] = { 0x01, 0x03, 'a', 'a', 'a',
                                  0x03, 0x01, 'c' };

    ssize_t r = tlv_remove(buf, sizeof(buf), 0x02);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
}

static void test_tlv_remove_first(void)
{
    uint8_t buf[12] = { 0x01, 0x03, 'a', 'a', 'a',
                        0x02, 0x02, 'b', 'b',
                        0x03, 0x01, 'c' };
    const uint8_t expected[7] = { 0x02, 0x02, 'b', 'b',
                                  0x03, 0x01, 'c' };

    ssize_t r = tlv_remove(buf, sizeof(buf), 0x01);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
}

static void test_tlv_remove_last(void)
{
    uint8_t buf[12] = { 0x01, 0x03, 'a', 'a', 'a',
                        0x02, 0x02, 'b', 'b',
                        0x03, 0x01, 'c' };
    const uint8_t expected[9] = { 0x01, 0x03, 'a', 'a', 'a',
                                  0x02, 0x02, 'b', 'b' };

    ssize_t r = tlv_remove(buf, sizeof(buf), 0x03);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
}

static void test_tlv_remove_only_element(void)
{
    uint8_t buf[7] = { 0x04, 0x05, 'q', 'q', 'q', 'q', 'q' };

    TEST_ASSERT_EQUAL_INT(0, tlv_remove(buf, sizeof(buf), 0x04));
}

static void test_tlv_remove_no_match(void)
{
    uint8_t buf[12] = { 0x01, 0x03, 'a', 'a', 'a',
                        0x02, 0x02, 'b', 'b',
                        0x03, 0x01, 'c' };
    const uint8_t original[12] = { 0x01, 0x03, 'a', 'a', 'a',
                                   0x02, 0x02, 'b', 'b',
                                   0x03, 0x01, 'c' };

    TEST_ASSERT_EQUAL_INT(sizeof(original), tlv_remove(buf, sizeof(buf), 0x09));
    TEST_ASSERT(memcmp(buf, original, sizeof(original)) == 0);

    TEST_ASSERT_EQUAL_INT(sizeof(original), tlv_remove(buf, sizeof(buf), 0x00));
    TEST_ASSERT(memcmp(buf, original, sizeof(original)) == 0);
}

static void test_tlv_remove_empty_buffer(void)
{
    uint8_t buf[4] = { 0xEE, 0xEE, 0xEE, 0xEE };

    TEST_ASSERT_EQUAL_INT(0, tlv_remove(buf, 0, 0x01));
    TEST_ASSERT_EQUAL_INT(0, tlv_remove(buf, 0, 0x00));
}

static void test_tlv_remove_zero_lengthgth(void)
{
    uint8_t buf[10] = { 0x01, 0x03, 'a', 'a', 'a',
                        0x02, 0x00,
                        0x03, 0x01, 'c' };
    const uint8_t expected[8] = { 0x01, 0x03, 'a', 'a', 'a',
                                  0x03, 0x01, 'c' };

    ssize_t r = tlv_remove(buf, sizeof(buf), 0x02);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
}

static void test_tlv_remove_tag_zero(void)
{
    uint8_t buf[12] = { 0x01, 0x03, 'a', 'a', 'a',
                        0x00, 0x02, 'z', 'z',
                        0x03, 0x01, 'c' };
    const uint8_t expected[8] = { 0x01, 0x03, 'a', 'a', 'a',
                                  0x03, 0x01, 'c' };

    ssize_t r = tlv_remove(buf, sizeof(buf), 0x00);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
}

static void test_tlv_remove_adjacent_duplicates(void)
{
    uint8_t buf[12] = { 0x05, 0x02, 'x', 'x',
                        0x05, 0x03, 'y', 'y', 'y',
                        0x01, 0x01, 'a' };
    const uint8_t expected[3] = { 0x01, 0x01, 'a' };

    ssize_t r = tlv_remove(buf, sizeof(buf), 0x05);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
}

static void test_tlv_remove_interleaved_duplicates(void)
{
    uint8_t buf[15] = { 0x05, 0x01, 'x',
                        0x01, 0x01, 'a',
                        0x05, 0x02, 'y', 'y',
                        0x02, 0x01, 'b',
                        0x05, 0x00 };
    const uint8_t expected[6] = { 0x01, 0x01, 'a',
                                  0x02, 0x01, 'b' };

    ssize_t r = tlv_remove(buf, sizeof(buf), 0x05);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
}

static void test_tlv_remove_duplicates_at_end(void)
{
    uint8_t buf[11] = { 0x01, 0x01, 'a',
                        0x05, 0x02, 'x', 'x',
                        0x05, 0x02, 'y', 'y' };
    const uint8_t expected[3] = { 0x01, 0x01, 'a' };

    ssize_t r = tlv_remove(buf, sizeof(buf), 0x05);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
}

static void test_tlv_remove_every_element(void)
{
    uint8_t buf[7] = { 0x05, 0x01, 'x',
                       0x05, 0x02, 'y', 'y' };

    TEST_ASSERT_EQUAL_INT(0, tlv_remove(buf, sizeof(buf), 0x05));
}

static void test_tlv_remove_long_element(void)
{
    const uint8_t expected[4] = { 0x02, 0x02, 'b', 'b' };
    ssize_t r;

    /* tag 1 with 300 bytes, tag 2 "bb", tag 1 with 255 bytes: remove tag 1 */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x01;
    long_buf[1] = 0xFF;
    long_buf[2] = 0x01;
    long_buf[3] = 0x2C;
    memset(&long_buf[4], 'l', 300);
    long_buf[304] = 0x02;
    long_buf[305] = 0x02;
    long_buf[306] = 'b';
    long_buf[307] = 'b';
    long_buf[308] = 0x01;
    long_buf[309] = 0xFF;
    long_buf[310] = 0x00;
    long_buf[311] = 0xFF;
    memset(&long_buf[312], 'm', 255);

    r = tlv_remove(long_buf, 567, 0x01);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT(memcmp(long_buf, expected, sizeof(expected)) == 0);
}

static void test_tlv_remove_keeps_long_elements(void)
{
    ssize_t r;

    /* tag 2 with 300 bytes, tag 1 "aa", tag 3 with 255 bytes: remove tag 1 */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x02;
    long_buf[1] = 0xFF;
    long_buf[2] = 0x01;
    long_buf[3] = 0x2C;
    memset(&long_buf[4], 'l', 300);
    long_buf[304] = 0x01;
    long_buf[305] = 0x02;
    long_buf[306] = 'a';
    long_buf[307] = 'a';
    long_buf[308] = 0x03;
    long_buf[309] = 0xFF;
    long_buf[310] = 0x00;
    long_buf[311] = 0xFF;
    memset(&long_buf[312], 'm', 255);

    r = tlv_remove(long_buf, 567, 0x01);

    TEST_ASSERT_EQUAL_INT(563, r);
    TEST_ASSERT_EQUAL_INT(0x02, long_buf[0]);
    TEST_ASSERT_EQUAL_INT(0xFF, long_buf[1]);
    TEST_ASSERT_EQUAL_INT(0x01, long_buf[2]);
    TEST_ASSERT_EQUAL_INT(0x2C, long_buf[3]);
    memset(long_ref, 'l', 300);
    TEST_ASSERT(memcmp(&long_buf[4], long_ref, 300) == 0);
    TEST_ASSERT_EQUAL_INT(0x03, long_buf[304]);
    TEST_ASSERT_EQUAL_INT(0xFF, long_buf[305]);
    TEST_ASSERT_EQUAL_INT(0x00, long_buf[306]);
    TEST_ASSERT_EQUAL_INT(0xFF, long_buf[307]);
    memset(long_ref, 'm', 255);
    TEST_ASSERT(memcmp(&long_buf[308], long_ref, 255) == 0);
}

static void test_tlv_remove_254_and_255(void)
{
    const uint8_t expected[6] = { 0x01, 0x01, 'a',
                                  0x03, 0x01, 'c' };
    ssize_t r;

    /* tag 2 with 254 bytes, tag 1 "a", tag 2 with 255 bytes, tag 3 "c": remove tag 2 */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x02;
    long_buf[1] = 0xFE;
    memset(&long_buf[2], 'l', 254);
    long_buf[256] = 0x01;
    long_buf[257] = 0x01;
    long_buf[258] = 'a';
    long_buf[259] = 0x02;
    long_buf[260] = 0xFF;
    long_buf[261] = 0x00;
    long_buf[262] = 0xFF;
    memset(&long_buf[263], 'm', 255);
    long_buf[518] = 0x03;
    long_buf[519] = 0x01;
    long_buf[520] = 'c';

    r = tlv_remove(long_buf, 521, 0x02);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT(memcmp(long_buf, expected, sizeof(expected)) == 0);
}

static void test_tlv_remove_malformed(void)
{
    uint8_t truncated[3] = { 0x01, 0x05, 'a' };
    uint8_t trailing[4] = { 0x01, 0x01, 'a', 0x02 };
    uint8_t noncanon[9] = { 0x01, 0xFF, 0x00, 0x05, 'a', 'b', 'c', 'd', 'e' };

    TEST_ASSERT_EQUAL_INT(-EBADMSG, tlv_remove(truncated, sizeof(truncated), 0x09));
    TEST_ASSERT_EQUAL_INT(-EBADMSG, tlv_remove(trailing, sizeof(trailing), 0x09));
    TEST_ASSERT_EQUAL_INT(-EBADMSG, tlv_remove(noncanon, sizeof(noncanon), 0x01));
}

Test *tests_tlv_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_tlv_decode_short),
        new_TestFixture(test_tlv_decode_sequence),
        new_TestFixture(test_tlv_decode_long_form),
        new_TestFixture(test_tlv_decode_longest_short),
        new_TestFixture(test_tlv_decode_shortest_long),
        new_TestFixture(test_tlv_decode_too_short),
        new_TestFixture(test_tlv_decode_truncated),
        new_TestFixture(test_tlv_decode_noncanonical),
        new_TestFixture(test_tlv_decode_error_untouched),
        new_TestFixture(test_tlv_find_hit),
        new_TestFixture(test_tlv_find_last_element),
        new_TestFixture(test_tlv_find_missing),
        new_TestFixture(test_tlv_find_empty),
        new_TestFixture(test_tlv_find_long_value),
        new_TestFixture(test_tlv_find_after_long_element),
        new_TestFixture(test_tlv_find_malformed),
        new_TestFixture(test_tlv_encode_short),
        new_TestFixture(test_tlv_encode_zero_lengthgth),
        new_TestFixture(test_tlv_encode_sequence),
        new_TestFixture(test_tlv_encode_longest_short),
        new_TestFixture(test_tlv_encode_shortest_long),
        new_TestFixture(test_tlv_encode_long),
        new_TestFixture(test_tlv_encode_nobufs_short),
        new_TestFixture(test_tlv_encode_nobufs_long),
        new_TestFixture(test_tlv_update_same_size),
        new_TestFixture(test_tlv_update_shrink_middle),
        new_TestFixture(test_tlv_update_grow_middle),
        new_TestFixture(test_tlv_update_grow_first),
        new_TestFixture(test_tlv_update_grow_last),
        new_TestFixture(test_tlv_update_shrink_first),
        new_TestFixture(test_tlv_update_only_element),
        new_TestFixture(test_tlv_update_to_zero_lengthgth),
        new_TestFixture(test_tlv_update_from_zero_lengthgth),
        new_TestFixture(test_tlv_update_first_of_duplicates),
        new_TestFixture(test_tlv_update_append_missing),
        new_TestFixture(test_tlv_update_append_to_empty),
        new_TestFixture(test_tlv_update_tag_zero),
        new_TestFixture(test_tlv_update_short_to_long),
        new_TestFixture(test_tlv_update_long_to_short),
        new_TestFixture(test_tlv_update_254_to_255),
        new_TestFixture(test_tlv_update_255_to_254),
        new_TestFixture(test_tlv_update_nobufs_grow),
        new_TestFixture(test_tlv_update_nobufs_append),
        new_TestFixture(test_tlv_update_nobufs_long),
        new_TestFixture(test_tlv_update_malformed),
        new_TestFixture(test_tlv_update_value_untouched),
        new_TestFixture(test_tlv_remove_middle),
        new_TestFixture(test_tlv_remove_first),
        new_TestFixture(test_tlv_remove_last),
        new_TestFixture(test_tlv_remove_only_element),
        new_TestFixture(test_tlv_remove_no_match),
        new_TestFixture(test_tlv_remove_empty_buffer),
        new_TestFixture(test_tlv_remove_zero_lengthgth),
        new_TestFixture(test_tlv_remove_tag_zero),
        new_TestFixture(test_tlv_remove_adjacent_duplicates),
        new_TestFixture(test_tlv_remove_interleaved_duplicates),
        new_TestFixture(test_tlv_remove_duplicates_at_end),
        new_TestFixture(test_tlv_remove_every_element),
        new_TestFixture(test_tlv_remove_long_element),
        new_TestFixture(test_tlv_remove_keeps_long_elements),
        new_TestFixture(test_tlv_remove_254_and_255),
        new_TestFixture(test_tlv_remove_malformed),
    };

    EMB_UNIT_TESTCALLER(tlv_tests, NULL, NULL, fixtures);

    return (Test *)&tlv_tests;
}

void tests_tlv(void)
{
    TESTS_RUN(tests_tlv_tests());
}
