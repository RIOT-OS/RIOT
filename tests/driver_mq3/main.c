/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the MQ-3 alcohol sensor driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "xtimer.h"
#include "mq3.h"
#include "periph_conf.h"
#include "periph/adc.h"

#ifndef MQ3_ADC_LINE
#error "MQ3_ADC_LINE is not specified"
#endif

int main(void)
{
    mq3_t dev;
    int res;

    puts("MQ-3 alcohol sensor test application\n");
    printf("Initializing MQ-3 sensor at ADC_LINE(%i)...   ", (int)MQ3_ADC_LINE);
    res = mq3_init(&dev, MQ3_ADC_LINE);
    if (res == 0) {
        puts("[ok]\n");
    }
    else {
        puts("[failed]");
        return 1;
    }

    puts("Will now read and print the sensor twice a second:");
    while (1) {
        int raw, ppm, alc_a, alc_b;
        raw = mq3_read_raw(&dev);
        ppm = mq3_read(&dev);
        alc_a = ppm / 1000;
        alc_b = ppm - (alc_a * 1000);

        printf("RAW: %4i, per mille: %1i.%03i\n", raw, alc_a, alc_b);

        xtimer_usleep(500 * 1000);
    }
    return 0;
}
