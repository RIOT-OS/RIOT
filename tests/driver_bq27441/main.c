/*
 * Copyright (C) 2018 Anatoliy Atanasov, Iliyan Stoyanov
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 * @file
 * @brief       test aplication for the BQ27441 Lithium Battery fuel gauge by TI
 *
 * @author      Anatoliy Atanasov <anatoliy@6lowpan.io>
 * @author      Iliyan Stoyanov <iliyan@6lowpan.io>
 * @}
 */

#include <stdio.h>
#include "xtimer.h"
#include "bq27441.h"
#include "bq27441_params.h"
#include "periph/gpio.h"

static void _gauge_cb(void *arg)
{
    (void)arg;
    printf("\n BQ27441 CB \n");
}

int main(void)
{
    puts("BQ27441 Lithium Battery Fuel Gauge test application");
    bq27441_t dev;
    dev.cb = _gauge_cb;
    dev.arg = NULL;
    const unsigned int BATTERY_CAPACITY = 850;
    gpio_t out_pin = GPIO_PIN(3, 3); // TODO: DEFINE CORRECTLY

    i2c_acquire(params_default[0].bus);
    if (bq27441_init(&dev, params_default)) {
        xtimer_sleep(2);
        if (bq27441_enter_config_mode(&dev, false)) {
            bq27441_set_capacity(&dev, BATTERY_CAPACITY);
            bq27441_set_gpout_polarity(&dev, false);
            bq27441_set_gpout_function(&dev, SOC_INT);
            bq27441_set_soc_int_delta(&dev, 1);
            bq27441_exit_config_mode(&dev, false);
        }
        if (bq27441_get_gpout_polarity(&dev)) {
            printf("GPOUT set to active-HIGH \n");
        }
        else {
            printf("GPOUT set to active-LOW \n");
        }

        if (bq27441_get_gpout_function(&dev)) {
            printf("GPOUT function set to BAT_LOW \n");
        }
        else {
            printf("GPOUT function set to SOC_INT \n");
        }
        printf("SOCI Delta: 0x%02x \n", bq27441_get_soc_int_delta(&dev));

        printf("Testing GPOUT Pulse\n");

        bq27441_pulse_gpout(&dev);
        int timeout = 10000;    // The pulse can take a while to occur. Set max to 10s
        while ((gpio_read(out_pin)) && timeout--) {
            xtimer_sleep(1);    // Wait for GPOUT to go high, or timeout to occur
        }
        if (timeout > 0) {
            // If GPOUT pulsed, print success message.
            printf("GPOUT test successful!");
            printf("( %d )", (10000 - timeout));
            printf("GPOUT will pulse whenever the SoC \n");
            printf("value changes by SOCI delta.\n");
            printf("Or when the battery changes from\n");
            printf(" charging to discharging, or vice-versa.\n");
        }
        else {
            // If GPOUT didn't pulse, something went wrong. Print error message
            // and loop forever.
            printf("GPOUT didn't pulse.\n");
            printf("Make sure it's connected to pin and reset. \n");
        }
        uint16_t device_id = bq27441_get_device_type(&dev); // Read deviceType from BQ27441
        uint16_t capacity = bq27441_get_capacity(&dev, REMAIN);
        uint16_t soc = bq27441_get_soc(&dev, FILTERED);
        uint16_t voltage = bq27441_get_voltage(&dev);
        uint16_t current = bq27441_get_current(&dev, AVG);
        uint16_t full_capacity = bq27441_get_capacity(&dev, FULL);
        int16_t power = bq27441_get_power(&dev);
        uint8_t health = bq27441_get_soh(&dev, PERCENT);
        printf("Device id: 0x%02x \n", device_id);
        printf("Capacity : 0x%02x mAh \n", capacity);
        printf("State of charge: 0x%02x %%\n", soc);
        printf("Voltage: 0x%02x mV\n", voltage);
        printf("Current id: 0x%02x mA\n", current);
        printf("Full Capacity: 0x%02x mAh\n", full_capacity);
        printf("Power: 0x%02x mW\n", power);
        printf("Health: 0x%02x %% \n", health);
    }
    else {
        printf("Could not connect to fuel gauge\n");
    }
    i2c_release(params_default[0].bus);

    return 0;
}
