/*
 * SPDX-FileCopyrightText: 2019 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_opt3001
 *
 * @{
 * @file
 * @brief       Register definitions for OPT3001 devices
 *
 * @author      Jannes Volkens <jannes.volkens@haw-hamburg.de>
 */

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name  OPT3001 registers
 * @{
 */
 #define OPT3001_REGS_RESULT                  0x00 /**< Result register */
 #define OPT3001_REGS_CONFIG                  0x01 /**< Configuration register */
 #define OPT3001_REGS_LOW_LIMIT               0x02 /**< Low Limit register */
 #define OPT3001_REGS_HIGH_LIMIT              0x03 /**< High Limit register */
 #define OPT3001_REGS_MANUFACTURER_ID         0x7E /**< Manufacturer ID register */
 #define OPT3001_REGS_DEVICE_ID               0x7F /**< Device ID register */

 #define OPT3001_DID_VALUE                    (0x3001) /**< Device ID value */

 #define OPT3001_CONFIG_RESET                 (0xC810) /**< Reset value */

 #define OPT3001_CONFIG_RN_FSR                (0xC000) /**< Automatic full-scale setting mode */

 #define OPT3001_REGS_CONFIG_CT_MASK          (0x0800) /**< Conversion time mask */
 #define OPT3001_REGS_CONFIG_CT(x)            (((uint16_t)(x)) \
                                              & OPT3001_REGS_CONFIG_CT_MASK) /**< Conversion time field */

 #define OPT3001_CONFIG_M_SHUTDOWN            (0x0) /**< Shutdown mode */
 #define OPT3001_CONFIG_M_SINGLE              (0x1) /**< Single-shot mode */
 #define OPT3001_CONFIG_M_CONTINUOUS          (0x2) /**< Continuous mode (also 0x3) */
 #define OPT3001_CONFIG_MOC_SHIFT             (9U) /**< Mode of conversion shift */
 #define OPT3001_CONFIG_MOC_MASK              (0x0600) /**< Mode of conversion mask */
 #define OPT3001_REGS_CONFIG_MOC(x)           (((uint16_t)(((uint16_t)(x)) \
                                              << OPT3001_CONFIG_MOC_SHIFT)) \
                                              & OPT3001_CONFIG_MOC_MASK) /**< Mode of conversion field */

 #define OPT3001_REGS_CONFIG_OVF              (1 << 8) /**< Overflow flag field */
 #define OPT3001_REGS_CONFIG_CRF              (1 << 7) /**< Conversion ready field */
 #define OPT3001_REGS_CONFIG_FH               (1 << 6) /**< Flag high field */
 #define OPT3001_REGS_CONFIG_FL               (1 << 5) /**< Flag low field */
 #define OPT3001_REGS_CONFIG_L                (1 << 4) /**< Latch field */
 #define OPT3001_REGS_CONFIG_POL              (1 << 3) /**< Polarity field */
 #define OPT3001_REGS_CONFIG_ME               (1 << 2) /**< Mask exponent field */

 #define OPT3001_REGS_CONFIG_FC_MASK          (0x0003) /**< Fault count field mask */

 #define OPT3001_REGS_LOW_LIMIT_EOC_ENABLE    (0xC000) /**< End-of-conversion enable */

 #define OPT3001_REGS_REG_EXPONENT(x)         ((x) >> 12) /**< Exponent */
 #define OPT3001_REGS_REG_MANTISSA(x)         ((x) & 0xFFF) /**< Mantissa */

/*
 * Time to wait for the conversion to complete.
 * The data sheet of the device (sect. 6.5) indicates that the conversion time is the integration time plus 3 ms.
 * It has been added a bit more time just to be safe.
 */
 #define OPT3001_REGS_INT_TIME_SHORT          100000 /**< Integration time of 100ms */
 #define OPT3001_REGS_INT_TIME_LONG           800000 /**< Integration time of 800ms */

 #define OPT3001_CONVERSION_TIME_OFFSET       4000 /**< Conversion time offset */

 #define OPT3001_CONVERSION_TIME_COMBINED     OPT3001_REGS_INT_TIME_SHORT \
                                              + OPT3001_REGS_INT_TIME_LONG \
                                              + OPT3001_CONVERSION_TIME_OFFSET /**< Combination of the conversion times */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
