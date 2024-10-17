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
#define PM_NUM_MODES           (3)
#define SAML21_PM_MODE_BACKUP  (0)  /**< Wakeup by some IRQs possible, but no RAM retention */
#define SAML21_PM_MODE_STANDBY (1)  /**< Just peripherals clocked by 32K OSC are active */
#define SAML21_PM_MODE_IDLE    (2)  /**< CPU sleeping, peripherals are active */
/** @} */

/**
 * @name    Peripheral power mode requirements
 * @{
 */
#define SAM0_GPIO_PM_BLOCK        SAML21_PM_MODE_BACKUP   /**< GPIO IRQs require STANDBY mode */
#define SAM0_RTCRTT_PM_BLOCK      SAML21_PM_MODE_BACKUP   /**< RTC/TRR require STANDBY mode */
#define SAM0_SPI_PM_BLOCK         SAML21_PM_MODE_STANDBY  /**< SPI in DMA mode require IDLE mode */
#define SAM0_TIMER_PM_BLOCK       SAML21_PM_MODE_STANDBY  /**< Timers require IDLE mode */
#define SAM0_UART_PM_BLOCK        SAML21_PM_MODE_STANDBY  /**< UART RX IRQ require IDLE mode */
#define SAM0_USB_IDLE_PM_BLOCK    SAML21_PM_MODE_BACKUP   /**< Idle USB require STANDBY mode */
#define SAM0_USB_ACTIVE_PM_BLOCK  SAML21_PM_MODE_STANDBY  /**< Active USB require IDLE mode */
/** @} */

/**
 * @brief   Override the default initial PM blocker
 *          All peripheral drivers ensure required pm modes are blocked
 */
#ifndef PM_BLOCKER_INITIAL
#define PM_BLOCKER_INITIAL      { 0, 0, 0 }
#endif

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
#define ADC_INPUTCTRL_MUXPOS_PA02 ADC_INPUTCTRL_MUXPOS_AIN0 /**< Alias for AIN0 */
#define ADC_INPUTCTRL_MUXPOS_PA03 ADC_INPUTCTRL_MUXPOS_AIN1 /**< Alias for AIN1 */
#define ADC_INPUTCTRL_MUXPOS_PB08 ADC_INPUTCTRL_MUXPOS_AIN2 /**< Alias for AIN2 */
#define ADC_INPUTCTRL_MUXPOS_PB09 ADC_INPUTCTRL_MUXPOS_AIN3 /**< Alias for AIN3 */
#define ADC_INPUTCTRL_MUXPOS_PA04 ADC_INPUTCTRL_MUXPOS_AIN4 /**< Alias for AIN4 */
#define ADC_INPUTCTRL_MUXPOS_PA05 ADC_INPUTCTRL_MUXPOS_AIN5 /**< Alias for AIN5 */
#define ADC_INPUTCTRL_MUXPOS_PA06 ADC_INPUTCTRL_MUXPOS_AIN6 /**< Alias for AIN6 */
#define ADC_INPUTCTRL_MUXPOS_PA07 ADC_INPUTCTRL_MUXPOS_AIN7 /**< Alias for AIN7 */
#define ADC_INPUTCTRL_MUXPOS_PB00 ADC_INPUTCTRL_MUXPOS_AIN8 /**< Alias for AIN8 */
#define ADC_INPUTCTRL_MUXPOS_PB01 ADC_INPUTCTRL_MUXPOS_AIN9 /**< Alias for AIN9 */
#define ADC_INPUTCTRL_MUXPOS_PB02 ADC_INPUTCTRL_MUXPOS_AIN10 /**< Alias for AIN10 */
#define ADC_INPUTCTRL_MUXPOS_PB03 ADC_INPUTCTRL_MUXPOS_AIN11 /**< Alias for AIN11 */
#define ADC_INPUTCTRL_MUXPOS_PB04 ADC_INPUTCTRL_MUXPOS_AIN12 /**< Alias for AIN12 */
#define ADC_INPUTCTRL_MUXPOS_PB05 ADC_INPUTCTRL_MUXPOS_AIN13 /**< Alias for AIN13 */
#define ADC_INPUTCTRL_MUXPOS_PB06 ADC_INPUTCTRL_MUXPOS_AIN14 /**< Alias for AIN14 */
#define ADC_INPUTCTRL_MUXPOS_PB07 ADC_INPUTCTRL_MUXPOS_AIN15 /**< Alias for AIN15 */
#define ADC_INPUTCTRL_MUXPOS_PA08 ADC_INPUTCTRL_MUXPOS_AIN16 /**< Alias for AIN16 */
#define ADC_INPUTCTRL_MUXPOS_PA09 ADC_INPUTCTRL_MUXPOS_AIN17 /**< Alias for AIN17 */
#define ADC_INPUTCTRL_MUXPOS_PA10 ADC_INPUTCTRL_MUXPOS_AIN18 /**< Alias for AIN18 */
#define ADC_INPUTCTRL_MUXPOS_PA11 ADC_INPUTCTRL_MUXPOS_AIN19 /**< Alias for AIN19 */

#define ADC_INPUTCTRL_MUXNEG_PA02 ADC_INPUTCTRL_MUXPOS_AIN0 /**< Alias for AIN0 */
#define ADC_INPUTCTRL_MUXNEG_PA03 ADC_INPUTCTRL_MUXPOS_AIN1 /**< Alias for AIN1 */
#define ADC_INPUTCTRL_MUXNEG_PB08 ADC_INPUTCTRL_MUXPOS_AIN2 /**< Alias for AIN2 */
#define ADC_INPUTCTRL_MUXNEG_PB09 ADC_INPUTCTRL_MUXPOS_AIN3 /**< Alias for AIN3 */
#define ADC_INPUTCTRL_MUXNEG_PA04 ADC_INPUTCTRL_MUXPOS_AIN4 /**< Alias for AIN4 */
#define ADC_INPUTCTRL_MUXNEG_PA05 ADC_INPUTCTRL_MUXPOS_AIN5 /**< Alias for AIN5 */
#define ADC_INPUTCTRL_MUXNEG_PA06 ADC_INPUTCTRL_MUXPOS_AIN6 /**< Alias for AIN6 */
#define ADC_INPUTCTRL_MUXNEG_PA07 ADC_INPUTCTRL_MUXPOS_AIN7 /**< Alias for AIN7 */
/** @} */

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
/* determined by tests/sys/ztimer_underflow */
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

/** @} */
#endif /* PERIPH_CPU_H */
