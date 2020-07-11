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
 * @name    Power mode configuration
 * @{
 */
#define PM_NUM_MODES        (3)
/** @} */

/**
 * @brief   Override the default initial PM blocker
 * @todo   Idle modes are enabled by default, deep sleep mode blocked
 */
#define PM_BLOCKER_INITIAL      0x00000001

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
 * @name   SAMD21 GCLK definitions
 * @{
 */
enum {
    SAM0_GCLK_MAIN = 0,                 /**< 48 MHz main clock      */
    SAM0_GCLK_1MHZ,                     /**< 1 MHz clock for xTimer */
    SAM0_GCLK_32KHZ,                    /**< 32 kHz clock           */
    SAM0_GCLK_1KHZ,                     /**< 1 kHz clock            */
    SAM0_GCLK_DISABLED = 0xF,           /**< disabled GCLK          */
};
/** @} */

/**
 * @brief   Override SPI hardware chip select macro
 *
 * As of now, we do not support HW CS, so we always set it to a fixed value
 */
#define SPI_HWCS(x)     (UINT_MAX - 1)

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

/**
 * @brief   The MCU has a 10 bit DAC
 */
#define DAC_RES_BITS        (10)

/**
 * @brief   The MCU has one DAC Output.
 */
#define DAC_NUMOF           (1)

/**
 * @name    Real time counter configuration
 * @{
 */
#define RTT_RUNSTDBY        (1)         /* Keep RTT running in sleep states */

#define RTT_MAX_VALUE       (0xffffffff)
#define RTT_CLOCK_FREQUENCY (32768U)                      /* in Hz */
#define RTT_MIN_FREQUENCY   (RTT_CLOCK_FREQUENCY / 1024U) /* in Hz */
#define RTT_MAX_FREQUENCY   (RTT_CLOCK_FREQUENCY)         /* in Hz */
/** @} */

/**
 * @brief   NVM User Row Mapping - Dedicated Entries
 *          Config values will be applied at power-on.
 * @{
 */
struct sam0_aux_cfg_mapping {
    uint64_t bootloader_size            :  3; /**< BOOTPROT: Bootloader Size            */
    uint64_t reserved_0                 :  1; /**< Factory settings - do not change.    */
    uint64_t eeprom_size                :  3; /**< one of eight different EEPROM sizes  */
    uint64_t reserved_1                 :  1; /**< Factory settings - do not change.    */
    uint64_t bod33_level                :  6; /**< BOD33 threshold level at power-on.   */
    uint64_t bod33_enable               :  1; /**< BOD33 Enable at power-on.            */
    uint64_t bod33_action               :  2; /**< BOD33 Action at power-on.            */
    uint64_t reserved_2                 :  8; /**< Factory settings - do not change.    */
    uint64_t wdt_enable                 :  1; /**< WDT Enable at power-on.              */
    uint64_t wdt_always_on              :  1; /**< WDT Always-On at power-on.           */
    uint64_t wdt_period                 :  4; /**< WDT Period at power-on.              */
    uint64_t wdt_window                 :  4; /**< WDT Window at power-on.              */
    uint64_t wdt_ewoffset               :  4; /**< WDT Early Warning Interrupt Offset   */
    uint64_t wdt_window_enable          :  1; /**< WDT Window mode enabled on power-on  */
    uint64_t bod33_hysteresis           :  1; /**< BOD33 Hysteresis configuration       */
    const uint64_t bod12_calibration    :  1; /**< Factory settings - do not change.    */
    uint64_t reserved_3                 :  6; /**< Factory settings - do not change.    */
    uint64_t nvm_locks                  : 16; /**< NVM Region Lock Bits.                */
};
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
