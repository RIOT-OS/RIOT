/*
 * SPDX-FileCopyrightText: 2018 Koen Zandberg
 * SPDX-FileCopyrightText: 2023 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_ili9341
 * @{
 *
 * @file
 * @brief       Device driver implementation for the ili9341 display controller
 *
 * @author      Koen Zandberg <koen@bergzand.net>
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

#include "ili9341.h"
#include "ili9341_internal.h"
#include "lcd.h"
#include "lcd_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

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

static int _init(lcd_t *dev, const lcd_params_t *params)
{
    assert(params->lines >= 16 && params->lines <= 320 && !(params->lines & 0x7));

    uint8_t command_params[4] = { 0 };

    /* Acquire once at release at the end */
    lcd_ll_acquire(dev);

    /* Soft Reset */
    lcd_ll_write_cmd(dev, LCD_CMD_SWRESET, NULL, 0);
    ztimer_sleep(ZTIMER_MSEC, 120);

    /* Display off */
    lcd_ll_write_cmd(dev, LCD_CMD_DISPOFF, NULL, 0);

    /* PWRCTL1/2 */
    command_params[0] = _ili9341_calc_pwrctl1(CONFIG_ILI9341_GVDD);
    lcd_ll_write_cmd(dev, LCD_CMD_PWCTRL1, command_params, 1);

    command_params[0] = 0x10; /* PWRCTL 0 0 0 */
    lcd_ll_write_cmd(dev, LCD_CMD_PWCTRL2, command_params, 1);

    /* VCOMCTL */
    command_params[0] = _ili9341_calc_vmh(CONFIG_ILI9341_VCOMH);
    command_params[1] = _ili9341_calc_vml(CONFIG_ILI9341_VCOML);
    lcd_ll_write_cmd(dev, LCD_CMD_VMCTRL1, command_params, 2);

    command_params[0] = 0x86;
    lcd_ll_write_cmd(dev, LCD_CMD_VMCTRL2, command_params, 1);

    /* Memory access CTL */
    command_params[0] = dev->params->rotation;
    command_params[0] |= dev->params->rgb ? 0 : LCD_MADCTL_BGR;
    lcd_ll_write_cmd(dev, LCD_CMD_MADCTL, command_params, 1);

    /* Frame control */
    command_params[0] = 0x00;
    command_params[1] = 0x18;
    lcd_ll_write_cmd(dev, LCD_CMD_FRAMECTL1, command_params, 2);

    /* Display function control */
    command_params[0] = 0x08;
    command_params[1] = 0x82;
    /* number of lines, see datasheet p. 166 (DISCTRL::NL) */
    command_params[2] = (params->lines >> 3) - 1;
    lcd_ll_write_cmd(dev, LCD_CMD_DFUNC, command_params, 3);

    /* Pixel format */
    command_params[0] = 0x55; /* 16 bit mode */
    lcd_ll_write_cmd(dev, LCD_CMD_PIXSET, command_params, 1);

    command_params[0] = 0x01;
    lcd_ll_write_cmd(dev, LCD_CMD_GAMSET, command_params, 1);

    /* Gamma correction */
    {
        static const uint8_t gamma_pos[] = {
            0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1,
            0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00
        };
        lcd_ll_write_cmd(dev, LCD_CMD_PGAMCTRL, gamma_pos,
                         sizeof(gamma_pos));
    }
    {
        static const uint8_t gamma_neg[] = {
            0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1,
            0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F
        };
        lcd_ll_write_cmd(dev, LCD_CMD_NGAMCTRL, gamma_neg,
                         sizeof(gamma_neg));

    }

    if (dev->params->inverted) {
        lcd_ll_write_cmd(dev, LCD_CMD_DINVON, NULL, 0);
    }
    /* Sleep out (turn off sleep mode) */
    lcd_ll_write_cmd(dev, LCD_CMD_SLPOUT, NULL, 0);
    /* Display on */
    lcd_ll_write_cmd(dev, LCD_CMD_DISPON, NULL, 0);

    /* Finally release the device */
    lcd_ll_release(dev);

    return 0;
}

const lcd_driver_t lcd_ili9341_driver = {
    .init = _init,
    .set_area = NULL, /* default implementation is used */
};
