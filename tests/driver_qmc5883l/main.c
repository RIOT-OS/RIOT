/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{

 * @file
 * @brief       Test application for the QMC5883L magnetic sensor

 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "xtimer.h"
#include "thread.h"
#include "thread_flags.h"

#include "qmc5883l.h"
#include "qmc5883l_params.h"

#define PWR_OFF_DELAY       (1u)
#define FLAG_DRDY           (0x0400)

/* allocate the device descriptor */
static qmc5883l_t _dev;

#ifdef MODULE_QMC5883L_INT
static thread_t *_tmain;

static void _on_drdy(void *arg)
{
    (void)arg;
    thread_flags_set(_tmain, FLAG_DRDY);
}
#endif

static void _read_and_dump(void)
{
    int16_t data[3];
    int res = qmc5883l_read(&_dev, data);
    if ((res == QMC5883L_OK) || (res == QMC5883L_OVERFLOW)) {
        printf("Reading - X:%6i Y:%6i Z:%6i [mGauss]",
               (int)data[0], (int)data[1], (int)data[2]);
        if (res == QMC5883L_OVERFLOW) {
            printf(" - OVERFLOWED");
        }
        puts("");
    }
    else if (res == QMC5883L_OVERFLOW) {
        puts("Reading - overflow");
    }
    else if (res == QMC5883L_NODATA) {
        puts("Reading - no new data available");
    }
}

int main(void)
{
    uint32_t delay = US_PER_MS;

    puts("QMC5883L test application");
    puts("Please refer to the README.md for more information\n");

    /* initialize the sensor with default configuration parameters */
    if (qmc5883l_init(&_dev, &qmc5883l_params[0]) != QMC5883L_OK) {
        puts("Error: unable to initialize device");
        return 1;
    }

    printf("QMC5883L successfully initialized.\nData rate:        ");
    switch (qmc5883l_params[0].odr) {
        case QMC5883L_ODR_10HZ:     puts("10Hz");   delay *= 100;   break;
        case QMC5883L_ODR_50HZ:     puts("50Hz");   delay *= 20;    break;
        case QMC5883L_ODR_100HZ:    puts("100Hz");  delay *= 10;    break;
        case QMC5883L_ODR_200HZ:    puts("200Hz");  delay *= 5;     break;
    }
    printf("Data range:       ");
    switch (qmc5883l_params[0].rng) {
        case QMC5883L_RNG_2G:       puts("2G");     break;
        case QMC5883L_RNG_8G:       puts("8G");     break;
    }
    printf("Over sample rate: ");
    switch (qmc5883l_params[0].osr) {
        case QMC5883L_OSR_512:      puts("512");    break;
        case QMC5883L_OSR_256:      puts("256");    break;
        case QMC5883L_OSR_128:      puts("128");    break;
        case QMC5883L_OSR_64:       puts("64");     break;
    }
#ifdef MODULE_QMC5883L_INT
    printf("Mode:             ");
    if (gpio_is_valid(qmc5883l_params[0].pin_drdy)) {
        puts("interrupt driven");
    }
    else {
        puts("polling");
    }
#endif
    puts("");

    /* test the driver's power cycling */
    puts("Power cycle test: powering device off now");
    if (qmc5883l_poweroff(&_dev) != QMC5883L_OK) {
        puts("Error: unable to power off device");
        return 1;
    }
    xtimer_sleep(PWR_OFF_DELAY);
    if (qmc5883l_poweron(&_dev) != QMC5883L_OK) {
        puts("Error: unable to power on the device again");
        return 1;
    }
    puts("Power cycle test: device is powered back on now");

#ifdef MODULE_QMC5883L_INT
    /* safe a reference to the main thread TCB so we can wait for flags */
    if (gpio_is_valid(qmc5883l_params[0].pin_drdy)) {
        _tmain = thread_get_active();

        if (qmc5883l_init_int(&_dev, _on_drdy, NULL) != QMC5883L_OK) {
            puts("Error: unable to configure interrupt callback");
            return 1;
        }

        while (1) {
            thread_flags_wait_any(FLAG_DRDY);
            _read_and_dump();
        }
    }
#endif

    while (1) {
        int ready;
        do {
            xtimer_usleep(delay);
            ready = qmc5883l_data_ready(&_dev);
        } while (ready != QMC5883L_OK);
        _read_and_dump();
    }

    /* should never be reached */
    return 0;
}
