/*
 * SPDX-FileCopyrightText: 2021 Silke Hofstra
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Short SenML SAUL test
 *
 * @author      Silke Hofstra <silke@slxh.eu>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "embUnit.h"
#include "senml/saul.h"
#include "fmt.h"

#define BUF_SIZE (128)

static uint8_t cbor_buf[BUF_SIZE];
static char result[2 * BUF_SIZE];
static char expect[] = "9FFF";

void test_senml_encode(void)
{
    size_t len = senml_saul_encode_cbor(cbor_buf, sizeof cbor_buf, saul_reg);

    /* Encode as hex */
    fmt_bytes_hex(result, cbor_buf, len);

    /* Compare hex result */
    TEST_ASSERT_EQUAL_INT(2 * len, sizeof(expect) - 1);
    TEST_ASSERT_EQUAL_INT(0, strncmp(expect, result, len));
}

Test *tests_senml(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_senml_encode),
    };
    EMB_UNIT_TESTCALLER(senml_tests, NULL, NULL, fixtures);
    return (Test *)&senml_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_senml());
    TESTS_END();
    return 0;
}
