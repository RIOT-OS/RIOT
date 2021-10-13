/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_saml21
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

#include "periph_cpu_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The Low Power SRAM is not retained during deep sleep.
 */
#define CPU_BACKUP_RAM_NOT_RETAINED (1)

/**
 * @name    Power mode configuration
 * @{
 */
#define PM_NUM_MODES        (2)
/** @} */

/**
 * @name   SAML21 GCLK definitions
 * @{
 */
enum {
    SAM0_GCLK_MAIN  = 0,                 /**< Main clock */
    SAM0_GCLK_TIMER = 1,                 /**< 4/8MHz clock for timers */
    SAM0_GCLK_32KHZ = 2,                 /**< 32 kHz clock */
    SAM0_GCLK_48MHZ = 3,                 /**< 48MHz clock */
};
/** @} */

#ifndef DOXYGEN
#define HAVE_ADC_RES_T
typedef enum {
    ADC_RES_6BIT  = 0xff,                       /**< not supported */
    ADC_RES_8BIT  = ADC_CTRLC_RESSEL_8BIT,      /**< ADC resolution: 8 bit */
    ADC_RES_10BIT = ADC_CTRLC_RESSEL_10BIT,     /**< ADC resolution: 10 bit */
    ADC_RES_12BIT = ADC_CTRLC_RESSEL_12BIT,     /**< ADC resolution: 12 bit */
    ADC_RES_14BIT = 0xfe,                       /**< not supported */
    ADC_RES_16BIT = 0xfd                        /**< not supported */
} adc_res_t;
#endif /* ndef DOXYGEN */

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
#define RTT_MIN_FREQUENCY   (RTT_CLOCK_FREQUENCY / 512U)  /* in Hz */
#define RTT_MAX_FREQUENCY   (RTT_CLOCK_FREQUENCY)         /* in Hz */
/* determined by tests/ztimer_underflow */
#define RTT_MIN_OFFSET      (8U)
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
    uint64_t reserved_2                 :  9; /**< Factory settings - do not change.    */
    uint64_t wdt_enable                 :  1; /**< WDT Enable at power-on.              */
    uint64_t wdt_always_on              :  1; /**< WDT Always-On at power-on.           */
    uint64_t wdt_period                 :  4; /**< WDT Period at power-on.              */
    uint64_t wdt_window                 :  4; /**< WDT Window at power-on.              */
    uint64_t wdt_ewoffset               :  4; /**< WDT Early Warning Interrupt Offset   */
    uint64_t wdt_window_enable          :  1; /**< WDT Window mode enabled on power-on  */
    uint64_t bod33_hysteresis           :  1; /**< BOD33 Hysteresis configuration       */
    uint64_t reserved_3                 :  6; /**< Factory settings - do not change.    */
    uint64_t nvm_locks                  : 16; /**< NVM Region Lock Bits.                */
};
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
