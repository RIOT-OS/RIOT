/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 */
#include <errno.h>
#include <stdlib.h>

#include "embUnit.h"

#include "net/inet_csum.h"

#include "unittests-constants.h"
#include "tests-inet_csum.h"

static void test_inet_csum__rfc_example(void)
{
    /* source: https://tools.ietf.org/html/rfc1071#section-3 */
    uint8_t data[] = {
        0x00, 0x01, 0xf2, 0x03, 0xf4, 0xf5, 0xf6, 0xf7
    };

    TEST_ASSERT_EQUAL_INT(0xddf2, inet_csum(0, data, sizeof(data)));
}

static void test_inet_csum__ipv6_pseudo_hdr(void)
{
    /* source: https://www.cloudshark.org/captures/ea72fbab241b (No. 56) */
    uint8_t data[] = {
        0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* IPv6 source */
        0x5a, 0x6d, 0x8f, 0xff, 0xfe, 0x56, 0x30, 0x09,
        0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* IPv6 destination */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x3a, /* payload length + next header */
        0x86, 0x00, 0xab, 0x32, 0x40, 0x58, 0x07, 0x08, /* ICMPv6 payload */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x03, 0x04, 0x40, 0xc0, 0x00, 0x00, 0x00, 0x1e,
        0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00,
        0x20, 0x02, 0x18, 0x3d, 0xdb, 0xa4, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x01, 0x01, 0x58, 0x6d, 0x8f, 0x56, 0x30, 0x09
    };

    /* result unnormalized: take 1's-complement of 0 */
    TEST_ASSERT_EQUAL_INT(0xffff, inet_csum(0x0, data, sizeof(data)));
}

static void test_inet_csum__set_initial_sum(void)
{
    /* source: https://www.cloudshark.org/captures/ea72fbab241b (No. 56) */
    uint8_t data[] = {
        0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* IPv6 source */
        0x5a, 0x6d, 0x8f, 0xff, 0xfe, 0x56, 0x30, 0x09,
        0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* IPv6 destination */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x86, 0x00, 0xab, 0x32, 0x40, 0x58, 0x07, 0x08, /* ICMPv6 payload */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x03, 0x04, 0x40, 0xc0, 0x00, 0x00, 0x00, 0x1e,
        0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00,
        0x20, 0x02, 0x18, 0x3d, 0xdb, 0xa4, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x01, 0x01, 0x58, 0x6d, 0x8f, 0x56, 0x30, 0x09
    };

    /* result unnormalized: take 1's-complement of 0
     * set next header and payload length as initial value */
    TEST_ASSERT_EQUAL_INT(0xffff, inet_csum(0x38 + 0x3a, data, sizeof(data)));
}

static void test_inet_csum__wraps_more_than_once(void)
{
    /* catches the corner-case that the internal wrap-around does not suffice
     * to be done once */
    uint8_t data[] = {
        0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xc8, 0x86, 0xcd, 0xff, 0xfe, 0x0f, 0xce, 0x49,
        0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x18, 0xaa, 0x2d, 0xff, 0xfe, 0x44, 0x43, 0xac
    };

    /* values were taken from a case I encountered in the wild */
    TEST_ASSERT_EQUAL_INT(0x0002, inet_csum(0x1785, data, sizeof(data)));
}

static void test_inet_csum__calculate_csum(void)
{
    /* source: http://en.wikipedia.org/w/index.php?title=IPv4_header_checksum&oldid=645516564
     * but left checksum 0 */
    uint8_t data[] = {
        0x45, 0x00, 0x00, 0x73, 0x00, 0x00, 0x40, 0x00,
        0x40, 0x11, 0x00, 0x00, 0xc0, 0xa8, 0x00, 0x01,
        0xc0, 0xa8, 0x00, 0xc7,
    };

    /* result unnormalized: take 1's-complement of 0xb861 */
    TEST_ASSERT_EQUAL_INT(0x479e, inet_csum(0, data, sizeof(data)));
}

static void test_inet_csum__odd_len(void)
{
    /* source: https://www.cloudshark.org/captures/ea72fbab241b (No. 1) */
    uint8_t data[] = {
        0xc0, 0xa8, 0x01, 0x91, 0x4b, 0x4b, 0x4b, 0x4b, /* IPv4 source + dest*/
        0xf6, 0xfb, 0x00, 0x35, 0x00, 0x27, 0xd1, 0xa2, /* UDP header */
        0xa5, 0x6f, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, /* DNS payload */
        0x00, 0x00, 0x00, 0x00, 0x09, 0x74, 0x65, 0x73,
        0x74, 0x2d, 0x69, 0x70, 0x76, 0x36, 0x03, 0x63,
        0x6f, 0x6d, 0x00, 0x00, 0x01, 0x00, 0x01,
    };

    /* result unnormalized: take 1's-complement of 0
     * set next header and payload length as initial value */
    TEST_ASSERT_EQUAL_INT(0xffff, inet_csum(17 + 39, data, sizeof(data)));
}

static void test_inet_csum__two_app_snips(void)
{
    /* CoAP header with Uri-Path and Content-Format options; odd length */
    uint8_t data_hdr[] = {
        0x50, 0x02, 0x00, 0x01, 0xb4, 0x74, 0x65, 0x73,
        0x74, 0x10, 0xff,
    };
    /* Single character payload, 'a' */
    uint8_t data_pyld[] = {
        0x61,
    };
    uint16_t hdr_sum, pyld_sum, hdr_expected = 0xdcfc;

    /* result unnormalized:
     * initial sum (0) is arbitrary, and incoming length (0) must be even;
     * we expect last byte is shifted left for this odd-sized header  */
    hdr_sum = inet_csum_slice(0, data_hdr, sizeof(data_hdr), 0);
    TEST_ASSERT_EQUAL_INT(hdr_expected, hdr_sum);

    /* Since header was odd length, we expect the single byte in the payload
     * snip is not shifted left */
    pyld_sum = inet_csum_slice(hdr_expected, data_pyld, sizeof(data_pyld), sizeof(data_hdr));
    TEST_ASSERT_EQUAL_INT(hdr_expected + 0x61, pyld_sum);
}

static void test_inet_csum__empty_app_buffer(void)
{
    /* CoAP header with Uri-Path and Content-Format options; odd length */
    uint8_t data_hdr[] = {
        0x50, 0x02, 0x00, 0x01, 0xb4, 0x74, 0x65, 0x73,
        0x74, 0x10, 0xff,
    };
    uint16_t hdr_sum, pyld_sum, hdr_expected = 0xdcfc;

    /* result unnormalized:
     * explictly using an odd-sized header for the first slice, to setup corner case  */
    hdr_sum = inet_csum_slice(0, data_hdr, sizeof(data_hdr), 0);
    TEST_ASSERT_EQUAL_INT(hdr_expected, hdr_sum);

    /* expect an empty buffer simply to reflect the incoming checksum */
    pyld_sum = inet_csum_slice(hdr_expected, NULL, 0, sizeof(data_hdr));
    TEST_ASSERT_EQUAL_INT(hdr_expected, pyld_sum);
}

Test *tests_inet_csum_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_inet_csum__rfc_example),
        new_TestFixture(test_inet_csum__ipv6_pseudo_hdr),
        new_TestFixture(test_inet_csum__set_initial_sum),
        new_TestFixture(test_inet_csum__wraps_more_than_once),
        new_TestFixture(test_inet_csum__calculate_csum),
        new_TestFixture(test_inet_csum__odd_len),
        new_TestFixture(test_inet_csum__two_app_snips),
        new_TestFixture(test_inet_csum__empty_app_buffer),
    };

    EMB_UNIT_TESTCALLER(inet_csum_tests, NULL, NULL, fixtures);

    return (Test *)&inet_csum_tests;
}

void tests_inet_csum(void)
{
    TESTS_RUN(tests_inet_csum_tests());
}
/** @} */
