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
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define MAX31343_I2C_ADDR           (0x68U) /**< 7-bit I2C device address (slave byte = 0xD0) */

/* Status/Config registers */
#define MAX31343_REG_STATUS         (0x00U) /**< Status register */
#define MAX31343_REG_INT_EN         (0x01U) /**< Interrupt enable register */
#define MAX31343_REG_RTC_RESET      (0x02U) /**< RTC reset register */
#define MAX31343_REG_RTC_CFG1       (0x03U) /**< RTC configuration register 1 */
#define MAX31343_REG_RTC_CFG2       (0x04U) /**< RTC configuration register 2 */
#define MAX31343_REG_TIMER_CFG      (0x05U) /**< Timer configuration register */
#define MAX31343_REG_TEMP_MSB       (0x1AU) /**< Temperature MSB register */
#define MAX31343_REG_TEMP_LSB       (0x1BU) /**< Temperature LSB register */
#define MAX31343_REG_TS_CONFIG      (0x1CU) /**< Temperature sensor configuration register */

#define MAX31343_REG_TIME0          (0x06U) /**< First time register (seconds) */
#define MAX31343_TIME_LEN           (7U)    /**< Number of time registers (sec..year) */

#define MAX31343_RTC_CFG1_ENOSC     (1U << 1)   /**< Enable oscillator bit in RTC_CFG1 */

#define MAX31343_RTC_CFG2_SQW_HZ_MASK   (0x07U) /**< SQW frequency field mask in RTC_CFG2 */

#define MAX31343_MONTH_CENTURY      (1U << 7)   /**< Century bit in month register */

#define MAX31343_TS_AUTOMODE        (1U << 7)   /**< Automatic temperature conversion enable bit */
#define MAX31343_TS_TTSINT_MASK     (0x38U)     /**< Temperature conversion interval field mask */
#define MAX31343_TS_TTSINT_SHIFT    (3U)        /**< Temperature conversion interval field shift */

#define MAX31343_REG_ALM1_SEC       (0x0DU) /**< Alarm 1 seconds register */
#define MAX31343_REG_ALM1_MIN       (0x0EU) /**< Alarm 1 minutes register */
#define MAX31343_REG_ALM1_HRS       (0x0FU) /**< Alarm 1 hours register */
#define MAX31343_REG_ALM1_DAY_DATE  (0x10U) /**< Alarm 1 day/date register */
#define MAX31343_REG_ALM1_MON       (0x11U) /**< Alarm 1 month register */
#define MAX31343_REG_ALM1_YEAR      (0x12U) /**< Alarm 1 year register */
#define MAX31343_ALM1_LEN           (6U)    /**< Number of alarm 1 registers */

#define MAX31343_ALM_MASK_BIT       (1U << 7)   /**< Alarm mask bit (disables field matching) */
#define MAX31343_ALM_DY_DT          (1U << 6)   /**< Day/date select bit in alarm day register */

#define MAX31343_STATUS_A1F         (1U << 0)   /**< Alarm 1 flag in status register */
#define MAX31343_STATUS_A2F         (1U << 1)   /**< Alarm 2 flag in status register */
#define MAX31343_STATUS_OSF         (1U << 6)   /**< Oscillator stop flag; set on power-on,
                                                    cleared by writing time registers */

#define MAX31343_INTEN_A1IE         (1U << 0)   /**< Alarm 1 interrupt enable bit */
#define MAX31343_INTEN_A2IE         (1U << 1)   /**< Alarm 2 interrupt enable bit */

#define MAX31343_REG_TRICKLE        (0x19U)         /**< Trickle charger configuration register */
#define MAX31343_TRICKLE_TCHE_SHIFT (4U)            /**< TCHE field shift in trickle register */
#define MAX31343_TRICKLE_TCHE_MASK  (0xF0U)         /**< TCHE field mask in trickle register */
#define MAX31343_TRICKLE_TCHE_ENABLE (0x5U)         /**< Only value that enables trickle charger;
                                                        all other values disable it */
#define MAX31343_TRICKLE_D_MASK     (0x0FU)         /**< D_TRICKLE field mask */
#define MAX31343_TRICKLE_D_CHARGER_OFF (1U << 3)    /**< Charger path disable bit (0=ON, 1=OFF) */
#define MAX31343_TRICKLE_D_DIODE    (1U << 2)       /**< Diode select bit (0=no diode,
                                                        1=with diode) */
#define MAX31343_TRICKLE_D_RES_MASK (0x03U)         /**< Resistor selection field mask */

#ifdef __cplusplus
}
#endif
