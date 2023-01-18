/*
 * Copyright (C) 2023 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for IEC 62056
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include "embUnit.h"
#include "iec62056/obis.h"
#include "iec62056/21.h"
#include "container.h"

typedef struct {
    const char *input;
    int expected_result;
    iec62056_obis_t obis;
} test_obis_input_t;

static const test_obis_input_t id_tests[] = {
    { "1-2:3.4.5.6", 0, { 1, 2, 3, 4, 5, 6 } },
    { "2:3.4.5.6", 0, { 255, 2, 3, 4, 5, 6 } },
    { "1-3.4.5.6", 0, { 1, 255, 3, 4, 5, 6 } },
    { "1-2:3.4.5", 0, { 1, 2, 3, 4, 5, 255 } },
    { "3.4", 0, { 255, 255, 3, 4, 255, 255 } },
    { "", -1, { 0 } },
    { "256-2:3.4.5.6", -1, { 0 } },
    { "1-256:3.4.5.6", -1, { 0 } },
    { "1-2:256.4.5.6", -1, { 0 } },
    { "1-2:3.256.5.6", -1, { 0 } },
    { "1-2:3.4.256.6", -1, { 0 } },
    { "1-2:3.4.5.256", -1, { 0 } },
};

static void test_iec62056_obis(void)
{
    for (size_t i = 0; i < ARRAY_SIZE(id_tests); i++) {
        iec62056_obis_t obis;
        memset(&obis, 0, sizeof(iec62056_obis_t));
        int res = iec62056_obis_from_string(&obis, id_tests[i].input, strlen(id_tests[i].input));
        TEST_ASSERT_EQUAL_INT(res, id_tests[i].expected_result);
        if (res == 0) {
            TEST_ASSERT_EQUAL_INT(0, memcmp(&id_tests[i].obis, &obis, sizeof(iec62056_obis_t)));
        }
    }
}

typedef struct {
    const char *input;
    int expected_result;
    int64_t mantissa;
    int64_t exponent;
    const char *unit;
} test_register_input_t;

static const test_register_input_t register_tests[] = {
    { "123.456*kWh", 0, 123456, -3, "kWh" },
    { "0011235", 0, 11235, 0, NULL },
    { "-11235.3248", 0, -112353248, -4, NULL },
    { "001*A", 0, 1, 0, "A" },
    { "01*A.123", -1, 0, 0, "" },
    { "AABBCC", -1, 0, 0, "" },
    { "12AB.3", -1, 0, 0, "" },
    { "12.3A", -1, 0, 0, "" }
};

static void test_iec62056_21_register(void)
{
    for (size_t i = 0; i < ARRAY_SIZE(register_tests); i++) {
        const test_register_input_t *test = &register_tests[i];
        char unit[20] = { 0 };
        int64_t mantissa, exponent;

        iec62056_21_dataset_t dataset = {
            .obis = { 0 },
            .value = register_tests[i].input,
            .value_length = strlen(register_tests[i].input),
        };

        int res = iec62056_21_dataset_parse_register_to_decfrac(&dataset, &mantissa, &exponent);
        iec62056_21_dataset_parse_register_unit(&dataset, unit, sizeof(unit));

        TEST_ASSERT_EQUAL_INT(test->expected_result, res);
        if (res == 0) {
            TEST_ASSERT_EQUAL_INT(test->mantissa, mantissa);
            TEST_ASSERT_EQUAL_INT(test->exponent, exponent);
            if (test->unit) {
                TEST_ASSERT_EQUAL_INT(0, strcmp(test->unit, unit));
            }
        }
    }
}

static const char input_telegram[] =
    "/Ene5\\XS210 ESMR 5.0\r\n"
    "\r\n"
    "1-3:0.2.8(50)\r\n"
    "0-0:1.0.0(230104215905W)\r\n"
    "0-0:96.1.1(4530303631303030303031313638373230)\r\n"
    "1-0:1.8.1(000233.793*kWh)\r\n"
    "1-0:1.8.2(000300.631*kWh)\r\n"
    "1-0:2.8.1(000000.043*kWh)\r\n"
    "1-0:2.8.2(000000.000*kWh)\r\n"
    "0-0:96.14.0(0002)\r\n"
    "1-0:1.7.0(00.416*kW)\r\n"
    "1-0:2.7.0(00.000*kW)\r\n"
    "0-0:96.7.21(00003)\r\n"
    "0-0:96.7.9(00001)\r\n"
    "1-0:99.97.0(0)(0-0:96.7.19)\r\n"
    "1-0:32.32.0(00001)\r\n"
    "1-0:32.36.0(00000)\r\n"
    "0-0:96.13.0()\r\n"
    "1-0:32.7.0(237.0*V)\r\n"
    "1-0:31.7.0(001*A)\r\n"
    "1-0:21.7.0(00.416*kW)\r\n"
    "1-0:22.7.0(00.000*kW)\r\n"
    "0-1:24.1.0(003)\r\n"
    "0-1:96.1.0(4730303538353330303437313231353230)\r\n"
    "0-1:24.2.1(230104215500W)(00079.660*m3)\r\n"
    "!3216\r\n";

static const char test_identifier[] = "Ene5\\XS210 ESMR 5.0";

typedef struct {
    iec62056_obis_t obis;
    const char **datasets;
} test_telegram_input_t;

static const test_telegram_input_t test_obis_output[] = {
    { .obis = { 1, 3, 0, 2, 8, 255 }, .datasets = (const char *[]){ "50", NULL } },
    { .obis = { 0, 0, 1, 0, 0, 255 }, .datasets = (const char *[]){ "230104215905W", NULL } },
    { .obis = { 0, 0, 96, 1, 1, 255 },
      .datasets = (const char *[]){ "4530303631303030303031313638373230", NULL } },
    { .obis = { 1, 0, 1, 8, 1, 255 }, .datasets = (const char *[]){ "000233.793*kWh", NULL } },
    { .obis = { 1, 0, 1, 8, 2, 255 }, .datasets = (const char *[]){ "000300.631*kWh", NULL } },
    { .obis = { 1, 0, 2, 8, 1, 255 }, .datasets = (const char *[]){ "000000.043*kWh", NULL } },
    { .obis = { 1, 0, 2, 8, 2, 255 }, .datasets = (const char *[]){ "000000.000*kWh", NULL } },
    { .obis = { 0, 0, 96, 14, 0, 255 }, .datasets = (const char *[]){ "0002", NULL } },
    { .obis = { 1, 0, 1, 7, 0, 255 }, .datasets = (const char *[]){ "00.416*kW", NULL } },
    { .obis = { 1, 0, 2, 7, 0, 255 }, .datasets = (const char *[]){ "00.000*kW", NULL } },
    { .obis = { 0, 0, 96, 7, 21, 255 }, .datasets = (const char *[]){ "00003", NULL } },
    { .obis = { 0, 0, 96, 7, 9, 255 }, .datasets = (const char *[]){ "00001", NULL } },
    { .obis = { 1, 0, 99, 97, 0, 255 }, .datasets = (const char *[]){ "0", "0-0:96.7.19", NULL } },
    { .obis = { 1, 0, 32, 32, 0, 255 }, .datasets = (const char *[]){ "00001", NULL } },
    { .obis = { 1, 0, 32, 36, 0, 255 }, .datasets = (const char *[]){ "00000", NULL } },
    { .obis = { 0, 0, 96, 13, 0, 255 }, .datasets = (const char *[]){ "", NULL } },
    { .obis = { 1, 0, 32, 7, 0, 255 }, .datasets = (const char *[]){ "237.0*V", NULL } },
    { .obis = { 1, 0, 31, 7, 0, 255 }, .datasets = (const char *[]){ "001*A", NULL } },
    { .obis = { 1, 0, 21, 7, 0, 255 }, .datasets = (const char *[]){ "00.416*kW", NULL } },
    { .obis = { 1, 0, 22, 7, 0, 255 }, .datasets = (const char *[]){ "00.000*kW", NULL } },
    { .obis = { 0, 1, 24, 1, 0, 255 }, .datasets = (const char *[]){ "003", NULL } },
    { .obis = { 0, 1, 96, 1, 0, 255 },
      .datasets = (const char *[]){ "4730303538353330303437313231353230", NULL } },
    { .obis = { 0, 1, 24, 2, 1, 255 },
      .datasets = (const char *[]){ "230104215500W", "00079.660*m3", NULL } },
};

static void _compare_obis(const iec62056_obis_t *a, const iec62056_obis_t *b)
{
    TEST_ASSERT_EQUAL_INT(0, memcmp(a, b, sizeof(iec62056_obis_t)));
}

static void _compare_dataset(const char *expected, const char *input, size_t len)
{
    TEST_ASSERT_NOT_NULL(expected);
    TEST_ASSERT_EQUAL_INT(strlen(expected), len);
    if (len) {
        TEST_ASSERT_EQUAL_INT(0, memcmp(expected, input, len));
    }
}

static int _test_telegram_callback(void *arg, const iec62056_21_dataset_t *dataset)
{
    size_t *index = arg;

    if (iec62056_21_dataset_without_value(dataset)) {
        *index += 1;
        _compare_obis(&test_obis_output[*index].obis, &dataset->obis);
    }
    else {
        const char *test_value = test_obis_output[*index].datasets[dataset->value_index];
        _compare_dataset(test_value, dataset->value, dataset->value_length);
    }
    return 0;
}

static void test_iec62056_21_telegram(void)
{
    iec62056_21_parser_t parser;
    size_t index = -1;

    iec62056_21_parser_init(&parser, _test_telegram_callback, &index);
    iec62056_21_parser_set_telegram(&parser, input_telegram, strlen(input_telegram));

    const char *identifier = NULL;
    ssize_t identifier_length = iec62056_21_get_header(&parser, &identifier);

    TEST_ASSERT_EQUAL_INT(19, identifier_length);
    TEST_ASSERT_EQUAL_INT(0, memcmp(identifier, test_identifier, identifier_length));

    iec62056_21_parse_telegram(&parser);
}

Test *tests_iec62056(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_iec62056_obis),
        new_TestFixture(test_iec62056_21_register),
        new_TestFixture(test_iec62056_21_telegram),
    };

    EMB_UNIT_TESTCALLER(iec62056_tests, NULL, NULL, fixtures);
    return (Test *)&iec62056_tests;
}


int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_iec62056());
    TESTS_END();

    return 0;
}
