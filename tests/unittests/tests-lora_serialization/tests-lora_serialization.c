/*
 * Copyright (C) 2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     unittests
 * @{
 *
 * @file
 * @brief       lora-serialization tests
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 * @author      Jose Ignacio Alamos <jose.alamos@haw-hamburg.de>
 *
 * @}
 */

#include <string.h>
#include "embUnit.h"
#include "tests-lora_serialization.h"
#include "lora_serialization.h"

#define LORA_SERIALIZATION_FAKE_CURSOR              (20U)
#define LORA_SERIALIZATION_UNIX_TIME_SIMPLE         (1)
#define LORA_SERIALIZATION_UNIX_TIME                (1467632413)
#define LORA_SERIALIZATION_LATITUDE                 (-33.905052)
#define LORA_SERIALIZATION_LONGITUDE                (151.26641)
#define LORA_SERIALIZATION_UINT8                    (10)
#define LORA_SERIALIZATION_UINT16                   (23453)
#define LORA_SERIALIZATION_TEMPERATURE              (80.12)
#define LORA_SERIALIZATION_NEGATIVE_TEMPERATURE     (-123.45)
#define LORA_SERIALIZATION_HUMIDITY                 (99.99)

#define LORA_SERIALIZATION_EXPECTED_UNIX_TIME_SIMPLE        { 1, 0, 0, 0 }
#define LORA_SERIALIZATION_EXPECTED_UNIX_TIME               { 0x1d, 0x4b, 0x7a, 0x57 }
#define LORA_SERIALIZATION_EXPECTED_COORDS                  { 0x64, 0xa6, 0xfa, 0xfd, \
                                                              0x6a, 0x24, 0x04, 0x09 }
#define LORA_SERIALIZATION_EXPECTED_UINT8                   { 0x0A }
#define LORA_SERIALIZATION_EXPECTED_UINT16                  { 0x9d, 0x5b }
#define LORA_SERIALIZATION_EXPECTED_TEMPERATURE             { 0x1f, 0x4c }
#define LORA_SERIALIZATION_EXPECTED_NEGATIVE_TEMPERATURE    { 0xcf, 0xc7 }
#define LORA_SERIALIZATION_EXPECTED_HUMIDITY                { 0x0f, 0x27 }

#define LORA_SERIALIZATION_EXPECTED_TEST {                \
        0x1d, 0x4b, 0x7a, 0x57,                           \
        0x64, 0xa6, 0xfa, 0xfd, 0x6a, 0x24, 0x04, 0x09,   \
        0x0A,                                             \
        0x9d, 0x5b,                                       \
        0x1f, 0x4c,                                       \
        0x0f, 0x27                                        \
};

static lora_serialization_t lora_serialization;

static void setUp(void)
{
    /* Initialize */
}

static void tearDown(void)
{
    /* Finalize */
}

static void test_lora_serialization_01(void)
{
    lora_serialization.cursor = LORA_SERIALIZATION_FAKE_CURSOR;
    lora_serialization_reset(&lora_serialization);

    TEST_ASSERT_EQUAL_INT(0, lora_serialization.cursor);
}

static void test_lora_serialization_02(void)
{
    /* should transform a simple unixtime to a byte array */
    lora_serialization_reset(&lora_serialization);

    lora_serialization_write_unix_time(&lora_serialization,
                                       LORA_SERIALIZATION_UNIX_TIME_SIMPLE);
    uint8_t expected[] = LORA_SERIALIZATION_EXPECTED_UNIX_TIME_SIMPLE;

    TEST_ASSERT_EQUAL_INT(0, memcmp(lora_serialization.buffer,
                                    expected, sizeof(expected)));
}

static void test_lora_serialization_03(void)
{
    /* should transform a given unixtime to a byte array */
    lora_serialization_reset(&lora_serialization);

    uint32_t now = LORA_SERIALIZATION_UNIX_TIME;
    uint8_t expected[] = LORA_SERIALIZATION_EXPECTED_UNIX_TIME;

    lora_serialization_write_unix_time(&lora_serialization, now);

    TEST_ASSERT_EQUAL_INT(0, memcmp(lora_serialization.buffer,
                                    expected, sizeof(expected)));
}

static void test_lora_serialization_04(void)
{
    /* should transform a coordinate to a byte array */
    lora_serialization_reset(&lora_serialization);
    uint8_t expected[] = LORA_SERIALIZATION_EXPECTED_COORDS;

    lora_serialization_write_coordinates(&lora_serialization,
                                         LORA_SERIALIZATION_LATITUDE,
                                         LORA_SERIALIZATION_LONGITUDE);

    TEST_ASSERT_EQUAL_INT(0, memcmp(lora_serialization.buffer,
                                    expected, sizeof(expected)));
}

static void test_lora_serialization_05(void)
{
    /* should transform an unsigned 8bit int to a byte array */
    lora_serialization_reset(&lora_serialization);
    uint8_t expected[] = LORA_SERIALIZATION_EXPECTED_UINT8;

    lora_serialization_write_uint8(&lora_serialization,
                                   LORA_SERIALIZATION_UINT8);

    TEST_ASSERT_EQUAL_INT(0, memcmp(lora_serialization.buffer,
                                    expected, sizeof(expected)));
}

static void test_lora_serialization_06(void)
{
    /* should transform an unsigned 16bit int to a byte array */
    lora_serialization_reset(&lora_serialization);
    uint8_t expected[] = LORA_SERIALIZATION_EXPECTED_UINT16;

    lora_serialization_write_uint16(&lora_serialization,
                                    LORA_SERIALIZATION_UINT16);

    TEST_ASSERT_EQUAL_INT(0, memcmp(lora_serialization.buffer,
                                    expected, sizeof(expected)));
}

static void test_lora_serialization_07(void)
{
    /* should transform a temperature to a byte array */
    lora_serialization_reset(&lora_serialization);
    uint8_t expected[] = LORA_SERIALIZATION_EXPECTED_TEMPERATURE;

    lora_serialization_write_temperature(&lora_serialization,
                                         LORA_SERIALIZATION_TEMPERATURE);

    TEST_ASSERT_EQUAL_INT(0, memcmp(lora_serialization.buffer,
                                    expected, sizeof(expected)));
}

static void test_lora_serialization_08(void)
{
    /* should transform a negative temperature to a byte array */
    lora_serialization_reset(&lora_serialization);
    uint8_t expected[] = LORA_SERIALIZATION_EXPECTED_NEGATIVE_TEMPERATURE;

    lora_serialization_write_temperature(&lora_serialization,
                                         LORA_SERIALIZATION_NEGATIVE_TEMPERATURE);

    TEST_ASSERT_EQUAL_INT(0, memcmp(lora_serialization.buffer,
                                    expected, sizeof(expected)));
}

static void test_lora_serialization_09(void)
{
    /* should transform a humidity to a byte array */
    lora_serialization_reset(&lora_serialization);
    uint8_t expected[] = LORA_SERIALIZATION_EXPECTED_HUMIDITY;

    lora_serialization_write_humidity(&lora_serialization,
                                      LORA_SERIALIZATION_HUMIDITY);

    TEST_ASSERT_EQUAL_INT(0, memcmp(lora_serialization.buffer,
                                    expected, sizeof(expected)));
}

static void test_lora_serialization_10(void)
{
    /* write multiple fields to one byte array */
    lora_serialization_reset(&lora_serialization);

    uint8_t expected[] = LORA_SERIALIZATION_EXPECTED_TEST;

    lora_serialization_write_unix_time(&lora_serialization,
                                       LORA_SERIALIZATION_UNIX_TIME);
    lora_serialization_write_coordinates(&lora_serialization,
                                         LORA_SERIALIZATION_LATITUDE,
                                         LORA_SERIALIZATION_LONGITUDE);
    lora_serialization_write_uint8(&lora_serialization,
                                   LORA_SERIALIZATION_UINT8);
    lora_serialization_write_uint16(&lora_serialization,
                                    LORA_SERIALIZATION_UINT16);
    lora_serialization_write_temperature(&lora_serialization,
                                         LORA_SERIALIZATION_TEMPERATURE);
    lora_serialization_write_humidity(&lora_serialization,
                                      LORA_SERIALIZATION_HUMIDITY);

    TEST_ASSERT_EQUAL_INT(0, memcmp(lora_serialization.buffer,
                                    expected, sizeof(expected)));
    TEST_ASSERT_EQUAL_INT(lora_serialization.cursor, sizeof(expected));
}


static void test_lora_serialization_11(void)
{
    /* fail to write more than LORA_SERIALIZATION_MAX_BUFFER_SIZE bytes */
    lora_serialization_reset(&lora_serialization);
    uint8_t iterations = LORA_SERIALIZATION_MAX_BUFFER_SIZE /
                         LORA_SERIALIZATION_UINT8_SIZE;

    for (uint8_t i = 0; i < iterations; i++) {
        lora_serialization_write_uint8(&lora_serialization,
                                       LORA_SERIALIZATION_UINT8);
    }

    TEST_ASSERT_EQUAL_INT(
            lora_serialization_write_uint8(
                &lora_serialization, LORA_SERIALIZATION_UINT8),
            -ENOBUFS);
}

Test *tests_lora_serialization_all(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_lora_serialization_01),
        new_TestFixture(test_lora_serialization_02),
        new_TestFixture(test_lora_serialization_03),
        new_TestFixture(test_lora_serialization_04),
        new_TestFixture(test_lora_serialization_05),
        new_TestFixture(test_lora_serialization_06),
        new_TestFixture(test_lora_serialization_07),
        new_TestFixture(test_lora_serialization_08),
        new_TestFixture(test_lora_serialization_09),
        new_TestFixture(test_lora_serialization_10),
        new_TestFixture(test_lora_serialization_11),
    };

    EMB_UNIT_TESTCALLER(lora_serialization_tests, setUp, tearDown, fixtures);
    return (Test *)&lora_serialization_tests;
}

void tests_lora_serialization(void)
{
    TESTS_RUN(tests_lora_serialization_all());
}
