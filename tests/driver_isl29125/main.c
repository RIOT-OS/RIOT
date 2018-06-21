/*
 * Copyright 2015 Ludwig Knüpfer
 * Copyright 2017 HAW Hamburg
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
 * @brief       Test application for the ISL29125 RGB light sensor
 *
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * @author      Martin Heusmann <martin.heusmann@haw-hamburg.de>
 * @author      Cenk Gündoğan <mail-github@cgundogan.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "xtimer.h"
#include "isl29125.h"
#include "isl29125_params.h"

#define SLEEP       (250 * 1000U)

void cb(void *arg)
{
    (void) arg;
    printf("INT: external interrupt\n");
}

int main(void)
{
    isl29125_t dev;
    isl29125_rgb_t data;
    color_rgb_t data8bit;
    memset(&data, 0x00, sizeof(data));

    /* Parameters for testing, change if needed. */
    uint16_t lower_threshold = 0;
    uint16_t higher_threshold = 8000;

    puts("ISL29125 light sensor test application\n");
    printf("Initializing ISL29125 sensor at I2C_%i... ", isl29125_params[0].i2c);
    if (isl29125_init(&dev, &isl29125_params[0]) == 0) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return 1;
    }

    puts("Initializing interrupt");

    if (isl29125_init_int(&dev, ISL29125_INTERRUPT_STATUS_RED,
                          ISL29125_INTERRUPT_PERSIST_1, ISL29125_INTERRUPT_CONV_DIS,
                          lower_threshold, higher_threshold, cb, NULL) != 0) {
        puts("[Failed]");
        return 1;
    }

    /* try out some modes */
    static const isl29125_mode_t modes[] = {
        ISL29125_MODE_DOWN, ISL29125_MODE_STANDBY, ISL29125_MODE_RGB,
        ISL29125_MODE_R, ISL29125_MODE_G, ISL29125_MODE_B,
        ISL29125_MODE_RG, ISL29125_MODE_GB};
    static const char* mode_names[] = {
        "ISL29125_MODE_DOWN", "ISL29125_MODE_STANDBY", "ISL29125_MODE_RGB",
        "ISL29125_MODE_R", "ISL29125_MODE_G", "ISL29125_MODE_B",
        "ISL29125_MODE_RG", "ISL29125_MODE_GB"};

    for (size_t i = 0; i < sizeof(modes); i++) {
        printf("Setting mode %s\n", mode_names[i]);
        isl29125_set_mode(&dev, modes[i]);
        xtimer_usleep(SLEEP);
        isl29125_read_rgb_color(&dev, &data8bit);
        printf("RGB value: (%3i / %3i / %3i) 8 bit\n",
                data8bit.r, data8bit.g, data8bit.b);
    }

    puts("Resetting mode to RGB and reading continuously");
    isl29125_set_mode(&dev, ISL29125_MODE_RGB);
    xtimer_usleep(SLEEP);
    while (1) {
        isl29125_read_rgb_lux(&dev, &data);
        printf("RGB value: (%5i / %5i / %5i) lux\n",
                (int)data.red, (int)data.green, (int)data.blue);
        xtimer_usleep(SLEEP);
        printf("IRQ-Status: %i \n", isl29125_read_irq_status(&dev));
    }

    return 0;
}
