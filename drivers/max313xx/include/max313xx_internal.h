/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @file
 * @brief       MAX31343 internal register map and bit field definitions
 *
 * @author      Jakob Müller <ja.mueller@tuhh.de>
 * @author      Christopher Büchse <christopher.buechse@tuhh.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "kernel_defines.h"

#if IS_USED(MODULE_MAX31331) && IS_USED(MODULE_MAX31343)
#  error "Only one driver variant can be used at a time!"
#endif

/**
 * @name MAX31331 and MAX31343 shared registers
 * @{
 */
#if IS_USED(MODULE_MAX31331) || IS_USED(MODULE_MAX31343) || DOXYGEN
#  define MAX313XX_I2C_ADDR           (0x68U) /**< 7-bit I2C device address (slave byte = 0xD0) */

/* Status/Config registers */
#  define MAX313XX_REG_STATUS         (0x00U) /**< Status register */
#  define MAX313XX_REG_INT_EN         (0x01U) /**< Interrupt enable register */
#  define MAX313XX_REG_RTC_RESET      (0x02U) /**< RTC reset register */
#  define MAX313XX_REG_RTC_CFG1       (0x03U) /**< RTC configuration register 1 */
#  define MAX313XX_REG_RTC_CFG2       (0x04U) /**< RTC configuration register 2 */

#  define MAX313XX_TIME_LEN           (7U)    /**< Number of time registers (sec..year) */

#  define MAX313XX_MONTH_CENTURY      (1U << 7)   /**< Century bit in month register */

#  define MAX313XX_ALM1_LEN           (6U)    /**< Number of alarm 1 registers */

#  define MAX313XX_ALM_MASK_BIT       (1U << 7)   /**< Alarm mask bit (disables field matching) */
#  define MAX313XX_ALM_DY_DT          (1U << 6)   /**< Day/date select bit in alarm day register */

#  define MAX313XX_STATUS_A1F         (1U << 0)   /**< Alarm 1 flag in status register */
#  define MAX313XX_STATUS_A2F         (1U << 1)   /**< Alarm 2 flag in status register */
#  define MAX313XX_STATUS_OSF         (1U << 6)   /**< Oscillator stop flag; set on power-on,
                                                       cleared by writing time registers */

#  define MAX313XX_INTEN_A1IE         (1U << 0)   /**< Alarm 1 interrupt enable bit */
#  define MAX313XX_INTEN_A2IE         (1U << 1)   /**< Alarm 2 interrupt enable bit */
#endif

#if IS_USED(MODULE_MAX31331)
#  define MAX313XX_REG_TIMER_CFG      (0x06U)
#  define MAX313XX_REG_TIME0          (0x08U)

#  define MAX313XX_RTC_CFG1_ENOSC     (1U << 0)

#  define MAX313XX_RTC_CFG2_ENCLKO    (1U << 2)
#  define MAX313XX_RTC_CFG2_CLKO_MASK (0x03U)

#  define MAX313XX_REG_ALM1_SEC       (0x0FU)
#  define MAX313XX_REG_ALM1_MIN       (0x10U)
#  define MAX313XX_REG_ALM1_HRS       (0x11U)
#  define MAX313XX_REG_ALM1_DAY_DATE  (0x12U)
#  define MAX313XX_REG_ALM1_MON       (0x13U)
#  define MAX313XX_REG_ALM1_YEAR      (0x14U)

#  define MAX313XX_REG_TRICKLE        (0x1BU)

#  define MAX313XX_TRICKLE_D_MASK     (0x08U)
#  define MAX313XX_TRICKLE_D_DIODE    (1U << 4)
#  define MAX313XX_TRICKLE_D_RES_MASK (0x06U)

#elif IS_USED(MODULE_MAX31343)
#  define MAX313XX_REG_TIMER_CFG      (0x05U)

#  define MAX313XX_REG_TIME0          (0x06U)

#  define MAX313XX_RTC_CFG1_ENOSC     (1U << 1)

#  define MAX313XX_RTC_CFG2_ENCLKO    (1U << 7)
#  define MAX313XX_RTC_CFG2_CLKO_MASK (0x78U)

#  define MAX313XX_REG_ALM1_SEC       (0x0DU)
#  define MAX313XX_REG_ALM1_MIN       (0x0EU)
#  define MAX313XX_REG_ALM1_HRS       (0x0FU)
#  define MAX313XX_REG_ALM1_DAY_DATE  (0x10U)
#  define MAX313XX_REG_ALM1_MON       (0x11U)
#  define MAX313XX_REG_ALM1_YEAR      (0x12U)

#  define MAX313XX_REG_TRICKLE        (0x19U)

#  define MAX313XX_TRICKLE_D_MASK     (0x0FU)
#  define MAX313XX_TRICKLE_D_CHARGER_OFF (1U << 3)
#  define MAX313XX_TRICKLE_D_DIODE    (1U << 2)
#  define MAX313XX_TRICKLE_D_RES_MASK (0x03U)

#elif DOXYGEN
#  define MAX313XX_REG_TIMER_CFG      /**< Timer configuration register */
/**
 * @brief First time register (seconds)
 *
 * @note The MAX31331 has an additional register for fractions of a second,
 *       which is currently unused.
 */
#  define MAX313XX_REG_TIME0

#  define MAX313XX_RTC_CFG1_ENOSC     /**< Enable oscillator bit in RTC_CFG1 */

#  define MAX313XX_RTC_CFG2_ENCLKO    /**< Enable the Clock Output on CLKO */
#  define MAX313XX_RTC_CFG2_CLKO_MASK /**< CLKOUT frequency field mask in RTC_CFG2 */

#  define MAX313XX_REG_ALM1_SEC       /**< Alarm 1 seconds register */
#  define MAX313Xx_REG_ALM1_MIN       /**< Alarm 1 minutes register */
#  define MAX313XX_REG_ALM1_HRS       /**< Alarm 1 hours register */
#  define MAX313XX_REG_ALM1_DAY_DATE  /**< Alarm 1 day/date register */
#  define MAX313XX_REG_ALM1_MON       /**< Alarm 1 month register */
#  define MAX313XX_REG_ALM1_YEAR      /**< Alarm 1 year register */

#  define MAX313XX_REG_TRICKLE        /**< Trickle charger configuration register */

#  define MAX313XX_TRICKLE_D_MASK     /**< DIODE field mask */
#  define MAX313XX_TRICKLE_D_DIODE    /**< Diode select bit (0=no diode, 1=with diode) */
#  define MAX313XX_TRICKLE_D_RES_MASK /**< Resistor selection field mask */
#endif
/** @} */

/* The device specific registers are defined unconditionally to allow using
 * `IS_USED(MODULE_MAX313..)` in the code instead of preprocessor conditions. */
/**
 * @name MAX31331 specific registers
 * @{
 */
#define MAX31331_REG_TS_CFG         (0x05U) /**< Timestamp configuration register */

#define MAX31331_TRICKLE_ENABLE     (1U << 0)  /**< Enable the trickle charger */
#define MAX31331_TRICKLE_RES_SHIFT  (1U << 0)  /**< TRICKLE field shift in trickle register */
/** @} */

/**
 * @name MAX31343 specific registers
 * @{
 */
#define MAX31343_RTC_CFG2_SQW_HZ_MASK (0x07U) /**< SQW frequency field mask in RTC_CFG2 */

#define MAX31343_REG_TEMP_MSB       (0x1AU)  /**< Temperature MSB register */
#define MAX31343_REG_TEMP_LSB       (0x1BU)  /**< Temperature LSB register */
#define MAX31343_REG_TS_CONFIG      (0x1CU)  /**< Temperature sensor configuration register */

#define MAX31343_TS_AUTOMODE        (1U << 7) /**< Automatic temperature conversion enable bit */
#define MAX31343_TS_TTSINT_MASK     (0x38U)  /**< Temperature conversion interval field mask */
#define MAX31343_TS_TTSINT_SHIFT    (3U)     /**< Temperature conversion interval field shift */

#define MAX31343_TRICKLE_TCHE_SHIFT (4U)     /**< TCHE field shift in trickle register */
#define MAX31343_TRICKLE_TCHE_MASK  (0xF0U)  /**< TCHE field mask in trickle register */
#define MAX31343_TRICKLE_TCHE_ENABLE (0x5U)  /**< Only value that enables trickle charger;
                                                  all other values disable it */
/** @} */

#ifdef __cplusplus
}
#endif
