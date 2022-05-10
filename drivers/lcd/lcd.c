/*
 * Copyright (C) 2018 Koen Zandberg
 *               2021 Francisco Molina
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_lcd
 * @{
 *
 * @file
 * @brief       Device driver implementation for the lcd display controller
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
#include "kernel_defines.h"

#include "lcd.h"
#include "lcd_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static void _lcd_spi_acquire(const lcd_t *dev)
{
    spi_acquire(dev->params->spi, dev->params->cs_pin, dev->params->spi_mode,
                dev->params->spi_clk);
}

static void _lcd_cmd_start(const lcd_t *dev, uint8_t cmd, bool cont)
{
    gpio_clear(dev->params->dcx_pin);
    spi_transfer_byte(dev->params->spi, dev->params->cs_pin, cont, cmd);
    gpio_set(dev->params->dcx_pin);
}

static void _write_cmd(const lcd_t *dev, uint8_t cmd, const uint8_t *data,
                       size_t len)
{
    _lcd_cmd_start(dev, cmd, len ? true : false);
    if (len) {
        spi_transfer_bytes(dev->params->spi, dev->params->cs_pin, false, data,
                           NULL, len);
    }
}

static void _lcd_set_area(const lcd_t *dev, uint16_t x1, uint16_t x2,
                              uint16_t y1, uint16_t y2)
{
    assert(dev->driver->set_area);
    dev->driver->set_area(dev, x1, x2, y1, y2);
}

int lcd_init(lcd_t *dev, const lcd_params_t *params)
{
    if (dev->driver->init) {
        return dev->driver->init(dev, params);
    }
    else {
        return -ENOTSUP;
    }
}

void lcd_write_cmd(const lcd_t *dev, uint8_t cmd, const uint8_t *data,
                       size_t len)
{
    _lcd_spi_acquire(dev);
    _write_cmd(dev, cmd, data, len);
    spi_release(dev->params->spi);
}

void lcd_read_cmd(const lcd_t *dev, uint8_t cmd, uint8_t *data, size_t len)
{
    assert(len);
    _lcd_spi_acquire(dev);
    _lcd_cmd_start(dev, cmd, true);
    /* Dummy transfer */
    spi_transfer_byte(dev->params->spi, dev->params->cs_pin, true, 0x00);
    spi_transfer_bytes(dev->params->spi, dev->params->cs_pin, false, NULL,
                       data, len);
    spi_release(dev->params->spi);
}


void lcd_fill(const lcd_t *dev, uint16_t x1, uint16_t x2, uint16_t y1,
                  uint16_t y2, uint16_t color)
{
    /* Send fill area to the display */

    /* Calculate number of pixels */
    int32_t num_pix = (x2 - x1 + 1) * (y2 - y1 + 1);

    DEBUG("[lcd]: Write x1: %" PRIu16 ", x2: %" PRIu16 ", "
          "y1: %" PRIu16 ", y2: %" PRIu16 ". Num pixels: %lu\n",
          x1, x2, y1, y2, (unsigned long)num_pix);

    /* Send fill area to the display */
    _lcd_spi_acquire(dev);

    _lcd_set_area(dev, x1, x2, y1, y2);
    /* Memory access command */
    _lcd_cmd_start(dev, LCD_CMD_RAMWR, true);

    if (IS_ACTIVE(CONFIG_LCD_LE_MODE)) {
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

void lcd_pixmap(const lcd_t *dev, uint16_t x1, uint16_t x2,
                    uint16_t y1, uint16_t y2, const uint16_t *color)
{
    size_t num_pix = (x2 - x1 + 1) * (y2 - y1 + 1);

    DEBUG("[lcd]: Write x1: %" PRIu16 ", x2: %" PRIu16 ", "
          "y1: %" PRIu16 ", y2: %" PRIu16 ". Num pixels: %lu\n",
          x1, x2, y1, y2, (unsigned long)num_pix);

    _lcd_spi_acquire(dev);

    /* Send fill area to the display */
    _lcd_set_area(dev, x1, x2, y1, y2);

    /* Memory access command */
    _lcd_cmd_start(dev, LCD_CMD_RAMWR, true);

    if (IS_ACTIVE(CONFIG_LCD_LE_MODE)) {
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

void lcd_invert_on(const lcd_t *dev)
{
    uint8_t command = (dev->params->inverted) ? LCD_CMD_DINVOFF
                                              : LCD_CMD_DINVON;

    lcd_write_cmd(dev, command, NULL, 0);
}

void lcd_invert_off(const lcd_t *dev)
{
    uint8_t command = (dev->params->inverted) ? LCD_CMD_DINVON
                                              : LCD_CMD_DINVOFF;

    lcd_write_cmd(dev, command, NULL, 0);
}

void lcd_set_brightness(const lcd_t *dev, uint8_t brightness)
{
    lcd_write_cmd(dev, LCD_CMD_WRDISBV, &brightness, 1);
    uint8_t param = 0x26;
    lcd_write_cmd(dev, LCD_CMD_WRCTRLD, &param, 1);
}
