/*
 * SPDX-FileCopyrightText: 2018 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <stdint.h>
#include "bitarithm.h"
#include "xfa.h"

#include "embUnit.h"

#include "tests-core.h"
#include "tests-core-xfa.h"

XFA_INIT(xfatest_t, xfatest);
XFA_INIT_CONST(xfatest_t, xfatest_const);
XFA_USE(xfatest_t, xfatest_use);
XFA_USE_CONST(xfatest_t, xfatest_use_const);

/* Verifying that cross file array linking is correct by iterating over an external array */
static void test_xfa_data(void)
{
    unsigned n = XFA_LEN(xfatest_t, xfatest);
    TEST_ASSERT_EQUAL_INT(2, n);
    unsigned found = 0;
    for (unsigned k = 0; k < n; ++k) {
        /* we do not want to enforce the order of the data elements */
        switch (xfatest[k].val) {
            case 12345:
                /* tests-core-xfa-data1.c */
                TEST_ASSERT_EQUAL_STRING("xfatest1", xfatest[k].text);
                TEST_ASSERT(!(found & BIT0));
                found |= BIT0;
                break;
            case 0xbeef:
                /* tests-core-xfa-data2.c */
                TEST_ASSERT_EQUAL_STRING("another test string", xfatest[k].text);
                TEST_ASSERT(!(found & BIT1));
                found |= BIT1;
                break;
            default:
                break;
        }
    }
    TEST_ASSERT_EQUAL_INT((1U << n) - 1, found);
}

static void test_xfa_const_data(void)
{
    unsigned n = XFA_LEN(xfatest_t, xfatest_const);
    TEST_ASSERT_EQUAL_INT(2, n);
    unsigned found = 0;
    for (unsigned k = 0; k < n; ++k) {
        /* we do not want to enforce the order of the data elements */
        switch (xfatest_const[k].val) {
            case 0xcafe:
                /* tests-core-xfa-data1.c */
                TEST_ASSERT_EQUAL_STRING("xfatest_const1", xfatest_const[k].text);
                ++found;
                break;
            case 32444:
                /* tests-core-xfa-data2.c */
                TEST_ASSERT_EQUAL_STRING("const string xfa 2", xfatest_const[k].text);
                ++found;
                break;
            default:
                break;
        }
    }
    TEST_ASSERT_EQUAL_INT(n, found);
}

static void test_xfa_use_data(void)
{
    unsigned n = XFA_LEN(xfatest_t, xfatest_use);
    TEST_ASSERT_EQUAL_INT(3, n);
    unsigned found = 0;
    for (unsigned k = 0; k < n; ++k) {
        /* we do not want to enforce the order of the data elements */
        switch (xfatest_use[k].val) {
            case 3333:
                /* tests-core-xfa-data1.c */
                TEST_ASSERT_EQUAL_STRING("xfatest_use1", xfatest_use[k].text);
                ++found;
                break;
            case 555:
                /* tests-core-xfa-data1.c */
                TEST_ASSERT_EQUAL_STRING("xfatest use again", xfatest_use[k].text);
                ++found;
                break;
            case 11111:
                /* tests-core-xfa-data2.c */
                TEST_ASSERT_EQUAL_STRING("xfatest_use2", xfatest_use[k].text);
                ++found;
                break;
            default:
                break;
        }
    }
    TEST_ASSERT_EQUAL_INT(n, found);
}

static void test_xfa_use_const_data(void)
{
    unsigned n = XFA_LEN(xfatest_t, xfatest_use_const);
    TEST_ASSERT_EQUAL_INT(2, n);
    unsigned found = 0;
    for (unsigned k = 0; k < n; ++k) {
        /* we do not want to enforce the order of the data elements */
        switch (xfatest_use_const[k].val) {
            case 4444:
                /* tests-core-xfa-data1.c */
                TEST_ASSERT_EQUAL_STRING("xfatest_use_const1", xfatest_use_const[k].text);
                ++found;
                break;
            case 22222:
                /* tests-core-xfa-data2.c */
                TEST_ASSERT_EQUAL_STRING("xfatest_use_const2", xfatest_use_const[k].text);
                ++found;
                break;
            default:
                break;
        }
    }
    TEST_ASSERT_EQUAL_INT(n, found);
}

Test *tests_core_xfa_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_xfa_data),
        new_TestFixture(test_xfa_const_data),
        new_TestFixture(test_xfa_use_data),
        new_TestFixture(test_xfa_use_const_data),
    };

    EMB_UNIT_TESTCALLER(core_xfa_tests, NULL, NULL,
                        fixtures);

    return (Test *)&core_xfa_tests;
}
