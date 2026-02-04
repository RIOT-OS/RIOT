/*
 * SPDX-FileCopyrightText: 2017 RWTH Aachen, Josua Arndt, Steffen Robertz
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 * @file
 * @brief       test application for the LC709203F Battery fuel gauge by on semiconductors
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @}
 */

#include <stdio.h>
#include "board.h"
#include "periph_conf.h"
#include "lc709203f.h"
#include "xtimer.h"
#include "lc709203f_params.h"

static void _gauge_cb(void *arg)
{
    (void)arg;
    printf("\n ALARM: Low RSOC \n");
}

int main(void)
{
    puts("LC709203F Fuel Gauge test application");
    lc709203f_t dev;
    dev.cb = _gauge_cb;
    dev.arg = NULL;
    if (lc709203f_init(&dev, params_default) == LC709203F_OK) {
        puts("The application will trigger an low_rsoc interrupt in 2s");
        xtimer_sleep(2);
        lc709203f_set_alarm_low_rsoc(&dev, 0);
        lc709203f_set_alarm_low_rsoc(&dev, 100);
        puts("Read low rsoc threshold level. Should be 100");
        printf("Alarm Low RSOC level: %u \n", lc709203f_get_alarm_low_rsoc(&dev));
        puts("Voltage, RSOC, ITE, ID and Cell temperature will be printed every 5s");
        while (1) {
            xtimer_sleep(5);
            printf("Voltage in mV: %u \n", lc709203f_get_voltage(&dev));
            printf("RSOC: %u \n", lc709203f_get_rsoc(&dev));
            printf("Ite in 0.1: %u \n", lc709203f_get_ite(&dev));
            printf("ID: %u \n", lc709203f_get_id(&dev));
            printf("Cell Temp in 0.1C: %u \n", lc709203f_get_cell_temp(&dev));
        }
    }
    else {
        printf("Could not connect to fuel gauge\n");
    }
    return 0;
}
