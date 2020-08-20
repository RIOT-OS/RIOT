/*
 * Copyright (C) 2018 Koen Zandberg
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
 * @brief       Short NanoCBOR library test
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <nanocbor/nanocbor.h>
#include <string.h>
#include "embUnit.h"

static uint8_t buf[64];

static uint8_t expected[] = {
  0x9f, 0xf5, 0xf4, 0x1a, 0xff, 0xff, 0xff, 0xff, 0x3a, 0x7f, 0xff, 0xff,
  0xff, 0xa4, 0x08, 0x18, 0x1e, 0x38, 0x1d, 0x19, 0x01, 0xf4, 0x39, 0x01,
  0xf3, 0x75, 0x74, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x61, 0x20,
  0x6c, 0x6f, 0x6e, 0x67, 0x20, 0x73, 0x74, 0x72, 0x69, 0x6e, 0x67, 0xfa,
  0x3e, 0xae, 0x14, 0x7b, 0x46, 0x62, 0x79, 0x74, 0x65, 0x7a, 0x00, 0xf6,
  0xff
};

void test_nanocbor_encode(void)
{
    nanocbor_encoder_t enc;
    nanocbor_encoder_init(&enc, buf, sizeof(buf));

    nanocbor_fmt_array_indefinite(&enc);
    nanocbor_fmt_bool(&enc, true);
    nanocbor_fmt_bool(&enc, false);
    nanocbor_fmt_uint(&enc, UINT32_MAX);
    nanocbor_fmt_int(&enc, INT32_MIN);
    nanocbor_fmt_map(&enc, 4);
    nanocbor_fmt_uint(&enc, 8);
    nanocbor_fmt_int(&enc, 30);
    nanocbor_fmt_int(&enc, -30);
    nanocbor_fmt_int(&enc, 500);
    nanocbor_fmt_int(&enc, -500);
    nanocbor_put_tstr(&enc, "this is a long string");
    nanocbor_fmt_float(&enc, 0.34);
    nanocbor_put_bstr(&enc, (uint8_t*)"bytez", sizeof("bytez"));
    nanocbor_fmt_null(&enc);
    nanocbor_fmt_end_indefinite(&enc);

    size_t required = nanocbor_encoded_len(&enc);

    TEST_ASSERT_EQUAL_INT(required, sizeof(expected));
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf, expected, sizeof(expected)));
}

enum {
    CFG_FOO,
    CFG_BAR,
    CFG_RANGE,
};

typedef struct {
    uint32_t foo;
    uint32_t bar;
    uint32_t min;
    uint32_t mean;
    uint32_t max;
} test_cfg_t;

static size_t _serialize_config(uint8_t *buffer, size_t len, const test_cfg_t *cfg)
{
    nanocbor_encoder_t enc;
    nanocbor_encoder_init(&enc, buffer, len);

    nanocbor_fmt_map_indefinite(&enc);

    /* write key - value pair*/
    nanocbor_fmt_uint(&enc, CFG_FOO);
    nanocbor_fmt_uint(&enc, cfg->foo);

    /* write key - value pair*/
    nanocbor_fmt_uint(&enc, CFG_BAR);
    nanocbor_fmt_uint(&enc, cfg->bar);

    /* write key - value pair*/
    nanocbor_fmt_uint(&enc, CFG_RANGE);
    nanocbor_fmt_array(&enc, 3);
    nanocbor_fmt_uint(&enc, cfg->min);
    nanocbor_fmt_uint(&enc, cfg->mean);
    nanocbor_fmt_uint(&enc, cfg->max);

    nanocbor_fmt_end_indefinite(&enc);

    return nanocbor_encoded_len(&enc);
}

static bool _decode_period(nanocbor_value_t *it, test_cfg_t *cfg)
{
    nanocbor_value_t array;

    if (nanocbor_enter_array(it, &array) <= 0) {
        return false;
    }

    if (nanocbor_get_uint32(&array, &cfg->min) <= 0) {
        return false;
    }

    if (nanocbor_get_uint32(&array, &cfg->mean) <= 0) {
        return false;
    }

    if (nanocbor_get_uint32(&array, &cfg->max) <= 0) {
        return false;
    }

    nanocbor_leave_container(it, &array);

    return true;
}

static bool _cfg_cb(nanocbor_value_t *map, uint32_t key, void *dst)
{
    test_cfg_t *cfg = dst;

    switch (key) {
    case CFG_FOO:
        return nanocbor_get_uint32(map, &cfg->foo) > 0;
    case CFG_BAR:
        return nanocbor_get_uint32(map, &cfg->bar) > 0;
    case CFG_RANGE:
        return _decode_period(map, cfg);
    default:
        nanocbor_skip(map);
    }

    return true;
}

static bool _parse_config(const uint8_t *buffer, size_t len, test_cfg_t *cfg)
{
    nanocbor_value_t msg, map;
    nanocbor_decoder_init(&msg, buffer, len);

    if (nanocbor_enter_map(&msg, &map) < 0) {
        return false;
    }

    while (!nanocbor_at_end(&map)) {
        uint32_t key;
        if (nanocbor_get_uint32(&map, &key) < 0) {
            return false;
        }

        if (!_cfg_cb(&map, key, cfg)) {
            return false;
        }
    }

    return true;
}

void test_nanocbor_encode_decode_cfg(void)
{
    uint8_t buffer[64];

    test_cfg_t cfg = {
        .foo  = 23,
        .bar  = 42,
        .min  = 5,
        .mean = 30,
        .max  = 60,
    };

    size_t len = _serialize_config(buffer, sizeof(buffer), &cfg);
    TEST_ASSERT(len < sizeof(buffer));

    test_cfg_t new_cfg;
    TEST_ASSERT(_parse_config(buffer, len, &new_cfg));

    TEST_ASSERT_EQUAL_INT(cfg.foo,  new_cfg.foo);
    TEST_ASSERT_EQUAL_INT(cfg.bar,  new_cfg.bar);
    TEST_ASSERT_EQUAL_INT(cfg.min,  new_cfg.min);
    TEST_ASSERT_EQUAL_INT(cfg.mean, new_cfg.mean);
    TEST_ASSERT_EQUAL_INT(cfg.max,  new_cfg.max);
}

Test *tests_nanocbor(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_nanocbor_encode),
        new_TestFixture(test_nanocbor_encode_decode_cfg),
    };
    EMB_UNIT_TESTCALLER(nanocbor_tests, NULL, NULL, fixtures);
    return (Test *)&nanocbor_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_nanocbor());
    TESTS_END();
    return 0;
}
