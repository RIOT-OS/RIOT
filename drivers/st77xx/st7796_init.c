/*
 * SPDX-FileCopyrightText: 2023 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_st77xx
 * @{
 *
 * @file
 * @brief       Controller specific initialization for ST7796
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * This file is only compiled if the `st7796` module is enabled.
 *
 * @}
 */

#include <assert.h>
#include <string.h>

#include "byteorder.h"
#include "kernel_defines.h"
#include "ztimer.h"

#include "st77xx.h"
#include "st7796_internal.h"
#include "lcd.h"
#include "lcd_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#if CONFIG_ST7796_CUSTOM_CONFIG
/* avdd in mV with 200 mV increments: 6600 = 6.6V
 * Datasheet page 223:
 *
 *      y = 0                       for avdd < 6200         (< 6.2V)
 *      y = (avdd - 6200) / 200     for avdd = 6200 to 6800 (6.2 to 6.8V)
 *      y = 3                       for avdd > 6800         (> 6.8V)
 *
 * default value after reset is 6.6 V (0x02)
 */
static inline uint8_t _st7796_calc_avdd(int16_t avdd)
{
    assert((avdd >= 6200) && (avdd <= 6800));
    assert((avdd % 200) == 0);

    return (avdd - 6200) / 200;
}

/* avcl in mV with 200 mV increments: -4800 = -4.8V
 * Datasheet page 223:
 *
 *      y = 0                           for avcl < -5000          (< -5.0V)
 *      y = 3 - ((avcl + 5000) / 200)   for avcl = -5000 to -4400 (-5.0V to -4.4V)
 *      y = 3                           for avcl > -4400          (> -4.4V)
 *
 * default value after reset is -4.4 V (0x00)
 */
static inline uint8_t _st7796_calc_avcl(int16_t avcl)
{
    assert((avcl >= -5000) && (avcl <= -4400));
    assert((avcl % 200) == 0);

    return 3 - ((avcl + 5000) / 200);
}

/* vcom in mV with 25 mV increments: 1325 = 1.325V
 * Datasheet page 227:
 *
 *      y = 0                   for vcom < 300         (< 0.3V)
 *      y = (vcom - 300) / 25   for vcom = 100 to 1875 (0.3V to 1.875V)
 *      y = 63                  for vcom > 1685        (> 1.875V)
 *
 * default value after reset is 1.0V (0x1c)
 */
static inline uint8_t _st7796_calc_vcom(int16_t vcom)
{
    assert((vcom >= 300) && (vcom <= 1875));
    assert((vcom % 25) == 0);

    return (vcom - 300) / 25;
}

/* vol in mV with 25 mV increments: 100 = 0.1V
 * Datasheet page 229:
 *
 *      y = 0                   for vol < -800        (< -0.8V)
 *      y = (vol + 800) / 25    for vol = -800 to 775 (-0.8V to 0.775V)
 *      y = 63                  for vol > 0.775       (> 0.775V)
 *
 * default value after reset is 0 V (0x00)
 */
static inline uint8_t _st7796_calc_vcom_offset(int16_t off)
{
    assert((off >= -800) && (off <= 775));
    assert((off % 25) == 0);

    return (off < 0) ? 32 + ((off + 800) / 25) : off / 25;
}

/* vrh in mV with 50 mV increments: 4800 = 4.8V
 * Datasheet page 224:
 *
 *      y = 0                   for vrh < 3550         (< 3.55V)
 *      y = (vrh - 3550) / 50   for vrh = 3550 to 5500 (3.55V to 5.5V)
 *      y = 39                  for vrh > 5500         (> -4.4V)
 *
 * default value after reset is 4.5 V (0x13)
 */
static inline uint8_t _st7796_calc_vrh(int16_t vrh)
{
    assert((vrh >= 3550) && (vrh <= 5500));
    assert((vrh % 50) == 0);

    return (vrh - 3550) / 50;
}
#endif /* CONFIG_ST7796_CUSTOM_CONFIG */

int st7796_init(lcd_t *dev, const lcd_params_t *params)
{
    (void)params;
    assert(params->lines <= 480);
    assert(params->rgb_channels <= 320);

    uint8_t command_params[5] = { 0 };
    (void)command_params;

#if CONFIG_ST7796_CUSTOM_CONFIG

    /* PWR1 (c0h): Power Control 1 */
    command_params[0] = (_st7796_calc_avdd(CONFIG_ST7796_AVDD) << 6) |
                        (_st7796_calc_avcl(CONFIG_ST7796_AVCL) << 4);
    command_params[1] = 0x25;   /* use reset default, TODO VGH and VGL config */
    lcd_ll_write_cmd(dev, LCD_CMD_PWCTRL1, command_params, 2);
    DEBUG("PWR1 (c0h): %02x %02x\n", command_params[0], command_params[1]);

    /* PWR2 (C1h): Power Control 2 */
    command_params[0] = _st7796_calc_vrh(CONFIG_ST7796_VRH);
    lcd_ll_write_cmd(dev, LCD_CMD_PWCTRL2, command_params, 1);
    DEBUG("PWR2 (C1h) %02x\n", command_params[0]);

    /* VCMPCTL (C5h): VCOM Control */
    command_params[0] = _st7796_calc_vcom(CONFIG_ST7796_VCOM);
    lcd_ll_write_cmd(dev, LCD_CMD_VCMPCTL, command_params, 1);
    DEBUG("VCMPCTL (C5h) %02x\n", command_params[0]);

    /* VCM Offset (C6h): Vcom Offset Register */
    command_params[0] = _st7796_calc_vcom_offset(CONFIG_ST7796_VCOM_OFFSET);
    lcd_ll_write_cmd(dev, LCD_CMD_VCM, command_params, 1);
    DEBUG("VCM (C6h) %02x\n", command_params[0]);

#else /* CONFIG_ST7796_CUSTOM_CONFIG */

#if 0 /* no need to write reset defaults, just for documentation purpose */

    /* PWR1 (c0h): Power Control 1 (== reset defaults) */
    command_params[0] = 0x80;   /* AVDD=6.6V, AVCL=-4.4 */
    command_params[1] = 0x25;   /* use reset default, TODO VGH and VGL config */
    lcd_ll_write_cmd(dev, LCD_CMD_PWCTRL1, command_params, 2);
    DEBUG("PWR1 (c0h): %02x %02x\n", command_params[0], command_params[1]);

    /* PWR2 (C1h): Power Control 2 (== reset defaults) */
    command_params[0] = 0x0b;   /* VRH=4.1V */
    lcd_ll_write_cmd(dev, LCD_CMD_PWCTRL2, command_params, 1);
    DEBUG("PWR2 (C1h) %02x\n", command_params[0]);

    /* VCMPCTL (C5h): VCOM Control (== reset defaults) */
    command_params[0] = 0x1c;   /* VCOM=1.0V */
    lcd_ll_write_cmd(dev, LCD_CMD_VCMPCTL, command_params, 1);
    DEBUG("VCMPCTL (C5h) %02x\n", command_params[0]);

    /* VCM Offset (C6h): Vcom Offset Register (== reset defaults) */
    command_params[0] = 0x00;   /* VCOM Offset=0V (VMF_REG=0) */
    lcd_ll_write_cmd(dev, LCD_CMD_VCM, command_params, 1);
    DEBUG("VCM (C6h) %02x\n", command_params[0]);

#endif /* no need to write reset defaults, just for documentation purpose */

#endif /* CONFIG_ST7796_CUSTOM_CONFIG */

    /* VGAMCTRL (E0h): Positive Voltage Gamma Control */
    {
        static const uint8_t gamma_pos[] = {
            0xf0, 0x09, 0x0b, 0x06, 0x04, 0x15, 0x2f,
            0x54, 0x42, 0x3c, 0x17, 0x14, 0x18, 0x1b,
        };
        lcd_ll_write_cmd(dev, LCD_CMD_PGAMCTRL, gamma_pos, sizeof(gamma_pos));
    }
    /* NVGAMCTRL (E1h): Negative Voltage Gamma Control */
    {
        static const uint8_t gamma_neg[] = {
            0xe0, 0x09, 0x0b, 0x06, 0x04, 0x03, 0x2b,
            0x43, 0x42, 0x3b, 0x16, 0x14, 0x17, 0x1b
        };
        lcd_ll_write_cmd(dev, LCD_CMD_NGAMCTRL, gamma_neg, sizeof(gamma_neg));
    }

    return 0;
}
