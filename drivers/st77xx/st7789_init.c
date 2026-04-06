/*
 * SPDX-FileCopyrightText: 2023 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_st77xx
 * @{
 *
 * @file
 * @brief       Controller specific initialization for ST7789
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * This file is only compiled if the `st7789` module is enabled.
 *
 * @}
 */

#include <assert.h>
#include <string.h>

#include "byteorder.h"
#include "kernel_defines.h"
#include "ztimer.h"

#include "st77xx.h"
#include "st7789_internal.h"
#include "lcd.h"
#include "lcd_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#if CONFIG_ST7789_CUSTOM_CONFIG
/* avdd in mV with 200 mV increments: 6600 = 6.6V
 * Datasheet page 289:
 *
 *      y = 0                       for avdd < 6400         (< 6.4V)
 *      y = (avdd - 6400) / 200     for avdd = 6400 to 6800 (6.4 to 6.8V)
 *      y = 2                       for avdd > 6800         (> 6.8V)
 *
 * default value after reset is 6.8 V (0x02)
 */
static inline uint8_t _st7789_calc_avdd(int16_t avdd)
{
    assert((avdd >= 6400) && (avdd <= 6800));
    assert((avdd % 200) == 0);

    return (avdd - 6400) / 200;
}

/* avcl in mV with 200 mV increments: -4800 = -4.8V
 * Datasheet page 289:
 *
 *      y = 0                           for avcl < -5000          (< -5.0V)
 *      y = 3 - ((avcl + 5000) / 200)   for avcl = -5000 to -4400 (-5.0V to -4.4V)
 *      y = 3                           for avcl > -4400          (> -4.4V)
 *
 * default value after reset is -4.8 V (0x02)
 */
static inline uint8_t _st7789_calc_avcl(int16_t avcl)
{
    assert((avcl >= -5000) && (avcl <= -4400));
    assert((avcl % 200) == 0);

    return 3 - ((avcl + 5000) / 200);
}

/* vcom in mV with 25 mV increments: 1325 = 1.325V
 * Datasheet page 270:
 *
 *      y = 0                   for vcom < 100         (< 0.1V)
 *      y = (vcom - 100) / 25   for vcom = 100 to 1675 (0.1V to 1.675V)
 *      y = 63                  for vcom > 1675        (> 1.675V)
 *
 * default value after reset is 0.9 V (0x20)
 */
static inline uint8_t _st7789_calc_vcom(int16_t vcom)
{
    assert((vcom >= 100) && (vcom <= 1675));
    assert((vcom % 25) == 0);

    return (vcom - 100) / 25;
}

/* vol in mV with 25 mV increments: 100 = 0.1V
 * Datasheet page 279:
 *
 *      y = 0                   for vol < -800        (< -0.8V)
 *      y = (vol + 800) / 25    for vol = -800 to 775 (-0.8V to 0.775V)
 *      y = 63                  for vol > 0.775       (> 0.775V)
 *
 * default value after reset is 0 V (0x00)
 */
static inline uint8_t _st7789_calc_vcom_offset_vdv(int16_t vol)
{
    assert((vol >= -800) && (vol <= 775));
    assert((vol % 25) == 0);

    return (vol + 800) / 25;
}

/* vrh in mV with 50 mV increments: 4800 = 4.8V
 * Datasheet page 277:
 *
 *      y = 0                   for vrh < 3550         (< 3.55V)
 *      y = (vrh - 3550) / 50   for vrh = 3550 to 5500 (3.55V to 5.5V)
 *      y = 39                  for vrh > 5500         (> -4.4V)
 *
 * default value after reset is 4.1 V (0x0b)
 */
static inline uint8_t _st7789_calc_vrh(int16_t vrh)
{
    assert((vrh >= 3550) && (vrh <= 5500));
    assert((vrh % 50) == 0);

    return (vrh - 3550) / 50;
}
#endif /* CONFIG_ST7789_CUSTOM_CONFIG */

int st7789_init(lcd_t *dev, const lcd_params_t *params)
{
    (void)params;
    assert(params->lines <= 320);
    assert(params->rgb_channels <= 240);

    uint8_t command_params[5] = { 0 };
    (void)command_params;

#if 0 /* no need to write reset defaults, just for documentation purpose */

    /* LCMCTRL (C0h): LCM Control (== reset defaults) */
    command_params[0] = 0x2c;   /* XOR RGB, MX and MH setting in command 36h */
    lcd_ll_write_cmd(dev, LCD_CMD_LCMCTRL, command_params, 1);
    DEBUG("LCMCTRL (C0h) %02x\n", command_params[0]);

    /* VDVVRHEN (C2h): VDV and VRH Command Enable (== reset defaults) */
    command_params[0] = 0x01;   /* CMDEN=1 (VDV and VDH command write enable */
    command_params[1] = 0xff;
    lcd_ll_write_cmd(dev, LCD_CMD_VDVVRHEN, command_params, 2);
    DEBUG("VDVVRHEN (C2h) %02x %02x\n", command_params[0], command_params[1]);

#endif /* no need to write reset defaults, just for documentation purpose */

#if CONFIG_ST7789_CUSTOM_CONFIG

    /* VCOMS (BBh): VCOM Setting */
    command_params[0] = _st7789_calc_vcom(CONFIG_ST7789_VCOM);
    lcd_ll_write_cmd(dev, LCD_CMD_VCOMS, command_params, 1);
    DEBUG("VCOMS (BBh) %02x\n", command_params[0]);

    /* VRHS (C3h): VRH Set */
    command_params[0] = _st7789_calc_vrh(CONFIG_ST7789_VRH);
    lcd_ll_write_cmd(dev, LCD_CMD_VRHS, command_params, 1);
    DEBUG("VRHS (C3h) %02x\n", command_params[0]);

    /* VDVS (C4h): VDV Set */
    command_params[0] = _st7789_calc_vcom_offset_vdv(CONFIG_ST7789_VDV);
    lcd_ll_write_cmd(dev, LCD_CMD_VDVS, command_params, 1);
    DEBUG("VDVS (C4h) %02x\n", command_params[0]);

    /* VCMOFSET (C5h): VCOM Offset Set */
    command_params[0] = _st7789_calc_vcom_offset_vdv(CONFIG_ST7789_VCOM_OFFSET);
    lcd_ll_write_cmd(dev, LCD_CMD_VCMOFSET, command_params, 1);
    DEBUG("VCMOFSET (C5h) %02x\n", command_params[0]);

    /* PWCTRL1 (D0h): Power Control 1 */
    command_params[0] = 0xa4;
    command_params[1] = (_st7789_calc_avdd(CONFIG_ST7789_AVDD) << 6) |
                        (_st7789_calc_avcl(CONFIG_ST7789_AVCL) << 4) | 0x01;
    lcd_ll_write_cmd(dev, LCD_CMD_PWCTRL1X, command_params, 2);
    DEBUG("PWCTRL1 (D0h): %02x %02x\n", command_params[0], command_params[1]);

#else /* CONFIG_ST7789_CUSTOM_CONFIG */

#if 0 /* no need to write reset defaults, just for documentation purpose */

    /* VCOMS (BBh): VCOM Setting (== reset defaults) */
    command_params[0] = 0x20;   /* VCOM=0.9V */
    lcd_ll_write_cmd(dev, LCD_CMD_VCOMS, command_params, 1);

    /* VRHS (C3h): VRH Set (== reset defaults) */
    command_params[0] = 0x0b;   /* VRH=4.1V */
    lcd_ll_write_cmd(dev, LCD_CMD_VRHS, command_params, 1);

    /* VDVS (C4h): VDV Set  (== reset defaults)*/
    command_params[0] = 0x20;   /* VDV=0V */
    lcd_ll_write_cmd(dev, LCD_CMD_VDVS, command_params, 1);

    /* VCMOFSET (C5h): VCOM Offset Set (== reset defaults) */
    command_params[0] = 0x20;   /* VCOMFS=0V */
    lcd_ll_write_cmd(dev, LCD_CMD_VCMOFSET, command_params, 1);

    /* PWCTRL1 (D0h): Power Control 1 (== reset defaults) */
    command_params[0] = 0xa4;
    command_params[1] = 0xa1;   /* AVDD=6.8V, AVCL=4.8V, VDS=2.3 */
    lcd_ll_write_cmd(dev, LCD_CMD_PWCTRL1X, command_params, 2);

#endif /* no need to write reset defaults, just for documentation purpose */

#endif /* CONFIG_ST7789_CUSTOM_CONFIG */

#if 0 /* no need to write reset defaults, just for documentation purpose */

    /* FRCTRL2 (C6h): Frame Rate Control in Normal Mode */
    command_params[0] = 0x0f; /* == reset default (60 Hz) */
    lcd_ll_write_cmd(dev, LCD_CMD_FRCTRL2, command_params, 1);
    DEBUG("FRCTRL2 (C6h) %02x\n", command_params[0]);

    /* PORCTRL (B2h): Porch Setting */
    command_params[0] = 0x0c; /* == reset defaults */
    command_params[1] = 0x0c;
    command_params[2] = 0x00;
    command_params[3] = 0x33;
    command_params[4] = 0x33;
    lcd_ll_write_cmd(dev, LCD_CMD_PORCTRL, command_params, 5);

    /* GCTRL (B7h): Gate Control */
    command_params[0] = 0x35; /* == reset defaults */
    lcd_ll_write_cmd(dev, LCD_CMD_GCTRL, command_params, 1);

#endif /* no need to write reset defaults, just for documentation purpose */

    /* VGAMCTRL (E0h): Positive Voltage Gamma Control */
    {
        static const uint8_t gamma_pos[] = {
            0xd0, 0x08, 0x11, 0x08, 0x0c, 0x15, 0x39,
            0x33, 0x50, 0x36, 0x13, 0x14, 0x29, 0x2d
        };
        lcd_ll_write_cmd(dev, LCD_CMD_PGAMCTRL, gamma_pos, sizeof(gamma_pos));
    }
    /* NVGAMCTRL (E1h): Negative Voltage Gamma Control */
    {
        static const uint8_t gamma_neg[] = {
            0xd0, 0x08, 0x10, 0x08, 0x06, 0x06, 0x39,
            0x44, 0x51, 0x0b, 0x16, 0x14, 0x2f, 0x32
        };
        lcd_ll_write_cmd(dev, LCD_CMD_NGAMCTRL, gamma_neg, sizeof(gamma_neg));
    }

    return 0;
}
