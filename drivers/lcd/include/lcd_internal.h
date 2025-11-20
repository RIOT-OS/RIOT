/*
 * Copyright (C) 2018 Koen Zandberg
 *               2021 Francisco Molina
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_lcd
 * @{
 *
 * @file
 * @brief       Device driver implementation for the lcd display controller
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name LCD commands
 *
 * Not exhaustive, please extend when required
 * @{
 */
#define LCD_CMD_SWRESET         0x01    /**< Software reset */
#define LCD_CMD_RDDIDIF         0x04    /**< Read display ID */
#define LCD_CMD_RDDST           0x09    /**< Read display status */
#define LCD_CMD_SLPIN           0x10    /**< Enter sleep mode */
#define LCD_CMD_SLPOUT          0x11    /**< Sleep out */
#define LCD_CMD_NORON           0x13    /**< Normal display mode on */
#define LCD_CMD_DINVOFF         0x20    /**< Display inversion off */
#define LCD_CMD_DINVON          0x21    /**< Display inversion on */

#define LCD_CMD_GAMSET          0x26    /**< Gamma Set */
#define LCD_CMD_DISPOFF         0x28    /**< Display OFF */
#define LCD_CMD_DISPON          0x29    /**< Display ON */
#define LCD_CMD_CASET           0x2A    /**< Column Address Set */
#define LCD_CMD_PASET           0x2b    /**< Page Address Set */
#define LCD_CMD_RAMWR           0x2c    /**< Memory Write */
#define LCD_CMD_RAMRD           0x2e    /**< Memory Read */
#define LCD_CMD_MADCTL          0x36    /**< Memory data access control */
#define LCD_CMD_IDMOFF          0x38    /**< Idle Mode OFF */
#define LCD_CMD_IDMON           0x39    /**< Idle Mode ON */
#define LCD_CMD_TEOFF           0x34    /**< Tearing Effect Line Off */
#define LCD_CMD_TEON            0x35    /**< Tearing Effect Line On */
#define LCD_CMD_COLMOD          0x3A    /**< Interface Pixel Format Set */
#define LCD_CMD_PIXSET          0x3A    /**< COLMOD: Pixel Format Set */
#define LCD_CMD_RAMWRC          0x3c    /**< Memory Write Continue */
#define LCD_CMD_RAMRDC          0x3e    /**< Memory Read Continue */
#define LCD_CMD_WRDISBV         0x51    /**< Write Display Brightness */
#define LCD_CMD_WRCTRLD         0x53    /**< Write Control Display */
#define LCD_CMD_RDCTRLD         0x54    /**< Read Control Display */
#define LCD_CMD_FRAMECTL1       0xb1    /**< Frame control normal*/
#define LCD_CMD_FRAMECTL2       0xb2    /**< Frame control idle */
#define LCD_CMD_FRAMECTL3       0xb3    /**< Frame control partial */
#define LCD_CMD_DFUNC           0xb6    /**< Display function control */
#define LCD_CMD_PWCTRL1         0xc0    /**< Power control 1 */
#define LCD_CMD_PWCTRL2         0xc1    /**< Power control 2 */
#define LCD_CMD_VMCTRL1         0xc5    /**< VCOM control 1 */
#define LCD_CMD_VMCTRL2         0xc7    /**< VCOM control 2 */
#define LCD_CMD_PGAMCTRL        0xe0    /**< Positive gamma correction */
#define LCD_CMD_NGAMCTRL        0xe1    /**< Negative gamma correction */
#define LCD_CMD_IFCTL           0xf6    /**< Interface control */
/** @} */

#define LCD_PIXSET_16BIT        0x55    /**< MCU and RGB 16 bit interface */
#define LCD_PIXSET_18BIT        0x66    /**< MCU and RGB 18 bit interface (not implemented) */

#ifdef __cplusplus
}
#endif
