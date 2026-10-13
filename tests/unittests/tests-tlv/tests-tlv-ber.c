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
 * MARK: - Decoding & Tags
 */

static void test_ber_tlv_decode_sequence(void)
{
    /* 1, 2, 3 and 4 byte tags, back to back */
    const uint8_t raw[] = { 0x5A, 0x03, 'a', 'b', 'c',
                            0x9F, 0x02, 0x01, 'x',
                            0x9F, 0x81, 0x01, 0x00,
                            0x9F, 0x81, 0x80, 0x01, 0x02, 'y', 'z' };
    tlv_tag_t tag = 0;
    size_t length = 0;
    const uint8_t *value = NULL;
    ssize_t decoded;

    decoded = ber_tlv_decode(raw, sizeof(raw), &tag, &length, &value);
    TEST_ASSERT_EQUAL_INT(5, decoded);
    TEST_ASSERT_EQUAL_INT(0x5A, tag);
    TEST_ASSERT_EQUAL_INT(sizeof("abc") - 1, length);
    TEST_ASSERT(value == &raw[2]);

    decoded = ber_tlv_decode(&raw[5], sizeof(raw) - 5, &tag, &length, &value);
    TEST_ASSERT_EQUAL_INT(4, decoded);
    TEST_ASSERT_EQUAL_INT(0x9F02, tag);
    TEST_ASSERT_EQUAL_INT(sizeof("x") - 1, length);
    TEST_ASSERT(value == &raw[8]);

    decoded = ber_tlv_decode(&raw[9], sizeof(raw) - 9, &tag, &length, &value);
    TEST_ASSERT_EQUAL_INT(4, decoded);
    TEST_ASSERT_EQUAL_INT(0x9F8101, tag);
    TEST_ASSERT_EQUAL_INT(sizeof("") - 1, length);

    decoded = ber_tlv_decode(&raw[13], sizeof(raw) - 13, &tag, &length, &value);
    TEST_ASSERT_EQUAL_INT(7, decoded); /* last element: nothing remaining */
    TEST_ASSERT_EQUAL_INT(0x9F818001, tag);
    TEST_ASSERT_EQUAL_INT(sizeof("yz") - 1, length);
    TEST_ASSERT(value == &raw[18]);
}

static void test_ber_tlv_decode_nested(void)
{
    /* constructed tag 0x70 holding tag 0x5A "abc" and tag 0x9F02 (empty) */
    const uint8_t raw[] = { 0x70, 0x08,
                            0x5A, 0x03, 'a', 'b', 'c',
                            0x9F, 0x02, 0x00 };
    tlv_tag_t tag = 0;
    size_t length = 0;
    const uint8_t *value = NULL;
    ssize_t decoded;

    decoded = ber_tlv_decode(raw, sizeof(raw), &tag, &length, &value);
    TEST_ASSERT_EQUAL_INT(sizeof(raw), decoded);
    TEST_ASSERT_EQUAL_INT(0x70, tag);
    TEST_ASSERT_EQUAL_INT(sizeof(raw) - 2, length);
    TEST_ASSERT(value == &raw[2]);

    decoded = ber_tlv_decode(value, length, &tag, &length, &value);
    TEST_ASSERT_EQUAL_INT(5, decoded);
    TEST_ASSERT_EQUAL_INT(0x5A, tag);
    TEST_ASSERT_EQUAL_INT(sizeof("abc") - 1, length);
    TEST_ASSERT(value == &raw[4]);

    decoded = ber_tlv_decode(&raw[7], sizeof(raw) - 7, &tag, &length, &value);
    TEST_ASSERT_EQUAL_INT(3, decoded);
    TEST_ASSERT_EQUAL_INT(0x9F02, tag);
    TEST_ASSERT_EQUAL_INT(sizeof("") - 1, length);
}

static void test_ber_tlv_decode_tag_edges(void)
{
    /* the last 1-byte tag numbers are 0x1E and (with other class bits) 0xFE */
    const uint8_t one_a[] = { 0x1E, 0x01, 'a' };
    const uint8_t one_b[] = { 0xFE, 0x01, 'a' };
    /* 0x1F selects the multi-byte form, whatever the class bits are */
    const uint8_t two_a[] = { 0x1F, 0x1F, 0x00 };         /* number 31: smallest */
    const uint8_t two_b[] = { 0xFF, 0x7F, 0x00 };         /* all bits set */
    const uint8_t three[] = { 0x9F, 0xFF, 0x7F, 0x00 };
    const uint8_t four[] = { 0x9F, 0xFF, 0xFF, 0x7F, 0x00 }; /* longest tag */
    tlv_tag_t tag = 0;
    size_t length = 0;
    const uint8_t *value = NULL;

    TEST_ASSERT_EQUAL_INT(sizeof(one_a), ber_tlv_decode(one_a, sizeof(one_a), &tag, &length, &value));
    TEST_ASSERT_EQUAL_INT(0x1E, tag);

    TEST_ASSERT_EQUAL_INT(sizeof(one_b), ber_tlv_decode(one_b, sizeof(one_b), &tag, &length, &value));
    TEST_ASSERT_EQUAL_INT(0xFE, tag);

    TEST_ASSERT_EQUAL_INT(sizeof(two_a), ber_tlv_decode(two_a, sizeof(two_a), &tag, &length, &value));
    TEST_ASSERT_EQUAL_INT(0x1F1F, tag);

    TEST_ASSERT_EQUAL_INT(sizeof(two_b), ber_tlv_decode(two_b, sizeof(two_b), &tag, &length, &value));
    TEST_ASSERT_EQUAL_INT(0xFF7F, tag);

    TEST_ASSERT_EQUAL_INT(sizeof(three), ber_tlv_decode(three, sizeof(three), &tag, &length, &value));
    TEST_ASSERT_EQUAL_INT(0x9FFF7F, tag);

    TEST_ASSERT_EQUAL_INT(sizeof(four), ber_tlv_decode(four, sizeof(four), &tag, &length, &value));
    TEST_ASSERT_EQUAL_INT(0x9FFFFF7F, tag);
    TEST_ASSERT_EQUAL_INT(sizeof("") - 1, length);
    TEST_ASSERT(value == &four[sizeof(four)]);
}

static void test_ber_tlv_decode_tag_too_long(void)
{
    /* a fifth tag byte (the fourth continuation byte) is not supported */
    const uint8_t five[] =    { 0x9F, 0x81, 0x81, 0x81, 0x01, 0x00 };
    const uint8_t endless[] = { 0x9F, 0x81, 0x81, 0x81, 0x81, 0x81, 0x01, 0x00 };

    TEST_ASSERT_EQUAL_INT(-EOVERFLOW, ber_tlv_decode(five, sizeof(five), NULL, NULL, NULL));
    TEST_ASSERT_EQUAL_INT(-EOVERFLOW, ber_tlv_decode(endless, sizeof(endless),NULL, NULL, NULL));
}

static void test_ber_tlv_decode_tag_leading_zero(void)
{
    /* the first byte after 0x1F must not be 0x00 or 0x80 (b7..b1 all zero) */
    const uint8_t zero[] = { 0x9F, 0x00, 0x01, 'a' };
    const uint8_t zero_cont[] = { 0x9F, 0x80, 0x01, 0x01, 'a' };
    /* later bytes may be zero */
    const uint8_t later[] = { 0x9F, 0x81, 0x00, 0x01, 'a' };
    tlv_tag_t tag = 0;
    size_t length = 0;
    const uint8_t *value = NULL;

    TEST_ASSERT_EQUAL_INT(-EBADMSG, ber_tlv_decode(zero, sizeof(zero), &tag, &length, &value));
    TEST_ASSERT_EQUAL_INT(-EBADMSG, ber_tlv_decode(zero_cont, sizeof(zero_cont), &tag, &length, &value));

    TEST_ASSERT_EQUAL_INT(sizeof(later), ber_tlv_decode(later, sizeof(later), &tag, &length, &value));
    TEST_ASSERT_EQUAL_INT(0x9F8100, tag);
}

static void test_ber_tlv_decode_tag_truncated(void)
{
    const uint8_t first[] = { 0x9F };                /* nothing behind 0x1F */
    const uint8_t cont[] = { 0x9F, 0x81 };           /* continuation, no last byte */
    const uint8_t cont2[] = { 0x9F, 0x81, 0x80 };
    const uint8_t no_length[] = { 0x5A };            /* a tag without a length */
    const uint8_t no_length2[] = { 0x9F, 0x02 };
    tlv_tag_t tag = 0;
    size_t length = 0;
    const uint8_t *value = NULL;

    TEST_ASSERT_EQUAL_INT(-EBADMSG, ber_tlv_decode(first, 0, &tag, &length, &value));
    TEST_ASSERT_EQUAL_INT(-EBADMSG, ber_tlv_decode(first, sizeof(first), &tag, &length, &value));
    TEST_ASSERT_EQUAL_INT(-EBADMSG, ber_tlv_decode(cont, sizeof(cont), &tag, &length, &value));
    TEST_ASSERT_EQUAL_INT(-EBADMSG, ber_tlv_decode(cont2, sizeof(cont2), &tag, &length, &value));
    TEST_ASSERT_EQUAL_INT(-EBADMSG, ber_tlv_decode(no_length, sizeof(no_length), &tag, &length, &value));
    TEST_ASSERT_EQUAL_INT(-EBADMSG, ber_tlv_decode(no_length2, sizeof(no_length2), &tag, &length, &value));
}

/*
 * MARK: - Decoding & Lengths
 */

static void test_ber_tlv_decode_zero_length(void)
{
    const uint8_t raw[] = { 0x5A, 0x00 };
    tlv_tag_t tag = 0;
    size_t length = 0x1234;
    const uint8_t *value = NULL;

    TEST_ASSERT_EQUAL_INT(sizeof(raw), ber_tlv_decode(raw, sizeof(raw), &tag, &length, &value));
    TEST_ASSERT_EQUAL_INT(0x5A, tag);
    TEST_ASSERT_EQUAL_INT(0, length);
    TEST_ASSERT(value == &raw[2]);
}

static void test_ber_tlv_decode_longest_short(void)
{
    tlv_tag_t tag = 0;
    size_t length = 0;
    const uint8_t *value = NULL;

    /* 127 (0x7F) is the longest length of the short form */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x5A;
    long_buf[1] = 0x7F;
    memset(&long_buf[2], 'a', 127);

    ssize_t decoded = ber_tlv_decode(long_buf, 2 + 127, &tag, &length, &value);

    TEST_ASSERT_EQUAL_INT(2 + 127, decoded);
    TEST_ASSERT_EQUAL_INT(0x5A, tag);
    TEST_ASSERT_EQUAL_INT(127, length);
    TEST_ASSERT(value == &long_buf[2]);
}

static void test_ber_tlv_decode_shortest_long(void)
{
    tlv_tag_t tag = 0;
    size_t length = 0;
    const uint8_t *value = NULL;

    /* 128 (0x80) needs the long form: 0x81 0x80 */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x5A;
    long_buf[1] = 0x81;
    long_buf[2] = 0x80;
    memset(&long_buf[3], 'a', 128);

    ssize_t decoded = ber_tlv_decode(long_buf, 3 + 128, &tag, &length, &value);

    TEST_ASSERT_EQUAL_INT(3 + 128, decoded);
    TEST_ASSERT_EQUAL_INT(0x5A, tag);
    TEST_ASSERT_EQUAL_INT(128, length);
    TEST_ASSERT(value == &long_buf[3]);
}

static void test_ber_tlv_decode_one_byte_long(void)
{
    tlv_tag_t tag = 0;
    size_t length = 0;
    const uint8_t *value = NULL;

    /* 255 (0xFF) is the longest length with one length byte: 0x81 0xFF */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x5A;
    long_buf[1] = 0x81;
    long_buf[2] = 0xFF;
    memset(&long_buf[3], 'a', 255);

    ssize_t decoded = ber_tlv_decode(long_buf, 3 + 255, &tag, &length, &value);

    TEST_ASSERT_EQUAL_INT(3 + 255, decoded);
    TEST_ASSERT_EQUAL_INT(0x5A, tag);
    TEST_ASSERT_EQUAL_INT(255, length);
    TEST_ASSERT(value == &long_buf[3]);
}

static void test_ber_tlv_decode_two_byte_long(void)
{
    tlv_tag_t tag = 0;
    size_t length = 0;
    const uint8_t *value = NULL;
    ssize_t decoded;

    /* 256 (0x0100) is the shortest length with two length bytes: 0x82 0x01 0x00 */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x5A;
    long_buf[1] = 0x82;
    long_buf[2] = 0x01;
    long_buf[3] = 0x00;
    memset(&long_buf[4], 'a', 256);
    decoded = ber_tlv_decode(long_buf, 4 + 256, &tag, &length, &value);
    TEST_ASSERT_EQUAL_INT(4 + 256, decoded);
    TEST_ASSERT_EQUAL_INT(0x5A, tag);
    TEST_ASSERT_EQUAL_INT(256, length);
    TEST_ASSERT(value == &long_buf[4]);

    /* 300 (0x012C), then another element */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x9F;
    long_buf[1] = 0x02;
    long_buf[2] = 0x82;
    long_buf[3] = 0x01;
    long_buf[4] = 0x2C;
    memset(&long_buf[5], 'a', sizeof(long_ref));
    long_buf[5 + sizeof(long_ref)] = 0x07;
    long_buf[5 + sizeof(long_ref) + 1] = sizeof("x") - 1;
    long_buf[5 + sizeof(long_ref) + 2] = 'x';
    decoded = ber_tlv_decode(long_buf, 5 + sizeof(long_ref) + 3, &tag, &length, &value);
    TEST_ASSERT_EQUAL_INT(5 + sizeof(long_ref), decoded);
    TEST_ASSERT_EQUAL_INT(0x9F02, tag);
    TEST_ASSERT_EQUAL_INT(sizeof(long_ref), length);
    TEST_ASSERT(value == &long_buf[5]);

    decoded = ber_tlv_decode(&long_buf[5 + sizeof(long_ref)], 3, &tag, &length, &value);
    TEST_ASSERT_EQUAL_INT(3, decoded);
    TEST_ASSERT_EQUAL_INT(0x07, tag);
    TEST_ASSERT_EQUAL_INT(sizeof("x") - 1, length);
}

static void test_ber_tlv_decode_length_reserved(void)
{
    const uint8_t indefinite[] = { 0x5A, 0x80, 'a', 0x00, 0x00 };
    const uint8_t reserved[] = { 0x5A, 0xFF, 0x00, 0x01, 'a' };
    tlv_tag_t tag = 0;
    size_t length = 0;
    const uint8_t *value = NULL;

    TEST_ASSERT_EQUAL_INT(-ENOTSUP, ber_tlv_decode(indefinite, sizeof(indefinite), &tag, &length, &value));
    TEST_ASSERT_EQUAL_INT(-EBADMSG, ber_tlv_decode(reserved, sizeof(reserved), &tag, &length, &value));
}

static void test_ber_tlv_decode_length_truncated(void)
{
    const uint8_t short_value[] = { 0x5A, 0x05, 'a' };       /* wants 5, has 1 */
    const uint8_t long_marker[] = { 0x5A, 0x81 };            /* no length bytes */
    const uint8_t long_half[] = { 0x5A, 0x82, 0x01 };        /* half a length */
    const uint8_t long_nodata[] = { 0x5A, 0x81, 0x80 };      /* 128, has 0 */
    const uint8_t huge[] = { 0x5A, 0x83, 0x01, 0x00, 0x00, 'a' }; /* 65536, has 1 */
    tlv_tag_t tag = 0;
    size_t length = 0;
    const uint8_t *value = NULL;

    TEST_ASSERT_EQUAL_INT(-EBADMSG, ber_tlv_decode(short_value, sizeof(short_value), &tag, &length, &value));
    TEST_ASSERT_EQUAL_INT(-EBADMSG, ber_tlv_decode(long_marker, sizeof(long_marker), &tag, &length, &value));
    TEST_ASSERT_EQUAL_INT(-EBADMSG, ber_tlv_decode(long_half, sizeof(long_half), &tag, &length, &value));
    TEST_ASSERT_EQUAL_INT(-EBADMSG, ber_tlv_decode(long_nodata, sizeof(long_nodata), &tag, &length, &value));
    TEST_ASSERT_EQUAL_INT(-EBADMSG, ber_tlv_decode(huge, sizeof(huge), &tag, &length, &value));

    /* long form, 300 bytes announced, 299 present */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x5A;
    long_buf[1] = 0x82;
    long_buf[2] = 0x01;
    long_buf[3] = 0x2C;
    memset(&long_buf[4], 0x42, sizeof(long_ref) - 1);
    TEST_ASSERT_EQUAL_INT(-EBADMSG, ber_tlv_decode(long_buf, 4 + sizeof(long_ref) - 1, &tag, &length, &value));

    /* short form, 127 bytes announced, 126 present */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x5A;
    long_buf[1] = 0x7F;
    memset(&long_buf[2], 0x42, 126);
    TEST_ASSERT_EQUAL_INT(-EBADMSG, ber_tlv_decode(long_buf, 2 + 126, &tag, &length, &value));
}

static void test_ber_tlv_decode_length_oversized(void)
{
    /* more length bytes than a size_t can hold, or a length the buffer doesn't have */
    const uint8_t four[] = { 0x5A, 0x84, 0xFF, 0xFF, 0xFF, 0xFF, 'a' };
    const uint8_t eight[] = { 0x5A, 0x88, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 'a' };
    const uint8_t nine[] = { 0x5A, 0x89, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 'a' };
    const uint8_t many[] = { 0x5A, 0xFE, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 'a' };
    tlv_tag_t tag = 0;
    size_t length = 0;
    const uint8_t *value = NULL;

    TEST_ASSERT_EQUAL_INT(-EBADMSG, ber_tlv_decode(four, sizeof(four), &tag, &length, &value));
    TEST_ASSERT_EQUAL_INT(-EBADMSG, ber_tlv_decode(eight, sizeof(eight), &tag, &length, &value));
    TEST_ASSERT_EQUAL_INT(-EOVERFLOW, ber_tlv_decode(nine, sizeof(nine), &tag, &length, &value));
    TEST_ASSERT_EQUAL_INT(-EBADMSG, ber_tlv_decode(many, sizeof(many), &tag, &length, &value));
}

static void test_ber_tlv_decode_error_untouched(void)
{
    const uint8_t tag_long[] = { 0x9F, 0x81, 0x81, 0x81, 0x01, 0x00 };
    const uint8_t tag_zero[] = { 0x9F, 0x00, 0x01, 'a' };
    const uint8_t short_value[] = { 0x5A, 0x05, 'a' };
    const uint8_t infinite[] = { 0x5A, 0x80, 'a' };
    tlv_tag_t tag;
    size_t length;
    const uint8_t *value;

    /* on -EBADMSG all outputs keep their old values */
    tag = 0x55;
    length = 0x1234;
    value = NULL;
    TEST_ASSERT_EQUAL_INT(-EOVERFLOW, ber_tlv_decode(tag_long, sizeof(tag_long), &tag, &length, &value));
    TEST_ASSERT_EQUAL_INT(0x55, tag);
    TEST_ASSERT_EQUAL_INT(0x1234, length);
    TEST_ASSERT(value == NULL);

    tag = 0x55;
    length = 0x1234;
    value = NULL;
    TEST_ASSERT_EQUAL_INT(-EBADMSG, ber_tlv_decode(tag_zero, sizeof(tag_zero), &tag, &length, &value));
    TEST_ASSERT_EQUAL_INT(0x55, tag);
    TEST_ASSERT_EQUAL_INT(0x1234, length);
    TEST_ASSERT(value == NULL);

    tag = 0x55;
    length = 0x1234;
    value = NULL;
    TEST_ASSERT_EQUAL_INT(-EBADMSG, ber_tlv_decode(short_value, sizeof(short_value), &tag, &length, &value));
    TEST_ASSERT_EQUAL_INT(0x55, tag);
    TEST_ASSERT_EQUAL_INT(0x1234, length);
    TEST_ASSERT(value == NULL);

    tag = 0x55;
    length = 0x1234;
    value = NULL;
    TEST_ASSERT_EQUAL_INT(-ENOTSUP, ber_tlv_decode(infinite, sizeof(infinite), &tag, &length, &value));
    TEST_ASSERT_EQUAL_INT(0x55, tag);
    TEST_ASSERT_EQUAL_INT(0x1234, length);
    TEST_ASSERT(value == NULL);
}

/*
 * MARK: - Lookup
 */

static void test_ber_tlv_find_hit(void)
{
    /* tag 0x5A "abc", 0x9F02 (empty), 0x9F8101 "z", 0x5F20 "xy" */
    const uint8_t raw[] = { 0x5A, 0x03, 'a', 'b', 'c',
                            0x9F, 0x02, 0x00,
                            0x9F, 0x81, 0x01, 0x01, 'z',
                            0x5F, 0x20, 0x02, 'x', 'y' };
    const uint8_t *value = NULL;
    ssize_t r;

    r = ber_tlv_find(raw, sizeof(raw), 0x5A, &value);
    TEST_ASSERT_EQUAL_INT(sizeof("abc") - 1, r);
    TEST_ASSERT(value == &raw[2]);

    /* an empty value is a hit, not an error */
    r = ber_tlv_find(raw, sizeof(raw), 0x9F02, &value);
    TEST_ASSERT_EQUAL_INT(sizeof("") - 1, r);

    value = NULL;
    r = ber_tlv_find(raw, sizeof(raw), 0x9F8101, &value);
    TEST_ASSERT_EQUAL_INT(sizeof("z") - 1, r);
    TEST_ASSERT(value == &raw[12]);

    /* the very last element of the buffer */
    value = NULL;
    r = ber_tlv_find(raw, sizeof(raw), 0x5F20, &value);
    TEST_ASSERT_EQUAL_INT(sizeof("xy") - 1, r);
    TEST_ASSERT(value == &raw[16]);
}

static void test_ber_tlv_find_first_of_duplicates(void)
{
    const uint8_t raw[] = { 0x9F, 0x02, 0x03, 'a', 'b', 'c',
                            0x5A, 0x00,
                            0x9F, 0x02, 0x02, 'x', 'y' };
    const uint8_t *value = NULL;

    ssize_t r = ber_tlv_find(raw, sizeof(raw), 0x9F02, &value);

    TEST_ASSERT_EQUAL_INT(sizeof("abc") - 1, r);
    TEST_ASSERT(value == &raw[3]);
}

static void test_ber_tlv_find_whole_tag_matches(void)
{
    const uint8_t raw[] = { 0x5A, 0x03, 'a', 'b', 'c',
                            0x9F, 0x02, 0x00,
                            0x9F, 0x81, 0x01, 0x01, 'z' };
    const uint8_t *value = NULL;

    /* parts of a multi-byte tag are not tags of their own */
    TEST_ASSERT_EQUAL_INT(-ENOENT, ber_tlv_find(raw, sizeof(raw), 0x9F, &value));
    TEST_ASSERT_EQUAL_INT(-ENOENT, ber_tlv_find(raw, sizeof(raw), 0x02, &value));
    TEST_ASSERT_EQUAL_INT(-ENOENT, ber_tlv_find(raw, sizeof(raw), 0x81, &value));
    TEST_ASSERT_EQUAL_INT(-ENOENT, ber_tlv_find(raw, sizeof(raw), 0x9F81, &value));
    TEST_ASSERT_EQUAL_INT(-ENOENT, ber_tlv_find(raw, sizeof(raw), 0x8101, &value));
    TEST_ASSERT_EQUAL_INT(-ENOENT, ber_tlv_find(raw, sizeof(raw), 0x9F0200, &value));
    TEST_ASSERT(value == NULL);
}

static void test_ber_tlv_find_missing(void)
{
    const uint8_t raw[] = { 0x5A, 0x03, 'a', 'b', 'c',
                            0x9F, 0x02, 0x00,
                            0x5B, 0x01, 'z' };
    const uint8_t *value = NULL;

    TEST_ASSERT_EQUAL_INT(-ENOENT, ber_tlv_find(raw, sizeof(raw), 0x09, &value));
    TEST_ASSERT_EQUAL_INT(-ENOENT, ber_tlv_find(raw, sizeof(raw), 0x00, &value));

    /* only the first element (5 bytes) is in range */
    TEST_ASSERT_EQUAL_INT(sizeof("abc") - 1, ber_tlv_find(raw, 5, 0x5A, &value));
    TEST_ASSERT_EQUAL_INT(-ENOENT, ber_tlv_find(raw, 5, 0x9F02, &value));

    /* only the first two elements (8 bytes) are in range */
    TEST_ASSERT_EQUAL_INT(-ENOENT, ber_tlv_find(raw, 8, 0x5B, &value));
}

static void test_ber_tlv_find_empty(void)
{
    const uint8_t raw[] = { 0x5A, 0x01, 'a' };
    const uint8_t *value = NULL;

    TEST_ASSERT_EQUAL_INT(-ENOENT, ber_tlv_find(raw, 0, 0x00, &value));
    TEST_ASSERT_EQUAL_INT(-ENOENT, ber_tlv_find(raw, 0, 0x5A, &value));
}

static void test_ber_tlv_find_after_long_element(void)
{
    const uint8_t *value = NULL;
    ssize_t r;

    /* tag 0x9F05 with 300 bytes, then tag 0x06 with "z": must skip the long element */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x9F;
    long_buf[1] = 0x05;
    long_buf[2] = 0x82;
    long_buf[3] = 0x01;
    long_buf[4] = 0x2C;
    memset(&long_buf[5], 'x', sizeof(long_ref));
    long_buf[5 + sizeof(long_ref)] = 0x06;
    long_buf[5 + sizeof(long_ref) + 1] = sizeof("z") - 1;
    long_buf[5 + sizeof(long_ref) + 2] = 'z';

    r = ber_tlv_find(long_buf, 5 + sizeof(long_ref) + 3, 0x06, &value);
    TEST_ASSERT_EQUAL_INT(sizeof("z") - 1, r);
    TEST_ASSERT(value == &long_buf[5 + sizeof(long_ref) + 2]);

    /* and the long element itself */
    value = NULL;
    r = ber_tlv_find(long_buf, 5 + sizeof(long_ref) + 3, 0x9F05, &value);
    TEST_ASSERT_EQUAL_INT(sizeof(long_ref), r);
    TEST_ASSERT(value == &long_buf[5]);
}

static void test_ber_tlv_find_malformed(void)
{
    const uint8_t trailing[] = { 0x5A, 0x01, 'a', 0x9F };        /* element + half a tag */
    const uint8_t truncated[] = { 0x5A, 0x05, 'a' };
    const uint8_t bad_tag[] = { 0x5A, 0x01, 'a', 0x9F, 0x00, 0x01, 'b' };
    const uint8_t *value = NULL;

    /* a match before the damage is still found */
    TEST_ASSERT_EQUAL_INT(1, ber_tlv_find(trailing, sizeof(trailing), 0x5A, &value));
    TEST_ASSERT_EQUAL_INT(1, ber_tlv_find(bad_tag, sizeof(bad_tag), 0x5A, &value));
    /* damage reached before a match */
    TEST_ASSERT_EQUAL_INT(-EBADMSG, ber_tlv_find(trailing, sizeof(trailing), 0x9F02, &value));
    TEST_ASSERT_EQUAL_INT(-EBADMSG, ber_tlv_find(bad_tag, sizeof(bad_tag), 0x9F02, &value));

    TEST_ASSERT_EQUAL_INT(-EBADMSG, ber_tlv_find(truncated, sizeof(truncated), 0x5A, &value));
    TEST_ASSERT_EQUAL_INT(-EBADMSG, ber_tlv_find(truncated, sizeof(truncated), 0x09, &value));
}

/*
 * MARK: - Encoding
 */

static void test_ber_tlv_encode_short(void)
{
    uint8_t buf[8] = { 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE };
    const uint8_t expected[5] = { 0x5A, 0x03, 'a', 'b', 'c' };

    /* capacity is exactly the encoded size */
    ssize_t r = ber_tlv_encode(buf, sizeof(expected), 0x5A, sizeof("abc") - 1, (const uint8_t *)"abc");

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(expected)]);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(buf) - 1]);
}

static void test_ber_tlv_encode_zero_length(void)
{
    uint8_t buf[4] = { 0xEE, 0xEE, 0xEE, 0xEE };

    ssize_t r = ber_tlv_encode(buf, 2, 0x5A, sizeof("") - 1, NULL);

    TEST_ASSERT_EQUAL_INT(2, r);
    TEST_ASSERT_EQUAL_INT(0x5A, buf[0]);
    TEST_ASSERT_EQUAL_INT(0x00, buf[1]);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[2]);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[3]);
}

static void test_ber_tlv_encode_two_byte_tag(void)
{
    uint8_t buf[8] = { 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE };
    const uint8_t expected[4] = { 0x9F, 0x02, 0x01, 'x' };

    ssize_t r = ber_tlv_encode(buf, sizeof(expected), 0x9F02, sizeof("x") - 1, (const uint8_t *)"x");

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(expected)]);
}

static void test_ber_tlv_encode_three_byte_tag(void)
{
    uint8_t buf[8] = { 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE };
    const uint8_t expected[6] = { 0x9F, 0x81, 0x01, 0x02, 'x', 'y' };

    ssize_t r = ber_tlv_encode(buf, sizeof(expected), 0x9F8101, sizeof("xy") - 1, (const uint8_t *)"xy");

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(expected)]);
}

static void test_ber_tlv_encode_four_byte_tag(void)
{
    uint8_t buf[8] = { 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE };
    const uint8_t expected[6] = { 0x9F, 0x81, 0x80, 0x01, 0x01, 'x' };

    ssize_t r = ber_tlv_encode(buf, sizeof(expected), 0x9F818001, sizeof("x") - 1, (const uint8_t *)"x");

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(expected)]);
}

static void test_ber_tlv_encode_longest_short(void)
{
    memset(long_buf, 0xEE, sizeof(long_buf));
    memset(long_ref, 0xAB, sizeof(long_ref));

    /* 127 (0x7F) fits the single length byte */
    ssize_t r = ber_tlv_encode(long_buf, 2 + 127, 0x5A, 127, long_ref);

    TEST_ASSERT_EQUAL_INT(2 + 127, r);
    TEST_ASSERT_EQUAL_INT(0x5A, long_buf[0]);
    TEST_ASSERT_EQUAL_INT(0x7F, long_buf[1]);
    TEST_ASSERT(memcmp(&long_buf[2], long_ref, 127) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, long_buf[2 + 127]);
}

static void test_ber_tlv_encode_shortest_long(void)
{
    memset(long_buf, 0xEE, sizeof(long_buf));
    memset(long_ref, 0xAB, sizeof(long_ref));

    /* 128 (0x80) needs 0x81 0x80 */
    ssize_t r = ber_tlv_encode(long_buf, 3 + 128, 0x5A, 128, long_ref);

    TEST_ASSERT_EQUAL_INT(3 + 128, r);
    TEST_ASSERT_EQUAL_INT(0x5A, long_buf[0]);
    TEST_ASSERT_EQUAL_INT(0x81, long_buf[1]);
    TEST_ASSERT_EQUAL_INT(0x80, long_buf[2]);
    TEST_ASSERT(memcmp(&long_buf[3], long_ref, 128) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, long_buf[3 + 128]);
}

static void test_ber_tlv_encode_one_byte_long(void)
{
    memset(long_buf, 0xEE, sizeof(long_buf));
    memset(long_ref, 0xAB, 255);

    /* 255 (0xFF) is the longest length with one length byte: 0x81 0xFF */
    ssize_t r = ber_tlv_encode(long_buf, 3 + 255, 0x5A, 255, long_ref);

    TEST_ASSERT_EQUAL_INT(3 + 255, r);
    TEST_ASSERT_EQUAL_INT(0x5A, long_buf[0]);
    TEST_ASSERT_EQUAL_INT(0x81, long_buf[1]);
    TEST_ASSERT_EQUAL_INT(0xFF, long_buf[2]);
    TEST_ASSERT(memcmp(&long_buf[3], long_ref, 255) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, long_buf[3 + 255]);
}

static void test_ber_tlv_encode_two_byte_long(void)
{
    ssize_t r;

    memset(long_ref, 0xAB, sizeof(long_ref));

    /* 256 (0x0100) is the shortest length with two length bytes */
    memset(long_buf, 0xEE, sizeof(long_buf));
    r = ber_tlv_encode(long_buf, 4 + 256, 0x5A, 256, long_ref);
    TEST_ASSERT_EQUAL_INT(4 + 256, r);
    TEST_ASSERT_EQUAL_INT(0x5A, long_buf[0]);
    TEST_ASSERT_EQUAL_INT(0x82, long_buf[1]);
    TEST_ASSERT_EQUAL_INT(0x01, long_buf[2]);
    TEST_ASSERT_EQUAL_INT(0x00, long_buf[3]);
    TEST_ASSERT(memcmp(&long_buf[4], long_ref, 256) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, long_buf[4 + 256]);

    /* 300 = 0x012C, with a 2-byte tag */
    memset(long_buf, 0xEE, sizeof(long_buf));
    r = ber_tlv_encode(long_buf, 5 + sizeof(long_ref), 0x9F02, sizeof(long_ref), long_ref);
    TEST_ASSERT_EQUAL_INT(5 + sizeof(long_ref), r);
    TEST_ASSERT_EQUAL_INT(0x9F, long_buf[0]);
    TEST_ASSERT_EQUAL_INT(0x02, long_buf[1]);
    TEST_ASSERT_EQUAL_INT(0x82, long_buf[2]);
    TEST_ASSERT_EQUAL_INT(0x01, long_buf[3]);
    TEST_ASSERT_EQUAL_INT(0x2C, long_buf[4]);
    TEST_ASSERT(memcmp(&long_buf[5], long_ref, sizeof(long_ref)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, long_buf[5 + sizeof(long_ref)]);
}

static void test_ber_tlv_encode_nobufs(void)
{
    uint8_t buf[8];
    const uint8_t untouched[8] = { 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE };
    ssize_t r;
    int bad;

    /* needs 5, one byte short */
    memset(buf, 0xEE, sizeof(buf));
    r = ber_tlv_encode(buf, 4, 0x5A, sizeof("abc") - 1, (const uint8_t *)"abc");
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, r);
    TEST_ASSERT(memcmp(buf, untouched, sizeof(untouched)) == 0);

    /* needs 2, one byte short */
    memset(buf, 0xEE, sizeof(buf));
    r = ber_tlv_encode(buf, 1, 0x5A, sizeof("") - 1, NULL);
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, r);
    TEST_ASSERT(memcmp(buf, untouched, sizeof(untouched)) == 0);

    /* needs 5, no room at all */
    memset(buf, 0xEE, sizeof(buf));
    r = ber_tlv_encode(buf, 0, 0x5A, sizeof("abc") - 1, (const uint8_t *)"abc");
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, r);
    TEST_ASSERT(memcmp(buf, untouched, sizeof(untouched)) == 0);

    /* the tag alone fits, the length does not: 4-byte tag + length needs 5 */
    memset(buf, 0xEE, sizeof(buf));
    r = ber_tlv_encode(buf, 4, 0x9F818001, sizeof("") - 1, NULL);
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, r);
    TEST_ASSERT(memcmp(buf, untouched, sizeof(untouched)) == 0);

    /* 128 bytes need 131: one byte short */
    memset(long_buf, 0xEE, sizeof(long_buf));
    memset(long_ref, 'x', sizeof(long_ref));
    r = ber_tlv_encode(long_buf, 3 + 128 - 1, 0x5A, 128, long_ref);
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, r);
    bad = 0;
    for (size_t i = 0; i < sizeof(long_buf); i++) {
        if (long_buf[i] != 0xEE) {
            bad++;
        }
    }
    TEST_ASSERT_EQUAL_INT(0, bad);

    /* 300 bytes need 304: the long header only partly fits */
    memset(long_buf, 0xEE, sizeof(long_buf));
    r = ber_tlv_encode(long_buf, 3, 0x5A, sizeof(long_ref), long_ref);
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
    r = ber_tlv_encode(long_buf, 4 + sizeof(long_ref) - 1, 0x5A, sizeof(long_ref), long_ref);
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

static void test_ber_tlv_writer_add_sequence(void)
{
    uint8_t buf[20];
    const uint8_t expected[13] = { 0x5A, 0x03, 'a', 'b', 'c',
                                   0x9F, 0x02, 0x00,
                                   0x9F, 0x81, 0x01, 0x01, 'z' };
    tlv_writer_t w = BER_TLV_WRITER(buf, 0, sizeof(buf));

    memset(buf, 0xEE, sizeof(buf));

    TEST_ASSERT_EQUAL_INT(5, tlv_writer_add(&w, 0x5A, sizeof("abc") - 1, (const uint8_t *)"abc"));
    TEST_ASSERT_EQUAL_INT(5, w.size);
    TEST_ASSERT_EQUAL_INT(8, tlv_writer_add(&w, 0x9F02, sizeof("") - 1, NULL));
    TEST_ASSERT_EQUAL_INT(8, w.size);
    TEST_ASSERT_EQUAL_INT(sizeof(expected), tlv_writer_add(&w, 0x9F8101, sizeof("z") - 1, (const uint8_t *)"z"));
    TEST_ASSERT_EQUAL_INT(sizeof(expected), w.size);

    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(expected)]);
    TEST_ASSERT(w.buffer == buf);
    TEST_ASSERT_EQUAL_INT(sizeof(buf), w.capacity);
}

static void test_ber_tlv_writer_add_behind_existing(void)
{
    uint8_t buf[12];
    const uint8_t initial[5] = { 0x5A, 0x03, 'a', 'a', 'a' };
    const uint8_t expected[10] = { 0x5A, 0x03, 'a', 'a', 'a',
                                   0x9F, 0x02, 0x02, 'n', 'n' };
    tlv_writer_t w = BER_TLV_WRITER(buf, sizeof(initial), sizeof(expected));

    memset(buf, 0xEE, sizeof(buf));
    memcpy(buf, initial, sizeof(initial));

    /* capacity is exactly the new size */
    TEST_ASSERT_EQUAL_INT(sizeof(expected), tlv_writer_add(&w, 0x9F02, sizeof("nn") - 1, (const uint8_t *)"nn"));
    TEST_ASSERT_EQUAL_INT(sizeof(expected), w.size);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(expected)]);
}

static void test_ber_tlv_writer_add_long(void)
{
    const uint8_t initial[3] = { 0x5A, 0x01, 'a' };
    tlv_writer_t w = BER_TLV_WRITER(long_buf, sizeof(initial), sizeof(long_buf));

    memset(long_buf, 0xEE, sizeof(long_buf));
    memcpy(long_buf, initial, sizeof(initial));
    memset(long_ref, 'n', sizeof(long_ref));

    ssize_t r = tlv_writer_add(&w, 0x9F02, sizeof(long_ref), long_ref);

    TEST_ASSERT_EQUAL_INT(sizeof(initial) + 5 + sizeof(long_ref), r);
    TEST_ASSERT_EQUAL_INT(sizeof(initial) + 5 + sizeof(long_ref), w.size);
    TEST_ASSERT_EQUAL_INT(0x9F, long_buf[3]);
    TEST_ASSERT_EQUAL_INT(0x02, long_buf[4]);
    TEST_ASSERT_EQUAL_INT(0x82, long_buf[5]);
    TEST_ASSERT_EQUAL_INT(0x01, long_buf[6]);
    TEST_ASSERT_EQUAL_INT(0x2C, long_buf[7]);
    TEST_ASSERT(memcmp(&long_buf[8], long_ref, sizeof(long_ref)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, long_buf[8 + sizeof(long_ref)]);
}

static void test_ber_tlv_writer_add_nobufs(void)
{
    uint8_t buf[12];
    const uint8_t initial[5] = { 0x5A, 0x03, 'a', 'a', 'a' };
    uint8_t unchanged[12];
    tlv_writer_t w = BER_TLV_WRITER(buf, sizeof(initial), 8);

    memset(buf, 0xEE, sizeof(buf));
    memcpy(buf, initial, sizeof(initial));
    memcpy(unchanged, buf, sizeof(buf));

    /* 3 bytes are free, tag 0x9F02 with "x" needs 4 */
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, tlv_writer_add(&w, 0x9F02, sizeof("x") - 1, (const uint8_t *)"x"));
    TEST_ASSERT_EQUAL_INT(sizeof(initial), w.size);
    TEST_ASSERT(memcmp(buf, unchanged, sizeof(unchanged)) == 0);

    /* an empty element with that tag needs 3 bytes: fits ... */
    TEST_ASSERT_EQUAL_INT(8, tlv_writer_add(&w, 0x9F02, sizeof("") - 1, NULL));
    TEST_ASSERT_EQUAL_INT(8, w.size);

    /* ... but nothing more does */
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, tlv_writer_add(&w, 0x5A, sizeof("") - 1, NULL));
    TEST_ASSERT_EQUAL_INT(8, w.size);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[8]);
}

/*
 * MARK: - Set
 */


static void test_ber_tlv_writer_set_same_size(void)
{
    uint8_t buf[20];
    const uint8_t initial[15] = { 0x5A, 0x03, 'a', 'a', 'a',
                                  0x9F, 0x02, 0x02, 'b', 'b',
                                  0x9F, 0x81, 0x01, 0x01, 'c' };
    const uint8_t expected[15] = { 0x5A, 0x03, 'a', 'a', 'a',
                                   0x9F, 0x02, 0x02, 'n', 'n',
                                   0x9F, 0x81, 0x01, 0x01, 'c' };
    tlv_writer_t w = BER_TLV_WRITER(buf, sizeof(initial), sizeof(initial));

    memset(buf, 0xEE, sizeof(buf));
    memcpy(buf, initial, sizeof(initial));

    /* tag 0x9F02: "bb" -> "nn"; capacity == size */
    ssize_t r = tlv_writer_set(&w, 0x9F02, sizeof("nn") - 1, (const uint8_t *)"nn");

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT_EQUAL_INT(sizeof(expected), w.size);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(expected)]);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(buf) - 1]);
}

static void test_ber_tlv_writer_set_shrink_middle(void)
{
    uint8_t buf[28];
    const uint8_t initial[23] = { 0x5A, 0x03, 'a', 'a', 'a',
                                  0x9F, 0x02, 0x0A, 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b',
                                  0x9F, 0x81, 0x01, 0x01, 'c' };
    const uint8_t expected[14] = { 0x5A, 0x03, 'a', 'a', 'a',
                                   0x9F, 0x02, 0x01, 'n',
                                   0x9F, 0x81, 0x01, 0x01, 'c' };
    tlv_writer_t w = BER_TLV_WRITER(buf, sizeof(initial), sizeof(initial));

    memset(buf, 0xEE, sizeof(buf));
    memcpy(buf, initial, sizeof(initial));

    /* tag 0x9F02: 10 bytes -> 1 byte, the tail moves down */
    ssize_t r = tlv_writer_set(&w, 0x9F02, sizeof("n") - 1, (const uint8_t *)"n");

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT_EQUAL_INT(sizeof(expected), w.size);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(initial)]);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(buf) - 1]);
}

static void test_ber_tlv_writer_set_grow_middle(void)
{
    uint8_t buf[22];
    const uint8_t initial[15] = { 0x5A, 0x03, 'a', 'a', 'a',
                                  0x9F, 0x02, 0x02, 'b', 'b',
                                  0x9F, 0x81, 0x01, 0x01, 'c' };
    const uint8_t expected[17] = { 0x5A, 0x03, 'a', 'a', 'a',
                                   0x9F, 0x02, 0x04, 'n', 'n', 'n', 'n',
                                   0x9F, 0x81, 0x01, 0x01, 'c' };
    tlv_writer_t w = BER_TLV_WRITER(buf, sizeof(initial), sizeof(expected));

    memset(buf, 0xEE, sizeof(buf));
    memcpy(buf, initial, sizeof(initial));

    /* tag 0x9F02: 2 -> 4 bytes, capacity is exactly the new size */
    ssize_t r = tlv_writer_set(&w, 0x9F02, sizeof("nnnn") - 1, (const uint8_t *)"nnnn");

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT_EQUAL_INT(sizeof(expected), w.size);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(expected)]);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(buf) - 1]);
}

static void test_ber_tlv_writer_set_grow_last(void)
{
    uint8_t buf[22];
    const uint8_t initial[15] = { 0x5A, 0x03, 'a', 'a', 'a',
                                  0x9F, 0x02, 0x02, 'b', 'b',
                                  0x9F, 0x81, 0x01, 0x01, 'c' };
    const uint8_t expected[18] = { 0x5A, 0x03, 'a', 'a', 'a',
                                   0x9F, 0x02, 0x02, 'b', 'b',
                                   0x9F, 0x81, 0x01, 0x04, 'n', 'n', 'n', 'n' };
    tlv_writer_t w = BER_TLV_WRITER(buf, sizeof(initial), sizeof(expected));

    memset(buf, 0xEE, sizeof(buf));
    memcpy(buf, initial, sizeof(initial));

    /* the last element grows: nothing to move */
    ssize_t r = tlv_writer_set(&w, 0x9F8101, sizeof("nnnn") - 1, (const uint8_t *)"nnnn");

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT_EQUAL_INT(sizeof(expected), w.size);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(expected)]);
}

static void test_ber_tlv_writer_set_to_zero_length(void)
{
    uint8_t buf[20];
    const uint8_t initial[15] = { 0x5A, 0x03, 'a', 'a', 'a',
                                  0x9F, 0x02, 0x02, 'b', 'b',
                                  0x9F, 0x81, 0x01, 0x01, 'c' };
    const uint8_t expected[13] = { 0x5A, 0x03, 'a', 'a', 'a',
                                   0x9F, 0x02, 0x00,
                                   0x9F, 0x81, 0x01, 0x01, 'c' };
    tlv_writer_t w = BER_TLV_WRITER(buf, sizeof(initial), sizeof(initial));

    memset(buf, 0xEE, sizeof(buf));
    memcpy(buf, initial, sizeof(initial));

    ssize_t r = tlv_writer_set(&w, 0x9F02, sizeof("") - 1, NULL);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT_EQUAL_INT(sizeof(expected), w.size);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(initial)]);
}

static void test_ber_tlv_writer_set_from_zero_length(void)
{
    uint8_t buf[20];
    const uint8_t initial[13] = { 0x5A, 0x03, 'a', 'a', 'a',
                                  0x9F, 0x02, 0x00,
                                  0x9F, 0x81, 0x01, 0x01, 'c' };
    const uint8_t expected[16] = { 0x5A, 0x03, 'a', 'a', 'a',
                                   0x9F, 0x02, 0x03, 'n', 'n', 'n',
                                   0x9F, 0x81, 0x01, 0x01, 'c' };
    tlv_writer_t w = BER_TLV_WRITER(buf, sizeof(initial), sizeof(expected));

    memset(buf, 0xEE, sizeof(buf));
    memcpy(buf, initial, sizeof(initial));

    ssize_t r = tlv_writer_set(&w, 0x9F02, sizeof("nnn") - 1, (const uint8_t *)"nnn");

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT_EQUAL_INT(sizeof(expected), w.size);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(expected)]);
}

static void test_ber_tlv_writer_set_first_of_duplicates(void)
{
    uint8_t buf[16];
    const uint8_t initial[10] = { 0x5A, 0x01, 'a',
                                  0x9F, 0x02, 0x01, 'b',
                                  0x5A, 0x01, 'c' };
    const uint8_t expected[11] = { 0x5A, 0x02, 'n', 'n',
                                   0x9F, 0x02, 0x01, 'b',
                                   0x5A, 0x01, 'c' };
    tlv_writer_t w = BER_TLV_WRITER(buf, sizeof(initial), sizeof(expected));

    memset(buf, 0xEE, sizeof(buf));
    memcpy(buf, initial, sizeof(initial));

    /* only the first tag 0x5A is replaced */
    ssize_t r = tlv_writer_set(&w, 0x5A, sizeof("nn") - 1, (const uint8_t *)"nn");

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT_EQUAL_INT(sizeof(expected), w.size);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(expected)]);
}

static void test_ber_tlv_writer_set_whole_tag_matches(void)
{
    uint8_t buf[32];
    const uint8_t initial[11] = { 0x02, 0x01, 'a',
                                  0x9F, 0x02, 0x01, 'b',
                                  0x9F, 0x01, 0x01, 'c' };
    const uint8_t after_multi[12] = { 0x02, 0x01, 'a',
                                      0x9F, 0x02, 0x02, 'n', 'n',
                                      0x9F, 0x01, 0x01, 'c' };
    const uint8_t after_single[12] = { 0x02, 0x01, 'z',
                                       0x9F, 0x02, 0x02, 'n', 'n',
                                       0x9F, 0x01, 0x01, 'c' };
    tlv_writer_t w = BER_TLV_WRITER(buf, sizeof(initial), sizeof(buf));

    memset(buf, 0xEE, sizeof(buf));
    memcpy(buf, initial, sizeof(initial));

    /* 0x9F02 is not 0x02, and not 0x9F01 */
    TEST_ASSERT_EQUAL_INT(sizeof(after_multi), tlv_writer_set(&w, 0x9F02, sizeof("nn") - 1, (const uint8_t *)"nn"));
    TEST_ASSERT_EQUAL_INT(sizeof(after_multi), w.size);
    TEST_ASSERT(memcmp(buf, after_multi, sizeof(after_multi)) == 0);

    TEST_ASSERT_EQUAL_INT(sizeof(after_single), tlv_writer_set(&w, 0x02, sizeof("z") - 1, (const uint8_t *)"z"));
    TEST_ASSERT_EQUAL_INT(sizeof(after_single), w.size);
    TEST_ASSERT(memcmp(buf, after_single, sizeof(after_single)) == 0);
}

static void test_ber_tlv_writer_set_append_missing(void)
{
    uint8_t buf[24];
    const uint8_t initial[10] = { 0x5A, 0x03, 'a', 'a', 'a',
                                  0x9F, 0x02, 0x02, 'b', 'b' };
    const uint8_t expected[17] = { 0x5A, 0x03, 'a', 'a', 'a',
                                   0x9F, 0x02, 0x02, 'b', 'b',
                                   0x9F, 0x81, 0x01, 0x03, 'n', 'n', 'n' };
    tlv_writer_t w = BER_TLV_WRITER(buf, sizeof(initial), sizeof(expected));

    memset(buf, 0xEE, sizeof(buf));
    memcpy(buf, initial, sizeof(initial));

    ssize_t r = tlv_writer_set(&w, 0x9F8101, sizeof("nnn") - 1, (const uint8_t *)"nnn");

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT_EQUAL_INT(sizeof(expected), w.size);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(expected)]);
}

static void test_ber_tlv_writer_set_append_to_empty(void)
{
    uint8_t buf[8];
    const uint8_t expected[5] = { 0x9F, 0x02, 0x02, 'x', 'y' };
    tlv_writer_t w = BER_TLV_WRITER(buf, 0, sizeof(expected));

    memset(buf, 0xEE, sizeof(buf));

    ssize_t r = tlv_writer_set(&w, 0x9F02, sizeof("xy") - 1, (const uint8_t *)"xy");

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT_EQUAL_INT(sizeof(expected), w.size);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, buf[sizeof(expected)]);
}

static void test_ber_tlv_writer_set_127_to_128(void)
{
    const uint8_t head[3] = { 0x01, 0x01, 'a' };
    const uint8_t tail[3] = { 0x03, 0x01, 'c' };
    ssize_t r;

    /* the length crosses from the short to the long form: 127 -> 128 bytes */
    memset(long_buf, 0xEE, sizeof(long_buf));
    memcpy(long_buf, head, sizeof(head));
    long_buf[3] = 0x9F;
    long_buf[4] = 0x02;
    long_buf[5] = 0x7F;
    memset(&long_buf[6], 'b', 127);
    memcpy(&long_buf[6 + 127], tail, sizeof(tail));
    memset(long_ref, 'n', sizeof(long_ref));

    tlv_writer_t w = BER_TLV_WRITER(long_buf, 6 + 127 + sizeof(tail), 7 + 128 + sizeof(tail));

    r = tlv_writer_set(&w, 0x9F02, 128, long_ref);

    TEST_ASSERT_EQUAL_INT(7 + 128 + sizeof(tail), r);
    TEST_ASSERT_EQUAL_INT(7 + 128 + sizeof(tail), w.size);
    TEST_ASSERT(memcmp(long_buf, head, sizeof(head)) == 0);
    TEST_ASSERT_EQUAL_INT(0x9F, long_buf[3]);
    TEST_ASSERT_EQUAL_INT(0x02, long_buf[4]);
    TEST_ASSERT_EQUAL_INT(0x81, long_buf[5]);
    TEST_ASSERT_EQUAL_INT(0x80, long_buf[6]);
    TEST_ASSERT(memcmp(&long_buf[7], long_ref, 128) == 0);
    TEST_ASSERT(memcmp(&long_buf[7 + 128], tail, sizeof(tail)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, long_buf[7 + 128 + sizeof(tail)]);
}

static void test_ber_tlv_writer_set_128_to_127(void)
{
    const uint8_t head[3] = { 0x01, 0x01, 'a' };
    const uint8_t tail[3] = { 0x03, 0x01, 'c' };
    ssize_t r;

    /* and back: 128 -> 127 bytes */
    memset(long_buf, 0xEE, sizeof(long_buf));
    memcpy(long_buf, head, sizeof(head));
    long_buf[3] = 0x9F;
    long_buf[4] = 0x02;
    long_buf[5] = 0x81;
    long_buf[6] = 0x80;
    memset(&long_buf[7], 'b', 128);
    memcpy(&long_buf[7 + 128], tail, sizeof(tail));
    memset(long_ref, 'n', sizeof(long_ref));

    tlv_writer_t w = BER_TLV_WRITER(long_buf, 7 + 128 + sizeof(tail), 7 + 128 + sizeof(tail));

    r = tlv_writer_set(&w, 0x9F02, 127, long_ref);

    TEST_ASSERT_EQUAL_INT(6 + 127 + sizeof(tail), r);
    TEST_ASSERT_EQUAL_INT(6 + 127 + sizeof(tail), w.size);
    TEST_ASSERT(memcmp(long_buf, head, sizeof(head)) == 0);
    TEST_ASSERT_EQUAL_INT(0x9F, long_buf[3]);
    TEST_ASSERT_EQUAL_INT(0x02, long_buf[4]);
    TEST_ASSERT_EQUAL_INT(0x7F, long_buf[5]);
    TEST_ASSERT(memcmp(&long_buf[6], long_ref, 127) == 0);
    TEST_ASSERT(memcmp(&long_buf[6 + 127], tail, sizeof(tail)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, long_buf[7 + 128 + sizeof(tail)]);
}

static void test_ber_tlv_writer_set_255_to_256(void)
{
    const uint8_t head[3] = { 0x01, 0x01, 'a' };
    const uint8_t tail[3] = { 0x03, 0x01, 'c' };
    ssize_t r;

    /* one length byte -> two length bytes: 255 -> 256 bytes */
    memset(long_buf, 0xEE, sizeof(long_buf));
    memcpy(long_buf, head, sizeof(head));
    long_buf[3] = 0x9F;
    long_buf[4] = 0x02;
    long_buf[5] = 0x81;
    long_buf[6] = 0xFF;
    memset(&long_buf[7], 'b', 255);
    memcpy(&long_buf[7 + 255], tail, sizeof(tail));
    memset(long_ref, 'n', sizeof(long_ref));

    tlv_writer_t w = BER_TLV_WRITER(long_buf, 7 + 255 + sizeof(tail),
                                    8 + 256 + sizeof(tail));

    r = tlv_writer_set(&w, 0x9F02, 256, long_ref);

    TEST_ASSERT_EQUAL_INT(8 + 256 + sizeof(tail), r);
    TEST_ASSERT_EQUAL_INT(8 + 256 + sizeof(tail), w.size);
    TEST_ASSERT(memcmp(long_buf, head, sizeof(head)) == 0);
    TEST_ASSERT_EQUAL_INT(0x9F, long_buf[3]);
    TEST_ASSERT_EQUAL_INT(0x02, long_buf[4]);
    TEST_ASSERT_EQUAL_INT(0x82, long_buf[5]);
    TEST_ASSERT_EQUAL_INT(0x01, long_buf[6]);
    TEST_ASSERT_EQUAL_INT(0x00, long_buf[7]);
    TEST_ASSERT(memcmp(&long_buf[8], long_ref, 256) == 0);
    TEST_ASSERT(memcmp(&long_buf[8 + 256], tail, sizeof(tail)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, long_buf[8 + 256 + sizeof(tail)]);
}

static void test_ber_tlv_writer_set_256_to_255(void)
{
    const uint8_t head[3] = { 0x01, 0x01, 'a' };
    const uint8_t tail[3] = { 0x03, 0x01, 'c' };
    ssize_t r;

    /* and back: 256 -> 255 bytes */
    memset(long_buf, 0xEE, sizeof(long_buf));
    memcpy(long_buf, head, sizeof(head));
    long_buf[3] = 0x9F;
    long_buf[4] = 0x02;
    long_buf[5] = 0x82;
    long_buf[6] = 0x01;
    long_buf[7] = 0x00;
    memset(&long_buf[8], 'b', 256);
    memcpy(&long_buf[8 + 256], tail, sizeof(tail));
    memset(long_ref, 'n', sizeof(long_ref));

    tlv_writer_t w = BER_TLV_WRITER(long_buf, 8 + 256 + sizeof(tail),
                                    8 + 256 + sizeof(tail));

    r = tlv_writer_set(&w, 0x9F02, 255, long_ref);

    TEST_ASSERT_EQUAL_INT(7 + 255 + sizeof(tail), r);
    TEST_ASSERT_EQUAL_INT(7 + 255 + sizeof(tail), w.size);
    TEST_ASSERT(memcmp(long_buf, head, sizeof(head)) == 0);
    TEST_ASSERT_EQUAL_INT(0x9F, long_buf[3]);
    TEST_ASSERT_EQUAL_INT(0x02, long_buf[4]);
    TEST_ASSERT_EQUAL_INT(0x81, long_buf[5]);
    TEST_ASSERT_EQUAL_INT(0xFF, long_buf[6]);
    TEST_ASSERT(memcmp(&long_buf[7], long_ref, 255) == 0);
    TEST_ASSERT(memcmp(&long_buf[7 + 255], tail, sizeof(tail)) == 0);
    TEST_ASSERT_EQUAL_INT(0xEE, long_buf[8 + 256 + sizeof(tail)]);
}

static void test_ber_tlv_writer_set_nobufs(void)
{
    ssize_t r;

    /* tag 0x9F02: 2 -> 4 bytes needs 17, capacity is 16 */
    uint8_t grow[22];
    const uint8_t grow_initial[15] = { 0x5A, 0x03, 'a', 'a', 'a',
                                       0x9F, 0x02, 0x02, 'b', 'b',
                                       0x9F, 0x81, 0x01, 0x01, 'c' };
    uint8_t grow_unchanged[22];
    tlv_writer_t grow_w = BER_TLV_WRITER(grow, sizeof(grow_initial), sizeof(grow_initial) + 1);
    memset(grow, 0xEE, sizeof(grow));
    memcpy(grow, grow_initial, sizeof(grow_initial));
    memcpy(grow_unchanged, grow, sizeof(grow));
    r = tlv_writer_set(&grow_w, 0x9F02, sizeof("nnnn") - 1, (const uint8_t *)"nnnn");
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, r);
    TEST_ASSERT_EQUAL_INT(sizeof(grow_initial), grow_w.size);
    TEST_ASSERT(memcmp(grow, grow_unchanged, sizeof(grow_unchanged)) == 0);

    /* appending tag 0x9F8101 with 4 bytes needs 8, capacity leaves 7 */
    uint8_t app[24];
    const uint8_t app_initial[10] = { 0x5A, 0x03, 'a', 'a', 'a',
                                      0x9F, 0x02, 0x02, 'b', 'b' };
    uint8_t app_unchanged[24];
    tlv_writer_t app_w = BER_TLV_WRITER(app, sizeof(app_initial), sizeof(app_initial) + 7);
    memset(app, 0xEE, sizeof(app));
    memcpy(app, app_initial, sizeof(app_initial));
    memcpy(app_unchanged, app, sizeof(app));
    r = tlv_writer_set(&app_w, 0x9F8101, sizeof("nnnn") - 1, (const uint8_t *)"nnnn");
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, r);
    TEST_ASSERT_EQUAL_INT(sizeof(app_initial), app_w.size);
    TEST_ASSERT(memcmp(app, app_unchanged, sizeof(app_unchanged)) == 0);

    /* an empty element with a 3-byte tag needs 4 bytes, only 3 are free */
    app_w.capacity = sizeof(app_initial) + 3;
    r = tlv_writer_set(&app_w, 0x9F8101, sizeof("") - 1, NULL);
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, r);
    TEST_ASSERT_EQUAL_INT(sizeof(app_initial), app_w.size);
    TEST_ASSERT(memcmp(app, app_unchanged, sizeof(app_unchanged)) == 0);
}

static void test_ber_tlv_writer_set_nobufs_long(void)
{
    ssize_t r;
    int bad;

    /* tag 0x01 "a", tag 0x9F02 with 127 bytes (130 in total); growing it to
     * 128 bytes needs 133 but the capacity is 132 */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x01;
    long_buf[1] = 0x01;
    long_buf[2] = 'a';
    long_buf[3] = 0x9F;
    long_buf[4] = 0x02;
    long_buf[5] = 0x7F;
    memset(&long_buf[6], 'b', 127);
    memset(long_ref, 'n', sizeof(long_ref));

    tlv_writer_t grow_w = BER_TLV_WRITER(long_buf, 6 + 127, 6 + 127 + 1);

    r = tlv_writer_set(&grow_w, 0x9F02, 128, long_ref);

    TEST_ASSERT_EQUAL_INT(-ENOBUFS, r);
    TEST_ASSERT_EQUAL_INT(6 + 127, grow_w.size);
    TEST_ASSERT_EQUAL_INT(0x01, long_buf[0]);
    TEST_ASSERT_EQUAL_INT(0x01, long_buf[1]);
    TEST_ASSERT_EQUAL_INT('a', long_buf[2]);
    TEST_ASSERT_EQUAL_INT(0x9F, long_buf[3]);
    TEST_ASSERT_EQUAL_INT(0x02, long_buf[4]);
    TEST_ASSERT_EQUAL_INT(0x7F, long_buf[5]);
    bad = 0;
    for (size_t i = 6; i < sizeof(long_buf); i++) {
        if (long_buf[i] != ((i < 6 + 127) ? 'b' : 0xEE)) {
            bad++;
        }
    }
    TEST_ASSERT_EQUAL_INT(0, bad);

    /* appending 300 bytes with a 2-byte tag needs 305, only 304 are free */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x01;
    long_buf[1] = 0x01;
    long_buf[2] = 'a';

    tlv_writer_t app_w = BER_TLV_WRITER(long_buf, 3, 3 + 5 + sizeof(long_ref) - 1);

    r = tlv_writer_set(&app_w, 0x9F09, sizeof(long_ref), long_ref);

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

static void test_ber_tlv_writer_set_malformed(void)
{
    ssize_t r;

    /* truncated value, tag present and absent */
    uint8_t truncated[16];
    const uint8_t truncated_initial[3] = { 0x5A, 0x05, 'a' };
    uint8_t truncated_orig[16];
    tlv_writer_t truncated_w = BER_TLV_WRITER(truncated, sizeof(truncated_initial), sizeof(truncated));
    memset(truncated, 0xEE, sizeof(truncated));
    memcpy(truncated, truncated_initial, sizeof(truncated_initial));
    memcpy(truncated_orig, truncated, sizeof(truncated));
    r = tlv_writer_set(&truncated_w, 0x5A, sizeof("nn") - 1, (const uint8_t *)"nn");
    TEST_ASSERT_EQUAL_INT(-EBADMSG, r);
    TEST_ASSERT_EQUAL_INT(sizeof(truncated_initial), truncated_w.size);
    TEST_ASSERT(memcmp(truncated, truncated_orig, sizeof(truncated_orig)) == 0);
    r = tlv_writer_set(&truncated_w, 0x9F09, sizeof("nn") - 1, (const uint8_t *)"nn");
    TEST_ASSERT_EQUAL_INT(-EBADMSG, r);
    TEST_ASSERT_EQUAL_INT(sizeof(truncated_initial), truncated_w.size);
    TEST_ASSERT(memcmp(truncated, truncated_orig, sizeof(truncated_orig)) == 0);

    /* half a tag after a valid element, tag absent */
    uint8_t trailing[16];
    const uint8_t trailing_initial[4] = { 0x5A, 0x01, 'a', 0x9F };
    uint8_t trailing_orig[16];
    tlv_writer_t trailing_w = BER_TLV_WRITER(trailing, sizeof(trailing_initial), sizeof(trailing));
    memset(trailing, 0xEE, sizeof(trailing));
    memcpy(trailing, trailing_initial, sizeof(trailing_initial));
    memcpy(trailing_orig, trailing, sizeof(trailing));
    r = tlv_writer_set(&trailing_w, 0x9F09, sizeof("nn") - 1, (const uint8_t *)"nn");
    TEST_ASSERT_EQUAL_INT(-EBADMSG, r);
    TEST_ASSERT_EQUAL_INT(sizeof(trailing_initial), trailing_w.size);
    TEST_ASSERT(memcmp(trailing, trailing_orig, sizeof(trailing_orig)) == 0);

    /* an invalid tag (leading zero) */
    uint8_t bad_tag[16];
    const uint8_t bad_tag_initial[7] = { 0x5A, 0x01, 'a', 0x9F, 0x00, 0x01, 'b' };
    uint8_t bad_tag_orig[16];
    tlv_writer_t bad_tag_w = BER_TLV_WRITER(bad_tag, sizeof(bad_tag_initial), sizeof(bad_tag));
    memset(bad_tag, 0xEE, sizeof(bad_tag));
    memcpy(bad_tag, bad_tag_initial, sizeof(bad_tag_initial));
    memcpy(bad_tag_orig, bad_tag, sizeof(bad_tag));
    r = tlv_writer_set(&bad_tag_w, 0x9F09, sizeof("nn") - 1, (const uint8_t *)"nn");
    TEST_ASSERT_EQUAL_INT(-EBADMSG, r);
    TEST_ASSERT_EQUAL_INT(sizeof(bad_tag_initial), bad_tag_w.size);
    TEST_ASSERT(memcmp(bad_tag, bad_tag_orig, sizeof(bad_tag_orig)) == 0);
}

/*
 * MARK: - Remove
 */

static void test_ber_tlv_writer_remove_middle(void)
{
    uint8_t buf[15] = { 0x5A, 0x03, 'a', 'a', 'a',
                        0x9F, 0x02, 0x02, 'b', 'b',
                        0x9F, 0x81, 0x01, 0x01, 'c' };
    const uint8_t expected[10] = { 0x5A, 0x03, 'a', 'a', 'a',
                                   0x9F, 0x81, 0x01, 0x01, 'c' };
    tlv_writer_t w = BER_TLV_WRITER(buf, sizeof(buf), sizeof(buf));

    ssize_t r = tlv_writer_remove(&w, 0x9F02);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT_EQUAL_INT(sizeof(expected), w.size);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
}

static void test_ber_tlv_writer_remove_first(void)
{
    uint8_t buf[15] = { 0x5A, 0x03, 'a', 'a', 'a',
                        0x9F, 0x02, 0x02, 'b', 'b',
                        0x9F, 0x81, 0x01, 0x01, 'c' };
    const uint8_t expected[10] = { 0x9F, 0x02, 0x02, 'b', 'b',
                                   0x9F, 0x81, 0x01, 0x01, 'c' };
    tlv_writer_t w = BER_TLV_WRITER(buf, sizeof(buf), sizeof(buf));

    ssize_t r = tlv_writer_remove(&w, 0x5A);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT_EQUAL_INT(sizeof(expected), w.size);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
}

static void test_ber_tlv_writer_remove_last(void)
{
    uint8_t buf[15] = { 0x5A, 0x03, 'a', 'a', 'a',
                        0x9F, 0x02, 0x02, 'b', 'b',
                        0x9F, 0x81, 0x01, 0x01, 'c' };
    const uint8_t expected[10] = { 0x5A, 0x03, 'a', 'a', 'a',
                                   0x9F, 0x02, 0x02, 'b', 'b' };
    tlv_writer_t w = BER_TLV_WRITER(buf, sizeof(buf), sizeof(buf));

    ssize_t r = tlv_writer_remove(&w, 0x9F8101);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT_EQUAL_INT(sizeof(expected), w.size);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
}

static void test_ber_tlv_writer_remove_no_match(void)
{
    uint8_t buf[15] = { 0x5A, 0x03, 'a', 'a', 'a',
                        0x9F, 0x02, 0x02, 'b', 'b',
                        0x9F, 0x81, 0x01, 0x01, 'c' };
    const uint8_t original[15] = { 0x5A, 0x03, 'a', 'a', 'a',
                                   0x9F, 0x02, 0x02, 'b', 'b',
                                   0x9F, 0x81, 0x01, 0x01, 'c' };
    tlv_writer_t w = BER_TLV_WRITER(buf, sizeof(buf), sizeof(buf));

    /* neither absent tags nor parts of multi-byte tags match */
    TEST_ASSERT_EQUAL_INT(sizeof(original), tlv_writer_remove(&w, 0x09));
    TEST_ASSERT_EQUAL_INT(sizeof(original), tlv_writer_remove(&w, 0x00));
    TEST_ASSERT_EQUAL_INT(sizeof(original), tlv_writer_remove(&w, 0x9F));
    TEST_ASSERT_EQUAL_INT(sizeof(original), tlv_writer_remove(&w, 0x02));
    TEST_ASSERT_EQUAL_INT(sizeof(original), tlv_writer_remove(&w, 0x9F81));
    TEST_ASSERT_EQUAL_INT(sizeof(original), w.size);
    TEST_ASSERT(memcmp(buf, original, sizeof(original)) == 0);
}

static void test_ber_tlv_writer_remove_empty_writer(void)
{
    uint8_t buf[4] = { 0xEE, 0xEE, 0xEE, 0xEE };
    tlv_writer_t w = BER_TLV_WRITER(buf, 0, sizeof(buf));

    TEST_ASSERT_EQUAL_INT(0, tlv_writer_remove(&w, 0x00));
    TEST_ASSERT_EQUAL_INT(0, tlv_writer_remove(&w, 0x9F02));
    TEST_ASSERT_EQUAL_INT(0, w.size);
}

static void test_ber_tlv_writer_remove_ignores_spare_room(void)
{
    uint8_t buf[14] = { 0x5A, 0x03, 'a', 'a', 'a',
                        0x9F, 0x02, 0x02, 'b', 'b',
                        0x5A, 0x01, 'c', 'c' };
    const uint8_t expected[5] = { 0x9F, 0x02, 0x02, 'b', 'b' };
    tlv_writer_t w = BER_TLV_WRITER(buf, 10, sizeof(buf));

    /* the element behind w.size is not part of the writer */
    ssize_t r = tlv_writer_remove(&w, 0x5A);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT_EQUAL_INT(sizeof(expected), w.size);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
}

static void test_ber_tlv_writer_remove_whole_tag_matches(void)
{
    uint8_t buf[11] = { 0x02, 0x01, 'a',
                        0x9F, 0x02, 0x01, 'b',
                        0x9F, 0x01, 0x01, 'c' };
    const uint8_t after_single[8] = { 0x9F, 0x02, 0x01, 'b',
                                      0x9F, 0x01, 0x01, 'c' };
    const uint8_t after_multi[4] = { 0x9F, 0x01, 0x01, 'c' };
    tlv_writer_t w = BER_TLV_WRITER(buf, sizeof(buf), sizeof(buf));

    /* 0x02 is not 0x9F02 */
    TEST_ASSERT_EQUAL_INT(sizeof(after_single), tlv_writer_remove(&w, 0x02));
    TEST_ASSERT_EQUAL_INT(sizeof(after_single), w.size);
    TEST_ASSERT(memcmp(buf, after_single, sizeof(after_single)) == 0);

    TEST_ASSERT_EQUAL_INT(sizeof(after_multi), tlv_writer_remove(&w, 0x9F02));
    TEST_ASSERT_EQUAL_INT(sizeof(after_multi), w.size);
    TEST_ASSERT(memcmp(buf, after_multi, sizeof(after_multi)) == 0);
}

static void test_ber_tlv_writer_remove_adjacent_duplicates(void)
{
    uint8_t buf[16] = { 0x01, 0x01, 'a',
                        0x9F, 0x02, 0x02, 'b', 'b',
                        0x9F, 0x02, 0x01, 'c',
                        0x03, 0x01, 'd' };
    const uint8_t expected[6] = { 0x01, 0x01, 'a',
                                  0x03, 0x01, 'd' };
    tlv_writer_t w = BER_TLV_WRITER(buf, sizeof(buf) - 1, sizeof(buf));

    /* the stray byte behind the elements is outside of the writer */
    ssize_t r = tlv_writer_remove(&w, 0x9F02);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT_EQUAL_INT(sizeof(expected), w.size);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
}

static void test_ber_tlv_writer_remove_interleaved_duplicates(void)
{
    uint8_t buf[19] = { 0x9F, 0x02, 0x01, 'a',
                        0x01, 0x01, 'b',
                        0x9F, 0x02, 0x00,
                        0x03, 0x02, 'c', 'c',
                        0x9F, 0x02, 0x01, 'd' };
    const uint8_t expected[7] = { 0x01, 0x01, 'b',
                                  0x03, 0x02, 'c', 'c' };
    tlv_writer_t w = BER_TLV_WRITER(buf, sizeof(buf) - 1, sizeof(buf));

    ssize_t r = tlv_writer_remove(&w, 0x9F02);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT_EQUAL_INT(sizeof(expected), w.size);
    TEST_ASSERT(memcmp(buf, expected, sizeof(expected)) == 0);
}

static void test_ber_tlv_writer_remove_every_element(void)
{
    uint8_t buf[11] = { 0x9F, 0x05, 0x01, 'a',
                        0x9F, 0x05, 0x02, 'b', 'b',
                        0x9F, 0x05 };
    tlv_writer_t w = BER_TLV_WRITER(buf, sizeof(buf) - 2, sizeof(buf));

    /* the stray bytes behind the elements are outside of the writer */
    TEST_ASSERT_EQUAL_INT(0, tlv_writer_remove(&w, 0x9F05));
    TEST_ASSERT_EQUAL_INT(0, w.size);
}

static void test_ber_tlv_writer_remove_long_element(void)
{
    const uint8_t expected[4] = { 0x5A, 0x02, 'b', 'b' };
    ssize_t r;

    /* tag 0x9F02 with 300 bytes, tag 0x5A "bb", tag 0x9F02 with 255 bytes: remove 0x9F02 */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x9F;
    long_buf[1] = 0x02;
    long_buf[2] = 0x82;
    long_buf[3] = 0x01;
    long_buf[4] = 0x2C;
    memset(&long_buf[5], 'l', sizeof(long_ref));
    long_buf[5 + sizeof(long_ref)] = 0x5A;
    long_buf[5 + sizeof(long_ref) + 1] = sizeof("bb") - 1;
    long_buf[5 + sizeof(long_ref) + 2] = 'b';
    long_buf[5 + sizeof(long_ref) + 3] = 'b';
    long_buf[5 + sizeof(long_ref) + 4] = 0x9F;
    long_buf[5 + sizeof(long_ref) + 5] = 0x02;
    long_buf[5 + sizeof(long_ref) + 6] = 0x81;
    long_buf[5 + sizeof(long_ref) + 7] = 0xFF;
    memset(&long_buf[5 + sizeof(long_ref) + 8], 'm', 255);

    tlv_writer_t w = BER_TLV_WRITER(long_buf, 5 + sizeof(long_ref) + 8 + 255, sizeof(long_buf));

    r = tlv_writer_remove(&w, 0x9F02);

    TEST_ASSERT_EQUAL_INT(sizeof(expected), r);
    TEST_ASSERT_EQUAL_INT(sizeof(expected), w.size);
    TEST_ASSERT(memcmp(long_buf, expected, sizeof(expected)) == 0);
}

static void test_ber_tlv_writer_remove_keeps_long_elements(void)
{
    ssize_t r;

    /* tag 0x5A with 300 bytes, tag 0x9F02 "aa", tag 0x9F8101 with 255 bytes: remove 0x9F02 */
    memset(long_buf, 0xEE, sizeof(long_buf));
    long_buf[0] = 0x5A;
    long_buf[1] = 0x82;
    long_buf[2] = 0x01;
    long_buf[3] = 0x2C;
    memset(&long_buf[4], 'l', sizeof(long_ref));
    long_buf[4 + sizeof(long_ref)] = 0x9F;
    long_buf[4 + sizeof(long_ref) + 1] = 0x02;
    long_buf[4 + sizeof(long_ref) + 2] = sizeof("aa") - 1;
    long_buf[4 + sizeof(long_ref) + 3] = 'a';
    long_buf[4 + sizeof(long_ref) + 4] = 'a';
    long_buf[4 + sizeof(long_ref) + 5] = 0x9F;
    long_buf[4 + sizeof(long_ref) + 6] = 0x81;
    long_buf[4 + sizeof(long_ref) + 7] = 0x01;
    long_buf[4 + sizeof(long_ref) + 8] = 0x81;
    long_buf[4 + sizeof(long_ref) + 9] = 0xFF;
    memset(&long_buf[4 + sizeof(long_ref) + 10], 'm', 255);

    tlv_writer_t w = BER_TLV_WRITER(long_buf, 4 + sizeof(long_ref) + 10 + 255, sizeof(long_buf));

    r = tlv_writer_remove(&w, 0x9F02);

    TEST_ASSERT_EQUAL_INT(4 + sizeof(long_ref) + 5 + 255, r);
    TEST_ASSERT_EQUAL_INT(4 + sizeof(long_ref) + 5 + 255, w.size);
    TEST_ASSERT_EQUAL_INT(0x5A, long_buf[0]);
    TEST_ASSERT_EQUAL_INT(0x82, long_buf[1]);
    TEST_ASSERT_EQUAL_INT(0x01, long_buf[2]);
    TEST_ASSERT_EQUAL_INT(0x2C, long_buf[3]);
    memset(long_ref, 'l', sizeof(long_ref));
    TEST_ASSERT(memcmp(&long_buf[4], long_ref, sizeof(long_ref)) == 0);
    TEST_ASSERT_EQUAL_INT(0x9F, long_buf[4 + sizeof(long_ref)]);
    TEST_ASSERT_EQUAL_INT(0x81, long_buf[4 + sizeof(long_ref) + 1]);
    TEST_ASSERT_EQUAL_INT(0x01, long_buf[4 + sizeof(long_ref) + 2]);
    TEST_ASSERT_EQUAL_INT(0x81, long_buf[4 + sizeof(long_ref) + 3]);
    TEST_ASSERT_EQUAL_INT(0xFF, long_buf[4 + sizeof(long_ref) + 4]);
    memset(long_ref, 'm', 255);
    TEST_ASSERT(memcmp(&long_buf[4 + sizeof(long_ref) + 5], long_ref, 255) == 0);
}

static void test_ber_tlv_writer_remove_malformed(void)
{
    uint8_t truncated[3] = { 0x5A, 0x05, 'a' };
    uint8_t trailing[4] = { 0x5A, 0x01, 'a', 0x9F };
    uint8_t bad_tag[7] = { 0x5A, 0x01, 'a', 0x9F, 0x00, 0x01, 'b' };
    tlv_writer_t truncated_w = BER_TLV_WRITER(truncated, sizeof(truncated), sizeof(truncated));
    tlv_writer_t trailing_w = BER_TLV_WRITER(trailing, sizeof(trailing), sizeof(trailing));
    tlv_writer_t bad_tag_w = BER_TLV_WRITER(bad_tag, sizeof(bad_tag), sizeof(bad_tag));

    TEST_ASSERT_EQUAL_INT(-EBADMSG, tlv_writer_remove(&truncated_w, 0x09));
    TEST_ASSERT_EQUAL_INT(-EBADMSG, tlv_writer_remove(&trailing_w, 0x09));
    TEST_ASSERT_EQUAL_INT(-EBADMSG, tlv_writer_remove(&bad_tag_w, 0x09));
}

/*
 * MARK: - Get
 */

static void test_ber_tlv_writer_get_hit(void)
{
    uint8_t buf[20] = { 0x5A, 0x03, 'a', 'b', 'c',
                        0x9F, 0x02, 0x00,
                        0x9F, 0x81, 0x01, 0x01, 'z',
                        0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE };
    const uint8_t *value = NULL;
    tlv_writer_t w = BER_TLV_WRITER(buf, 13, sizeof(buf));

    TEST_ASSERT_EQUAL_INT(sizeof("abc") - 1, tlv_writer_get(&w, 0x5A, &value));
    TEST_ASSERT(value == &buf[2]);

    TEST_ASSERT_EQUAL_INT(sizeof("") - 1, tlv_writer_get(&w, 0x9F02, &value));

    TEST_ASSERT_EQUAL_INT(sizeof("z") - 1, tlv_writer_get(&w, 0x9F8101, &value));
    TEST_ASSERT(value == &buf[12]);
}

static void test_ber_tlv_writer_get_missing(void)
{
    uint8_t buf[20] = { 0x5A, 0x03, 'a', 'b', 'c',
                        0x9F, 0x02, 0x00,
                        0x9F, 0x81, 0x01, 0x01, 'z',
                        0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE };
    const uint8_t *value = NULL;
    tlv_writer_t w = BER_TLV_WRITER(buf, 8, sizeof(buf));
    tlv_writer_t empty = BER_TLV_WRITER(buf, 0, sizeof(buf));

    TEST_ASSERT_EQUAL_INT(-ENOENT, tlv_writer_get(&w, 0x09, &value));
    TEST_ASSERT_EQUAL_INT(-ENOENT, tlv_writer_get(&w, 0x9F, &value));
    TEST_ASSERT(value == NULL);

    /* tag 0x9F8101 is in the buffer, but behind the writer's size */
    TEST_ASSERT_EQUAL_INT(-ENOENT, tlv_writer_get(&w, 0x9F8101, &value));

    TEST_ASSERT_EQUAL_INT(-ENOENT, tlv_writer_get(&empty, 0x5A, &value));
}

static void test_ber_tlv_writer_get_after_set(void)
{
    uint8_t buf[20];
    const uint8_t *value = NULL;
    tlv_writer_t w = BER_TLV_WRITER(buf, 0, sizeof(buf));

    memset(buf, 0xEE, sizeof(buf));

    TEST_ASSERT_EQUAL_INT(-ENOENT, tlv_writer_get(&w, 0x9F02, &value));

    tlv_writer_set(&w, 0x9F02, sizeof("abc") - 1, (const uint8_t *)"abc");
    TEST_ASSERT_EQUAL_INT(sizeof("abc") - 1, tlv_writer_get(&w, 0x9F02, &value));
    TEST_ASSERT(value == &buf[3]);

    tlv_writer_set(&w, 0x9F02, sizeof("z") - 1, (const uint8_t *)"z");
    TEST_ASSERT_EQUAL_INT(sizeof("z") - 1, tlv_writer_get(&w, 0x9F02, &value));
    TEST_ASSERT(value == &buf[3]);

    tlv_writer_remove(&w, 0x9F02);
    TEST_ASSERT_EQUAL_INT(-ENOENT, tlv_writer_get(&w, 0x9F02, &value));
}

Test *tests_tlv_ber_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_ber_tlv_decode_sequence),
        new_TestFixture(test_ber_tlv_decode_nested),
        new_TestFixture(test_ber_tlv_decode_tag_edges),
        new_TestFixture(test_ber_tlv_decode_tag_too_long),
        new_TestFixture(test_ber_tlv_decode_tag_leading_zero),
        new_TestFixture(test_ber_tlv_decode_tag_truncated),
        new_TestFixture(test_ber_tlv_decode_zero_length),
        new_TestFixture(test_ber_tlv_decode_longest_short),
        new_TestFixture(test_ber_tlv_decode_shortest_long),
        new_TestFixture(test_ber_tlv_decode_one_byte_long),
        new_TestFixture(test_ber_tlv_decode_two_byte_long),
        new_TestFixture(test_ber_tlv_decode_length_reserved),
        new_TestFixture(test_ber_tlv_decode_length_truncated),
        new_TestFixture(test_ber_tlv_decode_length_oversized),
        new_TestFixture(test_ber_tlv_decode_error_untouched),
        new_TestFixture(test_ber_tlv_find_hit),
        new_TestFixture(test_ber_tlv_find_first_of_duplicates),
        new_TestFixture(test_ber_tlv_find_whole_tag_matches),
        new_TestFixture(test_ber_tlv_find_missing),
        new_TestFixture(test_ber_tlv_find_empty),
        new_TestFixture(test_ber_tlv_find_after_long_element),
        new_TestFixture(test_ber_tlv_find_malformed),
        new_TestFixture(test_ber_tlv_encode_short),
        new_TestFixture(test_ber_tlv_encode_zero_length),
        new_TestFixture(test_ber_tlv_encode_two_byte_tag),
        new_TestFixture(test_ber_tlv_encode_three_byte_tag),
        new_TestFixture(test_ber_tlv_encode_four_byte_tag),
        new_TestFixture(test_ber_tlv_encode_longest_short),
        new_TestFixture(test_ber_tlv_encode_shortest_long),
        new_TestFixture(test_ber_tlv_encode_one_byte_long),
        new_TestFixture(test_ber_tlv_encode_two_byte_long),
        new_TestFixture(test_ber_tlv_encode_nobufs),
        new_TestFixture(test_ber_tlv_writer_add_sequence),
        new_TestFixture(test_ber_tlv_writer_add_behind_existing),
        new_TestFixture(test_ber_tlv_writer_add_long),
        new_TestFixture(test_ber_tlv_writer_add_nobufs),
        new_TestFixture(test_ber_tlv_writer_set_same_size),
        new_TestFixture(test_ber_tlv_writer_set_shrink_middle),
        new_TestFixture(test_ber_tlv_writer_set_grow_middle),
        new_TestFixture(test_ber_tlv_writer_set_grow_last),
        new_TestFixture(test_ber_tlv_writer_set_to_zero_length),
        new_TestFixture(test_ber_tlv_writer_set_from_zero_length),
        new_TestFixture(test_ber_tlv_writer_set_first_of_duplicates),
        new_TestFixture(test_ber_tlv_writer_set_whole_tag_matches),
        new_TestFixture(test_ber_tlv_writer_set_append_missing),
        new_TestFixture(test_ber_tlv_writer_set_append_to_empty),
        new_TestFixture(test_ber_tlv_writer_set_127_to_128),
        new_TestFixture(test_ber_tlv_writer_set_128_to_127),
        new_TestFixture(test_ber_tlv_writer_set_255_to_256),
        new_TestFixture(test_ber_tlv_writer_set_256_to_255),
        new_TestFixture(test_ber_tlv_writer_set_nobufs),
        new_TestFixture(test_ber_tlv_writer_set_nobufs_long),
        new_TestFixture(test_ber_tlv_writer_set_malformed),
        new_TestFixture(test_ber_tlv_writer_remove_middle),
        new_TestFixture(test_ber_tlv_writer_remove_first),
        new_TestFixture(test_ber_tlv_writer_remove_last),
        new_TestFixture(test_ber_tlv_writer_remove_no_match),
        new_TestFixture(test_ber_tlv_writer_remove_empty_writer),
        new_TestFixture(test_ber_tlv_writer_remove_ignores_spare_room),
        new_TestFixture(test_ber_tlv_writer_remove_whole_tag_matches),
        new_TestFixture(test_ber_tlv_writer_remove_adjacent_duplicates),
        new_TestFixture(test_ber_tlv_writer_remove_interleaved_duplicates),
        new_TestFixture(test_ber_tlv_writer_remove_every_element),
        new_TestFixture(test_ber_tlv_writer_remove_long_element),
        new_TestFixture(test_ber_tlv_writer_remove_keeps_long_elements),
        new_TestFixture(test_ber_tlv_writer_remove_malformed),
        new_TestFixture(test_ber_tlv_writer_get_hit),
        new_TestFixture(test_ber_tlv_writer_get_missing),
        new_TestFixture(test_ber_tlv_writer_get_after_set),
    };

    EMB_UNIT_TESTCALLER(tlv_ber_tests, NULL, NULL, fixtures);

    return (Test *)&tlv_ber_tests;
}

void tests_tlv_ber(void)
{
    TESTS_RUN(tests_tlv_ber_tests());
}
