/*
 * SPDX-FileCopyrightText: ???
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "tests-tlv.h"

#include <string.h>
#include "errno.h"
#include "tlv.h"

static uint8_t long_buf[576];
static uint8_t long_ref[300];

/*
 * MARK: - Decoding
 */

static void test_simple_tlv_decode_sequence(void)
{
    /* tag 1 "abc", tag 2 (empty), tag 3 "z" */
    const uint8_t raw[] = { 0x01, 0x03, 'a', 'b', 'c',
                            0x02, 0x00,
                            0x03, 0x01, 'z' };
    tlv_tag_t tag = 0;
    size_t length = 0;
    const uint8_t *value = NULL;
    ssize_t decoded;

    decoded = simple_tlv_decode(raw, sizeof(raw), &tag, &length, &value);
    TEST_ASSERT_EQUAL_INT(5, decoded);
    TEST_ASSERT_EQUAL_INT(0x01, tag);
    TEST_ASSERT_EQUAL_INT(sizeof("abc") - 1, length);
    TEST_ASSERT(value == &raw[2]);

    decoded = simple_tlv_decode(&raw[5], sizeof(raw) - 5, &tag, &length, &value);
    TEST_ASSERT_EQUAL_INT(2, decoded);
    TEST_ASSERT_EQUAL_INT(0x02, tag);
    TEST_ASSERT_EQUAL_INT(sizeof("") - 1, length);

    decoded = simple_tlv_decode(&raw[7], sizeof(raw) - 7, &tag, &length, &value);
    TEST_ASSERT_EQUAL_INT(3, decoded); /* last element: nothing remaining */
    TEST_ASSERT_EQUAL_INT(0x03, tag);
    TEST_ASSERT_EQUAL_INT(sizeof("z") - 1, length);
    TEST_ASSERT(value == &raw[9]);
}

static void test_simple_tlv_decode_reserved_tags(void)
{
    const uint8_t raw[] = { 0x00, 0x01, 'a',
                            0xFF, 0x01, 'b' };
    TEST_ASSERT_EQUAL_INT(-EBADMSG, simple_tlv_decode(raw, sizeof(raw), NULL, NULL, NULL));
    TEST_ASSERT_EQUAL_INT(-EBADMSG, simple_tlv_decode(&raw[3], sizeof(raw) - 3, NULL, NULL, NULL));
}

static void test_simple_tlv_decode_long_form(void)
{
    tlv_tag_t tag = 0;
    size_t length = 0;
    const uint8_t *value = NULL;
    ssize_t decoded;

    /* tag 9 with 300 (0x012C) bytes of 0xA5, then tag 7 with "x" */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x09;
    long_buf[1] = 0xFF;
    long_buf[2] = 0x01;
    long_buf[3] = 0x2C;
    memset(&long_buf[4], 0xA5, sizeof(long_ref));
    long_buf[4 + sizeof(long_ref)] = 0x07;
    long_buf[4 + sizeof(long_ref) + 1] = sizeof("x") - 1;
    long_buf[4 + sizeof(long_ref) + 2] = 'x';

    decoded = simple_tlv_decode(long_buf, 4 + sizeof(long_ref) + 3, &tag, &length, &value);
    TEST_ASSERT_EQUAL_INT(4 + sizeof(long_ref), decoded);
    TEST_ASSERT_EQUAL_INT(0x09, tag);
    TEST_ASSERT_EQUAL_INT(sizeof(long_ref), length);
    TEST_ASSERT(value == &long_buf[4]);

    decoded = simple_tlv_decode(&long_buf[4 + sizeof(long_ref)], 3, &tag, &length, &value);
    TEST_ASSERT_EQUAL_INT(3, decoded);
    TEST_ASSERT_EQUAL_INT(0x07, tag);
    TEST_ASSERT_EQUAL_INT(sizeof("x") - 1, length);
    TEST_ASSERT(value == &long_buf[4 + sizeof(long_ref) + 2]);
}

static void test_simple_tlv_decode_longest_short(void)
{
    tlv_tag_t tag = 0;
    size_t length = 0;
    const uint8_t *value = NULL;

    /* 254 (0xFE) is the longest length that fits the single length byte */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x01;
    long_buf[1] = 0xFE;
    memset(&long_buf[2], 'a', 254);

    ssize_t decoded = simple_tlv_decode(long_buf, 2 + 254, &tag, &length, &value);

    TEST_ASSERT_EQUAL_INT(2 + 254, decoded);
    TEST_ASSERT_EQUAL_INT(0x01, tag);
    TEST_ASSERT_EQUAL_INT(254, length);
    TEST_ASSERT(value == &long_buf[2]);
}

static void test_simple_tlv_decode_shortest_long(void)
{
    tlv_tag_t tag = 0;
    size_t length = 0;
    const uint8_t *value = NULL;

    /* 255 (0xFF) is the shortest length that needs the long form */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x02;
    long_buf[1] = 0xFF;
    long_buf[2] = 0x00;
    long_buf[3] = 0xFF;
    memset(&long_buf[4], 'b', 255);

    ssize_t decoded = simple_tlv_decode(long_buf, 4 + 255, &tag, &length, &value);

    TEST_ASSERT_EQUAL_INT(4 + 255, decoded);
    TEST_ASSERT_EQUAL_INT(0x02, tag);
    TEST_ASSERT_EQUAL_INT(255, length);
    TEST_ASSERT(value == &long_buf[4]);
}

static void test_simple_tlv_decode_too_short(void)
{
    const uint8_t raw[] = { 0x01 };
    tlv_tag_t tag = 0;
    size_t length = 0;
    const uint8_t *value = NULL;

    /* nothing at all */
    TEST_ASSERT_EQUAL_INT(-EBADMSG, simple_tlv_decode(raw, 0, &tag, &length, &value));

    /* a tag without a length */
    TEST_ASSERT_EQUAL_INT(-EBADMSG, simple_tlv_decode(raw, sizeof(raw), &tag, &length, &value));
}

static void test_simple_tlv_decode_truncated(void)
{
    const uint8_t short_value[] = { 0x01, 0x05, 'a' };        /* wants 5, has 1 */
    const uint8_t long_marker[] = { 0x01, 0xFF };             /* no length */
    const uint8_t long_hdr[] = { 0x01, 0xFF, 0x01 };          /* half a length */
    const uint8_t long_nodata[] = { 0x01, 0xFF, 0x01, 0x00 }; /* 256, has 0 */
    tlv_tag_t tag = 0;
    size_t length = 0;
    const uint8_t *value = NULL;

    TEST_ASSERT_EQUAL_INT(-EBADMSG, simple_tlv_decode(short_value, sizeof(short_value), &tag, &length, &value));

    TEST_ASSERT_EQUAL_INT(-EBADMSG, simple_tlv_decode(long_marker, sizeof(long_marker), &tag, &length, &value));

    TEST_ASSERT_EQUAL_INT(-EBADMSG, simple_tlv_decode(long_hdr, sizeof(long_hdr), &tag, &length, &value));

    TEST_ASSERT_EQUAL_INT(-EBADMSG, simple_tlv_decode(long_nodata, sizeof(long_nodata), &tag, &length, &value));

    /* long form, 300 bytes announced, 299 present */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x01;
    long_buf[1] = 0xFF;
    long_buf[2] = 0x01;
    long_buf[3] = 0x2C;
    memset(&long_buf[4], 0x42, sizeof(long_ref) - 1);
    TEST_ASSERT_EQUAL_INT(-EBADMSG, simple_tlv_decode(long_buf, 4 + sizeof(long_ref) - 1, &tag, &length, &value));

    /* short form, 254 bytes announced, 253 present */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x01;
    long_buf[1] = 0xFE;
    memset(&long_buf[2], 0x42, 253);
    TEST_ASSERT_EQUAL_INT(-EBADMSG, simple_tlv_decode(long_buf, 2 + 253, &tag, &length, &value));
}

static void test_simple_tlv_decode_noncanonical(void)
{
    /* the long form must not be used for lengths below 0xFF */
    const uint8_t length5[] = { 0x01, 0xFF, 0x00, 0x05, 'a', 'b', 'c', 'd', 'e' };
    const uint8_t length0[] = { 0x01, 0xFF, 0x00, 0x00 };
    tlv_tag_t tag = 0;
    size_t length = 0;
    const uint8_t *value = NULL;

    TEST_ASSERT_EQUAL_INT(-EBADMSG, simple_tlv_decode(length5, sizeof(length5), &tag, &length, &value));

    TEST_ASSERT_EQUAL_INT(-EBADMSG, simple_tlv_decode(length0, sizeof(length0), &tag, &length, &value));

    /* 254 = 0x00FE in long form, with all 254 bytes present */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x01;
    long_buf[1] = 0xFF;
    long_buf[2] = 0x00;
    long_buf[3] = 0xFE;
    memset(&long_buf[4], 0x42, 254);
    TEST_ASSERT_EQUAL_INT(-EBADMSG, simple_tlv_decode(long_buf, 4 + 254, &tag, &length, &value));
}

static void test_simple_tlv_decode_error_untouched(void)
{
    const uint8_t short_value[] = { 0x01, 0x05, 'a' };
    const uint8_t long_hdr[] = { 0x01, 0xFF, 0x01 };
    const uint8_t long_nodata[] = { 0x01, 0xFF, 0x01, 0x00 };
    const uint8_t noncanon[] = { 0x01, 0xFF, 0x00, 0x05, 'a', 'b', 'c', 'd', 'e' };
    tlv_tag_t tag;
    size_t length;
    const uint8_t *value;

    /* on -EBADMSG all outputs keep their old values */
    tag = 0x55;
    length = 0x1234;
    value = NULL;
    TEST_ASSERT_EQUAL_INT(-EBADMSG, simple_tlv_decode(short_value, sizeof(short_value), &tag, &length, &value));
    TEST_ASSERT_EQUAL_INT(0x55, tag);
    TEST_ASSERT_EQUAL_INT(0x1234, length);
    TEST_ASSERT(value == NULL);

    tag = 0x55;
    length = 0x1234;
    value = NULL;
    TEST_ASSERT_EQUAL_INT(-EBADMSG, simple_tlv_decode(long_hdr, sizeof(long_hdr), &tag, &length, &value));
    TEST_ASSERT_EQUAL_INT(0x55, tag);
    TEST_ASSERT_EQUAL_INT(0x1234, length);
    TEST_ASSERT(value == NULL);

    tag = 0x55;
    length = 0x1234;
    value = NULL;
    TEST_ASSERT_EQUAL_INT(-EBADMSG, simple_tlv_decode(long_nodata, sizeof(long_nodata), &tag, &length, &value));
    TEST_ASSERT_EQUAL_INT(0x55, tag);
    TEST_ASSERT_EQUAL_INT(0x1234, length);
    TEST_ASSERT(value == NULL);

    tag = 0x55;
    length = 0x1234;
    value = NULL;
    TEST_ASSERT_EQUAL_INT(-EBADMSG, simple_tlv_decode(noncanon, sizeof(noncanon), &tag, &length, &value));
    TEST_ASSERT_EQUAL_INT(0x55, tag);
    TEST_ASSERT_EQUAL_INT(0x1234, length);
    TEST_ASSERT(value == NULL);
}

/*
 * MARK: - Lookup
 */

static void test_simple_tlv_find_hit(void)
{
    /* tag 1 "abc", tag 2 (empty), tag 3 "z", tag 4 "xy" */
    const uint8_t raw[] = { 0x01, 0x03, 'a', 'b', 'c',
                            0x02, 0x00,
                            0x03, 0x01, 'z',
                            0x04, 0x02, 'x', 'y' };
    const uint8_t *value = NULL;
    ssize_t r;

    /* the first element */
    r = simple_tlv_find(raw, sizeof(raw), 0x01, &value);
    TEST_ASSERT_EQUAL_INT(sizeof("abc") - 1, r);
    TEST_ASSERT(value == &raw[2]);

    /* an element in the middle */
    value = NULL;
    r = simple_tlv_find(raw, sizeof(raw), 0x03, &value);
    TEST_ASSERT_EQUAL_INT(sizeof("z") - 1, r);
    TEST_ASSERT(value == &raw[9]);

    /* an empty value is a hit, not an error */
    r = simple_tlv_find(raw, sizeof(raw), 0x02, &value);
    TEST_ASSERT_EQUAL_INT(sizeof("") - 1, r);

    /* the very last element of the buffer */
    value = NULL;
    r = simple_tlv_find(raw, sizeof(raw), 0x04, &value);
    TEST_ASSERT_EQUAL_INT(sizeof("xy") - 1, r);
    TEST_ASSERT(value == &raw[12]);
}

static void test_simple_tlv_find_first_of_duplicates(void)
{
    const uint8_t raw[] = { 0x01, 0x03, 'a', 'b', 'c',
                            0x02, 0x00,
                            0x01, 0x02, 'x', 'y' };
    const uint8_t *value = NULL;

    ssize_t r = simple_tlv_find(raw, sizeof(raw), 0x01, &value);

    TEST_ASSERT_EQUAL_INT(sizeof("abc") - 1, r);
    TEST_ASSERT(value == &raw[2]);
}

#include "stdio.h"
#include "errno.h"
static void test_simple_tlv_find_missing(void)
{
    const uint8_t raw[] = { 0x01, 0x03, 'a', 'b', 'c',
                            0x02, 0x00,
                            0x03, 0x01, 'z',
                            0x01, 0x02, 'x', 'y' };
    const uint8_t *value = NULL;

    TEST_ASSERT_EQUAL_INT(-ENOENT, simple_tlv_find(raw, sizeof(raw), 0x09, &value));
    TEST_ASSERT_EQUAL_INT(-ENOENT, simple_tlv_find(raw, sizeof(raw), 0x00, &value));
    TEST_ASSERT(value == NULL);

    /* only the first element (5 bytes) is in range */
    TEST_ASSERT_EQUAL_INT(sizeof("abc") - 1, simple_tlv_find(raw, 5, 0x01, &value));
    TEST_ASSERT_EQUAL_INT(-ENOENT, simple_tlv_find(raw, 5, 0x02, &value));

    /* only the first two elements (7 bytes) are in range */
    TEST_ASSERT_EQUAL_INT(-ENOENT, simple_tlv_find(raw, 7, 0x03, &value));
}

static void test_simple_tlv_find_empty(void)
{
    const uint8_t raw[] = { 0x01, 0x01, 'a' };
    const uint8_t *value = NULL;

    TEST_ASSERT_EQUAL_INT(-ENOENT, simple_tlv_find(raw, 0, 0x00, &value));
    TEST_ASSERT_EQUAL_INT(-ENOENT, simple_tlv_find(raw, 0, 0x01, &value));
}

static void test_simple_tlv_find_after_long_element(void)
{
    const uint8_t *value = NULL;
    ssize_t r;

    /* tag 5 with 300 bytes, then tag 6 with "z": must skip the long element */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x05;
    long_buf[1] = 0xFF;
    long_buf[2] = 0x01;
    long_buf[3] = 0x2C;
    memset(&long_buf[4], 'x', sizeof(long_ref));
    long_buf[4 + sizeof(long_ref)] = 0x06;
    long_buf[4 + sizeof(long_ref) + 1] = sizeof("z") - 1;
    long_buf[4 + sizeof(long_ref) + 2] = 'z';

    r = simple_tlv_find(long_buf, 4 + sizeof(long_ref) + 3, 0x06, &value);
    TEST_ASSERT_EQUAL_INT(sizeof("z") - 1, r);
    TEST_ASSERT(value == &long_buf[4 + sizeof(long_ref) + 2]);

    /* and the long element itself */
    value = NULL;
    r = simple_tlv_find(long_buf, 4 + sizeof(long_ref) + 3, 0x05, &value);
    TEST_ASSERT_EQUAL_INT(sizeof(long_ref), r);
    TEST_ASSERT(value == &long_buf[4]);
}

static void test_simple_tlv_find_malformed(void)
{
    const uint8_t trailing[] = { 0x01, 0x01, 'a', 0x02 };     /* element + lone byte */
    const uint8_t truncated[] = { 0x01, 0x05, 'a' };
    const uint8_t noncanon[] = { 0x01, 0xFF, 0x00, 0x05, 'a', 'b', 'c', 'd', 'e' };
    const uint8_t *value = NULL;

    /* a match before the damage is still found */
    TEST_ASSERT_EQUAL_INT(1, simple_tlv_find(trailing, sizeof(trailing), 0x01, &value));
    /* damage reached before a match */
    TEST_ASSERT_EQUAL_INT(-EBADMSG, simple_tlv_find(trailing, sizeof(trailing), 0x02, &value));
    TEST_ASSERT_EQUAL_INT(-EBADMSG, simple_tlv_find(trailing, sizeof(trailing), 0x09, &value));

    TEST_ASSERT_EQUAL_INT(-EBADMSG, simple_tlv_find(truncated, sizeof(truncated), 0x01, &value));
    TEST_ASSERT_EQUAL_INT(-EBADMSG, simple_tlv_find(truncated, sizeof(truncated), 0x09, &value));

    TEST_ASSERT_EQUAL_INT(-EBADMSG, simple_tlv_find(noncanon, sizeof(noncanon), 0x01, &value));
}

/*
 * MARK: - Encoding
 */

static void test_simple_tlv_encode_short(void)
{
    uint8_t buf[8] = { 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE };
    const uint8_t expected[5] = { 0x01, 0x03, 'a', 'b', 'c' };

    /* capacity is exactly the encoded size */
    ssize_t r = simple_tlv_encode(buf, sizeof(expected), 0x01, sizeof("abc") - 1, (const uint8_t *)"abc");

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(expected)]);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(buf) - 1]);
}

static void test_simple_tlv_encode_zero_length(void)
{
    uint8_t buf[4] = { 0xEE, 0xEE, 0xEE, 0xEE };

    ssize_t r = simple_tlv_encode(buf, 2, 0x03, sizeof("") - 1, NULL);

    TEST_ASSERT_EQUAL_INT(2, r);
    TEST_ASSERT_EQUAL_INT(0x03, buf[0]);
    TEST_ASSERT_EQUAL_INT(0x00, buf[1]);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[2]);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[3]);
}

static void test_simple_tlv_encode_reserved(void)
{
    uint8_t buf[6] = { 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE };

    TEST_ASSERT_EQUAL_INT(-EINVAL, simple_tlv_encode(buf, sizeof(buf), 0x00, sizeof("a") - 1, (const uint8_t *)"a"));
    TEST_ASSERT_EQUAL_INT(-EINVAL, simple_tlv_encode(&buf[3], sizeof(buf) - 3, 0xFF, sizeof("b") - 1, (const uint8_t *)"b"));
    for (size_t i = 0; i < sizeof(buf); i += 1) {
        TEST_ASSERT(buf[i] == 0xEE);
    }
}

static void test_simple_tlv_encode_longest_short(void)
{
    memset(long_buf, 0xEE, sizeof(long_buf));
    memset(long_ref, 0xAB, 255);

    /* 254 (0xFE) fits the single length byte */
    ssize_t r = simple_tlv_encode(long_buf, 2 + 254, 0x01, 254, long_ref);

    TEST_ASSERT_EQUAL_INT(2 + 254, r);
    TEST_ASSERT_EQUAL_INT(0x01, long_buf[0]);
    TEST_ASSERT_EQUAL_INT(0xFE, long_buf[1]);
    TEST_ASSERT(memcmp(&long_buf[2], long_ref, 254) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, long_buf[2 + 254]);
}

static void test_simple_tlv_encode_shortest_long(void)
{
    memset(long_buf, 0xEE, sizeof(long_buf));
    memset(long_ref, 0xAB, 255);

    /* 255 (0xFF) needs the long form */
    ssize_t r = simple_tlv_encode(long_buf, 4 + 255, 0x01, 255, long_ref);

    TEST_ASSERT_EQUAL_INT(4 + 255, r);
    TEST_ASSERT_EQUAL_INT(0x01, long_buf[0]);
    TEST_ASSERT_EQUAL_INT(0xFF, long_buf[1]);
    TEST_ASSERT_EQUAL_INT(0x00, long_buf[2]);
    TEST_ASSERT_EQUAL_INT(0xFF, long_buf[3]);
    TEST_ASSERT(memcmp(&long_buf[4], long_ref, 255) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, long_buf[4 + 255]);
}

static void test_simple_tlv_encode_long(void)
{
    memset(long_buf, 0xEE, sizeof(long_buf));
    memset(long_ref, 0xAB, sizeof(long_ref));

    /* 300 = 0x012C */
    ssize_t r = simple_tlv_encode(long_buf, 4 + sizeof(long_ref), 0x02, sizeof(long_ref), long_ref);

    TEST_ASSERT_EQUAL_INT(4 + sizeof(long_ref), r);
    TEST_ASSERT_EQUAL_INT(0x02, long_buf[0]);
    TEST_ASSERT_EQUAL_INT(0xFF, long_buf[1]);
    TEST_ASSERT_EQUAL_INT(0x01, long_buf[2]);
    TEST_ASSERT_EQUAL_INT(0x2C, long_buf[3]);
    TEST_ASSERT(memcmp(&long_buf[4], long_ref, sizeof(long_ref)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, long_buf[4 + sizeof(long_ref)]);
}

static void test_simple_tlv_encode_nobufs(void)
{
    uint8_t buf[8];
    const uint8_t untouched[8] = { 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE };
    ssize_t r;
    int bad;

    /* needs 5, one byte short */
    memset(buf, 0xEE, sizeof(buf));
    r = simple_tlv_encode(buf, 4, 0x01, sizeof("abc") - 1, (const uint8_t *)"abc");
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, r);
    TEST_ASSERT(memcmp(buf, untouched, sizeof(untouched)) == 0);

    /* needs 2, one byte short */
    memset(buf, 0xEE, sizeof(buf));
    r = simple_tlv_encode(buf, 1, 0x01, sizeof("") - 1, NULL);
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, r);
    TEST_ASSERT(memcmp(buf, untouched, sizeof(untouched)) == 0);

    /* needs 5, no room at all */
    memset(buf, 0xEE, sizeof(buf));
    r = simple_tlv_encode(buf, 0, 0x01, sizeof("abc") - 1, (const uint8_t *)"abc");
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, r);
    TEST_ASSERT(memcmp(buf, untouched, sizeof(untouched)) == 0);

    /* 255 bytes need 259: one byte short */
    memset(long_buf, 0xEE, sizeof(long_buf));
    memset(long_ref, 'x', 255);
    r = simple_tlv_encode(long_buf, 4 + 254, 0x01, 255, long_ref);
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
    r = simple_tlv_encode(long_buf, 3, 0x01, 255, long_ref);
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, r);
    bad = 0;
    for (size_t i = 0; i < sizeof(long_buf); i++) {
        if (long_buf[i] != 0xEE) {
            bad++;
        }
    }
    TEST_ASSERT_EQUAL_INT(0, bad);
}

/*
 * MARK: - Add
 */

static void test_simple_tlv_writer_add_sequence(void)
{
    uint8_t buf[16];
    const uint8_t expected[10] = { 0x01, 0x03, 'a', 'b', 'c',
                                   0x02, 0x00,
                                   0x03, 0x01, 'z' };
    tlv_writer_t w = SIMPLE_TLV_WRITER(buf, 0, sizeof(buf));

    memset(buf, 0xEE, sizeof(buf));

    TEST_ASSERT_EQUAL_INT(5, tlv_writer_add(&w, 0x01, sizeof("abc") - 1, (const uint8_t *)"abc"));
    TEST_ASSERT_EQUAL_INT(5, w.size);
    TEST_ASSERT_EQUAL_INT(7, tlv_writer_add(&w, 0x02, sizeof("") - 1, NULL));
    TEST_ASSERT_EQUAL_INT(7, w.size);
    TEST_ASSERT_EQUAL_INT(sizeof(expected), tlv_writer_add(&w, 0x03, sizeof("z") - 1, (const uint8_t *)"z"));
    TEST_ASSERT_EQUAL_INT(sizeof(expected), w.size);

    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(expected)]);
    TEST_ASSERT(w.buffer == buf);
    TEST_ASSERT_EQUAL_INT(sizeof(buf), w.capacity);
}

static void test_simple_tlv_writer_add_behind_existing(void)
{
    uint8_t buf[12];
    const uint8_t initial[5] = { 0x01, 0x03, 'a', 'a', 'a' };
    const uint8_t expected[9] = { 0x01, 0x03, 'a', 'a', 'a',
                                  0x02, 0x02, 'n', 'n' };
    tlv_writer_t w = SIMPLE_TLV_WRITER(buf, sizeof(initial), sizeof(expected));

    memset(buf, 0xEE, sizeof(buf));
    memcpy(buf, initial, sizeof(initial));

    /* capacity is exactly the new size */
    TEST_ASSERT_EQUAL_INT(sizeof(expected), tlv_writer_add(&w, 0x02, sizeof("nn") - 1, (const uint8_t *)"nn"));
    TEST_ASSERT_EQUAL_INT(sizeof(expected), w.size);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(expected)]);
}

static void test_simple_tlv_writer_add_long(void)
{
    const uint8_t initial[3] = { 0x01, 0x01, 'a' };
    tlv_writer_t w = SIMPLE_TLV_WRITER(long_buf, sizeof(initial), sizeof(long_buf));

    memset(long_buf, 0xEE, sizeof(long_buf));
    memcpy(long_buf, initial, sizeof(initial));
    memset(long_ref, 'n', sizeof(long_ref));

    ssize_t r = tlv_writer_add(&w, 0x02, sizeof(long_ref), long_ref);

    TEST_ASSERT_EQUAL_INT(sizeof(initial) + 4 + sizeof(long_ref), r);
    TEST_ASSERT_EQUAL_INT(sizeof(initial) + 4 + sizeof(long_ref), w.size);
    TEST_ASSERT_EQUAL_INT(0x02, long_buf[3]);
    TEST_ASSERT_EQUAL_INT(0xFF, long_buf[4]);
    TEST_ASSERT_EQUAL_INT(0x01, long_buf[5]);
    TEST_ASSERT_EQUAL_INT(0x2C, long_buf[6]);
    TEST_ASSERT(memcmp(&long_buf[7], long_ref, sizeof(long_ref)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, long_buf[7 + sizeof(long_ref)]);
}

static void test_simple_tlv_writer_add_nobufs(void)
{
    uint8_t buf[12];
    const uint8_t initial[5] = { 0x01, 0x03, 'a', 'a', 'a' };
    uint8_t unchanged[12];
    tlv_writer_t w = SIMPLE_TLV_WRITER(buf, sizeof(initial), 8);

    memset(buf, 0xEE, sizeof(buf));
    memcpy(buf, initial, sizeof(initial));
    memcpy(unchanged, buf, sizeof(buf));

    /* 3 bytes are free, tag 2 with "xyz" needs 5 */
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, tlv_writer_add(&w, 0x02, sizeof("xyz") - 1, (const uint8_t *)"xyz"));
    TEST_ASSERT_EQUAL_INT(sizeof(initial), w.size);
    TEST_ASSERT(memcmp(buf, unchanged, sizeof(unchanged)) == 0);

    /* an empty element needs 2 bytes: fits ... */
    TEST_ASSERT_EQUAL_INT(7, tlv_writer_add(&w, 0x02, sizeof("") - 1, NULL));
    TEST_ASSERT_EQUAL_INT(7, w.size);

    /* ... but one more does not (1 byte free) */
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, tlv_writer_add(&w, 0x03, sizeof("") - 1, NULL));
    TEST_ASSERT_EQUAL_INT(7, w.size);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[7]);
}

/*
 * MARK: - Set
 */

static void test_simple_tlv_writer_set_same_size(void)
{
    uint8_t buf[16];
    const uint8_t initial[12] = { 0x01, 0x03, 'a', 'a', 'a',
                                  0x02, 0x02, 'b', 'b',
                                  0x03, 0x01, 'c' };
    const uint8_t expected[12] = { 0x01, 0x03, 'a', 'a', 'a',
                                   0x02, 0x02, 'n', 'n',
                                   0x03, 0x01, 'c' };
    tlv_writer_t w = SIMPLE_TLV_WRITER(buf, sizeof(initial), sizeof(initial));

    memset(buf, 0xEE, sizeof(buf));
    memcpy(buf, initial, sizeof(initial));

    /* tag 2: "bb" -> "nn"; capacity == size */
    ssize_t r = tlv_writer_set(&w, 0x02, sizeof("nn") - 1, (const uint8_t *)"nn");

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT_EQUAL_INT(sizeof(expected), w.size);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(expected)]);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(buf) - 1]);
}

static void test_simple_tlv_writer_set_shrink_middle(void)
{
    uint8_t buf[24];
    const uint8_t initial[20] = { 0x01, 0x03, 'a', 'a', 'a',
                                  0x02, 0x0A, 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b',
                                  0x03, 0x01, 'c' };
    const uint8_t expected[11] = { 0x01, 0x03, 'a', 'a', 'a',
                                   0x02, 0x01, 'n',
                                   0x03, 0x01, 'c' };
    tlv_writer_t w = SIMPLE_TLV_WRITER(buf, sizeof(initial), sizeof(initial));

    memset(buf, 0xEE, sizeof(buf));
    memcpy(buf, initial, sizeof(initial));

    /* tag 2: 10 bytes -> 1 byte, the tail moves down */
    ssize_t r = tlv_writer_set(&w, 0x02, sizeof("n") - 1, (const uint8_t *)"n");

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT_EQUAL_INT(sizeof(expected), w.size);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(initial)]);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(buf) - 1]);
}

static void test_simple_tlv_writer_set_grow_middle(void)
{
    uint8_t buf[18];
    const uint8_t initial[12] = { 0x01, 0x03, 'a', 'a', 'a',
                                  0x02, 0x02, 'b', 'b',
                                  0x03, 0x01, 'c' };
    const uint8_t expected[14] = { 0x01, 0x03, 'a', 'a', 'a',
                                   0x02, 0x04, 'n', 'n', 'n', 'n',
                                   0x03, 0x01, 'c' };
    tlv_writer_t w = SIMPLE_TLV_WRITER(buf, sizeof(initial), sizeof(expected));

    memset(buf, 0xEE, sizeof(buf));
    memcpy(buf, initial, sizeof(initial));

    /* tag 2: 2 -> 4 bytes, capacity is exactly the new size */
    ssize_t r = tlv_writer_set(&w, 0x02, sizeof("nnnn") - 1, (const uint8_t *)"nnnn");

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT_EQUAL_INT(sizeof(expected), w.size);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(expected)]);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(buf) - 1]);
}

static void test_simple_tlv_writer_set_grow_last(void)
{
    uint8_t buf[18];
    const uint8_t initial[12] = { 0x01, 0x03, 'a', 'a', 'a',
                                  0x02, 0x02, 'b', 'b',
                                  0x03, 0x01, 'c' };
    const uint8_t expected[15] = { 0x01, 0x03, 'a', 'a', 'a',
                                   0x02, 0x02, 'b', 'b',
                                   0x03, 0x04, 'n', 'n', 'n', 'n' };
    tlv_writer_t w = SIMPLE_TLV_WRITER(buf, sizeof(initial), sizeof(expected));

    memset(buf, 0xEE, sizeof(buf));
    memcpy(buf, initial, sizeof(initial));

    /* the last element grows: nothing to move */
    ssize_t r = tlv_writer_set(&w, 0x03, sizeof("nnnn") - 1, (const uint8_t *)"nnnn");

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT_EQUAL_INT(sizeof(expected), w.size);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(expected)]);
}

static void test_simple_tlv_writer_set_to_zero_length(void)
{
    uint8_t buf[16];
    const uint8_t initial[12] = { 0x01, 0x03, 'a', 'a', 'a',
                                  0x02, 0x02, 'b', 'b',
                                  0x03, 0x01, 'c' };
    const uint8_t expected[10] = { 0x01, 0x03, 'a', 'a', 'a',
                                   0x02, 0x00,
                                   0x03, 0x01, 'c' };
    tlv_writer_t w = SIMPLE_TLV_WRITER(buf, sizeof(initial), sizeof(initial));

    memset(buf, 0xEE, sizeof(buf));
    memcpy(buf, initial, sizeof(initial));

    ssize_t r = tlv_writer_set(&w, 0x02, sizeof("") - 1, NULL);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT_EQUAL_INT(sizeof(expected), w.size);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(initial)]);
}

static void test_simple_tlv_writer_set_from_zero_length(void)
{
    uint8_t buf[16];
    const uint8_t initial[10] = { 0x01, 0x03, 'a', 'a', 'a',
                                  0x02, 0x00,
                                  0x03, 0x01, 'c' };
    const uint8_t expected[13] = { 0x01, 0x03, 'a', 'a', 'a',
                                   0x02, 0x03, 'n', 'n', 'n',
                                   0x03, 0x01, 'c' };
    tlv_writer_t w = SIMPLE_TLV_WRITER(buf, sizeof(initial), sizeof(expected));

    memset(buf, 0xEE, sizeof(buf));
    memcpy(buf, initial, sizeof(initial));

    ssize_t r = tlv_writer_set(&w, 0x02, sizeof("nnn") - 1, (const uint8_t *)"nnn");

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT_EQUAL_INT(sizeof(expected), w.size);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(expected)]);
}

static void test_simple_tlv_writer_set_first_of_duplicates(void)
{
    uint8_t buf[16];
    const uint8_t initial[9] = { 0x01, 0x01, 'a',
                                 0x02, 0x01, 'b',
                                 0x01, 0x01, 'c' };
    const uint8_t expected[10] = { 0x01, 0x02, 'n', 'n',
                                   0x02, 0x01, 'b',
                                   0x01, 0x01, 'c' };
    tlv_writer_t w = SIMPLE_TLV_WRITER(buf, sizeof(initial), sizeof(expected));

    memset(buf, 0xEE, sizeof(buf));
    memcpy(buf, initial, sizeof(initial));

    /* only the first tag 1 is replaced */
    ssize_t r = tlv_writer_set(&w, 0x01, sizeof("nn") - 1, (const uint8_t *)"nn");

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT_EQUAL_INT(sizeof(expected), w.size);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(expected)]);
}

static void test_simple_tlv_writer_set_append_missing(void)
{
    uint8_t buf[18];
    const uint8_t initial[9] = { 0x01, 0x03, 'a', 'a', 'a',
                                 0x02, 0x02, 'b', 'b' };
    const uint8_t expected[14] = { 0x01, 0x03, 'a', 'a', 'a',
                                   0x02, 0x02, 'b', 'b',
                                   0x09, 0x03, 'n', 'n', 'n' };
    tlv_writer_t w = SIMPLE_TLV_WRITER(buf, sizeof(initial), sizeof(expected));

    memset(buf, 0xEE, sizeof(buf));
    memcpy(buf, initial, sizeof(initial));

    ssize_t r = tlv_writer_set(&w, 0x09, sizeof("nnn") - 1, (const uint8_t *)"nnn");

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT_EQUAL_INT(sizeof(expected), w.size);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(expected)]);
}

static void test_simple_tlv_writer_set_append_to_empty(void)
{
    uint8_t buf[8];
    const uint8_t expected[4] = { 0x07, 0x02, 'x', 'y' };
    tlv_writer_t w = SIMPLE_TLV_WRITER(buf, 0, sizeof(expected));

    memset(buf, 0xEE, sizeof(buf));

    ssize_t r = tlv_writer_set(&w, 0x07, sizeof("xy") - 1, (const uint8_t *)"xy");

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT_EQUAL_INT(sizeof(expected), w.size);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(expected)]);
}

static void test_simple_tlv_writer_set_254_to_255(void)
{
    const uint8_t head[3] = { 0x01, 0x01, 'a' };
    const uint8_t tail[3] = { 0x03, 0x01, 'c' };
    ssize_t r;

    /* the value crosses from the short to the long form: 254 -> 255 bytes */
    memset(long_buf, 0xEE, sizeof(long_buf));
    memcpy(long_buf, head, sizeof(head));
    long_buf[3] = 0x02;
    long_buf[4] = 0xFE;
    memset(&long_buf[5], 'b', 254);
    memcpy(&long_buf[5 + 254], tail, sizeof(tail));
    memset(long_ref, 'n', 255);

    tlv_writer_t w = SIMPLE_TLV_WRITER(long_buf, 5 + 254 + sizeof(tail),
                                       7 + 255 + sizeof(tail));

    r = tlv_writer_set(&w, 0x02, 255, long_ref);

    TEST_ASSERT_EQUAL_INT(7 + 255 + sizeof(tail), r);
    TEST_ASSERT_EQUAL_INT(7 + 255 + sizeof(tail), w.size);
    TEST_ASSERT(memcmp(long_buf, head, sizeof(head)) == 0);
    TEST_ASSERT_EQUAL_INT(0x02, long_buf[3]);
    TEST_ASSERT_EQUAL_INT(0xFF, long_buf[4]);
    TEST_ASSERT_EQUAL_INT(0x00, long_buf[5]);
    TEST_ASSERT_EQUAL_INT(0xFF, long_buf[6]);
    TEST_ASSERT(memcmp(&long_buf[7], long_ref, 255) == 0);
    TEST_ASSERT(memcmp(&long_buf[7 + 255], tail, sizeof(tail)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, long_buf[7 + 255 + sizeof(tail)]);
}

static void test_simple_tlv_writer_set_255_to_254(void)
{
    const uint8_t head[3] = { 0x01, 0x01, 'a' };
    const uint8_t tail[3] = { 0x03, 0x01, 'c' };
    ssize_t r;

    /* and back: 255 -> 254 bytes */
    memset(long_buf, 0xEE, sizeof(long_buf));
    memcpy(long_buf, head, sizeof(head));
    long_buf[3] = 0x02;
    long_buf[4] = 0xFF;
    long_buf[5] = 0x00;
    long_buf[6] = 0xFF;
    memset(&long_buf[7], 'b', 255);
    memcpy(&long_buf[7 + 255], tail, sizeof(tail));
    memset(long_ref, 'n', 255);

    tlv_writer_t w = SIMPLE_TLV_WRITER(long_buf, 7 + 255 + sizeof(tail),
                                       7 + 255 + sizeof(tail));

    r = tlv_writer_set(&w, 0x02, 254, long_ref);

    TEST_ASSERT_EQUAL_INT(5 + 254 + sizeof(tail), r);
    TEST_ASSERT_EQUAL_INT(5 + 254 + sizeof(tail), w.size);
    TEST_ASSERT(memcmp(long_buf, head, sizeof(head)) == 0);
    TEST_ASSERT_EQUAL_INT(0x02, long_buf[3]);
    TEST_ASSERT_EQUAL_INT(0xFE, long_buf[4]);
    TEST_ASSERT(memcmp(&long_buf[5], long_ref, 254) == 0);
    TEST_ASSERT(memcmp(&long_buf[5 + 254], tail, sizeof(tail)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, long_buf[7 + 255 + sizeof(tail)]);
}

static void test_simple_tlv_writer_set_nobufs(void)
{
    ssize_t r;

    /* tag 2: 2 -> 4 bytes needs 14, capacity is 13 */
    uint8_t grow[17];
    const uint8_t grow_initial[12] = { 0x01, 0x03, 'a', 'a', 'a',
                                       0x02, 0x02, 'b', 'b',
                                       0x03, 0x01, 'c' };
    uint8_t grow_unchanged[17];
    tlv_writer_t grow_w = SIMPLE_TLV_WRITER(grow, sizeof(grow_initial), sizeof(grow_initial) + 1);
    memset(grow, 0xEE, sizeof(grow));
    memcpy(grow, grow_initial, sizeof(grow_initial));
    memcpy(grow_unchanged, grow, sizeof(grow));
    r = tlv_writer_set(&grow_w, 0x02, sizeof("nnnn") - 1, (const uint8_t *)"nnnn");
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, r);
    TEST_ASSERT_EQUAL_INT(sizeof(grow_initial), grow_w.size);
    TEST_ASSERT(memcmp(grow, grow_unchanged, sizeof(grow_unchanged)) == 0);

    /* appending tag 9 with 4 bytes needs 15, capacity is 14 */
    uint8_t app[18];
    const uint8_t app_initial[9] = { 0x01, 0x03, 'a', 'a', 'a',
                                     0x02, 0x02, 'b', 'b' };
    uint8_t app_unchanged[18];
    tlv_writer_t app_w = SIMPLE_TLV_WRITER(app, sizeof(app_initial), sizeof(app_initial) + 5);
    memset(app, 0xEE, sizeof(app));
    memcpy(app, app_initial, sizeof(app_initial));
    memcpy(app_unchanged, app, sizeof(app));
    r = tlv_writer_set(&app_w, 0x09, sizeof("nnnn") - 1, (const uint8_t *)"nnnn");
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, r);
    TEST_ASSERT_EQUAL_INT(sizeof(app_initial), app_w.size);
    TEST_ASSERT(memcmp(app, app_unchanged, sizeof(app_unchanged)) == 0);

    /* an empty element needs 2 bytes, only 1 is free */
    app_w.capacity = sizeof(app_initial) + 1;
    r = tlv_writer_set(&app_w, 0x09, sizeof("") - 1, NULL);
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, r);
    TEST_ASSERT_EQUAL_INT(sizeof(app_initial), app_w.size);
    TEST_ASSERT(memcmp(app, app_unchanged, sizeof(app_unchanged)) == 0);
}

static void test_simple_tlv_writer_set_nobufs_long(void)
{
    ssize_t r;
    int bad;

    /* tag 1 "a", tag 2 with 254 bytes (259 in total); growing tag 2 to 255
     * bytes needs 262 but the capacity is 261 */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x01;
    long_buf[1] = 0x01;
    long_buf[2] = 'a';
    long_buf[3] = 0x02;
    long_buf[4] = 0xFE;
    memset(&long_buf[5], 'b', 254);
    memset(long_ref, 'n', 255);

    tlv_writer_t grow_w = SIMPLE_TLV_WRITER(long_buf, 5 + 254, 5 + 254 + 2);

    r = tlv_writer_set(&grow_w, 0x02, 255, long_ref);

    TEST_ASSERT_EQUAL_INT(-ENOBUFS, r);
    TEST_ASSERT_EQUAL_INT(5 + 254, grow_w.size);
    TEST_ASSERT_EQUAL_INT(0x01, long_buf[0]);
    TEST_ASSERT_EQUAL_INT(0x01, long_buf[1]);
    TEST_ASSERT_EQUAL_INT('a', long_buf[2]);
    TEST_ASSERT_EQUAL_INT(0x02, long_buf[3]);
    TEST_ASSERT_EQUAL_INT(0xFE, long_buf[4]);
    bad = 0;
    for (size_t i = 5; i < sizeof(long_buf); i++) {
        if (long_buf[i] != ((i < 5 + 254) ? 'b' : 0xEE)) {
            bad++;
        }
    }
    TEST_ASSERT_EQUAL_INT(0, bad);

    /* appending 300 bytes needs 304, only 303 are free */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x01;
    long_buf[1] = 0x01;
    long_buf[2] = 'a';
    memset(long_ref, 'n', sizeof(long_ref));

    tlv_writer_t app_w = SIMPLE_TLV_WRITER(long_buf, 3, 3 + 4 + sizeof(long_ref) - 1);

    r = tlv_writer_set(&app_w, 0x09, sizeof(long_ref), long_ref);

    TEST_ASSERT_EQUAL_INT(-ENOBUFS, r);
    TEST_ASSERT_EQUAL_INT(3, app_w.size);
    TEST_ASSERT_EQUAL_INT(0x01, long_buf[0]);
    TEST_ASSERT_EQUAL_INT(0x01, long_buf[1]);
    TEST_ASSERT_EQUAL_INT('a', long_buf[2]);
    bad = 0;
    for (size_t i = 3; i < sizeof(long_buf); i++) {
        if (long_buf[i] != 0xEE) {
            bad++;
        }
    }
    TEST_ASSERT_EQUAL_INT(0, bad);
}

static void test_simple_tlv_writer_set_malformed(void)
{
    ssize_t r;

    /* truncated value, tag present and absent */
    uint8_t truncated[16];
    const uint8_t truncated_initial[3] = { 0x01, 0x05, 'a' };
    uint8_t truncated_orig[16];
    tlv_writer_t truncated_w = SIMPLE_TLV_WRITER(truncated, sizeof(truncated_initial), sizeof(truncated));
    memset(truncated, 0xEE, sizeof(truncated));
    memcpy(truncated, truncated_initial, sizeof(truncated_initial));
    memcpy(truncated_orig, truncated, sizeof(truncated));
    r = tlv_writer_set(&truncated_w, 0x01, sizeof("nn") - 1, (const uint8_t *)"nn");
    TEST_ASSERT_EQUAL_INT(-EBADMSG, r);
    TEST_ASSERT_EQUAL_INT(sizeof(truncated_initial), truncated_w.size);
    TEST_ASSERT(memcmp(truncated, truncated_orig, sizeof(truncated_orig)) == 0);
    r = tlv_writer_set(&truncated_w, 0x09, sizeof("nn") - 1, (const uint8_t *)"nn");
    TEST_ASSERT_EQUAL_INT(-EBADMSG, r);
    TEST_ASSERT_EQUAL_INT(sizeof(truncated_initial), truncated_w.size);
    TEST_ASSERT(memcmp(truncated, truncated_orig, sizeof(truncated_orig)) == 0);

    /* a lone byte after a valid element, tag absent */
    uint8_t trailing[16];
    const uint8_t trailing_initial[4] = { 0x01, 0x01, 'a', 0x02 };
    uint8_t trailing_orig[16];
    tlv_writer_t trailing_w = SIMPLE_TLV_WRITER(trailing, sizeof(trailing_initial), sizeof(trailing));
    memset(trailing, 0xEE, sizeof(trailing));
    memcpy(trailing, trailing_initial, sizeof(trailing_initial));
    memcpy(trailing_orig, trailing, sizeof(trailing));
    r = tlv_writer_set(&trailing_w, 0x09, sizeof("nn") - 1, (const uint8_t *)"nn");
    TEST_ASSERT_EQUAL_INT(-EBADMSG, r);
    TEST_ASSERT_EQUAL_INT(sizeof(trailing_initial), trailing_w.size);
    TEST_ASSERT(memcmp(trailing, trailing_orig, sizeof(trailing_orig)) == 0);

    /* non-canonical long form */
    uint8_t noncanon[16];
    const uint8_t noncanon_initial[9] = { 0x01, 0xFF, 0x00, 0x05, 'a', 'b', 'c', 'd', 'e' };
    uint8_t noncanon_orig[16];
    tlv_writer_t noncanon_w = SIMPLE_TLV_WRITER(noncanon, sizeof(noncanon_initial), sizeof(noncanon));
    memset(noncanon, 0xEE, sizeof(noncanon));
    memcpy(noncanon, noncanon_initial, sizeof(noncanon_initial));
    memcpy(noncanon_orig, noncanon, sizeof(noncanon));
    r = tlv_writer_set(&noncanon_w, 0x01, sizeof("nn") - 1, (const uint8_t *)"nn");
    TEST_ASSERT_EQUAL_INT(-EBADMSG, r);
    TEST_ASSERT_EQUAL_INT(sizeof(noncanon_initial), noncanon_w.size);
    TEST_ASSERT(memcmp(noncanon, noncanon_orig, sizeof(noncanon_orig)) == 0);
}

/*
 * MARK: - Remove
 */

static void test_simple_tlv_writer_remove_middle(void)
{
    uint8_t buf[12] = { 0x01, 0x03, 'a', 'a', 'a',
                        0x02, 0x02, 'b', 'b',
                        0x03, 0x01, 'c' };
    const uint8_t expected[8] = { 0x01, 0x03, 'a', 'a', 'a',
                                  0x03, 0x01, 'c' };
    tlv_writer_t w = SIMPLE_TLV_WRITER(buf, sizeof(buf), sizeof(buf));

    ssize_t r = tlv_writer_remove(&w, 0x02);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT_EQUAL_INT(sizeof(expected), w.size);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
}

static void test_simple_tlv_writer_remove_first(void)
{
    uint8_t buf[12] = { 0x01, 0x03, 'a', 'a', 'a',
                        0x02, 0x02, 'b', 'b',
                        0x03, 0x01, 'c' };
    const uint8_t expected[7] = { 0x02, 0x02, 'b', 'b',
                                  0x03, 0x01, 'c' };
    tlv_writer_t w = SIMPLE_TLV_WRITER(buf, sizeof(buf), sizeof(buf));

    ssize_t r = tlv_writer_remove(&w, 0x01);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT_EQUAL_INT(sizeof(expected), w.size);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
}

static void test_simple_tlv_writer_remove_last(void)
{
    uint8_t buf[12] = { 0x01, 0x03, 'a', 'a', 'a',
                        0x02, 0x02, 'b', 'b',
                        0x03, 0x01, 'c' };
    const uint8_t expected[9] = { 0x01, 0x03, 'a', 'a', 'a',
                                  0x02, 0x02, 'b', 'b' };
    tlv_writer_t w = SIMPLE_TLV_WRITER(buf, sizeof(buf), sizeof(buf));

    ssize_t r = tlv_writer_remove(&w, 0x03);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT_EQUAL_INT(sizeof(expected), w.size);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
}

static void test_simple_tlv_writer_remove_no_match(void)
{
    uint8_t buf[12] = { 0x01, 0x03, 'a', 'a', 'a',
                        0x02, 0x02, 'b', 'b',
                        0x03, 0x01, 'c' };
    const uint8_t original[12] = { 0x01, 0x03, 'a', 'a', 'a',
                                   0x02, 0x02, 'b', 'b',
                                   0x03, 0x01, 'c' };
    tlv_writer_t w = SIMPLE_TLV_WRITER(buf, sizeof(buf), sizeof(buf));

    TEST_ASSERT_EQUAL_INT(sizeof(original), tlv_writer_remove(&w, 0x09));
    TEST_ASSERT_EQUAL_INT(sizeof(original), w.size);
    TEST_ASSERT(memcmp(buf, original, sizeof(original)) == 0);

    TEST_ASSERT_EQUAL_INT(sizeof(original), tlv_writer_remove(&w, 0x00));
    TEST_ASSERT_EQUAL_INT(sizeof(original), w.size);
    TEST_ASSERT(memcmp(buf, original, sizeof(original)) == 0);
}

static void test_simple_tlv_writer_remove_empty_writer(void)
{
    uint8_t buf[4] = { 0xEE, 0xEE, 0xEE, 0xEE };
    tlv_writer_t w = SIMPLE_TLV_WRITER(buf, 0, sizeof(buf));

    TEST_ASSERT_EQUAL_INT(0, tlv_writer_remove(&w, 0x00));
    TEST_ASSERT_EQUAL_INT(0, tlv_writer_remove(&w, 0x01));
    TEST_ASSERT_EQUAL_INT(0, w.size);
}

static void test_simple_tlv_writer_remove_ignores_spare_room(void)
{
    uint8_t buf[12] = { 0x01, 0x03, 'a', 'a', 'a',
                        0x02, 0x02, 'b', 'b',
                        0x01, 0x01, 'c' };
    const uint8_t expected[4] = { 0x02, 0x02, 'b', 'b' };
    tlv_writer_t w = SIMPLE_TLV_WRITER(buf, 9, sizeof(buf));

    /* the element behind w.size is not part of the writer */
    ssize_t r = tlv_writer_remove(&w, 0x01);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT_EQUAL_INT(sizeof(expected), w.size);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
}

static void test_simple_tlv_writer_remove_adjacent_duplicates(void)
{
    uint8_t buf[13] = { 0x01, 0x01, 'a',
                        0x02, 0x02, 'b', 'b',
                        0x02, 0x01, 'c',
                        0x03, 0x01, 'd' };
    const uint8_t expected[6] = { 0x01, 0x01, 'a',
                                  0x03, 0x01, 'd' };
    tlv_writer_t w = SIMPLE_TLV_WRITER(buf, sizeof(buf), sizeof(buf));

    ssize_t r = tlv_writer_remove(&w, 0x02);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT_EQUAL_INT(sizeof(expected), w.size);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
}

static void test_simple_tlv_writer_remove_interleaved_duplicates(void)
{
    uint8_t buf[17] = { 0x02, 0x01, 'a',
                        0x01, 0x01, 'b',
                        0x02, 0x00,
                        0x03, 0x02, 'c', 'c',
                        0x02, 0x03, 'd', 'd', 'd' };
    const uint8_t expected[7] = { 0x01, 0x01, 'b',
                                  0x03, 0x02, 'c', 'c' };
    tlv_writer_t w = SIMPLE_TLV_WRITER(buf, sizeof(buf), sizeof(buf));

    ssize_t r = tlv_writer_remove(&w, 0x02);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT_EQUAL_INT(sizeof(expected), w.size);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
}

static void test_simple_tlv_writer_remove_every_element(void)
{
    uint8_t buf[8] = { 0x05, 0x01, 'a',
                       0x05, 0x02, 'b', 'b',
                       0x05 };
    tlv_writer_t w = SIMPLE_TLV_WRITER(buf, sizeof(buf) - 1, sizeof(buf));

    /* the stray last byte is outside of the writer */
    TEST_ASSERT_EQUAL_INT(0, tlv_writer_remove(&w, 0x05));
    TEST_ASSERT_EQUAL_INT(0, w.size);
}

static void test_simple_tlv_writer_remove_long_element(void)
{
    const uint8_t expected[4] = { 0x02, 0x02, 'b', 'b' };
    ssize_t r;

    /* tag 1 with 300 bytes, tag 2 "bb", tag 1 with 255 bytes: remove tag 1 */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x01;
    long_buf[1] = 0xFF;
    long_buf[2] = 0x01;
    long_buf[3] = 0x2C;
    memset(&long_buf[4], 'l', sizeof(long_ref));
    long_buf[4 + sizeof(long_ref)] = 0x02;
    long_buf[4 + sizeof(long_ref) + 1] = sizeof("bb") - 1;
    long_buf[4 + sizeof(long_ref) + 2] = 'b';
    long_buf[4 + sizeof(long_ref) + 3] = 'b';
    long_buf[4 + sizeof(long_ref) + 4] = 0x01;
    long_buf[4 + sizeof(long_ref) + 5] = 0xFF;
    long_buf[4 + sizeof(long_ref) + 6] = 0x00;
    long_buf[4 + sizeof(long_ref) + 7] = 0xFF;
    memset(&long_buf[4 + sizeof(long_ref) + 8], 'm', 255);

    tlv_writer_t w = SIMPLE_TLV_WRITER(long_buf, 4 + sizeof(long_ref) + 8 + 255, sizeof(long_buf));

    r = tlv_writer_remove(&w, 0x01);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT_EQUAL_INT(sizeof(expected), w.size);
    TEST_ASSERT(memcmp(long_buf, expected, sizeof(expected)) == 0);
}

static void test_simple_tlv_writer_remove_keeps_long_elements(void)
{
    ssize_t r;

    /* tag 2 with 300 bytes, tag 1 "aa", tag 3 with 255 bytes: remove tag 1 */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x02;
    long_buf[1] = 0xFF;
    long_buf[2] = 0x01;
    long_buf[3] = 0x2C;
    memset(&long_buf[4], 'l', sizeof(long_ref));
    long_buf[4 + sizeof(long_ref)] = 0x01;
    long_buf[4 + sizeof(long_ref) + 1] = sizeof("aa") - 1;
    long_buf[4 + sizeof(long_ref) + 2] = 'a';
    long_buf[4 + sizeof(long_ref) + 3] = 'a';
    long_buf[4 + sizeof(long_ref) + 4] = 0x03;
    long_buf[4 + sizeof(long_ref) + 5] = 0xFF;
    long_buf[4 + sizeof(long_ref) + 6] = 0x00;
    long_buf[4 + sizeof(long_ref) + 7] = 0xFF;
    memset(&long_buf[4 + sizeof(long_ref) + 8], 'm', 255);

    tlv_writer_t w = SIMPLE_TLV_WRITER(long_buf, 4 + sizeof(long_ref) + 8 + 255, sizeof(long_buf));

    r = tlv_writer_remove(&w, 0x01);

    TEST_ASSERT_EQUAL_INT(4 + sizeof(long_ref) + 4 + 255, r);
    TEST_ASSERT_EQUAL_INT(4 + sizeof(long_ref) + 4 + 255, w.size);
    TEST_ASSERT_EQUAL_INT(0x02, long_buf[0]);
    TEST_ASSERT_EQUAL_INT(0xFF, long_buf[1]);
    TEST_ASSERT_EQUAL_INT(0x01, long_buf[2]);
    TEST_ASSERT_EQUAL_INT(0x2C, long_buf[3]);
    memset(long_ref, 'l', sizeof(long_ref));
    TEST_ASSERT(memcmp(&long_buf[4], long_ref, sizeof(long_ref)) == 0);
    TEST_ASSERT_EQUAL_INT(0x03, long_buf[4 + sizeof(long_ref)]);
    TEST_ASSERT_EQUAL_INT(0xFF, long_buf[4 + sizeof(long_ref) + 1]);
    TEST_ASSERT_EQUAL_INT(0x00, long_buf[4 + sizeof(long_ref) + 2]);
    TEST_ASSERT_EQUAL_INT(0xFF, long_buf[4 + sizeof(long_ref) + 3]);
    memset(long_ref, 'm', 255);
    TEST_ASSERT(memcmp(&long_buf[4 + sizeof(long_ref) + 4], long_ref, 255) == 0);
}

static void test_simple_tlv_writer_remove_malformed(void)
{
    uint8_t truncated[3] = { 0x01, 0x05, 'a' };
    uint8_t trailing[4] = { 0x01, 0x01, 'a', 0x02 };
    uint8_t noncanon[9] = { 0x01, 0xFF, 0x00, 0x05, 'a', 'b', 'c', 'd', 'e' };
    tlv_writer_t truncated_w = SIMPLE_TLV_WRITER(truncated, sizeof(truncated), sizeof(truncated));
    tlv_writer_t trailing_w = SIMPLE_TLV_WRITER(trailing, sizeof(trailing), sizeof(trailing));
    tlv_writer_t noncanon_w = SIMPLE_TLV_WRITER(noncanon, sizeof(noncanon), sizeof(noncanon));

    TEST_ASSERT_EQUAL_INT(-EBADMSG, tlv_writer_remove(&truncated_w, 0x09));
    TEST_ASSERT_EQUAL_INT(-EBADMSG, tlv_writer_remove(&trailing_w, 0x09));
    TEST_ASSERT_EQUAL_INT(-EBADMSG, tlv_writer_remove(&noncanon_w, 0x01));
}

/*
 * MARK: - Get
 */

static void test_simple_tlv_writer_get_hit(void)
{
    uint8_t buf[16] = { 0x01, 0x03, 'a', 'b', 'c',
                        0x02, 0x00,
                        0x03, 0x01, 'z',
                        0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE };
    const uint8_t *value = NULL;
    tlv_writer_t w = SIMPLE_TLV_WRITER(buf, 10, sizeof(buf));

    TEST_ASSERT_EQUAL_INT(sizeof("abc") - 1, tlv_writer_get(&w, 0x01, &value));
    TEST_ASSERT(value == &buf[2]);

    TEST_ASSERT_EQUAL_INT(sizeof("") - 1, tlv_writer_get(&w, 0x02, &value));

    TEST_ASSERT_EQUAL_INT(sizeof("z") - 1, tlv_writer_get(&w, 0x03, &value));
    TEST_ASSERT(value == &buf[9]);
}

static void test_simple_tlv_writer_get_missing(void)
{
    uint8_t buf[16] = { 0x01, 0x03, 'a', 'b', 'c',
                        0x02, 0x00,
                        0x07, 0x01, 'z',
                        0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE };
    const uint8_t *value = NULL;
    tlv_writer_t w = SIMPLE_TLV_WRITER(buf, 7, sizeof(buf));
    tlv_writer_t empty = SIMPLE_TLV_WRITER(buf, 0, sizeof(buf));

    TEST_ASSERT_EQUAL_INT(-ENOENT, tlv_writer_get(&w, 0x09, &value));
    TEST_ASSERT(value == NULL);

    /* tag 7 is in the buffer, but behind the writer's size */
    TEST_ASSERT_EQUAL_INT(-ENOENT, tlv_writer_get(&w, 0x07, &value));

    TEST_ASSERT_EQUAL_INT(-ENOENT, tlv_writer_get(&empty, 0x01, &value));
}

static void test_simple_tlv_writer_get_after_set(void)
{
    uint8_t buf[16];
    const uint8_t *value = NULL;
    tlv_writer_t w = SIMPLE_TLV_WRITER(buf, 0, sizeof(buf));

    memset(buf, 0xEE, sizeof(buf));

    TEST_ASSERT_EQUAL_INT(-ENOENT, tlv_writer_get(&w, 0x01, &value));

    tlv_writer_set(&w, 0x01, sizeof("abc") - 1, (const uint8_t *)"abc");
    TEST_ASSERT_EQUAL_INT(sizeof("abc") - 1, tlv_writer_get(&w, 0x01, &value));
    TEST_ASSERT(value == &buf[2]);

    tlv_writer_set(&w, 0x01, sizeof("z") - 1, (const uint8_t *)"z");
    TEST_ASSERT_EQUAL_INT(sizeof("z") - 1, tlv_writer_get(&w, 0x01, &value));
    TEST_ASSERT(value == &buf[2]);

    tlv_writer_remove(&w, 0x01);
    TEST_ASSERT_EQUAL_INT(-ENOENT, tlv_writer_get(&w, 0x01, &value));
}

Test *tests_tlv_simple_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_simple_tlv_decode_sequence),
        new_TestFixture(test_simple_tlv_decode_reserved_tags),
        new_TestFixture(test_simple_tlv_decode_long_form),
        new_TestFixture(test_simple_tlv_decode_longest_short),
        new_TestFixture(test_simple_tlv_decode_shortest_long),
        new_TestFixture(test_simple_tlv_decode_too_short),
        new_TestFixture(test_simple_tlv_decode_truncated),
        new_TestFixture(test_simple_tlv_decode_noncanonical),
        new_TestFixture(test_simple_tlv_decode_error_untouched),
        new_TestFixture(test_simple_tlv_find_hit),
        new_TestFixture(test_simple_tlv_find_first_of_duplicates),
        new_TestFixture(test_simple_tlv_find_missing),
        new_TestFixture(test_simple_tlv_find_empty),
        new_TestFixture(test_simple_tlv_find_after_long_element),
        new_TestFixture(test_simple_tlv_find_malformed),
        new_TestFixture(test_simple_tlv_encode_short),
        new_TestFixture(test_simple_tlv_encode_zero_length),
        new_TestFixture(test_simple_tlv_encode_reserved),
        new_TestFixture(test_simple_tlv_encode_longest_short),
        new_TestFixture(test_simple_tlv_encode_shortest_long),
        new_TestFixture(test_simple_tlv_encode_long),
        new_TestFixture(test_simple_tlv_encode_nobufs),
        new_TestFixture(test_simple_tlv_writer_add_sequence),
        new_TestFixture(test_simple_tlv_writer_add_behind_existing),
        new_TestFixture(test_simple_tlv_writer_add_long),
        new_TestFixture(test_simple_tlv_writer_add_nobufs),
        new_TestFixture(test_simple_tlv_writer_set_same_size),
        new_TestFixture(test_simple_tlv_writer_set_shrink_middle),
        new_TestFixture(test_simple_tlv_writer_set_grow_middle),
        new_TestFixture(test_simple_tlv_writer_set_grow_last),
        new_TestFixture(test_simple_tlv_writer_set_to_zero_length),
        new_TestFixture(test_simple_tlv_writer_set_from_zero_length),
        new_TestFixture(test_simple_tlv_writer_set_first_of_duplicates),
        new_TestFixture(test_simple_tlv_writer_set_append_missing),
        new_TestFixture(test_simple_tlv_writer_set_append_to_empty),
        new_TestFixture(test_simple_tlv_writer_set_254_to_255),
        new_TestFixture(test_simple_tlv_writer_set_255_to_254),
        new_TestFixture(test_simple_tlv_writer_set_nobufs),
        new_TestFixture(test_simple_tlv_writer_set_nobufs_long),
        new_TestFixture(test_simple_tlv_writer_set_malformed),
        new_TestFixture(test_simple_tlv_writer_remove_middle),
        new_TestFixture(test_simple_tlv_writer_remove_first),
        new_TestFixture(test_simple_tlv_writer_remove_last),
        new_TestFixture(test_simple_tlv_writer_remove_no_match),
        new_TestFixture(test_simple_tlv_writer_remove_empty_writer),
        new_TestFixture(test_simple_tlv_writer_remove_ignores_spare_room),
        new_TestFixture(test_simple_tlv_writer_remove_adjacent_duplicates),
        new_TestFixture(test_simple_tlv_writer_remove_interleaved_duplicates),
        new_TestFixture(test_simple_tlv_writer_remove_every_element),
        new_TestFixture(test_simple_tlv_writer_remove_long_element),
        new_TestFixture(test_simple_tlv_writer_remove_keeps_long_elements),
        new_TestFixture(test_simple_tlv_writer_remove_malformed),
        new_TestFixture(test_simple_tlv_writer_get_hit),
        new_TestFixture(test_simple_tlv_writer_get_missing),
        new_TestFixture(test_simple_tlv_writer_get_after_set),
    };

    EMB_UNIT_TESTCALLER(tlv_simple_tests, NULL, NULL, fixtures);

    return (Test *)&tlv_simple_tests;
}

void tests_tlv_simple(void)
{
    TESTS_RUN(tests_tlv_simple_tests());
}
