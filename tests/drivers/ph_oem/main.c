/*
 * SPDX-FileCopyrightText: 2019 University of Applied Sciences Emden / Leer
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the Atlas Scientific pH OEM sensor driver
 *
 * @author      Igor Knippenberg <igor.knippenberg@gmail.com>
 *
 * @}
 */

#include <stdio.h>

#include "xtimer.h"
#include "event/callback.h"

#include "ph_oem.h"
#include "ph_oem_params.h"
#include "ph_oem_regs.h"

#define SLEEP_SEC                   (5)

/* calibration test is off by default, so it won't reset your previous calibration */
#define CALIBRATION_TEST_ENABLED    (false)

static void reading_available_event_callback(event_t *event);

static ph_oem_t dev;

static event_queue_t event_queue;
static event_t event = { .handler = reading_available_event_callback };

static void reading_available_event_callback(event_t *event)
{
    (void)event;
    uint16_t data;

    puts("\n[EVENT - reading pH value from the device]");

    /* stop pH sensor from taking further readings*/
    ph_oem_set_device_state(&dev, PH_OEM_STOP_READINGS);

    /* reset interrupt pin in case of falling or rising flank */
    ph_oem_reset_interrupt_pin(&dev);

    ph_oem_read_ph(&dev, &data);
    printf("pH value raw: %d\n", data);

    ph_oem_read_compensation(&dev, &data);
    printf("pH reading was taken at %d Celsius\n", data);
}

static void interrupt_pin_callback(void *arg)
{
    puts("\n[IRQ - Reading done. Writing read-event to event queue]");
    (void)arg;

    /* Posting event to the event queue. Main is blocking with "event_wait"
     * and will execute the event callback after posting */
    event_post(&event_queue, &event);

    /* initiate new reading with "ph_oem_start_new_reading()" for this callback
       to be called again */
}

int main(void)
{
    uint16_t data;

    puts("Atlas Scientific pH OEM sensor driver test application\n");

    printf("Initializing pH OEM sensor at I2C_%i, address 0x%02x...",
           PH_OEM_PARAM_I2C, PH_OEM_PARAM_ADDR);

    if (ph_oem_init(&dev, ph_oem_params) == PH_OEM_OK) {
        puts("[OK]");
    }
    else {
        puts("[Failed]");
        return -1;
    }

    printf("Turning LED off... ");
    if (ph_oem_set_led_state(&dev, PH_OEM_LED_OFF) == PH_OEM_OK) {
        puts("[OK]");
        /* Sleep 2 seconds to actually see it turning off */
        xtimer_sleep(2);
    }
    else {
        puts("[Failed]");
        return -1;
    }

    printf("Turning LED on... ");
    if (ph_oem_set_led_state(&dev, PH_OEM_LED_ON) == PH_OEM_OK) {
        puts("[OK]");
    }
    else {
        puts("[Failed]");
        return -1;
    }

    /* Test changing the pH OEM i2c address to 0x66 and back to 0x65 in the
     * sensor as well as dev->params.addr
     */
    printf("Setting device address to 0x66... ");
    if (ph_oem_set_i2c_address(&dev, 0x66) == PH_OEM_OK) {
        puts("[OK]");
    }
    else {
        puts("[Failed]");
        return -1;
    }

    printf("Setting device address back to the default address 0x65... ");
    if (ph_oem_set_i2c_address(&dev, 0x65) == PH_OEM_OK) {
        puts("[OK]");
    }
    else {
        puts("[Failed]");
        return -1;
    }

    /* Test calibration process and if it is applied correctly in the pH OEM register */
    if (CALIBRATION_TEST_ENABLED) {
        printf("Clearing all previous calibrations... ");
        if (ph_oem_clear_calibration(&dev) == PH_OEM_OK) {
            puts("[OK]");
        }
        else {
            puts("[Failed]");
            return -1;
        }

        printf("Reading calibration state, should be 0... ");
        if (ph_oem_read_calibration_state(&dev, &data) == PH_OEM_OK
            && data == 0) {
            puts("[OK]");
        }
        else {
            puts("[Failed]");
            return -1;
        }

        /* Don't forget to provide temperature compensation for the calibration */
        printf("Setting temperature compensation to 22 Celsius... ");
        if (ph_oem_set_compensation(&dev, 2200)) {
            puts("[OK]");
        }
        else {
            puts("[Failed]");
            return -1;
        }

        /* Always start with mid point when doing a new calibration  */
        printf("Calibrating to midpoint... ");
        if (ph_oem_set_calibration(&dev, 6870, PH_OEM_CALIBRATE_MID_POINT)
            == PH_OEM_OK) {
            puts("[OK]");
        }
        else {
            puts("[Failed]");
            return -1;
        }

        printf("Reading calibration state, should be 2... ");
        if (ph_oem_read_calibration_state(&dev, &data) == PH_OEM_OK
            && data == 2) {
            puts("[OK]");
        }
        else {
            puts("[Failed]");
            return -1;
        }

        printf("Calibrating to lowpoint... ");
        if (ph_oem_set_calibration(&dev, 4000, PH_OEM_CALIBRATE_LOW_POINT)
            == PH_OEM_OK) {
            puts("[OK]");
        }
        else {
            puts("[Failed]");
            return -1;
        }

        printf("Reading calibration state, should be 3... ");
        if (ph_oem_read_calibration_state(&dev, &data) == PH_OEM_OK
            && data == 3) {
            puts("[OK]");
        }
        else {
            puts("[Failed]");
            return -1;
        }

        printf("Calibrating to highpoint... ");
        if (ph_oem_set_calibration(&dev, 9210, PH_OEM_CALIBRATE_HIGH_POINT)
            == PH_OEM_OK) {
            puts("[OK]");
        }
        else {
            puts("[Failed]");
            return -1;
        }

        printf("Reading calibration state, should be 7... ");
        if (ph_oem_read_calibration_state(&dev, &data) == PH_OEM_OK
            && data == 7) {
            puts("[OK]");
        }
        else {
            puts("[Failed]");
            return -1;
        }
    }

    if (gpio_is_valid(dev.params.interrupt_pin)) {
        /* Setting up and enabling the interrupt pin of the pH OEM */
        printf("Enabling interrupt pin... ");
        if (ph_oem_enable_interrupt(&dev, interrupt_pin_callback,
                                    &data) == PH_OEM_OK) {
            puts("[OK]");
        }
        else {
            puts("[Failed]");
            return -1;
        }

        /* initiate an event-queue. An event will be posted by the
         * "interrupt_pin_callback" after an IRQ occurs. */
        event_queue_init(&event_queue);
    }
    else {
        puts("Interrupt pin undefined");
    }

    printf("Setting temperature compensation to 22 °C... ");
    if (ph_oem_set_compensation(&dev, 2200) == PH_OEM_OK) {
        puts("[OK]");
    }
    else {
        puts("[Failed]");
        return -1;
    }

    while (1) {
        puts("\n[MAIN - Initiate reading]");

        /* blocking for ~420ms till reading is done if no interrupt pin defined */
        ph_oem_start_new_reading(&dev);

        if (gpio_is_valid(dev.params.interrupt_pin)) {
            /* when interrupt is defined, wait for the IRQ to fire and
             * the event to be posted, so the "reading_available_event_callback"
             * can be executed after */
            event_t *ev = event_wait(&event_queue);
            ev->handler(ev);
        }

        if (!gpio_is_valid(dev.params.interrupt_pin)) {

            if (ph_oem_read_ph(&dev, &data) == PH_OEM_OK) {
                printf("pH value raw: %d\n", data);
            }
            else {
                puts("[Reading pH failed]");
            }

            if (ph_oem_read_compensation(&dev, &data) == PH_OEM_OK) {
                printf("pH reading was taken at %d Celsius\n", data);
            }
            else {
                puts("[Reading compensation failed]");
            }
        }
        xtimer_sleep(SLEEP_SEC);
    }
    return 0;
}
