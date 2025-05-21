/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           STM32F1 CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "cpu_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN

/**
 * @brief   Starting address of the ROM bootloader
 *          see application note AN2606
 */
#if defined(CPU_LINE_STM32F103xB) || defined(CPU_LINE_STM32F103xE)
#define STM32_BOOTLOADER_ADDR   (0x1FFFF000)
#endif

/**
 * @brief   Readout Protection (RDP) option bytes
 */
#define STM32_OPTION_BYTES   ((uint32_t*) 0x1FFFF800)
#define GET_RDP(x) (x & 0xFF)

#endif /* ndef DOXYGEN */

/**
 * @name    Real time counter configuration
 * @{
 */
#define RTT_IRQ_PRIO        1

#define RTT_DEV             RTC
#define RTT_IRQ             RTC_IRQn
#define RTT_ISR             isr_rtc

#define RTT_MAX_VALUE       (0xffffffff)
#define RTT_CLOCK_FREQUENCY (32768U)                  /* in Hz */
#define RTT_MIN_FREQUENCY   (1U)                      /* in Hz */
/* RTC frequency of 32kHz is not recommended, see RM0008 Rev 20, p490 */
#define RTT_MAX_FREQUENCY   (RTT_CLOCK_FREQUENCY / 2) /* in Hz */
/** @} */

/**
 * @brief   Available number of ADC devices
 */
#define ADC_DEVS            (2U)

/**
 * @name    GPIO Definitions Missing in Vendor Files
 * @{
 */
/**
 * @brief   Possible values of the MODE0 field in the GPIO CRL register
 *
 * The MODE1 to MODE7 fields have the same values. Don't forget to shift the
 * constants to the field position for MODE1 to MODE7 by 4 times n bits, where
 * n is the pin number.
 *
 * In addition the MODE8 to MODE15 fields in the CRH register have the same
 * layout and semantics as the MODE0 to MODE 7 fields in the CRL register.
 */
enum {
    GPIO_CRL_MODE0_INPUT            = (0x0 << GPIO_CRL_MODE0_Pos),
    GPIO_CRL_MODE0_OUTPUT_10MHZ     = (0x1 << GPIO_CRL_MODE0_Pos),
    GPIO_CRL_MODE0_OUTPUT_2MHZ      = (0x2 << GPIO_CRL_MODE0_Pos),
    GPIO_CRL_MODE0_OUTPUT_50MHZ     = (0x3 << GPIO_CRL_MODE0_Pos),
};

/**
 * @brief   Possible values of the CNF0 field in the GPIO CRL register
 *
 * The CNF1 to CNF7 fields have the same values. Don't forget to shift the
 * constants to the field position for CNF1 to CNF7 by 4 times n bits, where
 * n is the pin number.
 *
 * In addition the CNF8 to CNF15 fields in the CRH register have the same
 * layout and semantics as the CNF0 to CNF 7 fields in the CRL register.
 */
enum {
    GPIO_CRL_CNF0_INPUT_ANALOG      = (0x0 << GPIO_CRL_CNF0_Pos),
    GPIO_CRL_CNF0_INPUT_FLOATING    = (0x1 << GPIO_CRL_CNF0_Pos),
    GPIO_CRL_CNF0_INPUT_PULL        = (0x2 << GPIO_CRL_CNF0_Pos),
    GPIO_CRL_CNF0_OUTPUT_PUSH_PULL  = (0x0 << GPIO_CRL_CNF0_Pos),
    GPIO_CRL_CNF0_OUTPUT_OPEN_DRAIN = (0x1 << GPIO_CRL_CNF0_Pos),
    GPIO_CRL_CNF0_AF_PUSH_PULL      = (0x2 << GPIO_CRL_CNF0_Pos),
    GPIO_CRL_CNF0_AF_OPEN_DRAIN     = (0x3 << GPIO_CRL_CNF0_Pos),
};
/** @} */

/**
 * @brief   Possible values of the `SWJ_CFG` field in the AFIO->MAPR register
 *
 * @details This wraps the vendor header file preprocessor macros into a
 *          C language `enum`.
 */
typedef enum {
     /**
      * @brief  Both JTAG-DP and SW-DP enabled, reset state
      */
    SWJ_CFG_FULL_SWJ    = 0,
    /**
     * @brief   Both JTAG-DP and SW-DP enabled, but NJTRST disabled and pin
     *          usable as GPIO
     */
    SWJ_CFG_NO_NJTRST   = AFIO_MAPR_SWJ_CFG_NOJNTRST,
    /**
     * @brief   Only SW-DP enabled, JTAG pins usable as GPIOS
     */
    SWJ_CFG_NO_JTAG_DP  = AFIO_MAPR_SWJ_CFG_JTAGDISABLE,
    /**
     * @brief   Neither JTAG-DP nor SW-DP enabled, JTAG and SWD pins usable as
     *          GPIOS
     */
    SWJ_CFG_DISABLED    = AFIO_MAPR_SWJ_CFG_DISABLE,
} afio_mapr_swj_cfg_t;

#ifndef CONFIG_AFIO_MAPR_SWJ_CFG
/**
 * @brief   By default, disable JTAG and keep only SWD
 *
 * This frees the JTAG pins for use as regular GPIOs. We do not support flashing
 * or debugging via JTAG anyway, so there is nothing lost except for a few bytes
 * of ROM to initialize the `SWJ_CFG` register.
 */
#define CONFIG_AFIO_MAPR_SWJ_CFG    SWJ_CFG_NO_JTAG_DP
#endif

/**
 * @brief   Read the current value of the AFIO->MAPR register reproducibly
 *
 * This will explicitly clear the write-only `SWJ_CFG` field [26:24], as the
 * values read back are undefined.
 */
static inline uint32_t afio_mapr_read(void)
{
    return AFIO->MAPR & (~(AFIO_MAPR_SWJ_CFG_Msk));
}

/**
 * @brief   Write to the AFIO->MAPR register apply the SWJ configuration
 *          specified via @ref CONFIG_AFIO_MAPR_SWJ_CFG
 *
 * @pre     @p new_value has all bits in the range [26:24] cleared (the
 *          `SWJ_CFG` field).
 */
static inline void afio_mapr_write(uint32_t new_value)
{
    AFIO->MAPR = CONFIG_AFIO_MAPR_SWJ_CFG | new_value;
}

#ifdef __cplusplus
}
#endif

/** @} */
