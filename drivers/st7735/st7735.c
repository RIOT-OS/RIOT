/*
 * Copyright (C) 2018 Koen Zandberg
 *               2021 Francisco Molina
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_st7735
 * @{
 *
 * @file
 * @brief       Device driver implementation for the st7735 display controller
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <assert.h>
#include <string.h>
#include "byteorder.h"
#include "periph/spi.h"
#include "ztimer.h"
#include "kernel_defines.h"

#include "st7735.h"
#include "st7735_internal.h"
#include "lcd.h"
#include "lcd_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static void _write_cmd(const lcd_t *dev, uint8_t cmd, const uint8_t *data,
                       size_t len)
{
    gpio_clear(dev->params->dcx_pin);
    spi_transfer_byte(dev->params->spi, dev->params->cs_pin, len ? true : false, cmd);
    gpio_set(dev->params->dcx_pin);
    if (len) {
        spi_transfer_bytes(dev->params->spi, dev->params->cs_pin, false, data,
                           NULL, len);
    }
}

/* datasheet page 178, table converted to equation.
 * gvdd in 1mv increments: 4850 = 4.85V */
static uint8_t _st7735_calc_pwrctl1(uint16_t gvdd)
{
    return (gvdd - 2850) / 50;
}

static uint8_t _st7735_calc_vmh(uint16_t vcomh)
{
    return (vcomh - 2700) / 25;
}

static uint8_t _st7735_calc_vml(int16_t vcoml)
{
    return (vcoml + 2500) / 25;
}

static int _init(lcd_t *dev, const lcd_params_t *params)
{
    assert(params->lines <= 162);
    dev->params = params;
    uint8_t command_params[4] = { 0 };

    gpio_init(dev->params->dcx_pin, GPIO_OUT);
    int res = spi_init_cs(dev->params->spi, dev->params->cs_pin);

    if (res != SPI_OK) {
        DEBUG("[st7735] init: error initializing the CS pin [%i]\n", res);
        return -1;
    }

    if (gpio_is_valid(dev->params->rst_pin)) {
        gpio_init(dev->params->rst_pin, GPIO_OUT);
        gpio_clear(dev->params->rst_pin);
        ztimer_sleep(ZTIMER_MSEC, 120);
        gpio_set(dev->params->rst_pin);
    }
    ztimer_sleep(ZTIMER_MSEC, 120);

    /* Acquire once at release at the end */
    spi_acquire(dev->params->spi, dev->params->cs_pin, dev->params->spi_mode,
                dev->params->spi_clk);

    /* Soft Reset */
    _write_cmd(dev, LCD_CMD_SWRESET, NULL, 0);
    ztimer_sleep(ZTIMER_MSEC, 120);

    /* Display off */
    _write_cmd(dev, LCD_CMD_DISPOFF, NULL, 0);

    /* PWRCTL1 */
    command_params[0] = _st7735_calc_pwrctl1(CONFIG_ST7735_GVDD);
    _write_cmd(dev, LCD_CMD_PWCTRL1, command_params, 1);

    /* PWCTR2 VGH = 14.7V, VGL = -7.35V */
    command_params[0] = 0x01;
    command_params[1] = 0x05;
    _write_cmd(dev, LCD_CMD_PWCTRL2, command_params, 2);

    /* PWCTR3 Opamp current small, Boost frequency */
    command_params[0] = 0x02;
    command_params[1] = 0x01;
    command_params[2] = 0x02;
    _write_cmd(dev, LCD_CMD_PWCTRL3, command_params, 3);

    /* PWCTR6 */
    command_params[0] = 0x02;
    command_params[1] = 0x11;
    command_params[2] = 0x15;
    _write_cmd(dev, LCD_CMD_PWCTRL6, command_params, 3);

    /* No display Inversion , Line inversion */
    command_params[0] = 0x07;
    _write_cmd(dev, LCD_CMD_INVCTR, command_params, 1);

    /* VCOMCTL */
    command_params[0] = _st7735_calc_vmh(CONFIG_ST7735_VCOMH);
    command_params[1] = _st7735_calc_vml(CONFIG_ST7735_VCOML);
    _write_cmd(dev, LCD_CMD_VMCTRL1, command_params, 2);

    command_params[0] = 0x86;
    _write_cmd(dev, LCD_CMD_VMCTRL2, command_params, 1);

    /* Memory access CTL */
    command_params[0] = dev->params->rotation;
    command_params[0] |= dev->params->rgb ? 0 : LCD_MADCTL_BGR;
    _write_cmd(dev, LCD_CMD_MADCTL, command_params, 1);

    /* Frame control */
    command_params[0] = 0x00;
    command_params[1] = 0x18;
    _write_cmd(dev, LCD_CMD_FRAMECTL1, command_params, 2);

    /* Display function control */
    command_params[0] = 0x08;
    command_params[1] = 0x82;
    /* number of lines, see datasheet p. 166 (DISCTRL::NL) */
    command_params[2] = (params->lines >> 3) - 1;
    _write_cmd(dev, LCD_CMD_DFUNC, command_params, 3);

    /* Pixel format */
    command_params[0] = 0x55; /* 16 bit mode */
    _write_cmd(dev, LCD_CMD_PIXSET, command_params, 1);

    command_params[0] = 0x01;
    _write_cmd(dev, LCD_CMD_GAMSET, command_params, 1);

    /* Gamma correction */
    {
        static const uint8_t gamma_pos[] = {
            0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d,
            0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10
        };
        _write_cmd(dev, LCD_CMD_PGAMCTRL, gamma_pos,
                   sizeof(gamma_pos));
    }
    {
        static const uint8_t gamma_neg[] = {
            0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D,
            0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10
        };
        _write_cmd(dev, LCD_CMD_NGAMCTRL, gamma_neg,
                   sizeof(gamma_neg));
    }

    if (dev->params->inverted) {
        _write_cmd(dev, LCD_CMD_DINVON, NULL, 0);
    }
    /* Sleep out (turn off sleep mode) */
    _write_cmd(dev, LCD_CMD_SLPOUT, NULL, 0);

    /* Normal display mode on */
    _write_cmd(dev, LCD_CMD_NORON, NULL, 0);
    ztimer_sleep(ZTIMER_MSEC, 1);

    /* Display on */
    _write_cmd(dev, LCD_CMD_DISPON, NULL, 0);
    spi_release(dev->params->spi);

    return 0;
}

static void _set_area(const lcd_t *dev, uint16_t x1, uint16_t x2,
                      uint16_t y1, uint16_t y2)
{
    be_uint16_t params[2];

    x1 += dev->params->offset_x;
    x2 += dev->params->offset_x;
    y1 += dev->params->offset_y;
    y2 += dev->params->offset_y;

    params[0] = byteorder_htons(x1);
    params[1] = byteorder_htons(x2);

    _write_cmd(dev, LCD_CMD_CASET, (uint8_t *)params,
               sizeof(params));
    params[0] = byteorder_htons(y1);
    params[1] = byteorder_htons(y2);
    _write_cmd(dev, LCD_CMD_PASET, (uint8_t *)params,
               sizeof(params));
}

const lcd_driver_t lcd_st7735_driver = {
    .init = _init,
    .set_area = _set_area,
};
