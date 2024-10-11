/*
 * Copyright (C) 2017 - 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_tmp00x
 *
 * @{
 * @file
 * @brief       Register definitions for TMP00X (TMP006 and TMP007) devices
 *
 * @author      Sebastian Meiling <s@mlng.net>
 * @author      Jannes Volkens <jannes.volkens@haw-hamburg.de>
 */

#ifndef TMP00X_REGS_H
#define TMP00X_REGS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    TMP00x registers
 * @{
 */
 #define TMP00X_REGS_V_OBJECT        0x00 /**< Sensor Voltage Register */
 #define TMP00X_REGS_T_AMBIENT       0x01 /**< Ambient Temperature Register */
 #define TMP00X_REGS_CONFIG          0x02 /**< Configuration Register */

 #define TMP00X_CONFIG_RST           (1 << 15) /**< Reset register */

 #define TMP00X_CONFIG_MOD_SHIFT     (12U) /**< Mode of operation shift */
 #define TMP00X_CONFIG_MOD_MASK      (0x7000) /**< Mode of operation mask */
 #define TMP00X_CONFIG_MOD(x)        (((uint16_t)(((uint16_t)(x)) \
                                     << TMP00X_CONFIG_MOD_SHIFT)) \
                                     & TMP00X_CONFIG_MOD_MASK) /**< Mode of operation */
 #define TMP00X_CONFIG_MOD_CC        (0x07) /**< Sensor and ambient continuous conversion */
 #define TMP00X_CONFIG_MOD_OFF       (0x00) /**< Power-down */

 #define TMP00X_CONFIG_CR_SHIFT      (9U) /**< ADC conversion rate shift */
 #define TMP00X_CONFIG_CR_MASK       (0x0E00) /**< ADC conversion rate mask */
 #define TMP00X_CONFIG_CR(x)         (((uint16_t)(((uint16_t)(x)) \
                                     << TMP00X_CONFIG_CR_SHIFT)) \
                                     & TMP00X_CONFIG_CR_MASK) /**< ADC conversion rate */

 #define TMP00X_DRDY_PIN_EN          (1 << 8) /**< EN: DRDY enable bit */
/** @} */

 /**
  * @ingroup  config
  * @{
  */
 #ifdef MODULE_TMP006
 #define TMP00X_REGS_DEVICE_ID    0xFF /**< Device ID Register */
 #define TMP00X_REGS_READ_STATUS  TMP00X_REGS_CONFIG
 #define TMP00X_DRDY              (1 << 7)  /**< DRDY: Data ready bit */
 #define TMP00X_DID_VALUE         (0x0067)  /**< Device ID */
 #elif defined(MODULE_TMP007)
 #define TMP00X_REGS_DEVICE_ID    0x1F /**< Device ID Register */
 #define TMP00X_REGS_READ_STATUS  TMP007_REGS_STATUS
 #define TMP00X_DRDY              (1 << 14) /**< DRDY: Data ready bit */
 #define TMP00X_DID_VALUE         (0x0078)  /**< Device ID */
 #else
 #error "TMP00X DRIVER not selected or supported"
 #endif
/** @} */

 /**
  * @name    TMP006 registers
  * @{
  */
 #define TMP006_REGS_MANUFACTURER_ID        0xFE /**< Manufacturer ID Register */
/** @} */

 /**
  * @name    TMP007 registers
  * @{
  */
 #define TMP007_REGS_OBJ_TEMP               0x03 /**< Object temperature result register */
 #define TMP007_REGS_STATUS                 0x04 /**< Status Register */
 #define TMP007_REGS_STAT_MASK_EN           0x05 /**< Mask and enable register*/
 #define TMP007_REGS_OBJ_HIGH_LIMIT_TEMP    0x06 /**< Object temperature high limit register */
 #define TMP007_REGS_OBJ_LOW_LIMIT_TEMP     0x07 /**< Object temperature low limit register */
 #define TMP007_REGS_LOCAL_HIGH_LIMIT_TEMP  0x08 /**< TDIE temperature high limit register */
 #define TMP007_REGS_LOCAL_LOW_LIMIT_TEMP   0x09 /**< TDIE temperature low limit register */
 #define TMP007_REGS_S0_COEFFCIENT          0x0A /**< S0 coefficient register */
 #define TMP007_REGS_A0_COEFFCIENT          0x0B /**< A0 coefficient register */
 #define TMP007_REGS_A1_COEFFCIENT          0x0C /**< A1 coefficient register */
 #define TMP007_REGS_B0_COEFFCIENT          0x0D /**< B0 coefficient register */
 #define TMP007_REGS_B1_COEFFCIENT          0x0E /**< B1 coefficient register */
 #define TMP007_REGS_B2_COEFFCIENT          0x0F /**< B2 coefficient register */
 #define TMP007_REGS_C_COEFFCIENT           0x10 /**< C coefficient register */
 #define TMP007_REGS_TC0_COEFFCIENT         0x11 /**< TC0 coefficient register */
 #define TMP007_REGS_TC1_COEFFCIENT         0x12 /**< TC1 coefficient register */
 #define TMP007_REGS_MEM_ACCES              0x2A /**< Memory access register */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* TMP00X_REGS_H */
