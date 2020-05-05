/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_samd5x
 * @brief           CPU specific definitions for internal peripheral handling
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include <limits.h>

#include "macros/units.h"
#include "periph_cpu_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   DFLL runs at at fixed frequency of 48 MHz
 */
#define SAM0_DFLL_FREQ_HZ       MHZ(48)

/**
￼ * @brief   XOSC is used to generate a fixed frequency of 48 MHz
￼ */
#define SAM0_XOSC_FREQ_HZ       (XOSC0_FREQUENCY ? XOSC0_FREQUENCY : XOSC1_FREQUENCY)

/**
 * @brief   DPLL must run with at least 96 MHz
 */
#define SAM0_DPLL_FREQ_MIN_HZ   MHZ(96)

/**
 * @brief   DPLL frequency must not exceed 200 MHz
 */
#define SAM0_DPLL_FREQ_MAX_HZ   MHZ(20)

/**
 * @name    Power mode configuration
 * @{
 */
#define PM_NUM_MODES        (3)
/** @} */

/**
 * @name   SAMD5x GCLK definitions
 * @{
 */
enum {
    SAM0_GCLK_MAIN = 0,                 /**< 120 MHz main clock       */
    SAM0_GCLK_32KHZ,                    /**< 32 kHz clock             */
    SAM0_GCLK_TIMER,                    /**< 4-8 MHz clock for xTimer */
    SAM0_GCLK_PERIPH,                   /**< 12-48 MHz (DFLL) clock   */
};
/** @} */

/**
 * @name   GCLK compatibility definitions
 * @{
 */
#define SAM0_GCLK_8MHZ      SAM0_GCLK_TIMER
#define SAM0_GCLK_48MHZ     SAM0_GCLK_PERIPH
/** @} */

/**
 * @brief   Override SPI hardware chip select macro
 *
 * As of now, we do not support HW CS, so we always set it to a fixed value
 */
#define SPI_HWCS(x)     (UINT_MAX - 1)

#ifndef DOXYGEN
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
#endif /* DOXYGEN */

/**
 * @brief   The MCU has a 12 bit DAC
 */
#define DAC_RES_BITS        (12)

/**
 * @brief   The MCU has two DAC outputs.
 */
#define DAC_NUMOF           (2)

/**
 * @name    Real time counter configuration
 * @{
 */
#define RTT_MAX_VALUE       (0xffffffff)
#define RTT_CLOCK_FREQUENCY (32768U)                      /* in Hz */
#define RTT_MIN_FREQUENCY   (RTT_CLOCK_FREQUENCY / 1024U) /* in Hz */
#define RTT_MAX_FREQUENCY   (RTT_CLOCK_FREQUENCY)         /* in Hz */
/** @} */

/**
 * @brief   RTC input pins that can be used for tamper detection and
 *          wake from Deep Sleep
 */
static const gpio_t rtc_tamper_pins[RTC_NUM_OF_TAMPERS] = {
    GPIO_PIN(PB, 0), GPIO_PIN(PB, 2), GPIO_PIN(PA, 2),
    GPIO_PIN(PC, 0), GPIO_PIN(PC, 1)
};

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
