/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_lm75
 *
 * @{
 * @file
 * @brief       Test program for the driver for the LM75 temperature sensor.
 *
 * A test setup for the driver for the lm75 temperature sensor
 * including support for the lm75a and tmp1075 sensors as well.
 *
 * @author      Vitor Batista <vitor.batista@ml-pa.com>
 *
 * @}
 */

#include "board.h"
#include "lm75.h"
#include "lm75_regs.h"
#include "lm75_params.h"
#include "xtimer.h"
#include "log.h"
#include <stdio.h>
#include <stdint.h>

/* prints currently set OS and HYST temperatures */
int lm75_check_set_temperature_limits(lm75_t *dev) {

    int t_os, t_hyst;
    /* get already set OS and HYST values */
    lm75_get_os_temp(dev, &t_os);
    printf("Set OS temp is %d.%dºC\n",   t_os / dev->lm75_params.res->os_mult, \
                                         t_os % dev->lm75_params.res->os_mult);

    lm75_get_hyst_temp(dev, &t_hyst);
    printf("Set HYST temp is %d.%dºC\n", t_hyst / dev->lm75_params.res->os_mult, \
                                         t_hyst % dev->lm75_params.res->os_mult);
    return 0;
}

/* This function prints the current temperature with maximum precision */
int lm75_print_temperature_raw(lm75_t *dev) {

    int temp;
    if (lm75_get_temperature_raw(dev, &temp) != 0) {
        return -1;
    }

    printf("%d.%04dºC\n", temp / dev->lm75_params.res->temp_mult, \
                             temp % dev->lm75_params.res->temp_mult);

    return 0;
}

/* This function prints the current temperature with maximum precision */
int lm75_print_temperature(lm75_t *dev) {

    int temp;
    if (lm75_get_temperature(dev, &temp) != 0) {
        return -1;
    }
    printf("%d.%dºC\n", temp / 1000, temp % 1000);

    return 0;
}

/* This function is implemented to make an explicit call
 * for the comparison functionality of the sensor */
int lm75_check_alarm_state(lm75_t *dev) {

    int temp, t_os, t_hyst;

    /* read the current temperature on the device's register */
    lm75_get_temperature_raw(dev, &temp);
    lm75_get_os_temp(dev, &t_os);
    lm75_get_hyst_temp(dev, &t_hyst);

    /* this is required not to discard the leading zero and
    * output a false result in the case the temperature is x.0625 */

    if ((temp / dev->lm75_params.res->temp_mult > t_os / dev->lm75_params.res->os_mult) || \
       ((temp / dev->lm75_params.res->temp_mult == t_os / dev->lm75_params.res->os_mult) && \
       (temp % dev->lm75_params.res->temp_mult > t_os % dev->lm75_params.res->os_mult))) {
                LOG_INFO("OS Limit exceeded\n");
    }

    else if ((temp / dev->lm75_params.res->temp_mult < t_hyst / dev->lm75_params.res->os_mult) || \
            ((temp / dev->lm75_params.res->temp_mult == t_hyst / dev->lm75_params.res->os_mult) && \
            (temp % dev->lm75_params.res->temp_mult < t_hyst % dev->lm75_params.res->os_mult))) {
                LOG_INFO("HYST Limit exceeded\n");
    }

    return 0;
}

static void cb(void *arg);

int main(void) {

    lm75_t descriptor;
    lm75_t *dev = &descriptor;
    bool alert_state;
    puts("Sensor test...");
    /* LM75 Sensor initialization */
    puts("Initialization...");
    if (lm75_init(dev, lm75_params) != LM75_SUCCESS) {
        puts("Initialization  failed");
        return -1;
    }
    else {
        puts("Initialization succeeded");
    }

    /* Set the hysteresis and overtemperature shutdown */
    if (lm75_set_temp_limits(dev, 24500, 29000, cb, NULL) != LM75_SUCCESS) {
        puts("error setting Hyst and/or OS temps");
    }

    lm75_check_set_temperature_limits(dev);

    /* Check already set values */
    while (1) {

        /* lm75_print_temperature_raw(dev); prints raw temp */
        lm75_print_temperature(dev); /* prints temp in mºC */
        lm75_check_set_temperature_limits(dev);
        lm75_low_power_mode(dev, 3000); /* testing in low power mode */
        /* xtimer_msleep(1000); testing in continuous mode */
        /* lm75_check_alarm_state(dev); checking if temp is within limits implicitly */
        if (lm75_get_os_pin(dev, &alert_state) != 0) {
            LOG_ERROR("Error reading OS pin state\n");
        }
        else if (alert_state == 1) {
            puts("OS pin is active");
        }
    }

    return 0;
}

static void cb(void *arg) {
    (void)arg;
    puts("INTERRUPT");
}
