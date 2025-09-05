/*
 * SPDX-FileCopyrightText: 2015 PHYTEC Messtechnik GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_tcs37727
 * @{
 *
 * @file
 * @brief       Register definitions for the TCS37727 driver.
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 */

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef TCS37727_AG_THRESHOLD_LOW
#define TCS37727_AG_THRESHOLD_LOW       200
#endif

#ifndef TCS37727_AG_THRESHOLD_HIGH
#define TCS37727_AG_THRESHOLD_HIGH      (65535 - TCS37727_AG_THRESHOLD_LOW)
#endif

/**
 * @name    Register Map
 *
 * @note    All setting register are defined for repeated byte protocol transaction.
 * @{
 */
#define TCS37727_ENABLE             0x80 /**< Enables states and interrupts */
#define TCS37727_ATIME              0x81 /**< RGBC time */
#define TCS37727_PTIME              0x82 /**< Proximity time */
#define TCS37727_WTIME              0x83 /**< Wait time */
#define TCS37727_AILTL              0x04 /**< Clear interrupt low threshold low byte */
#define TCS37727_AILTH              0x05 /**< Clear interrupt low threshold high byte */
#define TCS37727_AIHTL              0x06 /**< Clear interrupt high threshold low byte */
#define TCS37727_AIHTH              0x07 /**< Clear interrupt high threshold high byte */
#define TCS37727_PILTL              0x08 /**< Proximity interrupt low threshold low byte */
#define TCS37727_PILTH              0x09 /**< Proximity interrupt low threshold high byte */
#define TCS37727_PIHTL              0x0A /**< Proximity interrupt high threshold low byte */
#define TCS37727_PIHTH              0x0B /**< Proximity interrupt high threshold high byte */
#define TCS37727_PERS               0x8C /**< Interrupt persistence filters */
#define TCS37727_CONFIG             0x8D /**< Configuration */
#define TCS37727_PPULSE             0x8E /**< Proximity pulse count */
#define TCS37727_CONTROL            0x8F /**< Gain control register */
#define TCS37727_ID                 0x92 /**< Device IDID */
#define TCS37727_STATUS             0x93 /**< Device status */
#define TCS37727_CDATA              0x14 /**< Clear ADC data low byte */
#define TCS37727_CDATAH             0x15 /**< Clear ADC data high byte */
#define TCS37727_RDATA              0x16 /**< Red ADC data low byte */
#define TCS37727_RDATAH             0x17 /**< Red ADC data high byte */
#define TCS37727_GDATA              0x18 /**< Green ADC data low byte */
#define TCS37727_GDATAH             0x19 /**< Green ADC data high byte */
#define TCS37727_BDATA              0x1A /**< Blue ADC data low byte */
#define TCS37727_BDATAH             0x1B /**< Blue ADC data high byte */
#define TCS37727_PDATA              0x1C /**< Proximity ADC data low byte */
#define TCS37727_PDATAH             0x1D /**< Proximity ADC data high byte */
/** @} */

/**
 * @name    Command Register
 * @{
 */
#define TCS37727_BYTE_TRANS         0x80 /**< Repeated byte protocol transaction */
#define TCS37727_INC_TRANS          0xA0 /**< Auto-increment protocol transaction */
#define TCS37727_SF_PICLR           0xE5 /**< Proximity interrupt clear */
#define TCS37727_SF_CICLR           0xE6 /**< Clear channel interrupt clear */
#define TCS37727_SF_PCICLR          0xE7 /**< Proximity and Clear channel interrupt clear */
/** @} */

/**
 * @name    Enable Register
 * @{
 */
#define TCS37727_ENABLE_PIEN        (1 << 5) /**< Proximity interrupt enable */
#define TCS37727_ENABLE_AIEN        (1 << 4) /**< Clear channel interrupt enable */
#define TCS37727_ENABLE_WEN         (1 << 3) /**< Wait enable, activates the wait feature */
#define TCS37727_ENABLE_PEN         (1 << 2) /**< Proximity enable, activates the proximity function */
#define TCS37727_ENABLE_AEN         (1 << 1) /**< RGBC enable, actives the two-channel ADC */
#define TCS37727_ENABLE_PON         (1 << 0) /**< Power ON */
/** @} */

/**
 * @name    Control Register
 * @{
 */
#define TCS37727_CONTROL_PDRIVE_100     0x00 /**< 100 mA LED Drive Strength */
#define TCS37727_CONTROL_PDRIVE_50      0x04 /**< 50 mA LED Drive Strength */
#define TCS37727_CONTROL_PDRIVE_25      0x08 /**< 25 mA LED Drive Strength */
#define TCS37727_CONTROL_PDRIVE_12      0x0C /**< 12.5 mA LED Drive Strength */
#define TCS37727_CONTROL_PDRIVE_MASK    0x0C /**< PDRIVE Mask */
#define TCS37727_CONTROL_AGAIN_1        0x00 /**<  1x gain RGBC Gain Value */
#define TCS37727_CONTROL_AGAIN_4        0x01 /**<  4x gain RGBC Gain Value */
#define TCS37727_CONTROL_AGAIN_16       0x02 /**< 16x gain RGBC Gain Value */
#define TCS37727_CONTROL_AGAIN_60       0x03 /**< 60x gain RGBC Gain Value */
#define TCS37727_CONTROL_AGAIN_MASK     0x03 /**< AGAIN Mask */
/** @} */

/**
 * @name    Device ID
 * @{
 */
#define TCS37727_ID_VALUE           0x49
/** @} */

/**
 * @name    Predefined ATIME register values.
 * @{
 */
#define TCS37727_ATIME_MIN          2400    /* 2.4ms integration time, max count 1024 */
#define TCS37727_ATIME_MAX          614000  /* 614ms integration time, max count 0xffff */

#define TCS37727_ATIME_TO_REG(val)  (256 - (uint8_t)((val) / 2400))
#define TCS37727_ATIME_TO_US(reg)   ((256 - (uint8_t)(reg)) * 2400)
/** @} */

/**
 * @name    Coefficients for Lux and CT Equations (DN40)
 *
 * @note    Coefficients in integer format, multiplied by 1000.
 * @{
 */
#define DGF_IF                      310
#define R_COEF_IF                   136
#define G_COEF_IF                   1000
#define B_COEF_IF                   -444
#define CT_COEF_IF                  3810
#define CT_OFFSET_IF                1391
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
