/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_samd21
 * @brief           CPU specific definitions for internal peripheral handling
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include <limits.h>

#include "periph_cpu_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Override the default initial PM blocker
 * @todo   Idle modes are enabled by default, deep sleep mode blocked
 */
#define PM_BLOCKER_INITIAL  { .val_u32 = 0x00000001 }

/**
 * @name   SAMD21 sleep modes for PM
 * @{
 */
#define SAMD21_PM_STANDBY       (0U)    /**< Standby mode (stops main clock) */
#define SAMD21_PM_IDLE_2        (1U)    /**< Idle 2 (stops AHB, APB and CPU) */
#define SAMD21_PM_IDLE_1        (2U)    /**< Idle 1 (stops AHB and CPU)      */
#define SAMD21_PM_IDLE_0        (3U)    /**< Idle 0 (stops CPU)              */
/** @} */

/**
 * @brief   Mapping of pins to EXTI lines, -1 means not EXTI possible
 */
static const int8_t exti_config[2][32] = {
#ifdef CPU_MODEL_SAMD21J18A
    { 0,  1,  2,  3,  4,  5,  6,  7, -1,  9, 10, 11, 12, 13, 14, 15,
      0,  1,  2,  3,  4,  5,  6,  7, 12, 13, -1, 15,  8, -1, 10, 11},
    { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
      0,  1, -1, -1, -1, -1,  6,  7, -1, -1, -1, -1, -1, -1, 14, 15},
#elif CPU_MODEL_SAMD21G18A
    { 0,  1,  2,  3,  4,  5,  6,  7, -1,  9, 10, 11, 12, 13, 14, 15,
      0,  1,  2,  3,  4,  5,  6,  7, 12, 13, -1, 15,  8, -1, 10, 11},
    {-1, -1,  2,  3, -1, -1, -1, -1,  8,  9, 10, 11, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1,  6,  7, -1, -1, -1, -1, -1, -1, -1, -1},
#elif CPU_MODEL_SAMR21G18A
    {-1,  1, -1, -1,  4,  5,  6,  7, -1,  9, 10, 11, 12, 13, 14, 15,
     -1,  1,  2,  3, -1, -1,  6,  7, 12, 13, -1, 15,  8, -1, 10, 11},
    { 0, -1,  2,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      0,  1, -1, -1, -1, -1,  6,  7, -1, -1, -1, -1,  8, -1, -1, -1},
#elif CPU_MODEL_SAMR21E18A
    {-1, -1, -1, -1, -1, -1,  6,  7, -1,  9, 10, 11, -1, -1, 14, 15,
     -1,  1,  2,  3, -1, -1, -1, -1, 12, 13, -1, 15,  8, -1, 10, 11},
    { 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      0,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
#else
    #error Please define a proper CPU_MODEL.
#endif
};

/**
 * @brief   Override SPI hardware chip select macro
 *
 * As of now, we do not support HW CS, so we always set it to a fixed value
 */
#define SPI_HWCS(x)     (UINT_MAX - 1)

/**
 * @brief   PWM channel configuration data structure
 */
typedef struct {
    gpio_t pin;                 /**< GPIO pin */
    gpio_mux_t mux;             /**< pin function multiplex value */
    uint8_t chan;               /**< TCC channel to use */
} pwm_conf_chan_t;

/**
 * @brief   PWM device configuration data structure
 */
typedef struct {
    Tcc *dev;                   /**< TCC device to use */
    pwm_conf_chan_t chan[3];    /**< channel configuration */
} pwm_conf_t;

/**
 * @brief   Return the numeric id of a SERCOM device derived from its address
 *
 * @param[in] sercom    SERCOM device
 *
 * @return              numeric id of the given SERCOM device
 */
static inline int _sercom_id(SercomUsart *sercom)
{
    return ((((uint32_t)sercom) >> 10) & 0x7) - 2;
}

#ifndef DOXYGEN
/**
 * @brief   Override the ADC resolution configuration
 * @{
 */
#define HAVE_ADC_RES_T
typedef enum {
    ADC_RES_6BIT  = 0xff,                       /**< not supported */
    ADC_RES_8BIT  = ADC_CTRLB_RESSEL_8BIT,      /**< ADC resolution: 8 bit */
    ADC_RES_10BIT = ADC_CTRLB_RESSEL_10BIT,     /**< ADC resolution: 10 bit */
    ADC_RES_12BIT = ADC_CTRLB_RESSEL_12BIT,     /**< ADC resolution: 12 bit */
    ADC_RES_14BIT = 0xfe,                       /**< not supported */
    ADC_RES_16BIT = 0xfd                        /**< not supported */
} adc_res_t;
/** @} */
#endif /* ndef DOXYGEN */
#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
