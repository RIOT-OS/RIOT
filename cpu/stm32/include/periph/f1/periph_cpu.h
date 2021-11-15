/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           STM32F1 CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_F1_PERIPH_CPU_H
#define PERIPH_F1_PERIPH_CPU_H

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

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_F1_PERIPH_CPU_H */
/** @} */
