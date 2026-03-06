/*
 * SPDX-FileCopyrightText: 2023 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_lcd
 * @{
 *
 * @file
 * @brief       GPIO-driven low-level parallel interface implementation
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include <assert.h>

#include "lcd.h"
#include "lcd_ll_par_gpio.h"
#include "periph/gpio.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#if IS_USED(MODULE_LCD_PARALLEL)

void lcd_ll_par_gpio_init(lcd_t *dev)
{
    DEBUG("[lcd] %s\n", __func__);

    /* MCU 8080 8-/16-bit parallel interface is used */
    assert(gpio_is_valid(dev->params->dcx_pin));
    gpio_init(dev->params->dcx_pin, GPIO_OUT);

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
    }
    dev->word_access = false;
#endif /* IS_USED(MODULE_LCD_PARALLEL_16BIT) */

    /* initialize all data GPIOs as outputs */
    lcd_ll_par_gpio_set_data_dir(dev, true);
}

void lcd_ll_par_gpio_set_data_dir(lcd_t *dev, bool output)
{
    DEBUG("[lcd] %s %u\n", __func__, output);

    gpio_init(dev->params->d0_pin, output ? GPIO_OUT : GPIO_IN);
    gpio_init(dev->params->d1_pin, output ? GPIO_OUT : GPIO_IN);
    gpio_init(dev->params->d2_pin, output ? GPIO_OUT : GPIO_IN);
    gpio_init(dev->params->d3_pin, output ? GPIO_OUT : GPIO_IN);
    gpio_init(dev->params->d4_pin, output ? GPIO_OUT : GPIO_IN);
    gpio_init(dev->params->d5_pin, output ? GPIO_OUT : GPIO_IN);
    gpio_init(dev->params->d6_pin, output ? GPIO_OUT : GPIO_IN);
    gpio_init(dev->params->d7_pin, output ? GPIO_OUT : GPIO_IN);
#if IS_USED(MODULE_LCD_PARALLEL_16BIT)
    if (dev->params->mode == LCD_IF_PARALLEL_16BIT) {
        gpio_init(dev->params->d8_pin, output ? GPIO_OUT : GPIO_IN);
        gpio_init(dev->params->d9_pin, output ? GPIO_OUT : GPIO_IN);
        gpio_init(dev->params->d10_pin, output ? GPIO_OUT : GPIO_IN);
        gpio_init(dev->params->d11_pin, output ? GPIO_OUT : GPIO_IN);
        gpio_init(dev->params->d12_pin, output ? GPIO_OUT : GPIO_IN);
        gpio_init(dev->params->d13_pin, output ? GPIO_OUT : GPIO_IN);
        gpio_init(dev->params->d14_pin, output ? GPIO_OUT : GPIO_IN);
        gpio_init(dev->params->d15_pin, output ? GPIO_OUT : GPIO_IN);
    }
#endif /* IS_USED(MODULE_LCD_PARALLEL_16BIT) */
}

void lcd_ll_par_gpio_cmd_start(lcd_t *dev, uint8_t cmd, bool cont)
{
    DEBUG("[lcd] %s %02x\n", __func__, cmd);

    gpio_clear(dev->params->dcx_pin);
    lcd_ll_par_gpio_write_byte(dev, cont, cmd);
    gpio_set(dev->params->dcx_pin);
}

void lcd_ll_par_gpio_write_byte(lcd_t *dev, bool cont, uint8_t out)
{
    DEBUG("[lcd] %s %02x\n", __func__, out);

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

uint8_t lcd_ll_par_gpio_read_byte(lcd_t *dev, bool cont)
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

    DEBUG("[lcd] %s %02x\n", __func__, in);

    return in;
}

#if IS_USED(MODULE_LCD_PARALLEL_16BIT)

void lcd_ll_par_gpio_write_word(lcd_t *dev, bool cont, uint16_t out)
{
    DEBUG("[lcd] %s %04x\n", __func__, out);

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

uint16_t lcd_ll_par_gpio_read_word(lcd_t *dev, bool cont)
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

    DEBUG("[lcd] %s %04x\n", __func__, in);

    return in;
}
#endif /* IS_USED(MODULE_LCD_PARALLEL_16BIT) */

#if !IS_USED(MODULE_LCD_PARALLEL_LL_MCU)
/* If MCU-driven low-level implementation is not used, the GPIO-driven
 * implementation is used as driver. */
const lcd_ll_par_driver_t lcd_ll_par_driver = {
    .init = lcd_ll_par_gpio_init,
    .set_data_dir = lcd_ll_par_gpio_set_data_dir,
    .cmd_start = lcd_ll_par_gpio_cmd_start,
    .write_byte = lcd_ll_par_gpio_write_byte,
    .read_byte = lcd_ll_par_gpio_read_byte,
#if IS_USED(MODULE_LCD_PARALLEL_16BIT)
    .write_word = lcd_ll_par_gpio_write_word,
    .read_word = lcd_ll_par_gpio_read_word,
#endif
};
#endif

#endif /* IS_USED(MODULE_LCD_PARALLEL) */
