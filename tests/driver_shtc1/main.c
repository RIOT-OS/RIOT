/*
 * Copyright (C) 2017 RWTH Aachen, Steffen Robertz, Josua Arndt
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
 * @brief       Test application for the SHTC1 temperature and humidity sensor
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 *
 * @}
 */
#include <stdio.h>
#include "shtc1.h"
#include "shtc1_params.h"
#include "xtimer.h"

int main(void)
{
    shtc1_t shtc;

    if (shtc1_init(&shtc, &shtc1_params[0]) != SHTC1_OK) {
        printf("can't initialize the sensor");
        return -1;
    }
    while (1) {
        if (shtc1_measure(&shtc) == SHTC1_OK) {
            /*print temp value*/
            printf("Temperature: %.2f C \n Humidity: %.2f%%\n", shtc.values.temp, \
                   shtc.values.rel_humidity);
            return 0;
        }
        xtimer_sleep(2);
    }
    return 0;
}
