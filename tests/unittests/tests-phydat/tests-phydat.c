/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "embUnit.h"
#include "tests-phydat.h"

#include "phydat.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static void test_phydat_fit(void)
{
    /* verify that these big numbers are scaled to fit in phydat_t::val which is int16_t */
    long val0 =   100445;
    long val1 =  2000954;
    long val2 = 30000455;
    long val4 =  1234567;
    phydat_t dat;
    dat.scale = -6;
    dat.unit = UNIT_V;
    uint8_t res = phydat_fit(&dat, val0, 0, 0);
    /* Check that the result was rescaled to 10044e-5 */
    /* The scaled number is rounded toward zero */
    TEST_ASSERT_EQUAL_INT(1, res);
    TEST_ASSERT_EQUAL_INT(UNIT_V, dat.unit);
    TEST_ASSERT_EQUAL_INT(-5, dat.scale);
    TEST_ASSERT_EQUAL_INT( 10044, dat.val[0]);
    /* Fit the next value in the phydat vector */
    res = phydat_fit(&dat, val1, 1, res);
    TEST_ASSERT_EQUAL_INT(2, res);
    TEST_ASSERT_EQUAL_INT(UNIT_V, dat.unit);
    TEST_ASSERT_EQUAL_INT(-4, dat.scale);
    TEST_ASSERT_EQUAL_INT(  1004, dat.val[0]);
    TEST_ASSERT_EQUAL_INT( 20009, dat.val[1]);
    /* Fit the third value in the phydat vector */
    res = phydat_fit(&dat, val2, 2, res);
    TEST_ASSERT_EQUAL_INT(3, res);
    TEST_ASSERT_EQUAL_INT(UNIT_V, dat.unit);
    TEST_ASSERT_EQUAL_INT(-3, dat.scale);
    TEST_ASSERT_EQUAL_INT(   100, dat.val[0]);
    TEST_ASSERT_EQUAL_INT(  2000, dat.val[1]);
    TEST_ASSERT_EQUAL_INT( 30000, dat.val[2]);
    /* Overwrite the second value in the phydat vector */
    res = phydat_fit(&dat, val4, 1, res);
    TEST_ASSERT_EQUAL_INT(3, res);
    TEST_ASSERT_EQUAL_INT(UNIT_V, dat.unit);
    TEST_ASSERT_EQUAL_INT(-3, dat.scale);
    TEST_ASSERT_EQUAL_INT(   100, dat.val[0]);
    TEST_ASSERT_EQUAL_INT(  1234, dat.val[1]);
    TEST_ASSERT_EQUAL_INT( 30000, dat.val[2]);
}

Test *tests_phydat_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_phydat_fit),
    };

    EMB_UNIT_TESTCALLER(phydat_tests, NULL, NULL, fixtures);

    return (Test *)&phydat_tests;
}

void tests_phydat(void)
{
    TESTS_RUN(tests_phydat_tests());
}
