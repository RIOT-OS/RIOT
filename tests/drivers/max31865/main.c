/*
 * SPDX-FileCopyrightText: 2025 David Picard
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the MAX31865 RTD-to-Digital converter driver
 *
 * @author      David Picard
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "max31865.h"
#include "max31865_lut.h"           /* include BEFORE max31865_params.h */
#include "max31865_params.h"
#include "ztimer.h"
#include "log.h"

#define MEASUREMENT_SLEEP_MS        (1000)
#define MAX_LOOPS_MEAS              (3)

static void print_fault(max31865_fault_t value);

int main(void)
{
    max31865_t dev;
    int32_t temperature_cdegc;
    phydat_t phyTemp;
    max31865_fault_t fault_code;

    puts("MAX31865 RTD-to-Digital converter test application\n");

    /* initialize first configured sensor */
    printf("%s() >> setting config byte: 0x%02X\n", __FUNCTION__, max31865_params[0].cfg_byte);
    printf("Initializing MAX31865 converter...\t");
    if (max31865_init(&dev, &max31865_params[0]) == 0) {
        puts("[OK]\n");
    }
    else {
        puts("[Failed]");
        return 1;
    }
    uint8_t cfgreg = 0;
    max31865_clear_fault(&dev, &cfgreg);
    LOG_DEBUG("%s() >> Config register readback = 0x%02X\n", __FUNCTION__, cfgreg);

    /* periodically convert temperature values */
    while (1) {
        ztimer_sleep(ZTIMER_MSEC, MEASUREMENT_SLEEP_MS);

        if (max31865_read(&dev, &temperature_cdegc) == 0) {
            phyTemp.val[0] = temperature_cdegc;     /* unit: 0.01°C */
            phyTemp.scale = -2;             /* 1 cdegC = 1e-02 degC */
            phyTemp.unit = UNIT_TEMP_C;     /* set the unit */
            phydat_dump(&phyTemp, 1);       /* print the value in a pretty format */
        }
        else if (max31865_detect_fault(&dev, &fault_code) == 0) {
            print_fault(fault_code);
            max31865_clear_fault(&dev, NULL);
        }
        else {
            LOG_ERROR("%s() >> fault detection failed\n", __FUNCTION__);
        }
    }
    return 0;
}

static void print_fault(max31865_fault_t value)
{
    switch (value) {
    case MAX31865_FAULT_NO_FAULT:
        break;
    case MAX31865_FAULT_CIRCUIT:
        puts("Fault: Short or open circuit");
        break;
    case MAX31865_FAULT_VOLTAGE:
        puts("Fault: Under or over voltage");
        break;
    case MAX31865_FAULT_RTD_HIGH:
        puts("Fault: Temperature too high");
        break;
    case MAX31865_FAULT_RTD_LOW:
        puts("Fault: Temperature too low");
        break;
    default:
        puts("Fault: Unknown");
    }
}
