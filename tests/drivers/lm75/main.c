/*
 * SPDX-FileCopyrightText: 2021 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_lm75
 *
 * @{
 * @file
 * @brief       Test program for the driver for the LM75 temperature sensor.
 *
 * A test setup for the driver for the LM75 temperature sensor
 * including support for the LM75A and TMP1075 sensors as well.
 *
 * @author      Vitor Batista <vitor.batista@ml-pa.com>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>

#include "fmt.h"
#include "lm75.h"
#include "lm75_params.h"
#include "macros/math.h"
#include "tiny_strerror.h"

/* prints currently set OS and HYST temperatures */
static int lm75_check_set_temperature_limits(lm75_t *dev)
{
    int32_t t_raw;
    int32_t t_milli_c;
    char out[16];

    /* get already set OS and HYST values */
    lm75_get_os_temp(dev, &t_raw);
    t_milli_c = DIV_ROUND((1000 * t_raw), dev->lm75_params.res->os_mult);
    out[fmt_s32_dfp(out, t_milli_c, -3)] = '\0';
    printf("Set OS temp is %s ºC\n", out);

    lm75_get_hyst_temp(dev, &t_raw);
    t_milli_c = DIV_ROUND((1000 * t_raw), dev->lm75_params.res->os_mult);
    out[fmt_s32_dfp(out, t_milli_c, -3)] = '\0';
    printf("Set HYST temp is %s ºC\n", out);
    return 0;
}

/* This function prints the current temperature with maximum precision */
static int lm75_print_temperature(lm75_t *dev)
{
    int32_t temp;
    if (lm75_get_temperature(dev, &temp) != 0) {
        return -1;
    }
    char out[16];
    out[fmt_s32_dfp(out, temp, -3)] = '\0';
    printf("%s ºC\n", out);

    return 0;
}

static void cb(void *arg);

int main(void)
{
    lm75_t descriptor;
    lm75_t *dev = &descriptor;
    bool alert_state;
    puts("LM75 / TMP1075 sensor test...");
    /* LM75 Sensor initialization */
    puts("Initialization...");
    int err = lm75_init(dev, lm75_params);
    if (err) {
        printf("Initialization failed: %s\n", tiny_strerror(err));
        return -1;
    }

    puts("Initialization succeeded");

    /* Set the hysteresis and overtemperature shutdown */
    err = lm75_set_temp_limits(dev, 24500, 29000, cb, NULL);
    if (err) {
        printf("error setting Hyst and/or OS temps: %s\n", tiny_strerror(err));
    }

    lm75_check_set_temperature_limits(dev);

    /* Check already set values */
    while (1) {
        lm75_print_temperature(dev); /* prints temp in mºC */
        lm75_check_set_temperature_limits(dev);
        lm75_low_power_mode(dev, 3000); /* testing in low power mode */

        err = lm75_get_os_pin(dev, &alert_state);
        if (err) {
            printf("Error reading OS pin state: %s\n", tiny_strerror(err));
        }
        else if (alert_state == 1) {
            puts("OS pin is active");
        }
    }

    return 0;
}

static void cb(void *arg)
{
    (void)arg;
    puts("INTERRUPT");
}
