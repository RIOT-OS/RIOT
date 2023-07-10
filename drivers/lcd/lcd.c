/*
 * Copyright (C) 2018 Koen Zandberg
 *               2021 Francisco Molina
 *               2023 Gunar Schorcht
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
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include <assert.h>
#include <string.h>
#include "byteorder.h"
#include "periph/spi.h"
#include "kernel_defines.h"
#include "ztimer.h"

#include "lcd.h"
#include "lcd_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static inline void _lcd_write_byte(const lcd_t *dev, bool cont, uint8_t data)
{
    if (dev->params->spi != SPI_UNDEF) {
        /* SPI serial interface is used */
        spi_transfer_byte(dev->params->spi, dev->params->cs_pin, cont, data);
    }
    else {
        assert(false);
    }
}

static inline void _lcd_write_bytes(const lcd_t *dev, bool cont,
                                    const void *data, size_t len)
{
    if (dev->params->spi != SPI_UNDEF) {
        /* SPI serial interface is used */
        spi_transfer_bytes(dev->params->spi,
                           dev->params->cs_pin, cont, data, NULL, len);
    }
    else {
        assert(false);
    }
}

static inline void _lcd_read_bytes(const lcd_t *dev, bool cont,
                                   void *data, size_t len)
{
    if (dev->params->spi != SPI_UNDEF) {
        /* SPI serial interface is used */
        /* Dummy read */
        spi_transfer_byte(dev->params->spi,
                          dev->params->cs_pin, true, 0x00);
        spi_transfer_bytes(dev->params->spi,
                           dev->params->cs_pin, cont, NULL, data, len);
    }
    else {
        assert(false);
    }
}

static void _lcd_cmd_start(const lcd_t *dev, uint8_t cmd, bool cont)
{
    gpio_clear(dev->params->dcx_pin);
    _lcd_write_byte(dev, cont, cmd);
    gpio_set(dev->params->dcx_pin);
}

static void _lcd_set_area_default(const lcd_t *dev, uint16_t x1, uint16_t x2,
                                  uint16_t y1, uint16_t y2)
{
    be_uint16_t params[2];

    x1 += dev->params->offset_x;
    x2 += dev->params->offset_x;
    y1 += dev->params->offset_y;
    y2 += dev->params->offset_y;

    /* Function is called by a high level function of the LCD driver where
     * the device is already acquired. So we don't must acquire it here.
     * Therefore the low level write command function is called. */

    params[0] = byteorder_htons(x1);
    params[1] = byteorder_htons(x2);
    lcd_ll_write_cmd(dev, LCD_CMD_CASET, (uint8_t *)params,
                     sizeof(params));
    params[0] = byteorder_htons(y1);
    params[1] = byteorder_htons(y2);
    lcd_ll_write_cmd(dev, LCD_CMD_PASET, (uint8_t *)params,
                     sizeof(params));
}

static void _lcd_set_area(const lcd_t *dev, uint16_t x1, uint16_t x2,
                          uint16_t y1, uint16_t y2)
{
    if (dev->driver->set_area) {
        dev->driver->set_area(dev, x1, x2, y1, y2);
    }
    else {
        _lcd_set_area_default(dev, x1, x2, y1, y2);
    }
}

void lcd_ll_acquire(const lcd_t *dev)
{
    if (dev->params->spi != SPI_UNDEF) {
        /* SPI serial interface is used */
        spi_acquire(dev->params->spi, dev->params->cs_pin,
                    dev->params->spi_mode, dev->params->spi_clk);
    }
    else {
        assert(false);
    }
}

void lcd_ll_release(const lcd_t *dev)
{
    if (dev->params->spi != SPI_UNDEF) {
        /* SPI serial interface is used */
        spi_release(dev->params->spi);
    }
    else {
        assert(false);
    }
}

void lcd_ll_write_cmd(const lcd_t *dev, uint8_t cmd, const uint8_t *data,
                      size_t len)
{
    _lcd_cmd_start(dev, cmd, len ? true : false);
    if (len) {
        _lcd_write_bytes(dev, false, data, len);
    }
}

void lcd_ll_read_cmd(const lcd_t *dev, uint8_t cmd, uint8_t *data, size_t len)
{
    assert(len);
    _lcd_cmd_start(dev, cmd, len ? true : false);
    _lcd_read_bytes(dev, false, data, len);
}

int lcd_init(lcd_t *dev, const lcd_params_t *params)
{
    dev->params = params;

    assert(gpio_is_valid(dev->params->dcx_pin));
    gpio_init(dev->params->dcx_pin, GPIO_OUT);

    if (dev->params->spi != SPI_UNDEF) {
        /* SPI serial interface is used */
        int res = spi_init_cs(dev->params->spi, dev->params->cs_pin);

        if (res != SPI_OK) {
            DEBUG("[st7735] init: error initializing the CS pin [%i]\n", res);
            return -1;
        }
    }
    else {
       assert(false);
    }

    if (gpio_is_valid(dev->params->rst_pin)) {
        gpio_init(dev->params->rst_pin, GPIO_OUT);
        gpio_clear(dev->params->rst_pin);
        ztimer_sleep(ZTIMER_MSEC, 120);
        gpio_set(dev->params->rst_pin);
    }
    ztimer_sleep(ZTIMER_MSEC, 120);

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
    lcd_ll_acquire(dev);
    lcd_ll_write_cmd(dev, cmd, data, len);
    lcd_ll_release(dev);
}

void lcd_read_cmd(const lcd_t *dev, uint8_t cmd, uint8_t *data, size_t len)
{
    lcd_ll_acquire(dev);
    lcd_ll_read_cmd(dev, cmd, data, len);
    lcd_ll_release(dev);
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
    lcd_ll_acquire(dev);

    _lcd_set_area(dev, x1, x2, y1, y2);
    /* Memory access command */
    _lcd_cmd_start(dev, LCD_CMD_RAMWR, true);

    if (IS_ACTIVE(CONFIG_LCD_LE_MODE)) {
        color = htons(color);
    }

    for (int i = 0; i < (num_pix - 1); i++) {
        _lcd_write_bytes(dev, true, (uint8_t *)&color, sizeof(color));
    }
    _lcd_write_bytes(dev, false, (uint8_t *)&color, sizeof(color));
    lcd_ll_release(dev);
}

void lcd_pixmap(const lcd_t *dev, uint16_t x1, uint16_t x2,
                uint16_t y1, uint16_t y2, const uint16_t *color)
{
    size_t num_pix = (x2 - x1 + 1) * (y2 - y1 + 1);

    DEBUG("[lcd]: Write x1: %" PRIu16 ", x2: %" PRIu16 ", "
          "y1: %" PRIu16 ", y2: %" PRIu16 ". Num pixels: %lu\n",
          x1, x2, y1, y2, (unsigned long)num_pix);

    lcd_ll_acquire(dev);

    /* Send fill area to the display */
    _lcd_set_area(dev, x1, x2, y1, y2);

    /* Memory access command */
    _lcd_cmd_start(dev, LCD_CMD_RAMWR, true);

    if (IS_ACTIVE(CONFIG_LCD_LE_MODE)) {
        for (size_t i = 0; i < num_pix - 1; i++) {
            uint16_t ncolor = htons(*(color + i));
            _lcd_write_bytes(dev, true, &ncolor, sizeof(uint16_t));
        }
        uint16_t ncolor = htons(*(color + num_pix - 1));
        _lcd_write_bytes(dev, false, &ncolor, sizeof(uint16_t));
    }
    else {
        _lcd_write_bytes(dev, false, (const uint8_t *)color, num_pix * 2);
    }

    lcd_ll_release(dev);
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
