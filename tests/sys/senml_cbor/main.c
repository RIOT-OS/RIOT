/*
 * SPDX-FileCopyrightText: 2021 Silke Hofstra
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Short SenML CBOR test
 *
 * @author      Silke Hofstra <silke@slxh.eu>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "embUnit.h"
#include "senml/cbor.h"
#include "fmt.h"

#define BUF_SIZE (128)

static uint8_t cbor_buf[BUF_SIZE];
static char result[2 * BUF_SIZE];
static char expect[] = "89A5216943424F522074657374221A608404D001616D07187802F953B0A206"
                       "0102F953B0A2060202183DA2060302183DA2060402C48220190267A2060504"
                       "F5A20606036752494F54204F53A20607084400010203A201626B6705183D";

void test_senml_encode(void)
{
    nanocbor_encoder_t enc;

    /* Some common attributes to set on the first element */
    senml_attr_t attr = {
        .base_name = "CBOR test",
        .base_time = senml_duration_s(1619264720),
        .update_time = senml_duration_s(120),
        .unit = SENML_UNIT_METER,
    };

    /* A numeric (float) value */
    senml_value_t vf = { .attr = attr, .value = senml_float(61.5) };

    /* A numeric (double) value */
    senml_value_t vd = { .attr = { .time = senml_duration_s(1) },
                         .value = senml_double(61.5) };

    /* A numeric (int) value */
    senml_value_t vi = { .attr = { .time = senml_duration_s(2) },
                         .value = senml_int(61) };

    /* A numeric (uint) value */
    senml_value_t vu = { .attr = { .time = senml_duration_s(3) },
                         .value = senml_uint(61) };

    /* A numeric (decimal fraction) value */
    senml_value_t vdf = { .attr = { .time = senml_duration_s(4) },
                          .value = senml_decfrac(615, -1) };

    /* A boolean value */
    senml_bool_value_t vb = { .attr = { .time = senml_duration_s(5) },
                              .value = true };

    /* A string value */
    char string[] = "RIOT OS";
    senml_string_value_t vs = { .attr = { .time = senml_duration_s(6) },
                                .value = string, .len = sizeof string - 1 };

    /* A data value */
    uint8_t data[] = { 0x00, 0x01, 0x02, 0x03 };
    senml_data_value_t vdat = { .attr = { .time = senml_duration_s(7) },
                                .value = data, .len = sizeof data };

    /* A numeric (float) sum value */
    senml_attr_t sum = {
        .sum = senml_int(61),
        .unit = SENML_UNIT_KILOGRAM,
    };

    /* Initialize encoder, and start array */
    nanocbor_encoder_init(&enc, cbor_buf, sizeof cbor_buf);
    nanocbor_fmt_array(&enc, 9);

    /* Encode the values */
    senml_encode_value_cbor(&enc, &vf);
    senml_encode_value_cbor(&enc, &vd);
    senml_encode_value_cbor(&enc, &vi);
    senml_encode_value_cbor(&enc, &vu);
    senml_encode_value_cbor(&enc, &vdf);
    senml_encode_bool_cbor(&enc, &vb);
    senml_encode_string_cbor(&enc, &vs);
    senml_encode_data_cbor(&enc, &vdat);
    senml_encode_sum_cbor(&enc, &sum);

    size_t len = nanocbor_encoded_len(&enc);

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
