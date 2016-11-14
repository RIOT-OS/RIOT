/*
 * Copyright 2015 Ludwig Knüpfer
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
 *
 * @}
 */

#ifndef TEST_ISL29125_I2C
#error "TEST_ISL29125_I2C not defined"
#endif

#ifndef TEST_ISL29125_IRQ_PIN
#error "ISL29125_IRQ_PIN not defined"
#endif

#include <stdio.h>
#include <string.h>

#include "xtimer.h"
#include "isl29125.h"
#include "periph/gpio.h"

#define SLEEP       (250 * 1000U)

int main(void)
{
    isl29125_t dev;
    isl29125_rgb_t data;
    color_rgb_t data8bit;
    memset(&data, 0x00, sizeof(data));

    //Parameters for testing, change if needed.
    uint16_t lower_threshold = 0;
    uint16_t higher_threshold = 8000;
    int po = 0;     //GPIO_PIN(0, 22)=PA22
    int pi = 22;
    gpio_mode_t mode = GPIO_IN;
    gpio_flank_t flank = GPIO_FALLING;

    init_ext_int(po, pi, mode, flank);

    puts("ISL29125 light sensor test application\n");
    printf("Initializing ISL29125 sensor at I2C_%i... ", TEST_ISL29125_I2C);
    if (isl29125_init(&dev, TEST_ISL29125_I2C, TEST_ISL29125_IRQ_PIN,
                ISL29125_MODE_R, ISL29125_RANGE_10K,
                ISL29125_RESOLUTION_16, ISL29125_INTERRUPT_STATUS_RED,
                ISL29125_INTERRUPT_PERSIST_1, ISL29125_INTERRUPT_CONV_DIS,
                lower_threshold, higher_threshold) == 0) {
        puts("[OK]\n");
    }
    else {
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

    for (int i = 0; i < sizeof(modes); i++) {
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

void cb(void *arg)
{
    printf("INT: external interrupt from pin %i\n", (int)arg);
}

int init_ext_int(int po, int pi, gpio_mode_t mode, gpio_flank_t flank)
{
    if (gpio_init_int(GPIO_PIN(po, pi), mode, flank, cb, (void *)pi) < 0) {
        printf("error: init_int of GPIO_PIN(%i, %i) failed\n", po, pi);
        return 1;
    }
    printf("GPIO_PIN(%i, %i) successfully initialized as ext int\n", po, pi);

    return 0;
}
