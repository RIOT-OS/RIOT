/*
 * Copyright (C) 2018 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_st7735
 * @{
 *
 * @file
 * @brief       Device driver implementation for the st7735 display controller
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#ifndef ST7735_INTERNAL_H
#define ST7735_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lcd_internal.h"

/**
 * @name LCD ST7735 commands
 *
 * LCD commands extension available for st7735 LCD controllers
 * @{
 */
#define LCD_CMD_INVCTR          0xb4    /**< Display Inversion Control */
#define LCD_CMD_PWCTRL3         0xc2    /**< Power control 3 */
#define LCD_CMD_PWCTRL4         0xc3    /**< Power control 4 */
#define LCD_CMD_PWCTRL5         0xc4    /**< Power control 5 */
#define LCD_CMD_PWCTRL6         0xfc    /**< Power control 6 */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ST7735_INTERNAL_H */
