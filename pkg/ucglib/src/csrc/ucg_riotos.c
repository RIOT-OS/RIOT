/*
 * Copyright (C) 2018 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_ucglib
 * @{
 *
 * @file
 * @brief       Ucglib driver to interact with RIOT-OS drivers.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "ucg.h"

#include "xtimer.h"

#include "periph/spi.h"
#include "periph/i2c.h"
#include "periph/gpio.h"

#include <stdio.h>

#ifdef SPI_NUMOF
static spi_clk_t ucg_serial_clk_speed_to_spi_speed(uint32_t serial_clk_speed)
{
    if (serial_clk_speed < 100) {
        return SPI_CLK_10MHZ;
    }
    else if (serial_clk_speed < 200) {
        return SPI_CLK_5MHZ;
    }
    else if (serial_clk_speed < 1000) {
        return SPI_CLK_1MHZ;
    }
    else if (serial_clk_speed < 2500) {
        return SPI_CLK_400KHZ;
    }

    return SPI_CLK_100KHZ;
}
#endif /* SPI_NUMOF */

static void ucg_enable_pins(gpio_t *pins, uint32_t pins_enabled)
{
    uint8_t i;

    for (i = 0; i < 32; i++) {
        if (pins_enabled & (1 << i)) {
            if (pins[i] != GPIO_UNDEF) {
                if (i < UCG_PIN_COUNT) {
                    gpio_init(pins[i], GPIO_OUT);
                }
                else {
                    gpio_init(pins[i], GPIO_IN);
                }
            }
        }
    }
}

#ifdef SPI_NUMOF
int16_t ucg_com_riotos_hw_spi(ucg_t *ucg, int16_t msg, uint16_t arg, uint8_t *data)
{
    spi_t dev = (spi_t) ucg->dev;

    switch (msg) {
        case UCG_COM_MSG_POWER_UP:
            /* setup pins */
            ucg_enable_pins(ucg->pin_list, ucg->pins_enabled);

            /* setup Arduino SPI */
            spi_init_pins(dev);
            spi_acquire(dev, GPIO_UNDEF, SPI_MODE_0,
                        ucg_serial_clk_speed_to_spi_speed(((ucg_com_info_t *)data)->serial_clk_speed));

            break;
        case UCG_COM_MSG_POWER_DOWN:
            spi_release(dev);
            break;
        case UCG_COM_MSG_DELAY:
            xtimer_usleep(arg);
            break;
        case UCG_COM_MSG_CHANGE_RESET_LINE:
            if (ucg->pins_enabled & (1 << UCG_PIN_RST)) {
                gpio_write(ucg->pin_list[UCG_PIN_RST], arg);
            }
            break;
        case UCG_COM_MSG_CHANGE_CS_LINE:
            if (ucg->pins_enabled & (1 << UCG_PIN_CS)) {
                gpio_write(ucg->pin_list[UCG_PIN_CS], arg);
            }
            break;
        case UCG_COM_MSG_CHANGE_CD_LINE:
            if (ucg->pins_enabled & (1 << UCG_PIN_CD)) {
                gpio_write(ucg->pin_list[UCG_PIN_CD], arg);
            }
            break;
        case UCG_COM_MSG_SEND_BYTE:
            spi_transfer_byte(dev, GPIO_UNDEF, true, (uint8_t) arg);
            break;
        case UCG_COM_MSG_REPEAT_1_BYTE:
            while (arg--) {
                spi_transfer_byte(dev, GPIO_UNDEF, true, ((uint8_t *) data)[0]);
            }
            break;
        case UCG_COM_MSG_REPEAT_2_BYTES:
            while (arg--) {
                spi_transfer_bytes(dev, GPIO_UNDEF, true, data, NULL, 2);
            }
            break;
        case UCG_COM_MSG_REPEAT_3_BYTES:
            while (arg--) {
                spi_transfer_bytes(dev, GPIO_UNDEF, true, data, NULL, 3);
            }
            break;
        case UCG_COM_MSG_SEND_STR:
            spi_transfer_bytes(dev, GPIO_UNDEF, true, data, NULL, arg);
            break;
        case UCG_COM_MSG_SEND_CD_DATA_SEQUENCE:
            while (arg--) {
                if (*data != 0) {
                    if (ucg->pins_enabled & (1 << UCG_PIN_CD)) {
                        gpio_write(ucg->pin_list[UCG_PIN_CD], *data);
                    }
                }

                data++;
                spi_transfer_bytes(dev, GPIO_UNDEF, true, data, NULL, 1);
                data++;
            }
            break;
    }

    return 1;
}
#endif /* SPI_NUMOF */

ucg_int_t ucg_dev_dummy_cb(ucg_t *ucg, ucg_int_t msg, void *data)
{
    static uint32_t pixels;

    switch (msg) {
        case UCG_MSG_DEV_POWER_UP:
            puts("ucg: UCG_MSG_DEV_POWER_UP");
            return 1;
        case UCG_MSG_DEV_POWER_DOWN:
            puts("ucg: UCG_MSG_DEV_POWER_DOWN");
            return 1;
        case UCG_MSG_GET_DIMENSION:
            puts("ucg: UCG_MSG_GET_DIMENSION");
            ((ucg_wh_t *)data)->w = 128;
            ((ucg_wh_t *)data)->h = 128;
            return 1;
        case UCG_MSG_DRAW_PIXEL:
            pixels++;

            /* log each 128th draw */
            if (pixels % 128 == 0) {
                printf("ucg: UCG_MSG_DRAW_PIXEL (%" PRIu32 ")\n", pixels);
            }

            return 1;
    }
    return ucg_dev_default_cb(ucg, msg, data);
}
