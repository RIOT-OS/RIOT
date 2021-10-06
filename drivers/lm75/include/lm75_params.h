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
 * @brief       Default configuration parameters for the lm75 sensors.
 *
 *
 * @author      Vitor Batista <vitor.batista@ml-pa.com>
 *
 * @}
 */

#ifndef LM75_PARAMS_H
#define LM75_PARAMS_H

#include "board.h"
#include "lm75.h"
#include "lm75_regs.h"
#include "kernel_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LM75_PARAMS_I2C
#define LM75_PARAMS_I2C    I2C_DEV(0) /**< I2C BUS used */
#endif

/** 7-bit I2C slave address: 1-0-0-1-A2-A1-A0, where
   the last three bits A2, A1, A0 are defined
   by the voltage level on the ADDR pin */
#ifndef CONFIG_LM75_I2C_ADDR
#define CONFIG_LM75_I2C_ADDR         (0x48) /**< Default I2C address */
#endif

/* Device operation mode configuration - normal or shutdown */
#if IS_ACTIVE(CONFIG_NORMAL_MODE)
#define CONFIG_OPERATION_MODE     NORMAL_MODE
#elif IS_ACTIVE(CONFIG_SHUTDOWN_MODE)
#define CONFIG_OPERATION_MODE     SHUTDOWN_MODE
#endif

#ifndef CONFIG_OPERATION_MODE
#define CONFIG_OPERATION_MODE     NORMAL_MODE /**< Normal Mode is the default */

#endif

/* Device Overtemperature Shutdown operation mode configuration - comparator or interrupt */
#if IS_ACTIVE(CONFIG_COMPARATOR_MODE)
#define CONFIG_THERMOSTAT_MODE      COMPARATOR_MODE
#elif IS_ACTIVE(CONFIG_INTERRUPT_MODE)
#define CONFIG_THERMOSTAT_MODE      INTERRUPT_MODE
#endif

#ifndef CONFIG_THERMOSTAT_MODE
#define CONFIG_THERMOSTAT_MODE      COMPARATOR_MODE /**< Comparator Mode is the default */

#endif

/* Device Overtemperature Shutdown polarity configuration - OS active low or high */
#if IS_ACTIVE(CONFIG_OS_ACTIVE_LOW)
#define CONFIG_OS_POLARITY     OS_ACTIVE_LOW
#elif IS_ACTIVE(CONFIG_OS_ACTIVE_HIGH)
#define CONFIG_OS_POLARITY     OS_ACTIVE_HIGH
#endif

#ifndef CONFIG_OS_POLARITY
#define CONFIG_OS_POLARITY     OS_ACTIVE_LOW  /**< OS pin active on low is the default */

#endif

/* Device Overtemperatue Shutdown fault queue configuration -
 * number of faults that must occur consecutively until OS goes active */
#if IS_ACTIVE(CONFIG_FAULT_1)
#define CONFIG_FAULT_QUEUE     FAULT_1
#elif IS_ACTIVE(CONFIG_FAULT_2)
#define CONFIG_FAULT_QUEUE     FAULT_2
#elif (IS_ACTIVE(CONFIG_FAULT_3) && IS_USED(MODULE_TMP1075))
#define CONFIG_FAULT_QUEUE     FAULT_3
#elif (IS_ACTIVE(CONFIG_FAULT_4) && IS_USED(MODULE_LM75A))
#define CONFIG_FAULT_QUEUE     FAULT_4
#elif (IS_ACTIVE(CONFIG_FAULT_4) && IS_USED(MODULE_TMP1075))
#define CONFIG_FAULT_QUEUE     FAULT_4_TMP1075
#elif (IS_ACTIVE(CONFIG_FAULT_6) && IS_USED(MODULE_LM75A))
#define CONFIG_FAULT_QUEUE     FAULT_6
#endif

#ifndef CONFIG_FAULT_QUEUE
#define CONFIG_FAULT_QUEUE     FAULT_1     /**< One Fault is the default */

#endif

#ifndef LM75_PARAM_INT
#define LM75_PARAM_INT GPIO_UNDEF          /**< Pin used for Interrupts defined by the board */
#endif

#define LM75A_CONV_RATE           (100)    /**< temperature register updated every 100ms */

#define LM75A_OS_RES              (5)      /**< resolution in 0.5ºC */
#define LM75A_OS_MULT             (10)     /**< Must multiply by 10 to get temp in ºC */
#define LM75A_OS_SHIFT            (7)      /**< Only the 9 most significant bits are needed */
#define LM75A_TEMP_RES            (125)    /**< resolution in 0.125ºC */
#define LM75A_TEMP_MULT           (1000)   /**< Must multiply by 1000 to get temp in ºC */
#define LM75A_TEMP_SHIFT          (5)      /**< Only the 11 most significant bits are needed */

#define TMP1075_OS_RES            (625)    /**< resolution in 0.0625ºC */
#define TMP1075_OS_MULT           (10000)  /**< Must multiply by 10000 to get temp in ºC */
#define TMP1075_OS_SHIFT          (4)      /**< Only the 12 most significant bits are needed */
#define TMP1075_TEMP_RES          (625)    /**< resolution in 0.0625ºC */
#define TMP1075_TEMP_MULT         (10000)  /**< Must multiply by 10000 to get temp in ºC */
#define TMP1075_TEMP_SHIFT        (4)      /**< Only the 12 most significant bits are needed */

/* Device conversion rate configuration - only available in the TMP1075 sensor */
#if IS_ACTIVE(CONFIG_TMP1075_CONV_RATE_REG_27H)
#define CONFIG_TMP1075_CONV_RATE_REG     TMP1075_CONV_RATE_REG_27H
#define TMP1075_CONV_RATE                (28)
#elif IS_ACTIVE(CONFIG_TMP1075_CONV_RATE_REG_55)
#define CONFIG_TMP1075_CONV_RATE_REG     TMP1075_CONV_RATE_REG_55
#define TMP1075_CONV_RATE                (55)
#elif IS_ACTIVE(CONFIG_TMP1075_CONV_RATE_REG_110)
#define CONFIG_TMP1075_CONV_RATE_REG     TMP1075_CONV_RATE_REG_110
#define TMP1075_CONV_RATE                (110)
#elif IS_ACTIVE(CONFIG_TMP1075_CONV_RATE_REG_220)
#define CONFIG_TMP1075_CONV_RATE_REG     TMP1075_CONV_RATE_REG_220
#define TMP1075_CONV_RATE                (220)
#endif

#ifndef CONFIG_TMP1075_CONV_RATE_REG
#define CONFIG_TMP1075_CONV_RATE_REG     TMP1075_CONV_RATE_REG_27H /**< Default conv rate is 27.5ms */
#define TMP1075_CONV_RATE                (28) /**< Default conversion rate is 27.5 ms */
/* this was rounded up to 28ms to retain usage of integers and to keep all times in ms */
#endif

#ifndef LM75_PARAMS
#if IS_USED(MODULE_LM75A)
#define LM75_PARAMS        {     .res             = &lm75a_properties, \
                                 .gpio_alarm      = LM75_PARAM_INT, \
                                 .conv_rate       = LM75A_CONV_RATE, \
                                 .i2c_bus         = LM75_PARAMS_I2C, \
                                 .i2c_addr        = CONFIG_LM75_I2C_ADDR, \
                                 .shutdown_mode   = CONFIG_OPERATION_MODE, \
                                 .tm_mode         = CONFIG_THERMOSTAT_MODE, \
                                 .polarity        = CONFIG_OS_POLARITY, \
                                 .fault_q         = CONFIG_FAULT_QUEUE }

#endif

#if IS_USED(MODULE_TMP1075)
#define LM75_PARAMS        {    .res                 = &tmp1075_properties, \
                                .gpio_alarm          = LM75_PARAM_INT, \
                                .conv_rate           = TMP1075_CONV_RATE, \
                                .i2c_bus             = LM75_PARAMS_I2C, \
                                .i2c_addr            = CONFIG_LM75_I2C_ADDR, \
                                .shutdown_mode       = CONFIG_OPERATION_MODE, \
                                .tm_mode             = CONFIG_THERMOSTAT_MODE, \
                                .polarity            = CONFIG_OS_POLARITY, \
                                .fault_q             = CONFIG_FAULT_QUEUE, \
                                .conv_rate_reg       = CONFIG_TMP1075_CONV_RATE_REG }
#endif
#endif /* LM75_PARAMS */

/**
 * @brief LM75 power-up configuration
 */
static const lm75_params_t lm75_params[] =
{
   LM75_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* LM75_PARAMS_H */
/** @} */
