/*
 * SPDX-FileCopyrightText: 2018 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "embUnit.h"
#include "tests-analog_util.h"

#include "analog_util.h"

#define ENABLE_DEBUG 0
#include "debug.h"

typedef struct {
    int32_t expected;
    int sample;
    int32_t min;
    int32_t max;
    adc_res_t res;
} test_values_t;

/* Arbitrarily chosen test vectors */
/* TODO: Choose test vectors in a more qualified manner to catch any edge cases */
static test_values_t test_data[] = {
    {     0L,     0,          0L,      10000L, ADC_RES_16BIT},
    {  1000L,     0,       1000L,          0L, ADC_RES_16BIT},
    { 65535L, 65535,          0L,      65536L, ADC_RES_16BIT},
    { 32768L,   128,          0L,      65536L,  ADC_RES_8BIT},
    {  8192L,   128,          0L,      65536L, ADC_RES_10BIT},
    {   256L,     1,          0L,      65536L,  ADC_RES_8BIT},
    { 65280L,   255,          0L,      65536L,  ADC_RES_8BIT},
    {  1039L,    10,       1000L,       2000L,  ADC_RES_8BIT},
    { 17324L,  3000,      10000L,      20000L, ADC_RES_12BIT},
    { 11831L,  3000,      10000L,      20000L, ADC_RES_14BIT},
    {  2301L,  3000,         13L,      50000L, ADC_RES_16BIT},
    {  -134L, 56789,      -1000L,          0L, ADC_RES_16BIT},
    { 16062L, 45671,      30000L,      10000L, ADC_RES_16BIT},
    { -2535L, 30000,     -30000L,      30000L, ADC_RES_16BIT},
    {     0L, 65535,      65535L,          0L, ADC_RES_16BIT},
    { 65534L,     1,      65535L,          0L, ADC_RES_16BIT},
    {  3972L,  9876,      10000L,          0L, ADC_RES_14BIT},
};

#define TEST_DATA_NUMOF ARRAY_SIZE(test_data)

static void test_adc_util_map(void)
{
    for (unsigned int k = 0; k < TEST_DATA_NUMOF; ++k) {
        test_values_t *testp = &test_data[k];
        int32_t res = adc_util_map(testp->sample, testp->res, testp->min, testp->max);
        TEST_ASSERT_EQUAL_INT(testp->expected, res);
    }
}

Test *tests_adc_util_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_adc_util_map),
    };

    EMB_UNIT_TESTCALLER(adc_util_tests, NULL, NULL, fixtures);

    return (Test *)&adc_util_tests;
}

void tests_analog_util(void)
{
    TESTS_RUN(tests_adc_util_tests());
}
