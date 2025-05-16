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
 * @brief       Specific internal definitions for the ST7735 controller variant
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
 * @name LCD ST7735 commands
 *
 * LCD commands extension available for ST7735 LCD controllers
 * @{
 */
#define LCD_CMD_INVCTR          0xb4    /**< Display Inversion Control */
#define LCD_CMD_PWCTRL3         0xc2    /**< Power control 3 */
#define LCD_CMD_PWCTRL4         0xc3    /**< Power control 4 */
#define LCD_CMD_PWCTRL5         0xc4    /**< Power control 5 */
#define LCD_CMD_PWCTRL6         0xfc    /**< Power control 6 */
/** @} */

/**
 * @brief   ST7735 controller specific initialization part
 */
int st7735_init(lcd_t *dev, const lcd_params_t *params);

#ifdef __cplusplus
}
#endif
