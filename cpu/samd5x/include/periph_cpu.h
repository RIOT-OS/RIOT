/*
 * SPDX-FileCopyrightText: 2019 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

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

#include <limits.h>

#include "macros/units.h"
#include "periph_cpu_common.h"

#include "candev_samd5x.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   DFLL runs at at fixed frequency of 48 MHz
 */
#define SAM0_DFLL_FREQ_HZ       MHZ(48)

/**
 * @brief   XOSC is used to generate a fixed frequency of 48 MHz
 */
#define SAM0_XOSC_FREQ_HZ       (XOSC0_FREQUENCY ? XOSC0_FREQUENCY : XOSC1_FREQUENCY)

/**
 * @brief   DPLL must run with at least 96 MHz
 */
#define SAM0_DPLL_FREQ_MIN_HZ   MHZ(96)

/**
 * @brief   DPLL frequency must not exceed 200 MHz
 */
#define SAM0_DPLL_FREQ_MAX_HZ   MHZ(200)

/**
 * @name    Power mode configuration
 * @{
 */
#define PM_NUM_MODES            (4)     /**< Backup, Hibernate, Standby, Idle */

/**
 * @brief   Power modes
 */
enum {
    SAM0_PM_BACKUP = 0,
    SAM0_PM_HIBERNATE = 1,
    SAM0_PM_STANDBY = 2,
    SAM0_PM_IDLE = 3,
};
/** @} */

/**
 * @name   SAMD5x GCLK definitions
 * @{
 */
#define SAM0_GCLK_MAIN 0                /**< 120 MHz main clock       */
#ifndef SAM0_GCLK_32KHZ
#  define SAM0_GCLK_32KHZ 1             /**< 32 kHz clock             */
#endif
#ifndef SAM0_GCLK_TIMER
#  define SAM0_GCLK_TIMER 2             /**< 4-8 MHz clock for xTimer */
#endif
#ifndef SAM0_GCLK_PERIPH
#  define SAM0_GCLK_PERIPH 3            /**< 12-48 MHz (DFLL) clock   */
#endif
#ifndef SAM0_GCLK_100MHZ
#  define SAM0_GCLK_100MHZ 4            /**< 100MHz FDPLL clock       */
#endif
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

/**
 * @brief   Pins that can be used for ADC input
 */
static const gpio_t sam0_adc_pins[2][16] = {
    {   /* ADC0 pins */
        GPIO_PIN(PA, 2), GPIO_PIN(PA, 3), GPIO_PIN(PB, 8),  GPIO_PIN(PB, 9),
        GPIO_PIN(PA, 4), GPIO_PIN(PA, 5), GPIO_PIN(PA, 6),  GPIO_PIN(PA, 7),
        GPIO_PIN(PA, 8), GPIO_PIN(PA, 9), GPIO_PIN(PA, 10), GPIO_PIN(PA, 11),
        GPIO_PIN(PB, 0), GPIO_PIN(PB, 1), GPIO_PIN(PB, 2),  GPIO_PIN(PB, 3)
    },
    {   /* ADC1 pins */
        GPIO_PIN(PB, 8),  GPIO_PIN(PB, 9),  GPIO_PIN(PA, 8), GPIO_PIN(PA, 9),
        GPIO_PIN(PC, 2),  GPIO_PIN(PC, 3),  GPIO_PIN(PB, 4), GPIO_PIN(PB, 5),
        GPIO_PIN(PB, 6),  GPIO_PIN(PB, 7),  GPIO_PIN(PC, 0), GPIO_PIN(PC, 1),
        GPIO_PIN(PC, 30), GPIO_PIN(PC, 31), GPIO_PIN(PD, 0), GPIO_PIN(PD, 1)
    }
};

/**
 * @brief ADC pin aliases
 * @{
 */
#define ADC0_INPUTCTRL_MUXPOS_PA02 ADC_INPUTCTRL_MUXPOS_AIN0 /**< Alias for AIN0 */
#define ADC0_INPUTCTRL_MUXPOS_PA03 ADC_INPUTCTRL_MUXPOS_AIN1 /**< Alias for AIN1 */
#define ADC0_INPUTCTRL_MUXPOS_PB08 ADC_INPUTCTRL_MUXPOS_AIN2 /**< Alias for AIN2 */
#define ADC0_INPUTCTRL_MUXPOS_PB09 ADC_INPUTCTRL_MUXPOS_AIN3 /**< Alias for AIN3 */
#define ADC0_INPUTCTRL_MUXPOS_PA04 ADC_INPUTCTRL_MUXPOS_AIN4 /**< Alias for AIN4 */
#define ADC0_INPUTCTRL_MUXPOS_PA05 ADC_INPUTCTRL_MUXPOS_AIN5 /**< Alias for AIN5 */
#define ADC0_INPUTCTRL_MUXPOS_PA06 ADC_INPUTCTRL_MUXPOS_AIN6 /**< Alias for AIN6 */
#define ADC0_INPUTCTRL_MUXPOS_PA07 ADC_INPUTCTRL_MUXPOS_AIN7 /**< Alias for AIN7 */
#define ADC0_INPUTCTRL_MUXPOS_PA08 ADC_INPUTCTRL_MUXPOS_AIN8 /**< Alias for AIN8 */
#define ADC0_INPUTCTRL_MUXPOS_PA09 ADC_INPUTCTRL_MUXPOS_AIN9 /**< Alias for AIN9 */
#define ADC0_INPUTCTRL_MUXPOS_PA10 ADC_INPUTCTRL_MUXPOS_AIN10 /**< Alias for AIN10 */
#define ADC0_INPUTCTRL_MUXPOS_PA11 ADC_INPUTCTRL_MUXPOS_AIN11 /**< Alias for AIN11 */
#define ADC0_INPUTCTRL_MUXPOS_PB00 ADC_INPUTCTRL_MUXPOS_AIN12 /**< Alias for AIN12 */
#define ADC0_INPUTCTRL_MUXPOS_PB01 ADC_INPUTCTRL_MUXPOS_AIN13 /**< Alias for AIN13 */
#define ADC0_INPUTCTRL_MUXPOS_PB02 ADC_INPUTCTRL_MUXPOS_AIN14 /**< Alias for AIN14 */
#define ADC0_INPUTCTRL_MUXPOS_PB03 ADC_INPUTCTRL_MUXPOS_AIN15 /**< Alias for AIN15 */

#define ADC1_INPUTCTRL_MUXPOS_PB08 ADC_INPUTCTRL_MUXPOS_AIN0 /**< Alias for AIN0 */
#define ADC1_INPUTCTRL_MUXPOS_PB09 ADC_INPUTCTRL_MUXPOS_AIN1 /**< Alias for AIN1 */
#define ADC1_INPUTCTRL_MUXPOS_PA08 ADC_INPUTCTRL_MUXPOS_AIN2 /**< Alias for AIN2 */
#define ADC1_INPUTCTRL_MUXPOS_PA09 ADC_INPUTCTRL_MUXPOS_AIN3 /**< Alias for AIN3 */
#define ADC1_INPUTCTRL_MUXPOS_PC02 ADC_INPUTCTRL_MUXPOS_AIN4 /**< Alias for AIN4 */
#define ADC1_INPUTCTRL_MUXPOS_PC03 ADC_INPUTCTRL_MUXPOS_AIN5 /**< Alias for AIN5 */
#define ADC1_INPUTCTRL_MUXPOS_PB04 ADC_INPUTCTRL_MUXPOS_AIN6 /**< Alias for AIN6 */
#define ADC1_INPUTCTRL_MUXPOS_PB05 ADC_INPUTCTRL_MUXPOS_AIN7 /**< Alias for AIN7 */
#define ADC1_INPUTCTRL_MUXPOS_PB06 ADC_INPUTCTRL_MUXPOS_AIN8 /**< Alias for AIN8 */
#define ADC1_INPUTCTRL_MUXPOS_PB07 ADC_INPUTCTRL_MUXPOS_AIN9 /**< Alias for AIN9 */
#define ADC1_INPUTCTRL_MUXPOS_PC00 ADC_INPUTCTRL_MUXPOS_AIN10 /**< Alias for AIN10 */
#define ADC1_INPUTCTRL_MUXPOS_PC01 ADC_INPUTCTRL_MUXPOS_AIN11 /**< Alias for AIN11 */
#define ADC1_INPUTCTRL_MUXPOS_PC30 ADC_INPUTCTRL_MUXPOS_AIN12 /**< Alias for AIN12 */
#define ADC1_INPUTCTRL_MUXPOS_PC31 ADC_INPUTCTRL_MUXPOS_AIN13 /**< Alias for AIN13 */
#define ADC1_INPUTCTRL_MUXPOS_PD00 ADC_INPUTCTRL_MUXPOS_AIN14 /**< Alias for AIN14 */
#define ADC1_INPUTCTRL_MUXPOS_PD01 ADC_INPUTCTRL_MUXPOS_AIN15 /**< Alias for AIN15 */

#define ADC0_INPUTCTRL_MUXNEG_PA02 ADC_INPUTCTRL_MUXPOS_AIN0 /**< Alias for AIN0 */
#define ADC0_INPUTCTRL_MUXNEG_PA03 ADC_INPUTCTRL_MUXPOS_AIN1 /**< Alias for AIN1 */
#define ADC0_INPUTCTRL_MUXNEG_PB08 ADC_INPUTCTRL_MUXPOS_AIN2 /**< Alias for AIN2 */
#define ADC0_INPUTCTRL_MUXNEG_PB09 ADC_INPUTCTRL_MUXPOS_AIN3 /**< Alias for AIN3 */
#define ADC0_INPUTCTRL_MUXNEG_PA04 ADC_INPUTCTRL_MUXPOS_AIN4 /**< Alias for AIN4 */
#define ADC0_INPUTCTRL_MUXNEG_PA05 ADC_INPUTCTRL_MUXPOS_AIN5 /**< Alias for AIN5 */
#define ADC0_INPUTCTRL_MUXNEG_PA06 ADC_INPUTCTRL_MUXPOS_AIN6 /**< Alias for AIN6 */
#define ADC0_INPUTCTRL_MUXNEG_PA07 ADC_INPUTCTRL_MUXPOS_AIN7 /**< Alias for AIN7 */

#define ADC1_INPUTCTRL_MUXNEG_PB08 ADC_INPUTCTRL_MUXPOS_AIN0 /**< Alias for AIN0 */
#define ADC1_INPUTCTRL_MUXNEG_PB09 ADC_INPUTCTRL_MUXPOS_AIN1 /**< Alias for AIN1 */
#define ADC1_INPUTCTRL_MUXNEG_PA08 ADC_INPUTCTRL_MUXPOS_AIN2 /**< Alias for AIN2 */
#define ADC1_INPUTCTRL_MUXNEG_PA09 ADC_INPUTCTRL_MUXPOS_AIN3 /**< Alias for AIN3 */
#define ADC1_INPUTCTRL_MUXNEG_PC02 ADC_INPUTCTRL_MUXPOS_AIN4 /**< Alias for AIN4 */
#define ADC1_INPUTCTRL_MUXNEG_PC03 ADC_INPUTCTRL_MUXPOS_AIN5 /**< Alias for AIN5 */
#define ADC1_INPUTCTRL_MUXNEG_PB04 ADC_INPUTCTRL_MUXPOS_AIN6 /**< Alias for AIN6 */
#define ADC1_INPUTCTRL_MUXNEG_PB05 ADC_INPUTCTRL_MUXPOS_AIN7 /**< Alias for AIN7 */
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

/**
 * @brief   Pins that have peripheral function GCLK
 */
static const gpio_t gclk_io_pins[] = {
    GPIO_PIN(PA, 10), GPIO_PIN(PA, 11), GPIO_PIN(PA, 14),
    GPIO_PIN(PA, 15), GPIO_PIN(PA, 16), GPIO_PIN(PA, 17),
    GPIO_PIN(PA, 27), GPIO_PIN(PA, 30), GPIO_PIN(PB, 10),
    GPIO_PIN(PB, 11), GPIO_PIN(PB, 12), GPIO_PIN(PB, 13),
    GPIO_PIN(PB, 14), GPIO_PIN(PB, 15), GPIO_PIN(PB, 16),
    GPIO_PIN(PB, 17), GPIO_PIN(PB, 18), GPIO_PIN(PB, 19),
    GPIO_PIN(PB, 20), GPIO_PIN(PB, 21), GPIO_PIN(PB, 22),
    GPIO_PIN(PB, 23)
};

/**
 * @brief   GCLK IDs of pins that have peripheral function GCLK - This maps
 *          directly to gclk_io_pins.
 */
static const uint8_t gclk_io_ids[] = {
    4, 5, 0, 1, 2, 3, 1, 0, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1
};

/**
 * @brief   NVM User Page Mapping - Dedicated Entries
 *          Config values will be applied at power-on.
 */
struct sam0_aux_cfg_mapping {
    /* config word 0 */
    uint32_t bod33_disable              :  1; /**< BOD33 Disable at power-on.           */
    uint32_t bod33_level                :  8; /**< BOD33 threshold level at power-on.   */
    uint32_t bod33_action               :  2; /**< BOD33 Action at power-on.            */
    uint32_t bod33_hysteresis           :  4; /**< BOD33 Hysteresis configuration       */
    const uint32_t bod12_calibration    : 11; /**< Factory settings - do not change.    */
    uint32_t nvm_boot_size              :  4; /**< NVM Bootloader Size                  */
    uint32_t reserved_0                 :  2; /**< Factory settings - do not change.    */
    /* config word 1 */
    uint32_t smart_eeprom_blocks        :  4; /**< NVM Blocks per SmartEEPROM sector    */
    uint32_t smart_eeprom_page_size     :  3; /**< SmartEEPROM Page Size                */
    uint32_t ram_eccdis                 :  1; /**< RAM ECC Disable                      */
    uint32_t reserved_1                 :  8; /**< Factory settings - do not change.    */
    uint32_t wdt_enable                 :  1; /**< WDT Enable at power-on.              */
    uint32_t wdt_always_on              :  1; /**< WDT Always-On at power-on.           */
    uint32_t wdt_period                 :  4; /**< WDT Period at power-on.              */
    uint32_t wdt_window                 :  4; /**< WDT Window at power-on.              */
    uint32_t wdt_ewoffset               :  4; /**< WDT Early Warning Interrupt Offset   */
    uint32_t wdt_window_enable          :  1; /**< WDT Window mode enabled on power-on  */
    uint32_t reserved_2                 :  1; /**< Factory settings - do not change.    */
    /* config word 2 */
    uint32_t nvm_locks;                       /**< NVM Region Lock Bits.                */
    /* config word 3 */
    uint32_t user_page;                       /**< User page                            */
    /* config word 4 */
    uint32_t reserved_3;                      /**< Factory settings - do not change.    */
    /* config words 5,6,7 */
    uint32_t user_pages[3];                   /**< User pages                           */
};

/**
 * @name QSPI pins are fixed
 * @{
 */
#define SAM0_QSPI_PIN_CLK       GPIO_PIN(PB, 10)    /**< Clock        */
#define SAM0_QSPI_PIN_CS        GPIO_PIN(PB, 11)    /**< Chip Select  */
#define SAM0_QSPI_PIN_DATA_0    GPIO_PIN(PA,  8)    /**< D0 / MOSI    */
#define SAM0_QSPI_PIN_DATA_1    GPIO_PIN(PA,  9)    /**< D1 / MISO    */
#define SAM0_QSPI_PIN_DATA_2    GPIO_PIN(PA, 10)    /**< D2 / WP      */
#define SAM0_QSPI_PIN_DATA_3    GPIO_PIN(PA, 11)    /**< D3 / HOLD    */
#define SAM0_QSPI_MUX           GPIO_MUX_H          /**< QSPI mux     */
/** @} */

/**
 * @name SDHC pins are fixed
 * @{
 */
#define SAM0_SDHC_MUX           GPIO_MUX_I          /**< SDHC function  */

#define SAM0_SDHC0_PIN_SDCMD    GPIO_PIN(PA,  8)    /**< Command        */
#define SAM0_SDHC0_PIN_SDDAT0   GPIO_PIN(PA,  9)    /**< DATA0          */
#define SAM0_SDHC0_PIN_SDDAT1   GPIO_PIN(PA, 10)    /**< DATA1          */
#define SAM0_SDHC0_PIN_SDDAT2   GPIO_PIN(PA, 11)    /**< DATA2          */
#define SAM0_SDHC0_PIN_SDDAT3   GPIO_PIN(PB, 10)    /**< DATA3          */
#define SAM0_SDHC0_PIN_SDCK     GPIO_PIN(PB, 11)    /**< Clock          */

#define SAM0_SDHC1_PIN_SDCMD    GPIO_PIN(PA, 20)    /**< Command        */
#define SAM0_SDHC1_PIN_SDDAT0   GPIO_PIN(PB, 18)    /**< DATA0          */
#define SAM0_SDHC1_PIN_SDDAT1   GPIO_PIN(PB, 19)    /**< DATA1          */
#define SAM0_SDHC1_PIN_SDDAT2   GPIO_PIN(PB, 20)    /**< DATA2          */
#define SAM0_SDHC1_PIN_SDDAT3   GPIO_PIN(PB, 21)    /**< DATA3          */
#define SAM0_SDHC1_PIN_SDCK     GPIO_PIN(PA, 21)    /**< Clock          */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
