/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_lm75
 *
 * @{
 * @file
 * @brief       Registers for the lm75 and derived (lm75a and tmp1075) temperature sensors.
 *
 * @author      Vitor Batista <vitor.batista@ml-pa.com>
 */

#ifndef LM75_REGS_H
#define LM75_REGS_H

#ifdef __cplusplus
extern "C" {
#endif

/* LM75 register list */
#define LM75_TEMP_REG            (0x00) /**< Temperature register pointer */
#define LM75_CONF_REG            (0x01) /**< Configuration register pointer */
#define LM75_THYST_REG           (0x02) /**< Hysteresis register pointer */
#define LM75_TOS_REG             (0x03) /**< Overtemperature shutdown register pointer */

/* Device Operation mode */
#define NORMAL_MODE         0  /**< Continuous conversion mode */
#define SHUTDOWN_MODE       1  /**< Shutdown mode ON */

/* Device Thermostat  operation mode */
#define COMPARATOR_MODE     0  /**< OS operation in comparator mode */
#define INTERRUPT_MODE      1  /**< OS operation in interrupt mode */

/* OS polarity */
#define OS_ACTIVE_LOW       0  /**< OS pin active on Low voltage */
#define OS_ACTIVE_HIGH      1  /**< OS pin active on positive voltage */

/* Consecutive fault measurements to trigger the alert function */
#define FAULT_1         0   /**< OS/ALERT active after 1 fault */
#define FAULT_2         1   /**< OS/ALERT active after 2 faults */

/* LM75A exclusive registers */

#define FAULT_4         2   /**< OS active after 4 faults */
#define FAULT_6         3   /**< OS active after 6 faults */

/* TMP1075 exclusive registers */

/* Device ID register - only available in the TMP1075 sensor */
#define TMP1075_DEVICE_ID_REG   (0x0F) /**< ID register pointer */

/* fault queue values exclusive to the TMP1075 sensor */
#define FAULT_3                 2   /**< ALERT active after 3 faults */
#define FAULT_4_TMP1075         3   /**< ALERT active after 4 faults */

/* Conversion rate setting when device is in continuous conversion mode
 * Only configurable in the TMP1075 sensor */
#define TMP1075_CONV_RATE_REG_27H    0   /**< 27.5ms conversion rate */
#define TMP1075_CONV_RATE_REG_55     1   /**< 55ms conversion rate */
#define TMP1075_CONV_RATE_REG_110    2   /**< 110ms conversion rate */
#define TMP1075_CONV_RATE_REG_220    3   /**< 220ms conversion rate */

#ifdef __cplusplus
}
#endif

#endif /* LM75_REGS_H */
/** @} */
