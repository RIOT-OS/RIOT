/*
 * Copyright (C) 2022 Nicole Faerber, Siegen, Germany
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_baglejs-2
 * @{
 *
 * @file
 * @brief       Board initialization for the Bagle JS2
 *
 * @author      Nicole Faebrer <nicole.faerber@digitaluhr-manufaktur.de>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "mtd.h"
#include "mtd_spi_nor.h"

#include "periph/gpio.h"
#include "periph/spi.h"
#include "periph/pm.h"
#include "periph/adc.h"
#include "periph/pwm.h"
#include "periph/spi.h"

#include "timex.h"
#include "ztimer.h"


#ifdef MODULE_MTD
static const mtd_spi_nor_params_t _banglejs2_nor_params = {
    .opcode = &mtd_spi_nor_opcode_default,
    .wait_chip_erase = 9LU * US_PER_SEC,
    .wait_32k_erase = 160LU *US_PER_MS,
    .wait_sector_erase = 70LU * US_PER_MS,
    .wait_chip_wake_up = 1LU * US_PER_MS,
    .clk = BANGLEJS2_NOR_SPI_CLK,
    .flag = BANGLEJS2_NOR_FLAGS,
    .spi = BANGLEJS2_NOR_SPI_DEV,
    .mode = BANGLEJS2_NOR_SPI_MODE,
    .cs = BANGLEJS2_NOR_SPI_CS,
    .wp = GPIO_UNDEF,
    .hold = GPIO_UNDEF,
};

static mtd_spi_nor_t banglejs2_nor_dev = {
    .base = {
        .driver = &mtd_spi_nor_driver,
        .page_size = BANGLEJS2_NOR_PAGE_SIZE,
        .pages_per_sector = BANGLEJS2_NOR_PAGES_PER_SECTOR,
    },
    .params = &_banglejs2_nor_params,
};

mtd_dev_t *mtd0 = (mtd_dev_t *)&banglejs2_nor_dev;
#endif /* MODULE_MTD */


void board_init(void)
{
    gpio_init(VIBRATOR, GPIO_OUT);
    gpio_clear(VIBRATOR);

    gpio_init(HRM_INT, GPIO_IN);
    gpio_init(HRM_SDA, GPIO_IN);
    gpio_init(HRM_PWR, GPIO_OUT);
    gpio_clear(HRM_PWR);

    gpio_init(GPS_PWR, GPIO_OUT);
    gpio_clear(GPS_PWR);

    gpio_init(LCD_DISP, GPIO_OUT);
    gpio_clear(LCD_DISP);
    gpio_init(LCD_EXTCOMIN, GPIO_OUT);
    gpio_clear(LCD_EXTCOMIN);
    /* LCD CS is high active */
    gpio_init(LCD_CS, GPIO_OUT);
    gpio_clear(LCD_CS);

    gpio_init(ACCEL_INT, GPIO_IN);
    gpio_init(ACCEL_SDA, GPIO_IN);

    gpio_init(MAGN_SDA, GPIO_IN);
    gpio_init(ACCEL_SDA, GPIO_IN);
    gpio_init(ATM_PRESSURE_SDA, GPIO_IN);

    gpio_init(BUTTON0, GPIO_IN_PU);
    gpio_init(EXTPOWER_PRESENT, GPIO_IN);
    gpio_init(CHARGE_COMPLETE, GPIO_IN);

    gpio_init(SPI_FLASH_CS, GPIO_OUT);
    /* make sure SPI flash is not selected */
    gpio_set(SPI_FLASH_CS);
    gpio_init(SPI_FLASH_MISO, GPIO_IN);

    /* bring touch controller out of reset */
    gpio_init(TOUCH_RESET, GPIO_OUT);
    gpio_set(TOUCH_RESET);

    /* init PWMs */
    if (pwm_init(PWM_DEV(0), PWM_RIGHT, 1250, 100) > 0) {
        pwm_set(PWM_DEV(0), 0, 0); /* 0% LCD backlight brightness */
        pwm_poweroff(PWM_DEV(0));
    }

    if (pwm_init(PWM_DEV(1), PWM_RIGHT, 1250, 100) > 0) {
        pwm_set(PWM_DEV(1), 0, 50); /* 50% duty cylce @ 125kHz for EXTCOM ? */
        /* pwm_poweron(PWM_DEV(1)); */
    }

    if (pwm_init(PWM_DEV(2), PWM_RIGHT, 1250, 100) > 0) {
        pwm_set(PWM_DEV(2), 0, 0); /* vibration off */
        pwm_poweroff(PWM_DEV(2));
    }

    /* init ADC */
    adc_init(1); /* battery voltage monitor */
}

void board_power_off(void)
{
    ztimer_sleep(ZTIMER_MSEC, 5);

    /* power off peripherals as much as we can */
    gpio_clear(LCD_DISP);
    pwm_poweroff(PWM_DEV(0)); /* LCD ExtCOM */
    pwm_poweroff(PWM_DEV(1)); /* LCD backlight */
    pwm_poweroff(PWM_DEV(2)); /* vibration motor */
    gpio_clear(HRM_PWR);
    gpio_clear(GPS_PWR);

    /* keeping touch in reset should lower power usage */
    gpio_clear(TOUCH_RESET);

    /* finally call RIOT's system power off function */
    pm_off();
}
