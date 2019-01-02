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
 * @file        u8g2_riotos.h
 * @brief       U8g2 driver for interacting with RIOT-OS drivers
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "u8g2.h"
#include "log.h"
#include "xtimer.h"
#include "periph/spi.h"
#include "periph/i2c.h"
#include "periph/gpio.h"
#include "u8x8_riotos.h"

#ifdef MODULE_PERIPH_SPI
static spi_clk_t u8x8_pulse_width_to_spi_speed(uint32_t pulse_width)
{
    const uint32_t cycle_time = 2 * pulse_width;

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
#endif /* MODULE_PERIPH_SPI */

#ifdef MODULE_PERIPH_SPI
static spi_mode_t u8x8_spi_mode_to_spi_conf(uint32_t spi_mode)
{
    return (spi_mode_t) spi_mode;
}
#endif /* MODULE_PERIPH_SPI */

/*
- mention in readme that the scope of user ptr must be at least the scope of the u8g2
*/

/**
 * Enable the selected pins in RIOT-OS.
 */
static void u8x8_enable_pins(const u8x8_riotos_t *u8x8_riot_ptr)
{
    // No hw peripheral is being used, nothing to be done
    if (NULL == u8x8_riot_ptr)
    {
        return;
    }

    if (GPIO_UNDEF != u8x8_riot_ptr->pin_cs)
    {
        gpio_init(u8x8_riot_ptr->pin_cs, GPIO_OUT);
    }

    if (GPIO_UNDEF != u8x8_riot_ptr->pin_dc)
    {
        gpio_init(u8x8_riot_ptr->pin_dc, GPIO_OUT);
    }

    if (GPIO_UNDEF != u8x8_riot_ptr->pin_reset)
    {
        gpio_init(u8x8_riot_ptr->pin_reset, GPIO_OUT);
    }
}

uint8_t u8x8_gpio_and_delay_riotos(u8x8_t *u8g2, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    (void) arg_ptr;

    const u8x8_riotos_t *u8x8_riot_ptr = u8g2->user_ptr;

    switch (msg) {
        case U8X8_MSG_GPIO_AND_DELAY_INIT:
            u8x8_enable_pins(u8x8_riot_ptr);
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
            if (NULL != u8x8_riot_ptr && GPIO_UNDEF != u8x8_riot_ptr->pin_cs) {
                gpio_write(u8x8_riot_ptr->pin_cs, arg_int);
            }
            break;
        case U8X8_MSG_GPIO_DC:
            if (NULL != u8x8_riot_ptr && GPIO_UNDEF != u8x8_riot_ptr->pin_dc) {
                gpio_write(u8x8_riot_ptr->pin_dc, arg_int);
            }
            break;
        case U8X8_MSG_GPIO_RESET:
            if (NULL != u8x8_riot_ptr && GPIO_UNDEF != u8x8_riot_ptr->pin_reset) {
                gpio_write(u8x8_riot_ptr->pin_reset, arg_int);
            }
            break;
        default:
            return 0;
    }

    return 1;
}

#ifdef MODULE_PERIPH_SPI
uint8_t u8x8_byte_riotos_hw_spi(u8x8_t *u8g2, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    const u8x8_riotos_t *u8x8_riot_ptr = u8g2->user_ptr;

    // Check that user_ptr is correctly set
    if (NULL == u8x8_riot_ptr)
    {
        LOG_ERROR("u8x8_riot_ptr cannot be NULL");
        return 1;
    }

    spi_t dev = SPI_DEV(u8x8_riot_ptr->device_index);

    switch (msg) {
        case U8X8_MSG_BYTE_SEND:
            spi_transfer_bytes(dev, GPIO_UNDEF, true,
                               arg_ptr, NULL, (size_t)arg_int);
            break;
        case U8X8_MSG_BYTE_INIT:
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
#endif /* MODULE_PERIPH_SPI */

#ifdef MODULE_PERIPH_I2C
uint8_t u8x8_byte_riotos_hw_i2c(u8x8_t *u8g2, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    static uint8_t buffer[32]; /* u8g2/u8x8 will never send more than 32 bytes between START_TRANSFER and END_TRANSFER */
    static size_t index;

    const u8x8_riotos_t u8x8_riot_ptr = u8g2->user_ptr;

    // Check that user_ptr is correctly set
    if (NULL == u8x8_riot_ptr)
    {
        LOG_ERROR("u8x8_riot_ptr cannot be NULL");
        return 1;
    }

    i2c_t dev = I2C_DEV(u8x8_riot_ptr->device_index);

    switch (msg) {
        case U8X8_MSG_BYTE_SEND:
            memcpy(&buffer[index], arg_ptr, arg_int);
            index += arg_int;
            assert(index <= sizeof(buffer));
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
#endif /* MODULE_PERIPH_I2C */
