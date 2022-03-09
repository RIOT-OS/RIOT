/*
 * Copyright (C) 2016 Cenk Gündoğan <mail@cgundogan.de>
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
#include <stdint.h>

#include "embUnit/embUnit.h"

#include "color.h"

#include "tests-color.h"

#define HSV_EPSILON     (1E-10f)

static void test_str2rgb_upper_case__success(void)
{
    const char *color_str = "F09A1D";
    color_rgb_t rgb;

    color_str2rgb(color_str, &rgb);
    TEST_ASSERT_EQUAL_INT(0xF0, rgb.r);
    TEST_ASSERT_EQUAL_INT(0x9A, rgb.g);
    TEST_ASSERT_EQUAL_INT(0x1D, rgb.b);
}

static void test_str2rgb_lower_case__success(void)
{
    const char *color_str = "f09a1d";
    color_rgb_t rgb;

    color_str2rgb(color_str, &rgb);
    TEST_ASSERT_EQUAL_INT(0xF0, rgb.r);
    TEST_ASSERT_EQUAL_INT(0x9A, rgb.g);
    TEST_ASSERT_EQUAL_INT(0x1D, rgb.b);
}

static void test_rgb2str__success(void)
{
    char color_str[7] = { 0 };
    const color_rgb_t rgb = { .r = 0x0A, .g = 0xB1, .b = 0x3C };

    color_rgb2str(&rgb, color_str);

    TEST_ASSERT_EQUAL_STRING("0AB13C", (char *) color_str);
}

static void test_hex2rgb__success(void)
{
    const uint32_t hex = 0x8Fa1b9;
    color_rgb_t rgb;

    color_hex2rgb(hex, &rgb);
    TEST_ASSERT_EQUAL_INT(0x8F, rgb.r);
    TEST_ASSERT_EQUAL_INT(0xA1, rgb.g);
    TEST_ASSERT_EQUAL_INT(0xB9, rgb.b);
}

static void test_rgb2hex__success(void)
{
    uint32_t hex = 0x0;
    const color_rgb_t rgb = { .r = 0x0A, .g = 0xB1, .b = 0x3C };

    color_rgb2hex(&rgb, &hex);

    TEST_ASSERT_EQUAL_INT(0x000AB13C, hex);
}

static void test_rgb2hsv__black(void)
{
    color_hsv_t hsv;
    color_rgb_t rgb = { .r = 0x00, .g = 0x00, .b = 0x00 };

    color_rgb2hsv(&rgb, &hsv);

    /* XXX floats should never be compared for equality, so we check if we
     * are within HSV_EPSILON of tolerance */
    TEST_ASSERT(-HSV_EPSILON <= hsv.s);
    TEST_ASSERT(-HSV_EPSILON <= hsv.v);
    TEST_ASSERT( HSV_EPSILON >= hsv.s);
    TEST_ASSERT( HSV_EPSILON >= hsv.v);
    /* Hue for black is undefined so we don't check it */
}

static void test_rgb_invert__success(void)
{
    const color_rgb_t col = {.r = 100, .g = 128, .b =   0};
    const color_rgb_t res = {.r = 155, .g = 127, .b = 255};
    color_rgb_t tmp;

    color_rgb_invert(&col, &tmp);

    TEST_ASSERT_EQUAL_INT(res.r, tmp.r);
    TEST_ASSERT_EQUAL_INT(res.g, tmp.g);
    TEST_ASSERT_EQUAL_INT(res.b, tmp.b);
}

static void test_rgb_complementary__success(void)
{
    /* See example: https://helpx.adobe.com/illustrator/using/adjusting-colors.html */
    const color_rgb_t col = {.r = 102, .g = 153, .b =  51};
    const color_rgb_t res = {.r = 102, .g =  51, .b = 153};
    color_rgb_t tmp;

    color_rgb_complementary(&col, &tmp);

    TEST_ASSERT_EQUAL_INT(res.r, tmp.r);
    TEST_ASSERT_EQUAL_INT(res.g, tmp.g);
    TEST_ASSERT_EQUAL_INT(res.b, tmp.b);
}

Test *tests_color_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_str2rgb_upper_case__success),
        new_TestFixture(test_str2rgb_lower_case__success),
        new_TestFixture(test_hex2rgb__success),
        new_TestFixture(test_rgb2hex__success),
        new_TestFixture(test_rgb2str__success),
        new_TestFixture(test_rgb2hsv__black),
        new_TestFixture(test_rgb_invert__success),
        new_TestFixture(test_rgb_complementary__success),
    };

    EMB_UNIT_TESTCALLER(color_tests, NULL, NULL, fixtures);

    return (Test *)&color_tests;
}

void tests_color(void)
{
    TESTS_RUN(tests_color_tests());
}
/** @} */
