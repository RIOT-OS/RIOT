/*
 * SPDX-FileCopyrightText: 2018 Koen Zandberg
 * SPDX-FileCopyrightText: 2021 Francisco Molina
 * SPDX-FileCopyrightText: 2023 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
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

#include "st77xx.h"
#include "st77xx_internal.h"
#include "st7735_internal.h"
#include "st7789_internal.h"
#include "st7796_internal.h"

#include "lcd.h"
#include "lcd_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static int _init(lcd_t *dev, const lcd_params_t *params)
{
    uint8_t command_params[1] = { 0 };

    /* Acquire once and release at the end */
    lcd_ll_acquire(dev);

    /* Soft Reset (requires 120 ms if in Sleep In mode), default display off */
    lcd_ll_write_cmd(dev, LCD_CMD_SWRESET, NULL, 0);
    ztimer_sleep(ZTIMER_MSEC, 120);

    /* Sleep Out command to leave Sleep In state after reset, requires 120 ms */
    lcd_ll_write_cmd(dev, LCD_CMD_SLPOUT, NULL, 0);
    ztimer_sleep(ZTIMER_MSEC, 120);

    /* COLMOD (3Ah): Interface Pixel Format */
    command_params[0] = 0x55; /* 16 bit mode RGB & Control */
    lcd_ll_write_cmd(dev, LCD_CMD_COLMOD, command_params, 1);

    /* controller specific initialization part called */
    if (IS_USED(MODULE_ST7735) && (params->cntrl == ST77XX_CNTRL_ST7735)) {
        DEBUG("ST7735 used ...\n");
        st7735_init(dev, params);
    }
    else if (IS_USED(MODULE_ST7789) && (params->cntrl == ST77XX_CNTRL_ST7789)) {
        DEBUG("ST7789 used ...\n");
        st7789_init(dev, params);
    }
    else if (IS_USED(MODULE_ST7796) && (params->cntrl == ST77XX_CNTRL_ST7796)) {
        DEBUG("ST7796 used ...\n");
        st7796_init(dev, params);
    }

#if 0 /* no need to write reset defaults, just for documentation purpose */

    /* GAMSET (26h): Gamma Set (== reset defaults) */
    command_params[0] = 0x01;
    lcd_ll_write_cmd(dev, LCD_CMD_GAMSET, command_params, 1);

    /* TEON (35h): Tearing Effect Line ON (== reset defaults) */
    command_params[0] = 0x00;   /* TEM=0 (only V-Blanking) */
    lcd_ll_write_cmd(dev, LCD_CMD_VCOMS, command_params, 1);

#endif /* no need to write reset defaults, just for documentation purpose */

    /* MADCTL (36h): Memory Data Access Control */
    command_params[0] = dev->params->rotation;
    command_params[0] |= dev->params->rgb ? 0 : LCD_MADCTL_BGR;
    lcd_ll_write_cmd(dev, LCD_CMD_MADCTL, command_params, 1);

    /* enable Inversion if configured, reset default is off */
    if (dev->params->inverted) {
        /* INVON (21h): Display Inversion On */
        lcd_ll_write_cmd(dev, LCD_CMD_DINVON, NULL, 0);
    }

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
