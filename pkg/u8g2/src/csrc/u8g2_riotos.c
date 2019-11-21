/*
 * Copyright (C) 2016-2018 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_u8g2
 * @{
 *
 * @file
 * @brief       U8g2 driver for interacting with RIOT-OS drivers
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "u8g2.h"

#include "xtimer.h"
#include "periph/spi.h"
#include "periph/i2c.h"
#include "periph/gpio.h"

#ifdef SPI_NUMOF
static spi_clk_t u8x8_pulse_width_to_spi_speed(uint32_t pulse_width)
{
    uint32_t cycle_time = 2 * pulse_width;

    if (cycle_time < 100) {
        return SPI_CLK_10MHZ;
    } else if (cycle_time < 200) {
        return SPI_CLK_5MHZ;
    } else if (cycle_time < 1000) {
        return SPI_CLK_1MHZ;
    } else if (cycle_time < 2500) {
        return SPI_CLK_400KHZ;
    }

    return SPI_CLK_100KHZ;
}
#endif /* SPI_NUMOF */

#ifdef SPI_NUMOF
static spi_mode_t u8x8_spi_mode_to_spi_conf(uint32_t spi_mode)
{
    return (spi_mode_t) spi_mode;
}
#endif /* SPI_NUMOF */

static void u8x8_enable_pins(gpio_t* pins, uint32_t pins_enabled)
{
    uint8_t i;

    for (i = 0; i < 32; i++) {
        if (pins_enabled & ((uint32_t)1 << i)) {
            if (pins[i] != GPIO_UNDEF) {
                if (i < U8X8_PIN_OUTPUT_CNT) {
                    gpio_init(pins[i], GPIO_OUT);
                } else {
                    gpio_init(pins[i], GPIO_IN);
                }
            }
        }
    }
}

uint8_t u8x8_gpio_and_delay_riotos(u8x8_t *u8g2, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    (void) arg_ptr;

    switch (msg) {
        case U8X8_MSG_GPIO_AND_DELAY_INIT:
            u8x8_enable_pins(u8g2->pins, u8g2->pins_enabled);
            break;
        case U8X8_MSG_DELAY_MILLI:
            xtimer_usleep(arg_int * 1000);
            break;
        case U8X8_MSG_DELAY_10MICRO:
            xtimer_usleep(arg_int * 10);
            break;
        case U8X8_MSG_DELAY_100NANO:
            xtimer_nanosleep(arg_int * 100);
            break;
        case U8X8_MSG_GPIO_CS:
            if (u8g2->pins_enabled & (1 << U8X8_PIN_CS)) {
                gpio_write(u8g2->pins[U8X8_PIN_CS], arg_int);
            }
            break;
        case U8X8_MSG_GPIO_DC:
            if (u8g2->pins_enabled & (1 << U8X8_PIN_DC)) {
                gpio_write(u8g2->pins[U8X8_PIN_DC], arg_int);
            }
            break;
        case U8X8_MSG_GPIO_RESET:
            if (u8g2->pins_enabled & (1 << U8X8_PIN_RESET)) {
                gpio_write(u8g2->pins[U8X8_PIN_RESET], arg_int);
            }
            break;
        default:
            return 0;
    }

    return 1;
}

#ifdef SPI_NUMOF
uint8_t u8x8_byte_riotos_hw_spi(u8x8_t *u8g2, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    spi_t dev = (spi_t) u8g2->dev;

    switch (msg) {
        case U8X8_MSG_BYTE_SEND:
            spi_transfer_bytes(dev, GPIO_UNDEF, true,
                               arg_ptr, NULL, (size_t)arg_int);
            break;
        case U8X8_MSG_BYTE_INIT:
            spi_init_pins(dev);
            break;
        case U8X8_MSG_BYTE_SET_DC:
            u8x8_gpio_SetDC(u8g2, arg_int);
            break;
        case U8X8_MSG_BYTE_START_TRANSFER:
            spi_acquire(dev, GPIO_UNDEF,
                        u8x8_spi_mode_to_spi_conf(u8g2->display_info->spi_mode),
                        u8x8_pulse_width_to_spi_speed(u8g2->display_info->sck_pulse_width_ns));

            u8x8_gpio_SetCS(u8g2, u8g2->display_info->chip_enable_level);
            u8g2->gpio_and_delay_cb(u8g2, U8X8_MSG_DELAY_NANO, u8g2->display_info->post_chip_enable_wait_ns, NULL);
            break;
        case U8X8_MSG_BYTE_END_TRANSFER:
            u8g2->gpio_and_delay_cb(u8g2, U8X8_MSG_DELAY_NANO, u8g2->display_info->pre_chip_disable_wait_ns, NULL);
            u8x8_gpio_SetCS(u8g2, u8g2->display_info->chip_disable_level);

            spi_release(dev);
            break;
        default:
            return 0;
    }

    return 1;
}
#endif /* SPI_NUMOF */

#ifdef I2C_NUMOF
uint8_t u8x8_byte_riotos_hw_i2c(u8x8_t *u8g2, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    static uint8_t buffer[255];
    static uint8_t index;

    i2c_t dev = (i2c_t) u8g2->dev;

    switch (msg) {
        case U8X8_MSG_BYTE_SEND:
            memcpy(&buffer[index], arg_ptr, arg_int);
            index += arg_int;
            break;
        case U8X8_MSG_BYTE_INIT:
            break;
        case U8X8_MSG_BYTE_SET_DC:
            break;
        case U8X8_MSG_BYTE_START_TRANSFER:
            i2c_acquire(dev);
            index = 0;
            break;
        case U8X8_MSG_BYTE_END_TRANSFER:
            i2c_write_bytes(dev, u8x8_GetI2CAddress(u8g2), buffer, index, 0);
            i2c_release(dev);
            break;
        default:
            return 0;
    }

    return 1;
}
#endif /* I2C_NUMOF */
