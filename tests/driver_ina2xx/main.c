/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the INA2XX sensor driver
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#ifndef TEST_INA2XX_I2C
#error "TEST_INA2XX_I2C not defined"
#endif
#ifndef TEST_INA2XX_ADDR
#error "TEST_INA2XX_ADDR not defined"
#endif

#include <stdio.h>

#include "xtimer.h"
#include "ina2xx.h"

/* Use the following configuration:
 *
 *  - Continuous measurements, both shunt and bus voltage
 *  - +/- 320 mV Vshunt range
 *  - 32 V maximum bus voltage
 *  - 12 bit ADC resolution, no hardware averaging
 */
#define CONFIG   (INA2XX_MODE_CONTINUOUS_SHUNT_BUS | INA2XX_RANGE_320MV | \
                  INA2XX_BRNG_32V_FSR | INA2XX_SADC_12BIT | INA2XX_BADC_12BIT)
#define CALIBRATION (4096)
#define SLEEP_USEC    (100 * 1000U)

int main(void)
{
    ina2xx_t dev;
    int16_t val;

    puts("INA2XX sensor driver test application\n");

    printf("Initializing INA2XX sensor at I2C_%i, address 0x%02x... ",
        TEST_INA2XX_I2C, TEST_INA2XX_ADDR);
    if (ina2xx_init(&dev, TEST_INA2XX_I2C, TEST_INA2XX_ADDR) == 0) {
        puts("[OK]\n");
    } else {
        puts("[Failed]");
        return 1;
    }
    puts("Set configuration register");
    if (ina2xx_set_config(&dev, CONFIG) == 0) {
        puts("[OK]\n");
    } else {
        puts("[Failed]");
        return 1;
    }

    puts("Set calibration register");
    if (ina2xx_set_calibration(&dev, CALIBRATION) == 0) {
        puts("[OK]\n");
    } else {
        puts("[Failed]");
        return 1;
    }

    while (1) {
        /* Read shunt resistor voltage, in millivolts */
        ina2xx_read_shunt(&dev, &val);
        printf("shunt: %6d", val);

        /* Read VBUS voltage, in millivolts */
        ina2xx_read_bus(&dev, &val);
        /* The bus voltage is found in the topmost 13 bits of the bus voltage
         * register */
        val = (val >> INA2XX_BUS_VOLTAGE_SHIFT);
        printf("\tbus: %6d", val);

        /* Read current register, the scale depends on the value of the
         * calibration register */
        ina2xx_read_current(&dev, &val);
        printf("\tcurrent: %6d", val);

        /* Read power register, the scale depends on the value of the
         * calibration register */
        ina2xx_read_power(&dev, &val);
        printf("\tpower: %6d\n", val);

        xtimer_usleep(SLEEP_USEC);
    }

    return 0;
}
