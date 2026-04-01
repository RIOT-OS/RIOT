/*
 * SPDX-FileCopyrightText: 2018 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_apds99xx
 * @brief       Register definitions for Broadcom APDS99XX proximity and ambient light sensor
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name    Register addresses
 * @{
 */
#if MODULE_APDS9960
#define APDS99XX_REG_BASE     (0x80)    /**< Register base address is 0x80 */
#else
#define APDS99XX_REG_BASE     (0xa0)    /**< Register base address for autoincrement is 0xa0 */
#endif

#define APDS99XX_REG_ENABLE   (APDS99XX_REG_BASE + 0x00)  /**< Enable states and interrupts */
#define APDS99XX_REG_ATIME    (APDS99XX_REG_BASE + 0x01)  /**< ALS timing register */
#if MODULE_APDS9900 || MODULE_APDS9901 || MODULE_APDS9930
#define APDS99XX_REG_PTIME    (APDS99XX_REG_BASE + 0x02)  /**< Proximity timing register */
#endif
#define APDS99XX_REG_WTIME    (APDS99XX_REG_BASE + 0x03)  /**< Wait timing register */
#define APDS99XX_REG_AILTL    (APDS99XX_REG_BASE + 0x04)  /**< ALS interrupt low threshold low byte */
#define APDS99XX_REG_AILTH    (APDS99XX_REG_BASE + 0x05)  /**< ALS interrupt low threshold high byte */
#define APDS99XX_REG_AIHTL    (APDS99XX_REG_BASE + 0x06)  /**< ALS interrupt high threshold low byte */
#define APDS99XX_REG_AIHTH    (APDS99XX_REG_BASE + 0x07)  /**< ALS interrupt high threshold high byte */
#if !MODULE_APDS9960
#define APDS99XX_REG_PILTL    (APDS99XX_REG_BASE + 0x08)  /**< PRX interrupt low threshold low byte */
#endif
#define APDS99XX_REG_PILTH    (APDS99XX_REG_BASE + 0x09)  /**< PRX interrupt low threshold high byte */
#if !MODULE_APDS9960
#define APDS99XX_REG_PIHTL    (APDS99XX_REG_BASE + 0x0a)  /**< PRX interrupt high threshold low byte */
#endif
#define APDS99XX_REG_PIHTH    (APDS99XX_REG_BASE + 0x0b)  /**< PRX interrupt high threshold high byte */
#define APDS99XX_REG_PERS     (APDS99XX_REG_BASE + 0x0c)  /**< Interrupt persistence filters */
#define APDS99XX_REG_CONFIG   (APDS99XX_REG_BASE + 0x0d)  /**< Configuration register (one) */
#define APDS99XX_REG_PPCOUNT  (APDS99XX_REG_BASE + 0x0e)  /**< Proximity pulse count */
#define APDS99XX_REG_CONTROL  (APDS99XX_REG_BASE + 0x0f)  /**< Control */
#define APDS99XX_REG_ID       (APDS99XX_REG_BASE + 0x12)  /**< Device ID */
#define APDS99XX_REG_STATUS   (APDS99XX_REG_BASE + 0x13)  /**< Device status */

#define APDS99XX_REG_CDATAL   (APDS99XX_REG_BASE + 0x14)  /**< Clear channel / Ch0 ADC data low byte */
#define APDS99XX_REG_CDATAH   (APDS99XX_REG_BASE + 0x15)  /**< Clear channel / Ch0 ADC data high byte */

#if MODULE_APDS9900 || MODULE_APDS9901 || MODULE_APDS9930
#define APDS99XX_REG_IRDATAL  (APDS99XX_REG_BASE + 0x16)  /**< Ch1(IR) ADC data low byte */
#define APDS99XX_REG_IRDATAH  (APDS99XX_REG_BASE + 0x17)  /**< Ch1(IR) ADC data high byte */
#define APDS99XX_REG_PDATAL   (APDS99XX_REG_BASE + 0x18)  /**< Proximity ADC data low byte */
#define APDS99XX_REG_PDATAH   (APDS99XX_REG_BASE + 0x19)  /**< Proximity ADC data high byte */
#endif
#if MODULE_APDS9950 || MODULE_APDS9960
#define APDS99XX_REG_RDATAL   (APDS99XX_REG_BASE + 0x16)  /**< Red channel data low byte */
#define APDS99XX_REG_RDATAH   (APDS99XX_REG_BASE + 0x17)  /**< Red channel data high byte */
#define APDS99XX_REG_GDATAL   (APDS99XX_REG_BASE + 0x18)  /**< Green channel data low byte */
#define APDS99XX_REG_GDATAH   (APDS99XX_REG_BASE + 0x19)  /**< Green channel data high byte */
#define APDS99XX_REG_BDATAL   (APDS99XX_REG_BASE + 0x1a)  /**< Blue channel data low byte */
#define APDS99XX_REG_BDATAH   (APDS99XX_REG_BASE + 0x1b)  /**< Blue channel data high byte */
#endif
#if MODULE_APDS9950
#define APDS99XX_REG_PDATAL   (APDS99XX_REG_BASE + 0x1c)  /**< Proximity ADC data low byte */
#define APDS99XX_REG_PDATAH   (APDS99XX_REG_BASE + 0x1d)  /**< Proximity ADC data high byte */
#endif
#if MODULE_APDS9960
#define APDS99XX_REG_PDATA    (APDS99XX_REG_BASE + 0x1c)  /**< Proximity ADC data (only one byte) */
#endif

#define APDS99XX_REG_CLI_CMD  (0xe7)  /**< Clear ALS and proximity interrupt command */
/** @} */

/**
 * @name    Register structure definitions
 * @{
 */

/* Enable states and interrupts register (APDS99XX_REG_ENABLE) */
#define APDS99XX_REG_GEN      (0x40)  /**< Gesture Enable */
#define APDS99XX_REG_PIEN     (0x20)  /**< Proximity Interrupt Enable */
#define APDS99XX_REG_AIEN     (0x10)  /**< ALS Interrupt Enable */
#define APDS99XX_REG_WEN      (0x08)  /**< Wait Enable */
#define APDS99XX_REG_PEN      (0x04)  /**< Proximity Detect Enable */
#define APDS99XX_REG_AEN      (0x02)  /**< ALS Enable */
#define APDS99XX_REG_PON      (0x01)  /**< Power ON */

/* Device status register (APDS99XX_REG_STATUS) */
#define APDS99XX_REG_CPSAT    (0x80)  /**< Clear Photodiode Saturation */
#define APDS99XX_REG_PGSAT    (0x40)  /**< Analog saturation event occurred */
#define APDS99XX_REG_PINT     (0x20)  /**< Proximity Interrupt */
#define APDS99XX_REG_AINT     (0x10)  /**< ALS Interrupt */
#define APDS99XX_REG_GINT     (0x04)  /**< Gesture Interrupt */
#define APDS99XX_REG_PVALID   (0x02)  /**< Proximity Valid */
#define APDS99XX_REG_AVALID   (0x01)  /**< ALS Valid */

/** Control register (APDS99XX_REG_CONTROL) */
#define APDS99XX_REG_PDRIVE   (0xc0)  /**< LED Drive Strength */
#if !MODULE_APDS9960
#define APDS99XX_REG_PDIODE   (0x30)  /**< Proximity Diode Select */
#endif
#define APDS99XX_REG_PGAIN    (0x0c)  /**< Proximity Gain Control */
#define APDS99XX_REG_AGAIN    (0x03)  /**< ALS (and Color) Gain Control */

/** Configuration register (APDS99XX_REG_CONFIG) */
#define APDS99XX_REG_WLONG    (0x02)  /**< Wait Long */

/** Interrupt persistence filter register (APDS99XX_REG_PERS) */
#define APDS99XX_REG_PPERS    (0xf0)  /**< Proximity Interrupt persistence. */
#define APDS99XX_REG_APERS    (0x0f)  /**< ALS Interrupt persistence. */

#if MODULE_APDS9960
/** Proximity pulse count register (APDS99XX_REG_PPCOUNT) for APDS9960 only */
#define APDS99XX_REG_PPLEN    (0xc0)  /**< Proximity Pulse Length */
#define APDS99XX_REG_PPULSE   (0x3f)  /**< Proximity Pulse Count */
#endif

/** @} */
#ifdef __cplusplus
}
#endif

/** @} */
