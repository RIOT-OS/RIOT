/*
 * SPDX-FileCopyrightText: 2021 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_lm75
 *
 * @{
 * @file
 * @brief       Registers for the lm75 and derived (lm75a and tmp1075) temperature sensors.
 *
 * @author      Vitor Batista <vitor.batista@ml-pa.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LM75 register list
 * @{
 */
#define LM75_TEMP_REG            (0x00) /**< Temperature register pointer */
#define LM75_CONF_REG            (0x01) /**< Configuration register pointer */
#define LM75_THYST_REG           (0x02) /**< Hysteresis register pointer */
#define LM75_TOS_REG             (0x03) /**< Overtemperature shutdown register pointer */
/** @} */

/**
 * @name    Device Operation mode
 * @{
 */
#define NORMAL_MODE         0  /**< Continuous conversion mode */
#define SHUTDOWN_MODE       1  /**< Shutdown mode ON */
/** @} */

/**
 * @name    Device Thermostat operation mode
 * @{
 */
#define COMPARATOR_MODE     0  /**< OS operation in comparator mode */
#define INTERRUPT_MODE      1  /**< OS operation in interrupt mode */
/** @} */

/**
 * @name    OS polarity
 * @{
 */
#define OS_ACTIVE_LOW       0  /**< OS pin active on Low voltage */
#define OS_ACTIVE_HIGH      1  /**< OS pin active on positive voltage */
/** @} */

/**
 * @name    Consecutive fault measurements to trigger the alert function
 * @{
 */
#define FAULT_1         0   /**< OS/ALERT active after 1 fault */
#define FAULT_2         1   /**< OS/ALERT active after 2 faults */
/** @} */

/**
 * @name    LM75A exclusive registers
 * @{
 */
#define FAULT_4         2   /**< OS active after 4 faults */
#define FAULT_6         3   /**< OS active after 6 faults */
/** @} */

/**
 * @name    TMP1075 exclusive registers
 * @{
 */
#define TMP1075_DEVICE_ID_REG   (0x0F) /**< Device ID register */

/**
 * @name    fault queue values
 * @{
 */
#define FAULT_3                 2   /**< ALERT active after 3 faults */
#define FAULT_4_TMP1075         3   /**< ALERT active after 4 faults */
/** @} */

/**
 * @name    Conversion rate setting when device is in continuous conversion mode
 * @{
 */
#define TMP1075_CONV_RATE_REG_27H    0   /**< 27.5ms conversion rate */
#define TMP1075_CONV_RATE_REG_55     1   /**< 55ms conversion rate */
#define TMP1075_CONV_RATE_REG_110    2   /**< 110ms conversion rate */
#define TMP1075_CONV_RATE_REG_220    3   /**< 220ms conversion rate */
/** @} */
/** @} *//* end of "TMP1075 exclusive registers" group */

/**
 * @name    Constants for the configuration register
 * @{
 */
#define LM75_CONFIG_SHUTDOWN_MODE    0x01   /**< shut down sensor to conserve power */
#define TMP1075_CONFIG_ONE_SHOT_MODE 0x81   /**< perform a single measurement */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
