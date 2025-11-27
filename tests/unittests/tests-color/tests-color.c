/*
 * SPDX-FileCopyrightText: 2016 Cenk Gündoğan <mail@cgundogan.de>
 * SPDX-License-Identifier: LGPL-2.1-only
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

#include "container.h"

#include "tests-color.h"

#define HSV_EPSILON     (1E-2f)

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

static void test_rgb2hsv(void)
{
    struct { color_hsv_t hsv; color_rgb_t rgb; } h_r[] = {
        { { 0, 0, 0 }, { 0, 0, 0 } },           /*Black*/
        { { 0, 0, 100 }, { 255, 255, 255 } },   /*White*/
        { { 0, 100, 100 }, { 255, 0, 0 } },     /*Red*/
        { { 120, 100, 100 }, { 0, 255, 0 } },   /*Lime*/
        { { 240, 100, 100 }, { 0, 0, 255 } },   /*Blue*/
        { { 60, 100, 100 }, { 255, 255, 0 } },  /*Yellow*/
        { { 180, 100, 100 }, { 0, 255, 255 } }, /*Cyan*/
        { { 300, 100, 100 }, { 255, 0, 255 } }, /*Magenta*/
        { { 0, 0, 75 }, { 191, 191, 191 } },    /*Silver*/
        { { 0, 0, 50 }, { 128, 128, 128 } },    /*Gray*/
        { { 0, 100, 50 }, { 128, 0, 0 } },      /*Maroon*/
        { { 60, 100, 50 }, { 128, 128, 0 } },   /*Olive*/
        { { 120, 100, 50 }, { 0, 128, 0 } },    /*Green*/
        { { 300, 100, 50 }, { 128, 0, 128 } },  /*Purple*/
        { { 180, 100, 50 }, { 0, 128, 128 } },  /*Teal*/
        { { 240, 100, 50 }, { 0, 0, 128 } }     /*Navy*/
    };
    unsigned len = ARRAY_SIZE(h_r);

    for (unsigned i = 0; i < len; i++) {
        color_hsv_t hsv_o = { .h = h_r[i].hsv.h,
                              .s = h_r[i].hsv.s / 100,
                              .v = h_r[i].hsv.v / 100 };
        color_rgb_t rgb_o = { .r = h_r[i].rgb.r,
                              .g = h_r[i].rgb.g,
                              .b = h_r[i].rgb.b };

        color_hsv_t hsv;
        color_rgb2hsv(&rgb_o, &hsv);

        /* XXX floats should never be compared for equality, so we check if we
         * are within HSV_EPSILON of tolerance */
        TEST_ASSERT(-HSV_EPSILON <= hsv.s - hsv_o.s);
        TEST_ASSERT( HSV_EPSILON >= hsv.s - hsv_o.s);
        TEST_ASSERT(-HSV_EPSILON <= hsv.v - hsv_o.v);
        TEST_ASSERT( HSV_EPSILON >= hsv.v - hsv_o.v);
        /* Hue for grey is undefined so we don't check it */
        if (hsv.s >= 0.0001f) {
            TEST_ASSERT(-HSV_EPSILON <= hsv.h - hsv_o.h);
            TEST_ASSERT( HSV_EPSILON >= hsv.h - hsv_o.h);
        }
    }
}

static void test_hsv2rgb(void)
{
    struct { color_hsv_t hsv; color_rgb_t rgb; } h_r[] = {
        { { 0, 0, 0 }, { 0, 0, 0 } },           /*Black*/
        { { 0, 0, 100 }, { 255, 255, 255 } },   /*White*/
        { { 0, 100, 100 }, { 255, 0, 0 } },     /*Red*/
        { { 120, 100, 100 }, { 0, 255, 0 } },   /*Lime*/
        { { 240, 100, 100 }, { 0, 0, 255 } },   /*Blue*/
        { { 60, 100, 100 }, { 255, 255, 0 } },  /*Yellow*/
        { { 180, 100, 100 }, { 0, 255, 255 } }, /*Cyan*/
        { { 300, 100, 100 }, { 255, 0, 255 } }, /*Magenta*/
        { { 0, 0, 75 }, { 191, 191, 191 } },    /*Silver*/
        { { 0, 0, 50 }, { 128, 128, 128 } },    /*Gray*/
        { { 0, 100, 50 }, { 128, 0, 0 } },      /*Maroon*/
        { { 60, 100, 50 }, { 128, 128, 0 } },   /*Olive*/
        { { 120, 100, 50 }, { 0, 128, 0 } },    /*Green*/
        { { 300, 100, 50 }, { 128, 0, 128 } },  /*Purple*/
        { { 180, 100, 50 }, { 0, 128, 128 } },  /*Teal*/
        { { 240, 100, 50 }, { 0, 0, 128 } }     /*Navy*/
    };
    unsigned len = ARRAY_SIZE(h_r);

    for (unsigned i = 0; i < len; i++) {
        color_hsv_t hsv_o = { .h = h_r[i].hsv.h,
                              .s = h_r[i].hsv.s / 100,
                              .v = h_r[i].hsv.v / 100 };
        color_rgb_t rgb_o = { .r = h_r[i].rgb.r,
                              .g = h_r[i].rgb.g,
                              .b = h_r[i].rgb.b };

        color_rgb_t rgb;
        color_hsv2rgb(&hsv_o, &rgb);

        TEST_ASSERT(rgb.r <= rgb_o.r + 1 && rgb.r >= rgb_o.r - 1);
        TEST_ASSERT(rgb.g <= rgb_o.g + 1 && rgb.g >= rgb_o.g - 1);
        TEST_ASSERT(rgb.b <= rgb_o.b + 1 && rgb.b >= rgb_o.b - 1);

    }
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
        new_TestFixture(test_rgb2hsv),
        new_TestFixture(test_hsv2rgb),
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
