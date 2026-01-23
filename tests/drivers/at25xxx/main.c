/*
 * SPDX-FileCopyrightText: 2019 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the AT25XXX EEPROM driver
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "embUnit.h"

#include "at25xxx.h"
#include "at25xxx_params.h"

static at25xxx_t dev;

static void test_normal_write(void)
{
    const char data_in_a[] = "Hello EEPROM!";
    const char data_in_b[] = "This is a test.";
    char data_out[32];

    TEST_ASSERT_EQUAL_INT(0, at25xxx_write(&dev, 0, data_in_a, sizeof(data_in_a)));
    TEST_ASSERT_EQUAL_INT(0, at25xxx_read(&dev, 0, data_out, sizeof(data_out)));
    TEST_ASSERT_EQUAL_STRING(data_in_a, data_out);

    TEST_ASSERT_EQUAL_INT(0, at25xxx_write(&dev, 0, data_in_b, sizeof(data_in_b)));
    TEST_ASSERT_EQUAL_INT(0, at25xxx_read(&dev, 0, data_out, sizeof(data_out)));
    TEST_ASSERT_EQUAL_STRING(data_in_b, data_out);
}

static void test_page_write(void)
{
    const char data_in_a[] = "Hello EEPROM!";
    const char data_in_b[] = "This is a test.";
    char data_out[32];

    TEST_ASSERT_EQUAL_INT(0, at25xxx_write(&dev, AT25XXX_PARAM_PAGE_SIZE - 5, data_in_a, sizeof(data_in_a)));
    TEST_ASSERT_EQUAL_INT(0, at25xxx_read(&dev, AT25XXX_PARAM_PAGE_SIZE - 5, data_out, sizeof(data_out)));
    TEST_ASSERT_EQUAL_STRING(data_in_a, data_out);

    TEST_ASSERT_EQUAL_INT(0, at25xxx_write(&dev, AT25XXX_PARAM_PAGE_SIZE - 5, data_in_b, sizeof(data_in_b)));
    TEST_ASSERT_EQUAL_INT(0, at25xxx_read(&dev, AT25XXX_PARAM_PAGE_SIZE - 5, data_out, sizeof(data_out)));
    TEST_ASSERT_EQUAL_STRING(data_in_b, data_out);
}

static void test_page_clear(void)
{
    const char data_in_a[] = "Hello EEPROM!";
    char data_out[32];
    char data_clr[32];

    memset(data_clr, 0, sizeof(data_clr));

    TEST_ASSERT_EQUAL_INT(0, at25xxx_write(&dev, AT25XXX_PARAM_PAGE_SIZE - 5, data_in_a, sizeof(data_in_a)));
    TEST_ASSERT_EQUAL_INT(0, at25xxx_read(&dev, AT25XXX_PARAM_PAGE_SIZE - 5, data_out, sizeof(data_out)));
    TEST_ASSERT_EQUAL_STRING(data_in_a, data_out);

    TEST_ASSERT_EQUAL_INT(0, at25xxx_clear(&dev, AT25XXX_PARAM_PAGE_SIZE - 5, sizeof(data_out)));
    TEST_ASSERT_EQUAL_INT(0, at25xxx_read(&dev, AT25XXX_PARAM_PAGE_SIZE - 5, data_out, sizeof(data_out)));
    TEST_ASSERT_EQUAL_INT(0, memcmp(data_out, data_clr, sizeof(data_clr)));
}

static Test *tests_EEPROM_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_normal_write),
        new_TestFixture(test_page_write),
        new_TestFixture(test_page_clear),
    };

    EMB_UNIT_TESTCALLER(EEPROM_tests, NULL, NULL, fixtures);

    return (Test *)&EEPROM_tests;
}

int main(void)
{
    puts("AT25XXX EEPROM driver test application\n");

    at25xxx_init(&dev, &at25xxx_params[0]);

    printf("EEPROM size: %u kiB\n", (unsigned) (dev.params.size / 1024));
    printf("EEPROM page size: %d bytes\n", AT25XXX_PARAM_PAGE_SIZE);
    printf("EEPROM address length: %d bits\n\n", AT25XXX_PARAM_ADDR_LEN);

    TESTS_START();
    TESTS_RUN(tests_EEPROM_tests());
    TESTS_END();

    return 0;
}
