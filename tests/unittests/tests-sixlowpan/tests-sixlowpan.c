/*
 * SPDX-FileCopyrightText: 2015 Oliver Hahm <oliver.hahm@inria.fr>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 */
#include <errno.h>
#include <string.h>

#include "thread.h"

#include "tests-sixlowpan.h"
#include "embUnit.h"

#include "unittests-constants.h"

#include "net/sixlowpan.h"

#define NALP_0  (0x00) /* 00 00 00 00 */
#define NALP_1  (0x01) /* 00 00 00 01 */
#define NALP_2  (0x3F) /* 00 11 11 11 */
#define NALP_3  (0x20) /* 00 10 00 00 */
#define NALP_4  (0x2A) /* 00 10 10 10 */
#define NALP_5  (0x1E) /* 00 01 11 10 */

#define NO_NALP_0   (0xFF)  /* 11 11 11 11 */
#define NO_NALP_1   (0xC0)  /* 11 00 00 00 */
#define NO_NALP_2   (0x40)  /* 01 00 00 00 */
#define NO_NALP_3   (0x80)  /* 10 00 00 00 */
#define NO_NALP_4   (0xAA)  /* 10 10 10 10 */
#define NO_NALP_5   (0xD4)  /* 11 01 01 00 */

#define IPv6_DISP       (0x41)  /* 01 00 00 01 */
#define LOWPAN_HC1_DISP (0x42)  /* 01 00 00 10 */
#define LOWPAN_BC0_DISP (0x50)  /* 01 01 00 00 */
#define ESC_DISP        (0x7F)  /* 01 11 11 11 */
#define MESH_DISP       (0xB3)  /* 10 11 00 11 */
#define FRAG1_DISP      (0xC5)  /* 11 00 01 01 */
#define FRAGN_DISP      (0xE5)  /* 11 10 01 01 */

/* Test with 6LoWPAN dispatch byte indicating a none-LoWPAN frame (NALP = Not a
 * LoWPAN frame)
 * see https://tools.ietf.org/html/rfc4944#section-5.1
 */
static void test_sixlowpan_nalp_is_no_6lowpan_frame_0(void)
{
    TEST_ASSERT(sixlowpan_nalp(NALP_0));
}

static void test_sixlowpan_nalp_is_no_6lowpan_frame_1(void)
{
    TEST_ASSERT(sixlowpan_nalp(NALP_1));
}

static void test_sixlowpan_nalp_is_no_6lowpan_frame_2(void)
{
    TEST_ASSERT(sixlowpan_nalp(NALP_2));
}

static void test_sixlowpan_nalp_is_no_6lowpan_frame_3(void)
{
    TEST_ASSERT(sixlowpan_nalp(NALP_3));
}

static void test_sixlowpan_nalp_is_no_6lowpan_frame_4(void)
{
    TEST_ASSERT(sixlowpan_nalp(NALP_4));
}

static void test_sixlowpan_nalp_is_no_6lowpan_frame_5(void)
{
    TEST_ASSERT(sixlowpan_nalp(NALP_5));
}

/* Test with 6LoWPAN dispatch byte indicating some none-NALP value */
static void test_sixlowpan_nalp_is_6lowpan_frame_0(void)
{
    TEST_ASSERT(!sixlowpan_nalp(NO_NALP_0));
}

static void test_sixlowpan_nalp_is_6lowpan_frame_1(void)
{
    TEST_ASSERT(!sixlowpan_nalp(NO_NALP_1));
}

static void test_sixlowpan_nalp_is_6lowpan_frame_2(void)
{
    TEST_ASSERT(!sixlowpan_nalp(NO_NALP_2));
}

static void test_sixlowpan_nalp_is_6lowpan_frame_3(void)
{
    TEST_ASSERT(!sixlowpan_nalp(NO_NALP_3));
}

static void test_sixlowpan_nalp_is_6lowpan_frame_4(void)
{
    TEST_ASSERT(!sixlowpan_nalp(NO_NALP_4));
}

static void test_sixlowpan_nalp_is_6lowpan_frame_5(void)
{
    TEST_ASSERT(!sixlowpan_nalp(NO_NALP_5));
}

/* Test with 6LoWPAN dispatch byte indicating a LoWPAN frame */
static void test_sixlowpan_nalp_is_6lowpan_frame_6(void)
{
    TEST_ASSERT(!sixlowpan_nalp(IPv6_DISP));
}

static void test_sixlowpan_nalp_is_6lowpan_frame_7(void)
{
    TEST_ASSERT(!sixlowpan_nalp(LOWPAN_HC1_DISP));
}

static void test_sixlowpan_nalp_is_6lowpan_frame_8(void)
{
    TEST_ASSERT(!sixlowpan_nalp(LOWPAN_BC0_DISP));
}

static void test_sixlowpan_nalp_is_6lowpan_frame_9(void)
{
    TEST_ASSERT(!sixlowpan_nalp(ESC_DISP));
}

static void test_sixlowpan_nalp_is_6lowpan_frame_10(void)
{
    TEST_ASSERT(!sixlowpan_nalp(MESH_DISP));
}

static void test_sixlowpan_nalp_is_6lowpan_frame_11(void)
{
    TEST_ASSERT(!sixlowpan_nalp(FRAG1_DISP));
}

static void test_sixlowpan_nalp_is_6lowpan_frame_12(void)
{
    TEST_ASSERT(!sixlowpan_nalp(FRAGN_DISP));
}

Test *test_sixlowpan_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_sixlowpan_nalp_is_no_6lowpan_frame_0),
        new_TestFixture(test_sixlowpan_nalp_is_no_6lowpan_frame_1),
        new_TestFixture(test_sixlowpan_nalp_is_no_6lowpan_frame_2),
        new_TestFixture(test_sixlowpan_nalp_is_no_6lowpan_frame_3),
        new_TestFixture(test_sixlowpan_nalp_is_no_6lowpan_frame_4),
        new_TestFixture(test_sixlowpan_nalp_is_no_6lowpan_frame_5),

        new_TestFixture(test_sixlowpan_nalp_is_6lowpan_frame_0),
        new_TestFixture(test_sixlowpan_nalp_is_6lowpan_frame_1),
        new_TestFixture(test_sixlowpan_nalp_is_6lowpan_frame_2),
        new_TestFixture(test_sixlowpan_nalp_is_6lowpan_frame_3),
        new_TestFixture(test_sixlowpan_nalp_is_6lowpan_frame_4),
        new_TestFixture(test_sixlowpan_nalp_is_6lowpan_frame_5),
        new_TestFixture(test_sixlowpan_nalp_is_6lowpan_frame_6),
        new_TestFixture(test_sixlowpan_nalp_is_6lowpan_frame_7),
        new_TestFixture(test_sixlowpan_nalp_is_6lowpan_frame_8),
        new_TestFixture(test_sixlowpan_nalp_is_6lowpan_frame_9),
        new_TestFixture(test_sixlowpan_nalp_is_6lowpan_frame_10),
        new_TestFixture(test_sixlowpan_nalp_is_6lowpan_frame_11),
        new_TestFixture(test_sixlowpan_nalp_is_6lowpan_frame_12),
    };

    EMB_UNIT_TESTCALLER(test_sixlowpan_tests_caller, NULL, NULL, fixtures);

    return (Test *)&test_sixlowpan_tests_caller;
}

void tests_sixlowpan(void)
{
    TESTS_RUN(test_sixlowpan_tests());
}
/** @} */
