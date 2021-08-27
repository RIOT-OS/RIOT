/*
 * Copyright (C) 2018 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ili9341
 * @{
 *
 * @file
 * @brief       Device driver implementation for the ili9341 display controller
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <assert.h>
#include <string.h>
#include "byteorder.h"
#include "periph/spi.h"
#include "xtimer.h"
#include "kernel_defines.h"

#include "ili9341.h"
#include "ili9341_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static void _ili9341_spi_acquire(const ili9341_t *dev)
{
    spi_acquire(dev->params->spi, dev->params->cs_pin, dev->params->spi_mode,
                dev->params->spi_clk);
}

static void _ili9341_cmd_start(const ili9341_t *dev, uint8_t cmd, bool cont)
{
    gpio_clear(dev->params->dcx_pin);
    spi_transfer_byte(dev->params->spi, dev->params->cs_pin, cont, cmd);
    gpio_set(dev->params->dcx_pin);
}

/* datasheet page 178, table converted to equation.
 * gvdd in 1mv increments: 4850 = 4.85V */
static uint8_t _ili9341_calc_pwrctl1(uint16_t gvdd)
{
    return (gvdd - 2850) / 50;
}

static uint8_t _ili9341_calc_vmh(uint16_t vcomh)
{
    return (vcomh - 2700) / 25;
}

static uint8_t _ili9341_calc_vml(int16_t vcoml)
{
    return (vcoml + 2500) / 25;
}

static void _write_cmd(const ili9341_t *dev, uint8_t cmd, const uint8_t *data,
                       size_t len)
{
    _ili9341_cmd_start(dev, cmd, len ? true : false);
    if (len) {
        spi_transfer_bytes(dev->params->spi, dev->params->cs_pin, false, data,
                           NULL, len);
    }
}

static void _ili9341_set_area(const ili9341_t *dev, uint16_t x1, uint16_t x2,
                              uint16_t y1, uint16_t y2)
{
    be_uint16_t params[2];

    params[0] = byteorder_htons(x1);
    params[1] = byteorder_htons(x2);
    _write_cmd(dev, ILI9341_CMD_CASET, (uint8_t *)params,
               sizeof(params));
    params[0] = byteorder_htons(y1);
    params[1] = byteorder_htons(y2);
    _write_cmd(dev, ILI9341_CMD_PASET, (uint8_t *)params,
               sizeof(params));
}

int ili9341_init(ili9341_t *dev, const ili9341_params_t *params)
{
    assert(params->lines >= 16 && params->lines <= 320 && !(params->lines & 0x7));
    dev->params = params;
    uint8_t command_params[4] = { 0 };
    gpio_init(dev->params->dcx_pin, GPIO_OUT);
    int res = spi_init_cs(dev->params->spi, dev->params->cs_pin);
    if (res != SPI_OK) {
        DEBUG("[ili9341] init: error initializing the CS pin [%i]\n", res);
        return -1;
    }

    if (gpio_is_valid(dev->params->rst_pin)) {
        gpio_init(dev->params->rst_pin, GPIO_OUT);
        gpio_clear(dev->params->rst_pin);
        xtimer_msleep(120);
        gpio_set(dev->params->rst_pin);
    }
    xtimer_msleep(120);

    /* Acquire once at release at the end */
    _ili9341_spi_acquire(dev);
    /* Soft Reset */
    _write_cmd(dev, ILI9341_CMD_SWRESET, NULL, 0);
    xtimer_msleep(120);

    /* Display off */
    _write_cmd(dev, ILI9341_CMD_DISPOFF, NULL, 0);

    /* PWRCTL1/2 */
    command_params[0] = _ili9341_calc_pwrctl1(CONFIG_ILI9341_GVDD);
    _write_cmd(dev, ILI9341_CMD_PWCTRL1, command_params, 1);

    command_params[0] = 0x10; /* PWRCTL 0 0 0 */
    _write_cmd(dev, ILI9341_CMD_PWCTRL2, command_params, 1);

    /* VCOMCTL */
    command_params[0] = _ili9341_calc_vmh(CONFIG_ILI9341_VCOMH);
    command_params[1] = _ili9341_calc_vml(CONFIG_ILI9341_VCOML);
    _write_cmd(dev, ILI9341_CMD_VMCTRL1, command_params, 2);

    command_params[0] = 0x86;
    _write_cmd(dev, ILI9341_CMD_VMCTRL2, command_params, 1);

    /* Memory access CTL */
    command_params[0] = dev->params->rotation;
    command_params[0] |= dev->params->rgb ? 0 : ILI9341_MADCTL_BGR;
    _write_cmd(dev, ILI9341_CMD_MADCTL, command_params, 1);

    /* Frame control */
    command_params[0] = 0x00;
    command_params[1] = 0x18;
    _write_cmd(dev, ILI9341_CMD_FRAMECTL1, command_params, 2);

    /* Display function control */
    command_params[0] = 0x08;
    command_params[1] = 0x82;
    /* number of lines, see datasheet p. 166 (DISCTRL::NL) */
    command_params[2] = (params->lines >> 3) - 1;
    _write_cmd(dev, ILI9341_CMD_DFUNC, command_params, 3);

    /* Pixel format */
    command_params[0] = 0x55; /* 16 bit mode */
    _write_cmd(dev, ILI9341_CMD_PIXSET, command_params, 1);

    command_params[0] = 0x01;
    _write_cmd(dev, ILI9341_CMD_GAMSET, command_params, 1);

    /* Gamma correction */
    {
        static const uint8_t gamma_pos[] = {
            0x0F,
            0x31,
            0x2B,
            0x0C,
            0x0E,
            0x08,
            0x4E,
            0xF1,
            0x37,
            0x07,
            0x10,
            0x03,
            0x0E,
            0x09,
            0x00
        };
        _write_cmd(dev, ILI9341_CMD_PGAMCTRL, gamma_pos,
                   sizeof(gamma_pos));
    }
    {
        static const uint8_t gamma_neg[] = {
            0x00,
            0x0E,
            0x14,
            0x03,
            0x11,
            0x07,
            0x31,
            0xC1,
            0x48,
            0x08,
            0x0F,
            0x0C,
            0x31,
            0x36,
            0x0F
        };
        _write_cmd(dev, ILI9341_CMD_NGAMCTRL, gamma_neg,
                   sizeof(gamma_neg));

    }

    if (dev->params->inverted) {
        _write_cmd(dev, ILI9341_CMD_DINVON, NULL, 0);
    }
    /* Sleep out (turn off sleep mode) */
    _write_cmd(dev, ILI9341_CMD_SLPOUT, NULL, 0);
    /* Display on */
    _write_cmd(dev, ILI9341_CMD_DISPON, NULL, 0);
    spi_release(dev->params->spi);
    return 0;
}

void ili9341_write_cmd(const ili9341_t *dev, uint8_t cmd, const uint8_t *data,
                       size_t len)
{
    _ili9341_spi_acquire(dev);
    _write_cmd(dev, cmd, data, len);
    spi_release(dev->params->spi);
}

void ili9341_read_cmd(const ili9341_t *dev, uint8_t cmd, uint8_t *data, size_t len)
{
    assert(len);
    _ili9341_spi_acquire(dev);
    _ili9341_cmd_start(dev, cmd, true);
    /* Dummy transfer */
    spi_transfer_byte(dev->params->spi, dev->params->cs_pin, true, 0x00);
    spi_transfer_bytes(dev->params->spi, dev->params->cs_pin, false, NULL,
                       data, len);
    spi_release(dev->params->spi);
}

void ili9341_fill(const ili9341_t *dev, uint16_t x1, uint16_t x2, uint16_t y1,
                  uint16_t y2, uint16_t color)
{
    /* Send fill area to the display */

    /* Calculate number of pixels */
    int32_t num_pix = (x2 - x1 + 1) * (y2 - y1 + 1);

    DEBUG("[ili9341]: Write x1: %" PRIu16 ", x2: %" PRIu16 ", "
          "y1: %" PRIu16 ", y2: %" PRIu16 ". Num pixels: %lu\n",
          x1, x2, y1, y2, (unsigned long)num_pix);

    /* Send fill area to the display */
    _ili9341_spi_acquire(dev);

    _ili9341_set_area(dev, x1, x2, y1, y2);
    /* Memory access command */
    _ili9341_cmd_start(dev, ILI9341_CMD_RAMWR, true);

    if (IS_ACTIVE(CONFIG_ILI9341_LE_MODE)) {
        color = htons(color);
    }

    for (int i = 0; i < (num_pix - 1); i++) {
        spi_transfer_bytes(dev->params->spi, dev->params->cs_pin, true,
                           (uint8_t *)&color, NULL, sizeof(color));
    }
    spi_transfer_bytes(dev->params->spi, dev->params->cs_pin, false,
                       (uint8_t *)&color, NULL, sizeof(color));
    spi_release(dev->params->spi);
}

void ili9341_pixmap(const ili9341_t *dev, uint16_t x1, uint16_t x2,
                    uint16_t y1, uint16_t y2, const uint16_t *color)
{
    size_t num_pix = (x2 - x1 + 1) * (y2 - y1 + 1);

    DEBUG("[ili9341]: Write x1: %" PRIu16 ", x2: %" PRIu16 ", "
          "y1: %" PRIu16 ", y2: %" PRIu16 ". Num pixels: %lu\n",
          x1, x2, y1, y2, (unsigned long)num_pix);

    _ili9341_spi_acquire(dev);

    /* Send fill area to the display */
    _ili9341_set_area(dev, x1, x2, y1, y2);

    /* Memory access command */
    _ili9341_cmd_start(dev, ILI9341_CMD_RAMWR, true);

    if (IS_ACTIVE(CONFIG_ILI9341_LE_MODE)) {
        for (size_t i = 0; i < num_pix - 1; i++) {
            uint16_t ncolor = htons(*(color + i));
            spi_transfer_bytes(dev->params->spi, dev->params->cs_pin, true,
                               &ncolor, NULL, sizeof(uint16_t));
        }
        uint16_t ncolor = htons(*(color + num_pix - 1));
        spi_transfer_bytes(dev->params->spi, dev->params->cs_pin, false,
                           &ncolor, NULL, sizeof(uint16_t));
    }
    else {
        spi_transfer_bytes(dev->params->spi, dev->params->cs_pin, false,
                           (const uint8_t *)color, NULL, num_pix * 2);
    }

    spi_release(dev->params->spi);
}

void ili9341_invert_on(const ili9341_t *dev)
{
    uint8_t command = (dev->params->inverted) ? ILI9341_CMD_DINVOFF
                                              : ILI9341_CMD_DINVON;

    ili9341_write_cmd(dev, command, NULL, 0);
}

void ili9341_invert_off(const ili9341_t *dev)
{
    uint8_t command = (dev->params->inverted) ? ILI9341_CMD_DINVON
                                              : ILI9341_CMD_DINVOFF;

    ili9341_write_cmd(dev, command, NULL, 0);
}

void ili9341_set_brightness(const ili9341_t *dev, uint8_t brightness)
{
    ili9341_write_cmd(dev, ILI9341_CMD_WRDISBV, &brightness, 1);
    uint8_t param = 0x26;
    ili9341_write_cmd(dev, ILI9341_CMD_WRCTRLD, &param, 1);
}
