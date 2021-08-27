/*
 * Copyright (C) 2018 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ili9341
 * @{
 *
 * @file
 * @brief       Device driver implementation for the ili9341 display controller
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#ifndef ILI9341_INTERNAL_H
#define ILI9341_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name ILI9341 commands
 *
 * Not exhaustive, please extend when required
 * @{
 */
#define ILI9341_CMD_SWRESET         0x01    /**< Software reset */
#define ILI9341_CMD_RDDIDIF         0x04    /**< Read display ID */
#define ILI9341_CMD_SPLIN           0x10    /**< Enter sleep mode */
#define ILI9341_CMD_SLPOUT          0x11    /**< Sleep out */
#define ILI9341_CMD_DINVOFF         0x20    /**< Display inversion off */
#define ILI9341_CMD_DINVON          0x21    /**< Display inversion on */

#define ILI9341_CMD_GAMSET          0x26    /**< Gamma Set */
#define ILI9341_CMD_DISPOFF         0x28    /**< Display OFF */
#define ILI9341_CMD_DISPON          0x29    /**< Display ON */
#define ILI9341_CMD_CASET           0x2A    /**< Column Address Set */
#define ILI9341_CMD_PASET           0x2b    /**< Page Address Set */
#define ILI9341_CMD_RAMWR           0x2c    /**< Memory Write */
#define ILI9341_CMD_RAMRD           0x2e    /**< Memory Read */
#define ILI9341_CMD_MADCTL          0x36    /**< Memory data access control */
#define ILI9341_CMD_IDMOFF          0x38    /**< Idle Mode OFF */
#define ILI9341_CMD_IDMON           0x39    /**< Idle Mode ON */
#define ILI9341_CMD_PIXSET          0x3A    /**< COLMOD: Pixel Format Set */
#define ILI9341_CMD_WRDISBV         0x51    /**< Write Display Brightness */
#define ILI9341_CMD_WRCTRLD         0x53    /**< Write Control Display */
#define ILI9341_CMD_RDCTRLD         0x54    /**< Read Control Display */
#define ILI9341_CMD_FRAMECTL1       0xb1    /**< Frame control normal*/
#define ILI9341_CMD_FRAMECTL2       0xb2    /**< Frame control idle */
#define ILI9341_CMD_FRAMECTL3       0xb3    /**< Frame control partial */
#define ILI9341_CMD_DFUNC           0xb6    /**< Display function control */
#define ILI9341_CMD_PWCTRL1         0xc0    /**< Power control 1 */
#define ILI9341_CMD_PWCTRL2         0xc1    /**< Power control 2 */
#define ILI9341_CMD_VMCTRL1         0xc5    /**< VCOM control 1 */
#define ILI9341_CMD_VMCTRL2         0xc7    /**< VCOM control 2 */
#define ILI9341_CMD_PGAMCTRL        0xe0    /**< Positive gamma correction */
#define ILI9341_CMD_NGAMCTRL        0xe1    /**< Negative gamma correction */
#define ILI9341_CMD_IFCTL           0xf6    /**< Interface control */
/** @} */

#define ILI9341_PIXSET_16BIT        0x55    /**< MCU and RGB 16 bit interface */
#define ILI9341_PIXSET_18BIT        0x66    /**< MCU and RGB 18 bit interface (not implemented) */

#ifdef __cplusplus
}
#endif

#endif /* ILI9341_INTERNAL_H */
