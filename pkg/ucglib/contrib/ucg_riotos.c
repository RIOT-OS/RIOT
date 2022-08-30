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

#include <assert.h>
#include <stdio.h>

#include "ucg_riotos.h"

#include "ztimer.h"

#ifdef MODULE_PERIPH_SPI
#include "periph/spi.h"
#endif
#include "periph/gpio.h"

#ifdef MODULE_PERIPH_SPI
static spi_clk_t spi_clk_cache(spi_t bus, uint32_t period_ns)
{
    static uint32_t period_ns_cache;
    static spi_clk_t clk_cache;

    if (period_ns != period_ns_cache) {
        period_ns_cache = period_ns;
        clk_cache = spi_get_clk(bus, MHZ(1000) / period_ns);
    }

    return clk_cache;
}
#endif /* MODULE_PERIPH_SPI */

/**
 * @brief   Enable the selected pins in RIOT-OS.
 */
static void _enable_pins(const ucg_riotos_t *ucg_riot_ptr)
{
    /* no hardware peripheral is being used, nothing to be done */
    if (ucg_riot_ptr == NULL) {
        return;
    }

    if (gpio_is_valid(ucg_riot_ptr->pin_cs)) {
        gpio_init(ucg_riot_ptr->pin_cs, GPIO_OUT);
    }

    if (gpio_is_valid(ucg_riot_ptr->pin_cd)) {
        gpio_init(ucg_riot_ptr->pin_cd, GPIO_OUT);
    }

    if (gpio_is_valid(ucg_riot_ptr->pin_reset)) {
        gpio_init(ucg_riot_ptr->pin_reset, GPIO_OUT);
    }
}

#ifdef MODULE_PERIPH_SPI
int16_t ucg_com_hw_spi_riotos(ucg_t *ucg, int16_t msg, uint16_t arg, uint8_t *data)
{
    const ucg_riotos_t *ucg_riot_ptr = ucg_GetUserPtr(ucg);

    /* assert that user_ptr is correctly set */
    assert(ucg_riot_ptr != NULL);

    spi_t dev = SPI_DEV(ucg_riot_ptr->device_index);

    switch (msg) {
        case UCG_COM_MSG_POWER_UP:
            /* setup pins */
            _enable_pins(ucg_riot_ptr);

            /* setup SPI */
            spi_init_pins(dev);
            /* correct alignment of data can be assumed, as in pkg callers use
             * ucg_com_info_t to allocate memory */
            ucg_com_info_t *info = (void *)(uintptr_t)data;
            spi_acquire(dev, GPIO_UNDEF, SPI_MODE_0,
                        spi_clk_cache(dev, info->serial_clk_speed));

            break;
        case UCG_COM_MSG_POWER_DOWN:
            spi_release(dev);
            break;
        case UCG_COM_MSG_DELAY:
            ztimer_sleep(ZTIMER_USEC, arg);
            break;
        case UCG_COM_MSG_CHANGE_RESET_LINE:
            if (ucg_riot_ptr != NULL && gpio_is_valid(ucg_riot_ptr->pin_reset)) {
                gpio_write(ucg_riot_ptr->pin_reset, arg);
            }
            break;
        case UCG_COM_MSG_CHANGE_CS_LINE:
            if (ucg_riot_ptr != NULL && gpio_is_valid(ucg_riot_ptr->pin_cs)) {
                gpio_write(ucg_riot_ptr->pin_cs, arg);
            }
            break;
        case UCG_COM_MSG_CHANGE_CD_LINE:
            if (ucg_riot_ptr != NULL && gpio_is_valid(ucg_riot_ptr->pin_cd)) {
                gpio_write(ucg_riot_ptr->pin_cd, arg);
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
                    if (ucg_riot_ptr != NULL &&  ucg_riot_ptr->pin_cd != GPIO_UNDEF) {
                        gpio_write(ucg_riot_ptr->pin_cd, *data);
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
#endif /* MODULE_PERIPH_SPI */

ucg_int_t ucg_dev_dummy_riotos(ucg_t *ucg, ucg_int_t msg, void *data)
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
