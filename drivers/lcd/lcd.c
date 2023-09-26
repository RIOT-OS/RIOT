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
#include "kernel_defines.h"
#include "ztimer.h"

#if IS_USED(MODULE_LCD_SPI)
#include "periph/spi.h"
#endif

#include "lcd.h"
#include "lcd_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#if IS_USED(MODULE_LCD_PARALLEL)

static void lcd_ll_par_write_byte(lcd_t *dev, bool cont, uint8_t out)
{
    if (gpio_is_valid(dev->params->cs_pin)) {
        gpio_clear(dev->params->cs_pin);
    }

    gpio_clear(dev->params->wrx_pin);

    gpio_write(dev->params->d0_pin, out & 0x01);
    gpio_write(dev->params->d1_pin, out & 0x02);
    gpio_write(dev->params->d2_pin, out & 0x04);
    gpio_write(dev->params->d3_pin, out & 0x08);
    gpio_write(dev->params->d4_pin, out & 0x10);
    gpio_write(dev->params->d5_pin, out & 0x20);
    gpio_write(dev->params->d6_pin, out & 0x40);
    gpio_write(dev->params->d7_pin, out & 0x80);

    gpio_set(dev->params->wrx_pin);

    if (gpio_is_valid(dev->params->cs_pin) && !cont) {
        gpio_set(dev->params->cs_pin);
    };
}

static uint8_t lcd_ll_par_read_byte(lcd_t *dev, bool cont)
{
    uint8_t in = 0;

    if (gpio_is_valid(dev->params->cs_pin)) {
        gpio_clear(dev->params->cs_pin);
    }

    gpio_clear(dev->params->rdx_pin);

    in |= gpio_read(dev->params->d0_pin) ? 0x01 : 0;
    in |= gpio_read(dev->params->d1_pin) ? 0x02 : 0;
    in |= gpio_read(dev->params->d2_pin) ? 0x04 : 0;
    in |= gpio_read(dev->params->d3_pin) ? 0x08 : 0;
    in |= gpio_read(dev->params->d4_pin) ? 0x10 : 0;
    in |= gpio_read(dev->params->d5_pin) ? 0x20 : 0;
    in |= gpio_read(dev->params->d6_pin) ? 0x40 : 0;
    in |= gpio_read(dev->params->d7_pin) ? 0x80 : 0;

    gpio_set(dev->params->rdx_pin);

    if (gpio_is_valid(dev->params->cs_pin) && !cont) {
        gpio_set(dev->params->cs_pin);
    };

    return in;
}

#if IS_USED(MODULE_LCD_PARALLEL_16BIT)

static void lcd_ll_par_write_word(lcd_t *dev, bool cont, uint16_t out)
{
    if (gpio_is_valid(dev->params->cs_pin)) {
        gpio_clear(dev->params->cs_pin);
    }

    gpio_clear(dev->params->wrx_pin);

    gpio_write(dev->params->d0_pin, out & 0x0001);
    gpio_write(dev->params->d1_pin, out & 0x0002);
    gpio_write(dev->params->d2_pin, out & 0x0004);
    gpio_write(dev->params->d3_pin, out & 0x0008);
    gpio_write(dev->params->d4_pin, out & 0x0010);
    gpio_write(dev->params->d5_pin, out & 0x0020);
    gpio_write(dev->params->d6_pin, out & 0x0040);
    gpio_write(dev->params->d7_pin, out & 0x0080);
    gpio_write(dev->params->d8_pin, out & 0x0100);
    gpio_write(dev->params->d9_pin, out & 0x0200);
    gpio_write(dev->params->d10_pin, out & 0x0400);
    gpio_write(dev->params->d11_pin, out & 0x0800);
    gpio_write(dev->params->d12_pin, out & 0x1000);
    gpio_write(dev->params->d13_pin, out & 0x2000);
    gpio_write(dev->params->d14_pin, out & 0x4000);
    gpio_write(dev->params->d15_pin, out & 0x8000);

    gpio_set(dev->params->wrx_pin);

    if (gpio_is_valid(dev->params->cs_pin) && !cont) {
        gpio_set(dev->params->cs_pin);
    };
}

static uint16_t lcd_ll_par_read_word(lcd_t *dev, bool cont)
{
    uint16_t in = 0;

    if (gpio_is_valid(dev->params->cs_pin)) {
        gpio_clear(dev->params->cs_pin);
    }

    gpio_clear(dev->params->rdx_pin);

    in |= gpio_read(dev->params->d0_pin) ? 0x0001 : 0;
    in |= gpio_read(dev->params->d1_pin) ? 0x0002 : 0;
    in |= gpio_read(dev->params->d2_pin) ? 0x0004 : 0;
    in |= gpio_read(dev->params->d3_pin) ? 0x0008 : 0;
    in |= gpio_read(dev->params->d4_pin) ? 0x0010 : 0;
    in |= gpio_read(dev->params->d5_pin) ? 0x0020 : 0;
    in |= gpio_read(dev->params->d6_pin) ? 0x0040 : 0;
    in |= gpio_read(dev->params->d7_pin) ? 0x0080 : 0;
    in |= gpio_read(dev->params->d8_pin) ? 0x01000 : 0;
    in |= gpio_read(dev->params->d9_pin) ? 0x02000 : 0;
    in |= gpio_read(dev->params->d10_pin) ? 0x0400 : 0;
    in |= gpio_read(dev->params->d11_pin) ? 0x0800 : 0;
    in |= gpio_read(dev->params->d12_pin) ? 0x1000 : 0;
    in |= gpio_read(dev->params->d13_pin) ? 0x2000 : 0;
    in |= gpio_read(dev->params->d14_pin) ? 0x4000 : 0;
    in |= gpio_read(dev->params->d15_pin) ? 0x8000 : 0;

    gpio_set(dev->params->rdx_pin);

    if (gpio_is_valid(dev->params->cs_pin) && !cont) {
        gpio_set(dev->params->cs_pin);
    };

    return in;
}
#endif /* IS_USED(MODULE_LCD_PARALLEL_16BIT) */

static void lcd_ll_par_write_bytes(lcd_t *dev, bool cont,
                                   const void *data, size_t len)
{
    assert(len);

    const uint8_t *data_out = data;

#if IS_USED(MODULE_LCD_PARALLEL_16BIT)
    if (dev->word_access) {
        /* len has to be a multiple of two for word access */
        assert((len % 2) == 0);
        for (size_t i = 0; i < len; i += 2) {
            /* data[i] is the high byte and data[i+1] is the low byte in BE */
            uint16_t out_word = (data_out[i] << 8) + data_out[i + 1];
            lcd_ll_par_write_word(dev, i == (len - 2) ? cont : true, out_word);
        }
        return;
    }
#endif

    for (size_t i = 0; i < len; i++) {
        lcd_ll_par_write_byte(dev, i == (len - 1) ? cont : true, data_out[i]);
    }
}

static void lcd_ll_par_read_bytes(lcd_t *dev, bool cont,
                                  void *data, size_t len)
{
    assert(len);

    uint8_t *data_in = data;

#if IS_USED(MODULE_LCD_PARALLEL_16BIT)
    if (dev->word_access) {
        /* len has to be a multiple of two for word access */
        assert((len % 2) == 0);
        for (size_t i = 0; i < len; i += 2) {
            uint16_t in_word = lcd_ll_par_read_word(dev, i == (len - 2) ? cont : true);
            data_in[i] = in_word >> 8;       /* data[i] is the high byte in BE */
            data_in[i + 1] = in_word & 0xff; /* data[i+1] is the low byte in BE */
        }
        return;
    }
#endif

    for (size_t i = 0; i < len; i++) {
        data_in[i] = lcd_ll_par_read_byte(dev, i == (len - 1) ? cont : true);
    }
}

#endif /* IS_USED(MODULE_LCD_PARALLEL) */

static inline void lcd_ll_write_byte(lcd_t *dev, bool cont, uint8_t data)
{
#if IS_USED(MODULE_LCD_SPI)
    if (dev->params->spi != SPI_UNDEF) {
        /* SPI serial interface is used */
        spi_transfer_byte(dev->params->spi, dev->params->cs_pin, cont, data);
    }
    else {
#endif
#if IS_USED(MODULE_LCD_PARALLEL)
        /* MCU 8080 8-/16-bit parallel interface is used */
        lcd_ll_par_write_byte(dev, cont, data);
#else
        assert(false);
#endif
#if IS_USED(MODULE_LCD_SPI)
    }
#endif
}

static inline void lcd_ll_write_bytes(lcd_t *dev, bool cont,
                                      const void *data, size_t len)
{
#if IS_USED(MODULE_LCD_SPI)
    if (dev->params->spi != SPI_UNDEF) {
        /* SPI serial interface is used */
        spi_transfer_bytes(dev->params->spi,
                           dev->params->cs_pin, cont, data, NULL, len);
    }
    else {
#endif
#if IS_USED(MODULE_LCD_PARALLEL)
        /* MCU 8080 8-/16-bit parallel interface is used */
        lcd_ll_par_write_bytes(dev, cont, data, len);
#else
        assert(false);
#endif
#if IS_USED(MODULE_LCD_SPI)
    }
#endif
}

static inline void lcd_ll_read_bytes(lcd_t *dev, bool cont,
                                     void *data, size_t len)
{
#if IS_USED(MODULE_LCD_SPI)
    if (dev->params->spi != SPI_UNDEF) {
        /* SPI serial interface is used */
        /* Dummy read */
        spi_transfer_byte(dev->params->spi,
                          dev->params->cs_pin, true, 0x00);
        spi_transfer_bytes(dev->params->spi,
                           dev->params->cs_pin, cont, NULL, data, len);
    }
    else {
#endif
#if IS_USED(MODULE_LCD_PARALLEL)
        /* MCU 8080 8-/16-bit parallel interface is used */

        /* switch GPIO mode to input */
        gpio_init(dev->params->d0_pin, GPIO_IN);
        gpio_init(dev->params->d1_pin, GPIO_IN);
        gpio_init(dev->params->d2_pin, GPIO_IN);
        gpio_init(dev->params->d3_pin, GPIO_IN);
        gpio_init(dev->params->d4_pin, GPIO_IN);
        gpio_init(dev->params->d5_pin, GPIO_IN);
        gpio_init(dev->params->d6_pin, GPIO_IN);
        gpio_init(dev->params->d7_pin, GPIO_IN);
#if IS_USED(MODULE_LCD_PARALLEL_16BIT)
        if (dev->params->mode == LCD_IF_PARALLEL_16BIT) {
            gpio_init(dev->params->d8_pin, GPIO_IN);
            gpio_init(dev->params->d9_pin, GPIO_IN);
            gpio_init(dev->params->d10_pin, GPIO_IN);
            gpio_init(dev->params->d11_pin, GPIO_IN);
            gpio_init(dev->params->d12_pin, GPIO_IN);
            gpio_init(dev->params->d13_pin, GPIO_IN);
            gpio_init(dev->params->d14_pin, GPIO_IN);
            gpio_init(dev->params->d15_pin, GPIO_IN);
        }
#endif /* IS_USED(MODULE_LCD_PARALLEL_16BIT) */

        /* Dummy read */
        lcd_ll_par_read_byte(dev, true);
        lcd_ll_par_read_bytes(dev, cont, data, len);

        /* switch GPIO mode back to output */
        gpio_init(dev->params->d0_pin, GPIO_OUT);
        gpio_init(dev->params->d1_pin, GPIO_OUT);
        gpio_init(dev->params->d2_pin, GPIO_OUT);
        gpio_init(dev->params->d3_pin, GPIO_OUT);
        gpio_init(dev->params->d4_pin, GPIO_OUT);
        gpio_init(dev->params->d5_pin, GPIO_OUT);
        gpio_init(dev->params->d6_pin, GPIO_OUT);
        gpio_init(dev->params->d7_pin, GPIO_OUT);
#if IS_USED(MODULE_LCD_PARALLEL_16BIT)
        if (dev->params->mode == LCD_IF_PARALLEL_16BIT) {
            gpio_init(dev->params->d8_pin, GPIO_OUT);
            gpio_init(dev->params->d9_pin, GPIO_OUT);
            gpio_init(dev->params->d10_pin, GPIO_OUT);
            gpio_init(dev->params->d11_pin, GPIO_OUT);
            gpio_init(dev->params->d12_pin, GPIO_OUT);
            gpio_init(dev->params->d13_pin, GPIO_OUT);
            gpio_init(dev->params->d14_pin, GPIO_OUT);
            gpio_init(dev->params->d15_pin, GPIO_OUT);
        }
#endif /* IS_USED(MODULE_LCD_PARALLEL_16BIT) */
#else
        assert(false);
#endif
#if IS_USED(MODULE_LCD_SPI)
    }
#endif
}

static void lcd_ll_cmd_start(lcd_t *dev, uint8_t cmd, bool cont)
{
    gpio_clear(dev->params->dcx_pin);
    lcd_ll_write_byte(dev, cont, cmd);
    gpio_set(dev->params->dcx_pin);

#if IS_USED(MODULE_LCD_PARALLEL_16BIT)
    /* only the RAMRD and RAMRDC commands use 16-bit data access */
    if (((cmd == LCD_CMD_RAMWR) || (cmd == LCD_CMD_RAMWRC) ||
         (cmd == LCD_CMD_RAMRD) || (cmd == LCD_CMD_RAMRDC)) &&
        (dev->params->mode == LCD_IF_PARALLEL_16BIT)) {
        dev->word_access = true;
    }
#endif
}

static void lcd_ll_set_area_default(lcd_t *dev, uint16_t x1, uint16_t x2,
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

static void lcd_ll_set_area(lcd_t *dev, uint16_t x1, uint16_t x2,
                            uint16_t y1, uint16_t y2)
{
    if (dev->driver->set_area) {
        dev->driver->set_area(dev, x1, x2, y1, y2);
    }
    else {
        lcd_ll_set_area_default(dev, x1, x2, y1, y2);
    }
}

void lcd_ll_acquire(lcd_t *dev)
{
#if IS_USED(MODULE_LCD_SPI)
    if (dev->params->spi != SPI_UNDEF) {
        /* SPI serial interface is used */
        spi_acquire(dev->params->spi, dev->params->cs_pin,
                    dev->params->spi_mode, dev->params->spi_clk);
    }
    else {
#endif
#if IS_USED(MODULE_LCD_PARALLEL)
        /* MCU 8080 8-/16-bit parallel interface is used */
        mutex_lock(&dev->lock);
#else
        assert(false);
#endif
#if IS_USED(MODULE_LCD_SPI)
    }
#endif
}

void lcd_ll_release(lcd_t *dev)
{
#if IS_USED(MODULE_LCD_PARALLEL_16BIT)
    /* reset word to byte access */
    dev->word_access = false;
#endif

#if IS_USED(MODULE_LCD_SPI)
    if (dev->params->spi != SPI_UNDEF) {
        /* SPI serial interface is used */
        spi_release(dev->params->spi);
    }
    else {
#endif
#if IS_USED(MODULE_LCD_PARALLEL)
        /* MCU 8080 8-/16-bit parallel interface is used */
        mutex_unlock(&dev->lock);
#else
        assert(false);
#endif
#if IS_USED(MODULE_LCD_SPI)
    }
#endif
}

void lcd_ll_write_cmd(lcd_t *dev, uint8_t cmd, const uint8_t *data,
                      size_t len)
{
    DEBUG("[%s] command 0x%02x (%u) ", __func__, cmd, len);
    if (IS_USED(ENABLE_DEBUG) && len) {
        for (uint8_t i = 0; i < len; i++) {
             DEBUG("0x%02x ", data[i]);
        }
    }
    DEBUG("\n");

    lcd_ll_cmd_start(dev, cmd, len ? true : false);
    if (len) {
        lcd_ll_write_bytes(dev, false, data, len);
    }
}

void lcd_ll_read_cmd(lcd_t *dev, uint8_t cmd, uint8_t *data, size_t len)
{
    assert(len);

    DEBUG("[%s] command 0x%02x (%u) ", __func__, cmd, len);

    lcd_ll_cmd_start(dev, cmd, true);
    lcd_ll_read_bytes(dev, false, data, len);

    if (IS_USED(ENABLE_DEBUG) && len) {
        for (uint8_t i = 0; i < len; i++) {
             DEBUG("0x%02x ", data[i]);
        }
    }
    DEBUG("\n");
}

int lcd_init(lcd_t *dev, const lcd_params_t *params)
{
    dev->params = params;

    assert(gpio_is_valid(dev->params->dcx_pin));
    gpio_init(dev->params->dcx_pin, GPIO_OUT);

#if IS_USED(MODULE_LCD_SPI)
    if (dev->params->spi != SPI_UNDEF) {
        /* SPI serial interface is used */
        int res = spi_init_cs(dev->params->spi, dev->params->cs_pin);

        if (res != SPI_OK) {
            DEBUG("[st7735] init: error initializing the CS pin [%i]\n", res);
            return -1;
        }
    }
    else {
#endif
#if IS_USED(MODULE_LCD_PARALLEL)
        /* MCU 8080 8-/16-bit parallel interface is used */
        if (gpio_is_valid(dev->params->cs_pin)) {
            gpio_init(dev->params->cs_pin, GPIO_OUT);
            gpio_set(dev->params->cs_pin);
        }

        assert(gpio_is_valid(dev->params->wrx_pin));
        gpio_init(dev->params->wrx_pin, GPIO_OUT);
        gpio_set(dev->params->wrx_pin);

        if (gpio_is_valid(dev->params->rdx_pin)) {
            gpio_init(dev->params->rdx_pin, GPIO_OUT);
            gpio_set(dev->params->rdx_pin);
        }

        assert(gpio_is_valid(dev->params->d0_pin));
        assert(gpio_is_valid(dev->params->d1_pin));
        assert(gpio_is_valid(dev->params->d2_pin));
        assert(gpio_is_valid(dev->params->d3_pin));
        assert(gpio_is_valid(dev->params->d4_pin));
        assert(gpio_is_valid(dev->params->d5_pin));
        assert(gpio_is_valid(dev->params->d6_pin));
        assert(gpio_is_valid(dev->params->d7_pin));
        gpio_init(dev->params->d0_pin, GPIO_OUT);
        gpio_init(dev->params->d1_pin, GPIO_OUT);
        gpio_init(dev->params->d2_pin, GPIO_OUT);
        gpio_init(dev->params->d3_pin, GPIO_OUT);
        gpio_init(dev->params->d4_pin, GPIO_OUT);
        gpio_init(dev->params->d5_pin, GPIO_OUT);
        gpio_init(dev->params->d6_pin, GPIO_OUT);
        gpio_init(dev->params->d7_pin, GPIO_OUT);
#if IS_USED(MODULE_LCD_PARALLEL_16BIT)
        if (dev->params->mode == LCD_IF_PARALLEL_16BIT) {
            assert(gpio_is_valid(dev->params->d8_pin));
            assert(gpio_is_valid(dev->params->d9_pin));
            assert(gpio_is_valid(dev->params->d10_pin));
            assert(gpio_is_valid(dev->params->d11_pin));
            assert(gpio_is_valid(dev->params->d12_pin));
            assert(gpio_is_valid(dev->params->d13_pin));
            assert(gpio_is_valid(dev->params->d14_pin));
            assert(gpio_is_valid(dev->params->d15_pin));
            gpio_init(dev->params->d8_pin, GPIO_OUT);
            gpio_init(dev->params->d9_pin, GPIO_OUT);
            gpio_init(dev->params->d10_pin, GPIO_OUT);
            gpio_init(dev->params->d11_pin, GPIO_OUT);
            gpio_init(dev->params->d12_pin, GPIO_OUT);
            gpio_init(dev->params->d13_pin, GPIO_OUT);
            gpio_init(dev->params->d14_pin, GPIO_OUT);
            gpio_init(dev->params->d15_pin, GPIO_OUT);
        }
        dev->word_access = false;
#endif /* IS_USED(MODULE_LCD_PARALLEL_16BIT) */
#else
        assert(false);
#endif
#if IS_USED(MODULE_LCD_SPI)
    }
#endif

    if (gpio_is_valid(dev->params->rst_pin)) {
        gpio_init(dev->params->rst_pin, GPIO_OUT);
        gpio_clear(dev->params->rst_pin);
        ztimer_sleep(ZTIMER_MSEC, 120);
        gpio_set(dev->params->rst_pin);
    }
    ztimer_sleep(ZTIMER_MSEC, 120);

#if IS_USED(MODULE_LCD_PARALLEL)
    mutex_init(&dev->lock);
#endif

    /* controller-specific init function has to be defined */
    assert(dev->driver->init);
    return dev->driver->init(dev, params);
}

void lcd_write_cmd(lcd_t *dev, uint8_t cmd, const uint8_t *data,
                   size_t len)
{
    lcd_ll_acquire(dev);
    lcd_ll_write_cmd(dev, cmd, data, len);
    lcd_ll_release(dev);
}

void lcd_read_cmd(lcd_t *dev, uint8_t cmd, uint8_t *data, size_t len)
{
    lcd_ll_acquire(dev);
    lcd_ll_read_cmd(dev, cmd, data, len);
    lcd_ll_release(dev);
}

void lcd_fill(lcd_t *dev, uint16_t x1, uint16_t x2, uint16_t y1,
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

    lcd_ll_set_area(dev, x1, x2, y1, y2);
    /* Memory access command */
    lcd_ll_cmd_start(dev, LCD_CMD_RAMWR, true);

    if (IS_ACTIVE(CONFIG_LCD_LE_MODE)) {
        color = htons(color);
    }

    for (int i = 0; i < (num_pix - 1); i++) {
        lcd_ll_write_bytes(dev, true, (uint8_t *)&color, sizeof(color));
    }
    lcd_ll_write_bytes(dev, false, (uint8_t *)&color, sizeof(color));
    lcd_ll_release(dev);
}

void lcd_pixmap(lcd_t *dev, uint16_t x1, uint16_t x2,
                uint16_t y1, uint16_t y2, const uint16_t *color)
{
    size_t num_pix = (x2 - x1 + 1) * (y2 - y1 + 1);

    DEBUG("[lcd]: Write x1: %" PRIu16 ", x2: %" PRIu16 ", "
          "y1: %" PRIu16 ", y2: %" PRIu16 ". Num pixels: %lu\n",
          x1, x2, y1, y2, (unsigned long)num_pix);

    lcd_ll_acquire(dev);

    /* Send fill area to the display */
    lcd_ll_set_area(dev, x1, x2, y1, y2);

    /* Memory access command */
    lcd_ll_cmd_start(dev, LCD_CMD_RAMWR, true);

    if (IS_ACTIVE(CONFIG_LCD_LE_MODE)) {
        for (size_t i = 0; i < num_pix - 1; i++) {
            uint16_t ncolor = htons(*(color + i));
            lcd_ll_write_bytes(dev, true, &ncolor, sizeof(uint16_t));
        }
        uint16_t ncolor = htons(*(color + num_pix - 1));
        lcd_ll_write_bytes(dev, false, &ncolor, sizeof(uint16_t));
    }
    else {
        lcd_ll_write_bytes(dev, false, (const uint8_t *)color, num_pix * 2);
    }

    lcd_ll_release(dev);
}

void lcd_invert_on(lcd_t *dev)
{
    uint8_t command = (dev->params->inverted) ? LCD_CMD_DINVOFF
                                              : LCD_CMD_DINVON;

    lcd_write_cmd(dev, command, NULL, 0);
}

void lcd_invert_off(lcd_t *dev)
{
    uint8_t command = (dev->params->inverted) ? LCD_CMD_DINVON
                                              : LCD_CMD_DINVOFF;

    lcd_write_cmd(dev, command, NULL, 0);
}

void lcd_set_brightness(lcd_t *dev, uint8_t brightness)
{
    lcd_write_cmd(dev, LCD_CMD_WRDISBV, &brightness, 1);
    uint8_t param = 0x26;
    lcd_write_cmd(dev, LCD_CMD_WRCTRLD, &param, 1);
}
