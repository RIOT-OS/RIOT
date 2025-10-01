/*
 * SPDX-FileCopyrightText: 2015-2016 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

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

#include <limits.h>

#include "periph_cpu_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Power mode configuration
 * @{
 */
#define PM_NUM_MODES        (4)
/** @} */

/**
 * @brief   Override the default initial PM blocker
 *          Idle modes are enabled by default, deep sleep mode blocked
 */
#ifndef PM_BLOCKER_INITIAL
#  define PM_BLOCKER_INITIAL    { 1, 0, 0, 0 }
#endif

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
 * @name   SPI configuration
 * @{
 */
#define SAM0_SPI_PM_BLOCK        SAMD21_PM_IDLE_1 /**< Stay in Idle 0 mode */
/** @} */

/**
 * @name   USB configuration
 * @{
 */
#define SAM0_USB_ACTIVE_PM_BLOCK SAMD21_PM_IDLE_1 /**< Stay in Idle 0 mode */
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

/**
 * @brief   Pins that can be used for ADC input
 */
static const gpio_t sam0_adc_pins[1][20] = {
    {
        GPIO_PIN(PA, 2), GPIO_PIN(PA, 3), GPIO_PIN(PB, 8),  GPIO_PIN(PB, 9),
        GPIO_PIN(PA, 4), GPIO_PIN(PA, 5), GPIO_PIN(PA, 6),  GPIO_PIN(PA, 7),
        GPIO_PIN(PB, 0), GPIO_PIN(PB, 1), GPIO_PIN(PB, 2),  GPIO_PIN(PB, 3),
        GPIO_PIN(PB, 4), GPIO_PIN(PB, 5), GPIO_PIN(PB, 6),  GPIO_PIN(PB, 7),
        GPIO_PIN(PA, 8), GPIO_PIN(PA, 9), GPIO_PIN(PA, 10), GPIO_PIN(PA, 11),
    }
};

/**
 * @brief ADC pin aliases
 * @{
 */
#define ADC_INPUTCTRL_MUXPOS_PA02 ADC_INPUTCTRL_MUXPOS_PIN0 /**< Alias for PIN0 */
#define ADC_INPUTCTRL_MUXPOS_PA03 ADC_INPUTCTRL_MUXPOS_PIN1 /**< Alias for PIN1 */
#define ADC_INPUTCTRL_MUXPOS_PB08 ADC_INPUTCTRL_MUXPOS_PIN2 /**< Alias for PIN2 */
#define ADC_INPUTCTRL_MUXPOS_PB09 ADC_INPUTCTRL_MUXPOS_PIN3 /**< Alias for PIN3 */
#define ADC_INPUTCTRL_MUXPOS_PA04 ADC_INPUTCTRL_MUXPOS_PIN4 /**< Alias for PIN4 */
#define ADC_INPUTCTRL_MUXPOS_PA05 ADC_INPUTCTRL_MUXPOS_PIN5 /**< Alias for PIN5 */
#define ADC_INPUTCTRL_MUXPOS_PA06 ADC_INPUTCTRL_MUXPOS_PIN6 /**< Alias for PIN6 */
#define ADC_INPUTCTRL_MUXPOS_PA07 ADC_INPUTCTRL_MUXPOS_PIN7 /**< Alias for PIN7 */
#define ADC_INPUTCTRL_MUXPOS_PB00 ADC_INPUTCTRL_MUXPOS_PIN8 /**< Alias for PIN8 */
#define ADC_INPUTCTRL_MUXPOS_PB01 ADC_INPUTCTRL_MUXPOS_PIN9 /**< Alias for PIN9 */
#define ADC_INPUTCTRL_MUXPOS_PB02 ADC_INPUTCTRL_MUXPOS_PIN10 /**< Alias for PIN10 */
#define ADC_INPUTCTRL_MUXPOS_PB03 ADC_INPUTCTRL_MUXPOS_PIN11 /**< Alias for PIN11 */
#define ADC_INPUTCTRL_MUXPOS_PB04 ADC_INPUTCTRL_MUXPOS_PIN12 /**< Alias for PIN12 */
#define ADC_INPUTCTRL_MUXPOS_PB05 ADC_INPUTCTRL_MUXPOS_PIN13 /**< Alias for PIN13 */
#define ADC_INPUTCTRL_MUXPOS_PB06 ADC_INPUTCTRL_MUXPOS_PIN14 /**< Alias for PIN14 */
#define ADC_INPUTCTRL_MUXPOS_PB07 ADC_INPUTCTRL_MUXPOS_PIN15 /**< Alias for PIN15 */
#define ADC_INPUTCTRL_MUXPOS_PA08 ADC_INPUTCTRL_MUXPOS_PIN16 /**< Alias for PIN16 */
#define ADC_INPUTCTRL_MUXPOS_PA09 ADC_INPUTCTRL_MUXPOS_PIN17 /**< Alias for PIN17 */
#define ADC_INPUTCTRL_MUXPOS_PA10 ADC_INPUTCTRL_MUXPOS_PIN18 /**< Alias for PIN18 */
#define ADC_INPUTCTRL_MUXPOS_PA11 ADC_INPUTCTRL_MUXPOS_PIN19 /**< Alias for PIN19 */

#define ADC_INPUTCTRL_MUXNEG_PA02 ADC_INPUTCTRL_MUXPOS_PIN0 /**< Alias for PIN0 */
#define ADC_INPUTCTRL_MUXNEG_PA03 ADC_INPUTCTRL_MUXPOS_PIN1 /**< Alias for PIN1 */
#define ADC_INPUTCTRL_MUXNEG_PB08 ADC_INPUTCTRL_MUXPOS_PIN2 /**< Alias for PIN2 */
#define ADC_INPUTCTRL_MUXNEG_PB09 ADC_INPUTCTRL_MUXPOS_PIN3 /**< Alias for PIN3 */
#define ADC_INPUTCTRL_MUXNEG_PA04 ADC_INPUTCTRL_MUXPOS_PIN4 /**< Alias for PIN4 */
#define ADC_INPUTCTRL_MUXNEG_PA05 ADC_INPUTCTRL_MUXPOS_PIN5 /**< Alias for PIN5 */
#define ADC_INPUTCTRL_MUXNEG_PA06 ADC_INPUTCTRL_MUXPOS_PIN6 /**< Alias for PIN6 */
#define ADC_INPUTCTRL_MUXNEG_PA07 ADC_INPUTCTRL_MUXPOS_PIN7 /**< Alias for PIN7 */
/** @} */

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

/** @} */
