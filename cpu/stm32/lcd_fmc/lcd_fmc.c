/*
 * SPDX-FileCopyrightText: 2023 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_stm32
 * @ingroup     drivers_periph_fmc
 * @{
 *
 * @file
 * @brief       FMC peripheral driver implementation
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#include <assert.h>

#include "periph/gpio.h"
#include "lcd.h"
#include "lcd_internal.h"
#include "ztimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef LCD_FMC_NUMOF
#define LCD_FMC_NUMOF  1
#endif

#define FMC_LCD_CMD(d)  (*((__IO uint16_t *)(d->bank->address + d->cmd_offset)))
#define FMC_LCD_DATA(d) (*((__IO uint16_t *)(d->bank->address + d->data_offset)))

/* sanity check */
static_assert(LCD_FMC_NUMOF == ARRAY_SIZE(lcd_fmc_desc),
              "LCD_FMC_NUMOF is not equal to the number of elements in lcd_fmc_desc");

#if LCD_FMC_NUMOF > 1
/* In the case that multiple LCDs are connected to FMC banks, an array
 * for mapping the LCD device pointer to the FMC bank descriptor is used.
 * This requires that the FMC bank descriptors for LCDs in `lcd_fmc_desc`
 * are defined in same order as the LCD devices. */
static lcd_t *_lcd_fmc_desc_map[LCD_FMC_NUMOF];
static uint8_t _lcd_index = 0;

static inline uint8_t _dev_to_lcd_fmc_desc(lcd_t *dev)
{
    for (uint8_t i = 0; i < LCD_FMC_NUMOF; i++) {
        if (_lcd_fmc_desc_map[i] == dev) {
            return i;
        }
    }
    assert(false);
}
#endif

static void lcd_ll_mcu_init(lcd_t *dev)
{
#if LCD_FMC_NUMOF > 1
    /* The FMC bank descriptors for LCDs in `lcd_fmc_desc` must be defined
     * in same order as the LCD display devices. We suppose that the
     * LCDs are initialized in that order. */
    assert(_lcd_index < LCD_FMC_NUMOF);
    _lcd_fmc_desc_map[_lcd_index++] = dev;
#else
    (void)dev;
#endif
}

static void lcd_ll_mcu_set_data_dir(lcd_t *dev, bool output)
{
    /* no action needed */
    (void)dev;
    (void)output;
}

static void lcd_ll_mcu_cmd_start(lcd_t *dev, uint8_t cmd, bool cont)
{
    DEBUG("[lcd_ll_mcu] write cmd: %02x\n", cmd);

    (void)cont;
#if LCD_FMC_NUMOF > 1
    const lcd_fmc_desc_t *desc = &lcd_fmc_desc[_dev_to_lcd_fmc_desc(dev)];
#else
    (void)dev;
    const lcd_fmc_desc_t *desc = lcd_fmc_desc;
#endif

    FMC_LCD_CMD(desc) = cmd;
    /* data synchronization barrier seems to be necessary on some STM32 MCUs. */
    __DSB();
}

static void lcd_ll_mcu_write_byte(lcd_t *dev, bool cont, uint8_t out)
{
    DEBUG("[lcd_ll_mcu] write byte: %02x\n", out);

    (void)cont;
#if LCD_FMC_NUMOF > 1
    const lcd_fmc_desc_t *desc = &lcd_fmc_desc[_dev_to_lcd_fmc_desc(dev)];
#else
    (void)dev;
    const lcd_fmc_desc_t *desc = lcd_fmc_desc;
#endif

    FMC_LCD_DATA(desc) = out;
    /* data synchronization barrier seems to be necessary on some STM32 MCUs. */
    __DSB();
}

static uint8_t lcd_ll_mcu_read_byte(lcd_t *dev, bool cont)
{
    (void)cont;
#if LCD_FMC_NUMOF > 1
    const lcd_fmc_desc_t *desc = &lcd_fmc_desc[_dev_to_lcd_fmc_desc(dev)];
#else
    (void)dev;
    const lcd_fmc_desc_t *desc = lcd_fmc_desc;
#endif

    uint8_t in = FMC_LCD_DATA(desc);
    DEBUG("[lcd_ll_mcu] read byte: %02x\n", in);
    return in;
}

#if IS_USED(MODULE_LCD_PARALLEL_16BIT)

static void lcd_ll_mcu_write_word(lcd_t *dev, bool cont, uint16_t out)
{
    DEBUG("[lcd_ll_mcu] write word: %04x\n", out);

    (void)cont;
#if LCD_FMC_NUMOF > 1
    const lcd_fmc_desc_t *desc = &lcd_fmc_desc[_dev_to_lcd_fmc_desc(dev)];
#else
    (void)dev;
    const lcd_fmc_desc_t *desc = lcd_fmc_desc;
#endif

    FMC_LCD_DATA(desc) = out;
    /* data synchronization barrier seems to be necessary on some STM32 MCUs. */
    __DSB();
}

static uint16_t lcd_ll_mcu_read_word(lcd_t *dev, bool cont)
{
    (void)cont;
#if LCD_FMC_NUMOF > 1
    const lcd_fmc_desc_t *desc = &lcd_fmc_desc[_dev_to_lcd_fmc_desc(dev)];
#else
    (void)dev;
    const lcd_fmc_desc_t *desc = lcd_fmc_desc;
#endif

    uint16_t in = FMC_LCD_DATA(desc);
    DEBUG("[lcd_ll_mcu] read word: %04x\n", in);
    return in;
}

#endif /* MODULE_LCD_PARALLEL_16BIT */

const lcd_ll_par_driver_t lcd_ll_par_driver = {
    .init = lcd_ll_mcu_init,
    .set_data_dir = lcd_ll_mcu_set_data_dir,
    .cmd_start = lcd_ll_mcu_cmd_start,
    .write_byte = lcd_ll_mcu_write_byte,
    .read_byte = lcd_ll_mcu_read_byte,
#if IS_USED(MODULE_LCD_PARALLEL_16BIT)
    .write_word = lcd_ll_mcu_write_word,
    .read_word = lcd_ll_mcu_read_word,
#endif
};
