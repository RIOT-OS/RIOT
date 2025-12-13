/*
 * SPDX-FileCopyrightText: 2018 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <sht1x.h>
#include <stdlib.h>

#include "buildinfo/cpu.h"
#include "container.h"
#include "embUnit/embUnit.h"
#include "tests-sht1x.h"

/** @brief Maximum difference from correct temperature value [in e-02 °C] */
static const int16_t max_diff_temp = 1;
/** @brief Maximum difference from correct humidity value [in e-02 %] */
static const int16_t max_diff_hum = 10;

/*
 * Configure tested values steps
 *
 * Verify less values on boards as without hwfloat it takes minutes to test.
 * Checking that the trend is valid is enough to find int overflow
 * or similar errors.
 *
 * Keep the full range test on native as it is "instant"
 */

#ifdef CPU_NATIVE
#define TEMPERATURE_TEST_STEPS      1
/* Use 0.13°c steps. */
#define HUMIDITY_TEST_TEMP_STEPS    13
#else /* CPU_NATIVE */
#define TEMPERATURE_TEST_STEPS      100
#define HUMIDITY_TEST_TEMP_STEPS    1300
#endif /* CPU_NATIVE */

static int16_t expected_temp(const sht1x_dev_t *dev, uint16_t _raw)
{
    static const double d1_table[] = { -40.1, -39.8, -39.7, -39.6, -39.4 };
    double d1 = d1_table[dev->vdd];
    double d2 = (dev->conf & SHT1X_CONF_LOW_RESOLUTION) ? 0.04 : 0.01;
    double raw = (double)_raw;
    double temp = d1 + d2 * raw;

    return (int16_t)(temp * 100.0);
}

static int16_t expected_hum(const sht1x_dev_t *dev, uint16_t _raw, int16_t _temp)
{
    static const double c1 = -2.0468;
    static const double t1 = 0.01;
    double temp = ((double)_temp) / 100.0;
    double raw = (double)_raw;
    double c2, c3, t2, hum_linear, hum_real;

    if (dev->conf & SHT1X_CONF_LOW_RESOLUTION) {
        c2 = 0.5872;
        c3 = -4.0845e-04;
        t2 = 0.00128;
    }
    else {
        c2 = 0.0367;
        c3 = -1.5955e-06;
        t2 = 0.00008;
    }

    hum_linear = c1 + c2 * raw + c3 * (raw * raw);
    hum_real = (temp - 25.0) * (t1 + t2 * raw) + hum_linear;
    return (int16_t)(hum_real * 100.0);
}

static void test_sht1x_conversion(void)
{
    const uint8_t vdds[] = {
        SHT1X_VDD_5_0V, SHT1X_VDD_4_0V, SHT1X_VDD_3_5V, SHT1X_VDD_3_0V,
        SHT1X_VDD_2_5V,
    };
    const uint8_t confs[] = { SHT1X_CONF_LOW_RESOLUTION, 0 };
    const uint16_t max_raw_temps[] = { 0xfff, 0x3fff };
    const uint16_t max_raw_hums[] = { 0xff, 0xfff };
    sht1x_dev_t dev = { .conf = 0 };

    for (size_t i_res = 0; i_res < ARRAY_SIZE(confs); i_res++) {
        dev.conf = confs[i_res];
        uint16_t max_raw_temp = max_raw_temps[i_res];
        uint16_t max_raw_hum = max_raw_hums[i_res];
        for (size_t i_vdd = 0; i_vdd < ARRAY_SIZE(vdds); i_vdd++) {
            dev.vdd = vdds[i_vdd];
            for (uint16_t raw_temp = 0; raw_temp <= max_raw_temp;
                    raw_temp += TEMPERATURE_TEST_STEPS) {
                int16_t got_temp = sht1x_temperature(&dev, raw_temp);
                int16_t exp_temp = expected_temp(&dev, raw_temp);

                TEST_ASSERT(((got_temp - max_diff_temp) <= exp_temp) &&
                            ((got_temp + max_diff_temp) >= exp_temp));
            }
        }

        /* Testing for temperatures between -10.00°C and 65.00°C */
        for (int16_t temp = -1000; temp < 6500;
                temp += HUMIDITY_TEST_TEMP_STEPS) {
            for (uint16_t raw_hum = 0; raw_hum <= max_raw_hum; raw_hum++) {
                int16_t exp_hum = expected_hum(&dev, raw_hum, temp);
                if ((exp_hum < 0) || (exp_hum > 10000)) {
                    /* Result out of range, ignore it */
                    continue;
                }
                int16_t got_hum = sht1x_humidity(&dev, raw_hum, temp);
                TEST_ASSERT(((got_hum - max_diff_hum) <= exp_hum) &&
                            ((got_hum + max_diff_hum) >= exp_hum));
            }
        }
    }
}

Test *tests_sht1x_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_sht1x_conversion),
    };

    EMB_UNIT_TESTCALLER(sht1x_tests, NULL, NULL, fixtures);

    return (Test *)&sht1x_tests;
}

void tests_sht1x(void)
{
    TESTS_RUN(tests_sht1x_tests());
}
