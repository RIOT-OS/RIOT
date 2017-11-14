/*
 * Copyright (C) 2015-2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_sltb001a
 * @{
 *
 * @file
 * @brief       Board specific implementations SLTB001A board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"

#include "periph/gpio.h"
#include "periph/i2c.h"

#if BMP280_ENABLED || CCS811_ENABLED || ICM_20648_ENABLED || \
    SI1133_ENABLED || SI7021_ENABLED || SI7210A_ENABLED || \
    RGB_LED1_ENABLED || RGB_LED2_ENABLED || RGB_LED3_ENABLED || \
    RGB_LED4_ENABLED
static inline void board_usleep(uint32_t delay)
{
    /* decrement + compare take two cycles, therefore divide by two */
    uint32_t count = (delay * (SystemCoreClock / 1000 / 1000)) / 2;

    while (count--) {}
}

static void board_pic_init(void)
{
    gpio_init(PIC_INT_WAKE_PIN, GPIO_OD);
    gpio_set(PIC_INT_WAKE_PIN);

    i2c_init_master(PIC_I2C, I2C_SPEED_NORMAL);
}

static void board_pic_write(uint8_t addr, uint8_t value)
{
    /* toggle the pin for 4 us */
    gpio_clear(PIC_INT_WAKE_PIN);
    board_usleep(4);

    /* write to gpio expander */
    i2c_acquire(PIC_I2C);
    uint8_t bytes[] = { addr, value };
    i2c_write_bytes(PIC_I2C, PIC_I2C_ADDR, bytes, 2);
    i2c_release(PIC_I2C);

    /* put PIC in sleep mode again */
    gpio_set(PIC_INT_WAKE_PIN);
}
#endif

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();

    /* initialize the LEDs */
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);

    /* initialize the push buttons */
    gpio_init(PB0_PIN, GPIO_IN);
    gpio_init(PB1_PIN, GPIO_IN);

    /* initialize the environmental sensors (multiple ones) */
#if BMP280_ENABLED || CCS811_ENABLED || ICM_20648_ENABLED || \
    SI1133_ENABLED || SI7021_ENABLED || SI7210A_ENABLED || \
    RGB_LED1_ENABLED || RGB_LED2_ENABLED || RGB_LED3_ENABLED || \
    RGB_LED4_ENABLED
    board_pic_init();
#endif

    /* enable the CCS811 air quality/gas sensor */
#if CCS811_ENABLED
    board_pic_write(CCS811_PIC_ADDR, (1 << CCS811_PIC_EN_BIT) | (1 << CCS811_PIC_WAKE_BIT));
#endif

    /* enable the IMU sensor */
#if ICM_20648_ENABLED
    board_pic_write(ICM20648_PIC_ADDR, 1 << ICM20648_PIC_EN_BIT);
#endif

    /* enable the environmental sensors */
#if BMP280_ENABLED || SI1133_ENABLED || SI7021_ENABLED || SI7210A_ENABLED
    board_pic_write(ENV_SENSE_PIC_ADDR, 1 << ENV_SENSE_PIC_BIT);
#endif

    /* enable the RGB leds */
#if RGB_LED1_ENABLED || RGB_LED2_ENABLED || RGB_LED3_ENABLED || RGB_LED4_ENABLED
    board_pic_write(RGB_LED_ADDR,
        (1 << RGB_LED_EN_BIT) |
        (RGB_LED1_ENABLED << RGB_LED1_EN_BIT) |
        (RGB_LED2_ENABLED << RGB_LED2_EN_BIT) |
        (RGB_LED3_ENABLED << RGB_LED3_EN_BIT) |
        (RGB_LED4_ENABLED << RGB_LED4_EN_BIT));
#endif
}
