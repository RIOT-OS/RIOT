/*
 * Copyright (C) 2018 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_saml1x
 * @brief           CPU specific definitions for internal peripheral handling
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "periph_cpu_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Power mode configuration
 * @{
 */
#define PM_NUM_MODES        (1)
/** @} */

/**
 * @brief   Override the default initial PM blocker
 * @todo   Idle modes are enabled by default, deep sleep mode blocked
 */
#define PM_BLOCKER_INITIAL  0x00000001

/**
 * @name   SAML1x GCLK definitions
 * @{
 */
enum {
    SAM0_GCLK_MAIN = 0,                 /**< 16 MHz main clock      */
    SAM0_GCLK_32KHZ,                    /**< 32 kHz clock           */
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
#define RTT_MAX_VALUE       (0xffffffff)
#define RTT_CLOCK_FREQUENCY (32768U)                      /* in Hz */
#define RTT_MIN_FREQUENCY   (RTT_CLOCK_FREQUENCY / 1024U)  /* in Hz */
#define RTT_MAX_FREQUENCY   (RTT_CLOCK_FREQUENCY)         /* in Hz */
/** @} */

/**
 * @brief   RTC input pins that can be used for tamper detection and
 *          wake from Deep Sleep
 */
static const gpio_t rtc_tamper_pins[RTC_NUM_OF_TAMPERS] = {
    GPIO_PIN(PA, 8), GPIO_PIN(PA, 9), GPIO_PIN(PA, 16),
    GPIO_PIN(PA, 17)
};

/**
 * @brief   NVM User Row Mapping - Dedicated Entries
 *          Config values will be applied at power-on.
 */
struct sam0_aux_cfg_mapping {
    /* config word 0 */
    uint32_t secure_region_unlock       :  3; /**< NVM Secure Region UnLock Bits        */
    uint32_t non_secure_region_unlock   :  3; /**< NVM Non-Secure Region UnLock Bits    */
    uint32_t reserved_0                 :  1; /**< Reserved                             */
    uint32_t bod33_level                :  6; /**< BOD33 threshold level at power-on.   */
    uint32_t bod33_disable              :  1; /**< BOD33 Disable at power-on.           */
    uint32_t bod33_action               :  2; /**< BOD33 Action at power-on.            */
    const uint32_t bod12_calibration    :  9; /**< Factory settings - do not change.    */
    uint32_t wdt_run_standby            :  1; /**< WDT Runstdby at power-on             */
    uint32_t wdt_enable                 :  1; /**< WDT Enable at power-on.              */
    uint32_t wdt_always_on              :  1; /**< WDT Always-On at power-on.           */
    uint32_t wdt_period                 :  4; /**< WDT Period at power-on.              */
    /* config word 1 */
    uint32_t wdt_window                 :  4; /**< WDT Window at power-on.              */
    uint32_t wdt_ewoffset               :  4; /**< WDT Early Warning Interrupt Offset   */
    uint32_t wdt_window_enable          :  1; /**< WDT Window mode enabled on power-on  */
    uint32_t bod33_hysteresis           :  1; /**< BOD33 Hysteresis configuration       */
    uint32_t reserved_1                 :  1; /**< Reserved                             */
    uint32_t ram_execute_never          :  1; /**< RAM is eXecute Never                 */
    uint32_t data_execute_never         :  1; /**< Data Flash is eXecute Never          */
    uint32_t reserved_2                 : 19; /**< Reserved                             */
    /* config word 2 */
    uint32_t secure_flash_as_size       :  8; /**< Secure Flash (AS region) Size = AS*0x100 */
    uint32_t nsc_size                   :  6; /**< Non-Secure Callable Flash (APPLICATION region) Size = ANSC*0x20 */
    uint32_t reserved_3                 :  2; /**< Reserved                             */
    uint32_t secure_flash_data_size     :  4; /**< Secure Data Flash Size = DS*0x100    */
    uint32_t reserved_4                 :  4; /**< Reserved                             */
    uint32_t secure_ram_size            :  7; /**< Secure SRAM Size = RS*0x80           */
    uint32_t reserved_5                 :  1; /**< Reserved                             */
    /* config word 3 */
    uint32_t user_row_write_enable      :  1; /**< User Row Write Enable                */
    uint32_t reserved_6                 : 31; /**< Reserved                             */
    /* config word 4 */
    uint32_t nonsec_a;            /**< Peripherals Non-Secure Status Fuses for Bridge A */
    /* config word 5 */
    uint32_t nonsec_b;            /**< Peripherals Non-Secure Status Fuses for Bridge B */
    /* config word 6 */
    uint32_t nonsec_c;            /**< Peripherals Non-Secure Status Fuses for Bridge C */
    /* config word 7 */
    uint32_t user_crc;            /**< CRC of NVM User Row bits 223:64 (words 2…6)      */
};

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
