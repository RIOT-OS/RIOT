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
 * @ingroup     drivers_st77xx
 * @{
 *
 * @file
 * @brief       Device driver implementation for the ST77xx display controller
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

#include "st77xx.h"
#include "st77xx_internal.h"
#include "lcd.h"
#include "lcd_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static int _init(lcd_t *dev, const lcd_params_t *params)
{
    if (params->cntrl == ST77XX_CNTRL_ST7735) {
        assert(params->lines <= 162);
        assert(params->rgb_channels <= 132);
    }
    if (params->cntrl == ST77XX_CNTRL_ST7789) {
        assert(params->lines <= 320);
        assert(params->rgb_channels <= 240);
    }
    if (params->cntrl == ST77XX_CNTRL_ST7796) {
        assert(params->lines <= 480);
        assert(params->rgb_channels <= 320);
    }
    else {
        assert(0);
    }

    uint8_t command_params[4] = { 0 };

    /* Acquire once and release at the end */
    lcd_ll_acquire(dev);

    /* Soft Reset */
    lcd_ll_write_cmd(dev, LCD_CMD_SWRESET, NULL, 0);
    ztimer_sleep(ZTIMER_MSEC, 120);

    /* Display off */
    lcd_ll_write_cmd(dev, LCD_CMD_DISPOFF, NULL, 0);

    /* TODO: instead of using a wrong initialization command sequence for power
     * and frame control, default values after reset are used. */

    /* Memory access CTL */
    command_params[0] = dev->params->rotation;
    command_params[0] |= dev->params->rgb ? 0 : LCD_MADCTL_BGR;
    lcd_ll_write_cmd(dev, LCD_CMD_MADCTL, command_params, 1);

    /* Pixel format */
    command_params[0] = 0x55; /* 16 bit mode */
    lcd_ll_write_cmd(dev, LCD_CMD_PIXSET, command_params, 1);

    command_params[0] = 0x01;
    lcd_ll_write_cmd(dev, LCD_CMD_GAMSET, command_params, 1);

    /* Gamma correction */
    {
        static const uint8_t gamma_pos[] = {
            0xf0, 0x09, 0x0b, 0x06, 0x04, 0x15, 0x2f,
            0x54, 0x42, 0x3c, 0x17, 0x14, 0x18, 0x1b,
        };
        lcd_ll_write_cmd(dev, LCD_CMD_PGAMCTRL, gamma_pos,
                         sizeof(gamma_pos));
    }
    {
        static const uint8_t gamma_neg[] = {
            0xe0, 0x09, 0x0b, 0x06, 0x04, 0x03, 0x2b,
            0x43, 0x42, 0x3b, 0x16, 0x14, 0x17, 0x1b
        };
        lcd_ll_write_cmd(dev, LCD_CMD_NGAMCTRL, gamma_neg,
                         sizeof(gamma_neg));
    }

    if (dev->params->inverted) {
        lcd_ll_write_cmd(dev, LCD_CMD_DINVON, NULL, 0);
    }
    /* Sleep out (turn off sleep mode) */
    lcd_ll_write_cmd(dev, LCD_CMD_SLPOUT, NULL, 0);

    /* Normal display mode on */
    lcd_ll_write_cmd(dev, LCD_CMD_NORON, NULL, 0);
    ztimer_sleep(ZTIMER_MSEC, 1);

    /* Display on */
    lcd_ll_write_cmd(dev, LCD_CMD_DISPON, NULL, 0);

    /* Finally release the device */
    lcd_ll_release(dev);

    return 0;
}

const lcd_driver_t lcd_st77xx_driver = {
    .init = _init,
    .set_area = NULL, /* default implementation is used */
};
