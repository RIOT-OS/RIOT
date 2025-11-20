/*
 * Copyright (C) 2018 Koen Zandberg
 *               2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_st77xx
 * @{
 *
 * @file
 * @brief       Specific internal definitions for the ST7789 controller variant
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "st77xx_internal.h"

/**
 * @name LCD ST7789 commands
 *
 * LCD commands extension available for ST7789 LCD controllers
 * @{
 */
#define LCD_CMD_RAMWRC          0x3c    /**< Memory Write Continue */
#define LCD_CMD_RAMRDC          0x3e    /**< Memory Read Continue */
#define LCD_CMD_PORCTRL         0xb2    /**< Porch Control */
#define LCD_CMD_GCTRL           0xb7    /**< Gate Control */
#define LCD_CMD_VCOMS           0xbb    /**< VCOM Setting */
#define LCD_CMD_LCMCTRL         0xc0    /**< LCM Control */
#define LCD_CMD_VDVVRHEN        0xc2    /**< VDV and VRH Command Enable */
#define LCD_CMD_VRHS            0xc3    /**< VRH Set */
#define LCD_CMD_VDVS            0xc4    /**< VDV Set */
#define LCD_CMD_VCMOFSET        0xc4    /**< VCOM Offset Set */
#define LCD_CMD_FRCTRL2         0xc6    /**< Frame Rate Control in Normal Mode */
#define LCD_CMD_PWCTRL1X        0xd0    /**< Power Control 1 */
/** @} */

/**
 * @brief   ST7789 controller specific initialization part
 */
int st7789_init(lcd_t *dev, const lcd_params_t *params);

#ifdef __cplusplus
}
#endif
