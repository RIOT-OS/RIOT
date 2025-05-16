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
 * @name LCD ST7796 commands
 *
 * LCD commands extension available for ST7796 LCD controllers
 * @{
 */
#define LCD_CMD_VCMPCTL         0xc5    /**< VCOM Control */
#define LCD_CMD_VCM             0xc6    /**< VCOM Offset */
/** @} */

/**
 * @brief   ST7796 controller specific initialization part
 */
int st7796_init(lcd_t *dev, const lcd_params_t *params);

#ifdef __cplusplus
}
#endif
