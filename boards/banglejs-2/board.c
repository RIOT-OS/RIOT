/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_adafruit-clue
 * @{
 *
 * @file
 * @brief       Board initialization for the Adafruit Clue
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"

#include "periph/gpio.h"
#include "periph/spi.h"
#include "periph/pm.h"
#include "periph/adc.h"
#include "periph/pwm.h"
#include "periph/spi.h"

#include "cst816s.h"

// #define ENABLE_TOUCH

//char touch_ev=0;
#ifdef ENABLE_TOUCH
cst816s_t _input_dev;
cst816s_touch_data_t tdata;

static const cst816s_params_t _cst816s_input_params = {
    .i2c_dev = I2C_DEV(0),
    .i2c_addr = TOUCH_I2C_ADDR,
    .irq = TOUCH_INT,
    .irq_flank = GPIO_FALLING,
    .reset = TOUCH_RESET,
};

static void touch_cb(void *arg)
{
    (void) arg;

    // can not read I2C from IRQ context, just set a flag
    touch_ev=1;
}
#endif

void board_init(void)
{
    gpio_init(VIBRATOR, GPIO_OUT);
    gpio_clear(VIBRATOR);

    gpio_init(HRM_PWR, GPIO_OUT);
    gpio_clear(HRM_PWR);

    gpio_init(GPS_PWR, GPIO_OUT);
    gpio_clear(GPS_PWR);

    gpio_init(LCD_DISP, GPIO_OUT);
    gpio_clear(LCD_DISP);
    gpio_init(LCD_EXTCOMIN, GPIO_OUT);
    gpio_clear(LCD_EXTCOMIN);

    gpio_init(BUTTON0, GPIO_IN_PU);
    gpio_init(EXTPOWER_PRESENT, GPIO_IN);
    gpio_init(CHARGE_COMPLETE, GPIO_IN);

    // bring touch controller out of reset
    gpio_set(TOUCH_RESET);

    // LCD CS is high active
    gpio_init(LCD_CS, GPIO_OUT);
    gpio_clear(LCD_CS);

    // init PWMs
    pwm_init(PWM_DEV(0), PWM_RIGHT, 1250, 100);
    pwm_set(PWM_DEV(0), 0, 80); // 80% LCD backlight brightness
    pwm_poweron(PWM_DEV(0));

    pwm_init(PWM_DEV(1), PWM_RIGHT, 1250, 100);
    pwm_set(PWM_DEV(1), 0, 50); // 50% duty cylce @ 125kHz for EXTCOM ?
    pwm_poweron(PWM_DEV(1));
    
    // init ADC
    adc_init(1); // battery voltage monitor

    // init touchscreen
#ifdef ENABLE_TOUCH
    if (cst816s_init(&_input_dev, &_cst816s_input_params, touch_cb, NULL) != CST816S_OK) {
        printf("cst init fail\n");
    };
#endif
}

void board_power_off(void)
{
    // power off peripherals as much as we can
    gpio_clear(LCD_DISP);
    gpio_clear(LCD_BACKLIGHT);
    gpio_clear(VIBRATOR);
    gpio_clear(HRM_PWR);
    gpio_clear(GPS_PWR);

    // keeping touch in reset should lower power usage
    gpio_clear(TOUCH_RESET);

    // finally call RIOT's system power off function
    pm_off();
}
