/*
 * Copyright (C) 2020 iosabi
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
 * @brief       Test application for the Si1133 UV, IR and visible light sensor.
 *
 * @author      iosabi <iosabi@protonmail.com>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "si1133.h"
#include "si1133_params.h"
#include "xtimer.h"
#include "board.h"

/* Helper macro to define _si1133_strerr */
#define CASE_SI1133_ERROR_STRING(X)                                            \
    case X:                                                                    \
        return #X;

static const char *_si1133_strerr(si1133_ret_code_t err)
{
    switch (err) {
        CASE_SI1133_ERROR_STRING(SI1133_OK);
        CASE_SI1133_ERROR_STRING(SI1133_ERR_PARAMS);
        CASE_SI1133_ERROR_STRING(SI1133_ERR_I2C);
        CASE_SI1133_ERROR_STRING(SI1133_ERR_LOGIC);
        CASE_SI1133_ERROR_STRING(SI1133_ERR_NODEV);
        CASE_SI1133_ERROR_STRING(SI1133_ERR_OVERFLOW);
    }
    return NULL;
}

#define EXPECT_RET_CODE(expected, actual)                                      \
    do {                                                                       \
        si1133_ret_code_t actual_value = (actual);                             \
        si1133_ret_code_t expected_value = (expected);                         \
        if (actual_value != expected_value) {                                  \
            printf(                                                            \
                "ERROR: " #actual " = %s\nExpected value " #expected " (%s)\n",\
                _si1133_strerr(actual_value), _si1133_strerr(expected_value)); \
            failures++;                                                        \
        }                                                                      \
    } while (0)

static si1133_t dev;

int main(void)
{
    uint32_t failures = 0;

    puts("Testing Si1133 in blocking mode:");
    static const si1133_params_t blocking_params = {
        .i2c_dev = SI1133_PARAM_I2C_DEV,
        .address = SI1133_PARAM_ADDR
    };
    EXPECT_RET_CODE(SI1133_OK, si1133_init(&dev, &blocking_params));

    static const si1133_sensor_t sensor_list[] = {
        SI1133_SENS_SMALL_IR,
        SI1133_SENS_MEDIUM_IR,
        SI1133_SENS_LARGE_IR,
        SI1133_SENS_WHITE,
        SI1133_SENS_LARGE_WHITE,
        SI1133_SENS_UV,
        SI1133_SENS_DEEP_UV,
    };
    /* Test reading a sample one by one. */
    for (uint32_t i = 0; i < ARRAY_SIZE(sensor_list); i++) {
        EXPECT_RET_CODE(SI1133_OK,
                        si1133_easy_configure(&dev, sensor_list[i], 0, 0));
        int32_t value;
        EXPECT_RET_CODE(SI1133_OK,
                        si1133_capture_sensors(&dev, &value, 1));
        if (value >= 0x7fffff) {
            printf("ERROR: Sensor sample overflow, got %" PRId32 "\n", value);
            failures++;
        }
        printf(" - sensor 0x%.2x: %" PRId32 "\n", (int)sensor_list[i], value);
    }

    /* Test increasing the sw_gain until we get an overflow. */
    for (uint32_t sw_gain = 0; sw_gain <= 7; sw_gain++) {
        uint8_t sensor_mask =
            SI1133_SENS_LARGE_IR |
            SI1133_SENS_LARGE_WHITE |
            SI1133_SENS_UV;
        EXPECT_RET_CODE(SI1133_OK,
                        si1133_easy_configure(&dev, sensor_mask, 1, sw_gain));
        int32_t values[3];
        si1133_ret_code_t ret =
            si1133_capture_sensors(&dev, values, ARRAY_SIZE(values));
        printf("INFO: sw_gain=%" PRIu32 " LARGE_IR=%10" PRId32
               " LARGE_WHITE=%10" PRId32 " UV=%10" PRId32 "\n",
               sw_gain, values[0], values[1], values[2]);
        if (ret == SI1133_OK) {
            continue;
        }
        /* If we didn't get an OK we should have an overflow condition. */
        EXPECT_RET_CODE(SI1133_ERR_OVERFLOW, ret);
        /* One of the values must be in overflow state. */
        bool overflowed = false;
        for (uint32_t i = 0; i < ARRAY_SIZE(values); i++) {
            overflowed = overflowed || values[i] == 0x7fffff;
        }
        if (!overflowed) {
            printf(
                "ERROR: Sensor overflow but no value in overflowed state.\n");
            for (uint32_t i = 0; i < ARRAY_SIZE(values); i++) {
                printf("  values[%" PRIu32 "] = %" PRId32 "\n", i, values[i]);
            }
            failures++;
        }
        else {
            printf("NOTE: Overflow test OK.\n");
        }
    }
    /* Reading any sensor after overflowing should not fail. */
    EXPECT_RET_CODE(SI1133_OK,
                    si1133_easy_configure(&dev, SI1133_SENS_SMALL_IR, 1, 0));
    int32_t value;
    EXPECT_RET_CODE(SI1133_OK,
                    si1133_capture_sensors(&dev, &value, 1));

    /* Test reading most sensors at once. The maximum is 6 sensors. */
    uint32_t all = 0;
    for (uint32_t i = 0; i < ARRAY_SIZE(sensor_list); i++) {
        all |= sensor_list[i];
    }
    EXPECT_RET_CODE(SI1133_ERR_PARAMS, si1133_easy_configure(&dev, all, 1, 0));

    /* All except one is lower than the limit of 6. */
    all &= ~SI1133_SENS_WHITE;
    EXPECT_RET_CODE(SI1133_OK, si1133_easy_configure(&dev, all, 1, 0));
    int32_t values[6];
    EXPECT_RET_CODE(SI1133_OK,
                    si1133_capture_sensors(&dev, values, ARRAY_SIZE(values)));

    if (failures != 0) {
        printf("Result: FAILED %" PRIu32 "\n", failures);
    }
    else {
        puts("Result: OK\n");
    }
    return 0;
}
