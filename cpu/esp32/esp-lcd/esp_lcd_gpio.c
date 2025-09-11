/*
 * SPDX-FileCopyrightText: 2023 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       GPIO-driven low-Level parallel interface implementation for LCDs
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#include <assert.h>

#include "periph/gpio.h"
#include "lcd.h"
#include "ztimer.h"

#if MODULE_ESP_LCD_GPIO

#include "soc/gpio_reg.h"

#define ENABLE_DEBUG 0
#include "debug.h"

typedef struct {
    uint32_t set_mask_0;    /* port 0 set mask */
    uint32_t set_mask_1;    /* port 0 set mask */
    uint32_t clr_mask_0;    /* port 1 clear mask */
    uint32_t clr_mask_1;    /* port 1 clear mask */
} _pin_mask_t;

static _pin_mask_t _low_byte_masks[256] = {};

#if IS_USED(MODULE_LCD_PARALLEL_16BIT)
static _pin_mask_t _high_byte_masks[256] = {};
#endif

/*
 * Following functions are not implemented by intention to let the
 * GPIO-driven low-level implementation handle the configuration
 * of the GPIOs. The function `_lcd_ll_mcu_set_data_dir` is used to
 * initialize the GPIO masks when the clear masks are completely 0.
 */
#if 0

static void _lcd_ll_mcu_init(lcd_t *dev)
{
    (void)dev;
}

static void _lcd_ll_mcu_cmd_start(lcd_t *dev, uint8_t cmd, bool cont)
{
    (void)dev;
    (void)cmd;
    (void)cont;
}

#endif

static void _lcd_ll_mcu_set_data_dir(lcd_t *dev, bool output)
{
    DEBUG("[lcd_ll_mcu] %s %u\n", __func__, output);

    /* sanity check to ensure that data pins can be handled as array */
    assert((&dev->params->d7_pin - &dev->params->d0_pin) == 7);
#if IS_USED(MODULE_LCD_PARALLEL_16BIT)
    assert((&dev->params->d15_pin - &dev->params->d8_pin) == 7);
#endif

    if ((_low_byte_masks[0].clr_mask_0 == 0) &&
        (_low_byte_masks[0].clr_mask_1 == 0)) {
        /* initialize the mask array if it is not yet initialized */
        const gpio_t *pins = &dev->params->d0_pin;

        for (unsigned data = 0; data < 256; data++) {
            for (unsigned i = 0; i < 8; i++) {
                if (data & (1 << i)) {
                    /* set mask */
                    if (pins[i] < 32) {
                        _low_byte_masks[data].set_mask_0 |= 1 << pins[i];
                    }
                    else {
                        _low_byte_masks[data].set_mask_1 |= 1 << (pins[i] - 32);
                    }
                }
                else {
                    /* clear mask */
                    if (pins[i] < 32) {
                        _low_byte_masks[data].clr_mask_0 |= 1 << pins[i];
                    }
                    else {
                        _low_byte_masks[data].clr_mask_1 |= 1 << (pins[i] - 32);
                    }
                }
            }
        }
#if IS_USED(MODULE_LCD_PARALLEL_16BIT)
        pins = &dev->params->d8_pin;

        for (unsigned data = 0; data < 256; data++) {
            for (unsigned i = 0; i < 8; i++) {
                if (data & (1 << i)) {
                    /* set mask */
                    if (pins[i] < 32) {
                        _high_byte_masks[data].set_mask_0 |= 1 << pins[i];
                    }
                    else {
                        _high_byte_masks[data].set_mask_1 |= 1 << (pins[i] - 32);
                    }
                }
                else {
                    /* clear mask */
                    if (pins[i] < 32) {
                        _high_byte_masks[data].clr_mask_0 |= 1 << pins[i];
                    }
                    else {
                        _high_byte_masks[data].clr_mask_1 |= 1 << (pins[i] - 32);
                    }
                }
            }
        }
#endif
    }

    gpio_init(dev->params->d0_pin, output ? GPIO_OUT : GPIO_IN);
    gpio_init(dev->params->d1_pin, output ? GPIO_OUT : GPIO_IN);
    gpio_init(dev->params->d2_pin, output ? GPIO_OUT : GPIO_IN);
    gpio_init(dev->params->d3_pin, output ? GPIO_OUT : GPIO_IN);
    gpio_init(dev->params->d4_pin, output ? GPIO_OUT : GPIO_IN);
    gpio_init(dev->params->d5_pin, output ? GPIO_OUT : GPIO_IN);
    gpio_init(dev->params->d6_pin, output ? GPIO_OUT : GPIO_IN);
    gpio_init(dev->params->d7_pin, output ? GPIO_OUT : GPIO_IN);
#if IS_USED(MODULE_LCD_PARALLEL_16BIT)
    gpio_init(dev->params->d8_pin, output ? GPIO_OUT : GPIO_IN);
    gpio_init(dev->params->d9_pin, output ? GPIO_OUT : GPIO_IN);
    gpio_init(dev->params->d10_pin, output ? GPIO_OUT : GPIO_IN);
    gpio_init(dev->params->d11_pin, output ? GPIO_OUT : GPIO_IN);
    gpio_init(dev->params->d12_pin, output ? GPIO_OUT : GPIO_IN);
    gpio_init(dev->params->d13_pin, output ? GPIO_OUT : GPIO_IN);
    gpio_init(dev->params->d14_pin, output ? GPIO_OUT : GPIO_IN);
    gpio_init(dev->params->d15_pin, output ? GPIO_OUT : GPIO_IN);
#endif /* IS_USED(MODULE_LCD_PARALLEL_16BIT) */
}

static void _lcd_ll_mcu_write_data(lcd_t *dev, bool cont,
                                   uint16_t data, unsigned pin_num)
{
    if (gpio_is_valid(dev->params->cs_pin)) {
        gpio_clear(dev->params->cs_pin);
    }

    gpio_clear(dev->params->wrx_pin);

    uint8_t _byte = data & 0xff;

    uint32_t set_mask_0 = _low_byte_masks[_byte].set_mask_0;
    uint32_t clr_mask_0 = _low_byte_masks[_byte].clr_mask_0;
    uint32_t set_mask_1 = _low_byte_masks[_byte].set_mask_1;
    uint32_t clr_mask_1 = _low_byte_masks[_byte].clr_mask_1;

#if IS_USED(MODULE_LCD_PARALLEL_16BIT)
    _byte = data >> 8;

    set_mask_0 |= _high_byte_masks[_byte].set_mask_0;
    clr_mask_0 |= _high_byte_masks[_byte].clr_mask_0;
    set_mask_1 |= _high_byte_masks[_byte].set_mask_1;
    clr_mask_1 |= _high_byte_masks[_byte].clr_mask_1;
#endif

    *((uint32_t *)GPIO_OUT_W1TS_REG) = set_mask_0;
    *((uint32_t *)GPIO_OUT_W1TC_REG) = clr_mask_0;
    *((uint32_t *)GPIO_OUT1_W1TS_REG) = set_mask_1;
    *((uint32_t *)GPIO_OUT1_W1TC_REG) = clr_mask_1;

    gpio_set(dev->params->wrx_pin);

    if (gpio_is_valid(dev->params->cs_pin) && !cont) {
        gpio_set(dev->params->cs_pin);
    }
}

static uint16_t _lcd_ll_mcu_read_data(lcd_t *dev, bool cont, unsigned pin_num)
{
    const gpio_t *pins = &dev->params->d0_pin;

    if (gpio_is_valid(dev->params->cs_pin)) {
        gpio_clear(dev->params->cs_pin);
    }

    gpio_clear(dev->params->rdx_pin);

    uint32_t in_0 = *((uint32_t *)GPIO_IN_REG);
    uint32_t in_1 = *((uint32_t *)GPIO_IN1_REG);

    gpio_set(dev->params->rdx_pin);

    if (gpio_is_valid(dev->params->cs_pin) && !cont) {
        gpio_set(dev->params->cs_pin);
    };

    uint16_t in = 0;

    for (unsigned i = 0; i < pin_num; i++) {
        if (pins[i] < 32) {
            in |= in_0 & (1 << pins[i]) ? 1 : 0;
        }
        else {
            in |= in_1 & (1 << (pins[i] - 32)) ? 1 : 0;
        }
    }

    return in;
}

static void _lcd_ll_mcu_write_byte(lcd_t *dev, bool cont, uint8_t out)
{
    DEBUG("[lcd_ll_mcu] write byte: %02x\n", out);

    _lcd_ll_mcu_write_data(dev, cont, out, 8);
}

static uint8_t _lcd_ll_mcu_read_byte(lcd_t *dev, bool cont)
{
    return _lcd_ll_mcu_read_data(dev, cont, 8);
}

#if IS_USED(MODULE_LCD_PARALLEL_16BIT)

static void _lcd_ll_mcu_write_word(lcd_t *dev, bool cont, uint16_t out)
{
    DEBUG("[lcd_ll_mcu] write word: %04x\n", out);

    _lcd_ll_mcu_write_data(dev, cont, out, 16);
}

static uint16_t _lcd_ll_mcu_read_word(lcd_t *dev, bool cont)
{
    return _lcd_ll_mcu_read_data(dev, cont, 16);
}

#endif /* IS_USED(MODULE_LCD_PARALLEL_16BIT) */

const lcd_ll_par_driver_t lcd_ll_par_driver = {
    .init = lcd_ll_par_gpio_init,               /* GPIO-driven `init` is used */
    .set_data_dir = _lcd_ll_mcu_set_data_dir,
    .cmd_start = lcd_ll_par_gpio_cmd_start,     /* GPIO-driven `cmd_start` is used */
    .write_byte = _lcd_ll_mcu_write_byte,
    .read_byte = _lcd_ll_mcu_read_byte,
#if IS_USED(MODULE_LCD_PARALLEL_16BIT)
    .write_word = _lcd_ll_mcu_write_word,
    .read_word = _lcd_ll_mcu_read_word,
#endif
};

#else /* MODULE_ESP_LCD_GPIO */

/* the GPIO-driven low-level interface is not used */
const lcd_ll_par_driver_t lcd_ll_par_driver = {
};

#endif /* MODULE_ESP_LCD_GPIO */
