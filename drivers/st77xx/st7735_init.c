/*
 * SPDX-FileCopyrightText: 2023 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_st77xx
 * @{
 *
 * @file
 * @brief       Controller specific initialization for ST7735
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * This file is only compiled if the `st7735` module is enabled.
 *
 * @}
 */

#include <assert.h>
#include <string.h>

#include "byteorder.h"
#include "kernel_defines.h"
#include "ztimer.h"

#include "st77xx.h"
#include "st7735_internal.h"
#include "lcd.h"
#include "lcd_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#if CONFIG_ST7735_CUSTOM_CONFIG
/* avdd in mV with in 100 mV steps: 4600 = 4.6V
 * Datasheet page 130:
 *
 *      y = 0                           for avdd < 4500         (< 4.5V)
 *      y = (5100 - avdd) / 100         for avdd = 4500 to 5100 (4.5V to 5.1V)
 *      y = 6                           for avdd > 5100         (> 5.1V)
 *
 * default value after reset is 4.9 V (0x04)
 */
static inline uint8_t _st7735_calc_avdd(uint16_t avdd)
{
    assert((avdd >= 4500) && (avdd <= 5100));
    assert((avdd % 100) == 0);

    return (5100 - avdd) / 100;
}

/* gvdd in mV with 50 mV increments: 4650 = 4.65V
 * Datasheet page 130:
 *
 *      y = 31                          for gddv < 3150         (< 3.15V)
 *      y = 31 - ((gddv - 3150) / 50)   for gddv = 3150 to 4700 (3.15V to 4.7V)
 *      y = 0                           for gddv > 4700         (> 4.7V)
 *
 * default value after reset is 4.6 V (0x02)
 */
static inline uint8_t _st7735_calc_gvdd(uint16_t gvdd)
{
    assert((gvdd >= 3150) && (gvdd <= 4700));
    assert((gvdd % 50) == 0);

    return 31 - ((gvdd - 3150) / 50);
}

/* gvcl in mV with 50 mV increments: -4650 = -4.65V
 * Datasheet page 130:
 *
 *      y = 0                           for gdcl < -4700          (< -4.7V)
 *      y = 31 - (-3150 - gvcl) / 50)   for gddv = -4700 to -3150 (-4.7V to -3.15V)
 *      y = 31                          for gddv > -3150          (> -3.15V)
 *
 * default value after reset is -4.6 V (0x02)
 */
static inline uint8_t _st7735_calc_gvcl(int16_t gvcl)
{
    assert((gvcl >= -4700) && (gvcl <= -3150));
    assert((gvcl % 50) == 0);

    return 31 - ((-3150 - gvcl) / 50);
}

/* vcom in mV with 25 mV increments: -625 = -0.625V
 * Datasheet page 140:
 *
 *      y = 63                          for vcom < -2000         (> 2V)
 *      y = 63 - ((2000 + vcom) / 25)   for vcom = -2000 to -425 (-2V to -0.425V)
 *      y = 0                           for vcom > -425          (< -0.425V)
 *
 * default value after reset is 4.9 V (4)
 */
static inline uint8_t _st7735_calc_vcom(int16_t vcom)
{
    assert((vcom >= -2000) && (vcom <= 425));
    assert((vcom % 25) == 0);

    return 63 - ((2000 + vcom) / 25);
}

/* vgh in mV with 100 mV increments: 11200 = 11.2V
 * vgl in mV with 2500 mV increments: 12500 = 12.5V
 * Datasheet page 132
 */
static inline uint8_t _st7735_calc_vghl(uint16_t vgh, int16_t vgl, uint16_t avdd)
{
    assert((vgh >= 10000) && (vgh <= 15000));
    assert((vgl >= -13000) && (vgl <= 7500));
    assert((vgh >= ((avdd * 2) + 2100)) && (vgh <= ((3 * avdd) + 2400)));

    uint16_t bt = vgh / avdd;
    uint16_t h25 = 0;
    assert((bt == 2) || (bt == 3)); /* bt must be either 2 or 3 */

    if ((vgh - (bt * avdd)) > 2100) {
        /* if there remains an offset of at least 2.1V, use VGH25 */
        h25 = ((vgh - (bt * avdd)) - 2100) / 100;
        assert(h25 <= 3);
    }

    bt -= 2;    /* convert (3 * AVDD) to 01b and (2 * AVDD) to 00b */

    if (bt && h25) {
        /* represents 3 * AVDD + VGH25 */
        bt++;
    }
    else {
        h25 = 3;
    }

    uint16_t sel = (vgl < -12500) ? 3 : 2 - ((vgl + 12500) / 2500);

    return (h25 << 6) + (sel << 2) + bt;
}
#endif /* CONFIG_ST7735_CUSTOM_CONFIG */

int st7735_init(lcd_t *dev, const lcd_params_t *params)
{
    (void)params;
    assert(params->lines <= 162);
    assert(params->rgb_channels <= 132);

    uint8_t command_params[6] = { 0 };

    /* INVCTR (B4h): Display Inversion Control */
    command_params[0] = 0x07;   /* NLA=1, NLB=1, NLC=1 Line inversion in all modes */
    lcd_ll_write_cmd(dev, LCD_CMD_INVCTR, command_params, 1);

#if CONFIG_ST7735_CUSTOM_CONFIG

    /* PWCTR1 (C0h): Power Control 1 */
    command_params[0] = (_st7735_calc_avdd(CONFIG_ST7735_AVDD) << 5) |
                         _st7735_calc_gvdd(CONFIG_ST7735_GVDD);
    com mand_params[1] = _st7735_calc_gvcl(CONFIG_ST7735_GVCL);
    command_params[2] = 0x84;   /* AUTO mode */
    lcd_ll_write_cmd(dev, LCD_CMD_PWCTRL1, command_params, 3);
    DEBUG("PWCTRL1 (C0h): %02x %02x %02x\n",
          command_params[0], command_params[1], command_params[2]);

    /* PWCTR2 (C1h): Power Control 2 (== reset defaults) */
    command_params[0] = _st7735_calc_vghl(CONFIG_ST7735_VGH, CONFIG_ST7735_VGL,
                                          CONFIG_ST7735_AVDD);
    lcd_ll_write_cmd(dev, LCD_CMD_PWCTRL2, command_params, 1);
    DEBUG("PWCTRL2 (C1h): %02x\n", command_params[0]);

    /* VMCTR1 (C5h): VCOM Control 1 */
    command_params[0] = _st7735_calc_vcom(CONFIG_ST7735_VCOM);
    lcd_ll_write_cmd(dev, LCD_CMD_VMCTRL1, command_params, 1);
    DEBUG("VMCTR1 (C5h): %02x\n", command_params[0]);

#else /* CONFIG_ST7735_CUSTOM_CONFIG */

#if 0 /* no need to write reset defaults, just for documentation purpose */

    /* PWCTR1 (C0h): Power Control 1 (== reset defaults) */
    command_params[0] = 0x82;   /* AVDD=4.9V, GVDD=4.6V */
    command_params[1] = 0x02;   /* GVCL=-4.6V */
    command_params[2] = 0x84;   /* AUTO mode */
    lcd_ll_write_cmd(dev, LCD_CMD_PWCTRL1, command_params, 3);

    /* PWCTR2 (C1h): Power Control 2 (== reset defaults) */
    command_params[0] = 0xc5;   /* VGH=3*AVDD, VGL=-10V */
    lcd_ll_write_cmd(dev, LCD_CMD_PWCTRL2, command_params, 1);

    /* VMCTR1 (C5h): VCOM Control 1 (== reset defaults) */
    command_params[0] = 0x04;   /* VCOM=-0.525V */
    lcd_ll_write_cmd(dev, LCD_CMD_VMCTRL1, command_params, 1);

#endif /* no need to write reset defaults, just for documentation purpose */

#endif /* CONFIG_ST7735_CUSTOM_CONFIG */

#if 0 /* no need to write reset defaults, just for documentation purpose */

    /* PWCTR3 (C2h): Power Control 3 Normal Mode (== reset defaults) */
    command_params[0] = 0x0a;   /* AP=Medium Low, SAP=Small */
    command_params[0] = 0x00;   /* DCA=BCLK/1 BCLK/1 BCLK/1 BCLK/1 BCLK/1 */
    lcd_ll_write_cmd(dev, LCD_CMD_PWCTRL3, command_params, 2);

    /* PWCTR4 (C3h): Power Control 4 Idle Mode (== reset defaults) */
    command_params[0] = 0x8a;   /* AP=Medium Low, SAP=Small */
    command_params[1] = 0x2e;   /* DCA=BCLK/2 BCLK/1 BCLK/2 BCLK/4 BCLK/2 */
    lcd_ll_write_cmd(dev, LCD_CMD_PWCTRL4, command_params, 2);

    /* PWCTR5 (C4h): Power Control 5 Partial Mode (== reset defaults) */
    command_params[0] = 0x8a;   /* AP=Medium Low, SAP=Small */
    command_params[1] = 0xaa;   /* DCA=BCLK/2 BCLK/2 BCLK/2 BCLK/2 BCLK/2 */
    lcd_ll_write_cmd(dev, LCD_CMD_PWCTRL5, command_params, 2);

    /* FRMCTRL1 (B1H): Frame Control 1 in Normal mode (== reset defaults) */
    /* Frame rate = fosc/((RNTA * 2 + 40) x (lines + FPA + BPA)) with fosc = 624 kHz */
    command_params[0] = 0x01;   /* RNTA=1 */
    command_params[1] = 0x2c;   /* FPA (Front Porch) = 44 lines */
    command_params[2] = 0x2d;   /* BPA (Back Porch) = 45 lines */
    lcd_ll_write_cmd(dev, LCD_CMD_FRAMECTL1, command_params, 3);

    /* FRMCTRL2 (B2H): Frame Control 2 in Idle mode (== reset defaults) */
    /* Frame rate = fosc/((RNTB * 2 + 40) x (lines + FPB + BPB)) with fosc = 624 kHz */
    command_params[0] = 0x01;   /* RNTB=1 */
    command_params[1] = 0x2c;   /* FPB (Front Porch) = 44 lines */
    command_params[2] = 0x2d;   /* BPB (Back Porch) = 45 lines */
    lcd_ll_write_cmd(dev, LCD_CMD_FRAMECTL2, command_params, 3);

    /* FRMCTRL3 (B3H): Frame Control 3 in Partal mode (== reset defaults) */
    /* Frame rate = fosc/((RNTC * 2 + 40) x (lines + FPC + BPC)) with fosc = 624 kHz */
    command_params[0] = 0x01;   /* RNTC=1 */
    command_params[1] = 0x2c;   /* FPC (Front Porch) = 44 lines */
    command_params[2] = 0x2d;   /* BPC (Back Porch) = 45 lines */
    command_params[3] = 0x01;   /* RNTD=1 */
    command_params[4] = 0x2c;   /* FPD (Front Porch) = 44 lines */
    command_params[5] = 0x2d;   /* BPD (Back Porch) = 45 lines */
    lcd_ll_write_cmd(dev, LCD_CMD_FRAMECTL3, command_params, 6);

#endif /* no need to write reset defaults, just for documentation purpose */

    /* GMCTRP1 (E0h): Gamma +polarity Correction Characteristics Setting */
    {
        static const uint8_t gamma_pos[] = {
            0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d,
            0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10
        };
        lcd_ll_write_cmd(dev, LCD_CMD_PGAMCTRL, gamma_pos,
                         sizeof(gamma_pos));
    }
    /* GMCTRN1 (E1h): Gamma -polarity Correction Characteristics Setting */
    {
        static const uint8_t gamma_neg[] = {
            0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D,
            0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10
        };
        lcd_ll_write_cmd(dev, LCD_CMD_NGAMCTRL, gamma_neg,
                         sizeof(gamma_neg));
    }

    return 0;
}
