/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2017 OTA keys S.A.
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
 * @brief           Common CPU definitions for the STM32 family
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Vincent Dupont <vincent@otakeys.com>
 */

#ifndef PERIPH_CPU_COMMON_H
#define PERIPH_CPU_COMMON_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef Doxygen
/**
 * @brief   Starting address of the ROM bootloader
 *          see application note AN2606
 */
#define STM32_BOOTLOADER_ADDR
#endif

/**
 * @brief   Length of the CPU_ID in octets
 *
 * This is the same for all members of the stm32 family
 */
#define CPUID_LEN           (12U)

/**
 * @brief   Starting address of the CPU ID
 */
#ifndef CPUID_ADDR
#define CPUID_ADDR          (UID_BASE)
#endif

/**
 * @brief   CPU specific LSI clock speed
 */
#if defined(CPU_FAM_STM32F0) || defined (CPU_FAM_STM32F1) || \
    defined(CPU_FAM_STM32F3)
#define CLOCK_LSI           (40000U)
#elif defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
#define CLOCK_LSI           (37000U)
#elif defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) || \
      defined(CPU_FAM_STM32F7) || defined(CPU_FAM_STM32L4) || \
      defined(CPU_FAM_STM32WB) || defined(CPU_FAM_STM32G4) || \
      defined(CPU_FAM_STM32G0) || defined(CPU_FAM_STM32L5) || \
      defined(CPU_FAM_STM32U5) || defined(CPU_FAM_STM32MP1) || \
      defined(CPU_FAM_STM32WL)
#define CLOCK_LSI           (32000U)
#else
#error "error: LSI clock speed not defined for your target CPU"
#endif

/**
 * @brief   Available peripheral buses
 */
typedef enum {
    APB1,           /**< APB1 bus */
    APB2,           /**< APB2 bus */
#if defined(CPU_FAM_STM32WL) || defined(CPU_FAM_STM32U5)
    APB3,
#endif
#if defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32WB) || \
    defined(CPU_FAM_STM32G4) || defined(CPU_FAM_STM32G0) || \
    defined(CPU_FAM_STM32L5) || defined(CPU_FAM_STM32U5) || \
    defined(CPU_FAM_STM32WL)
    APB12,          /**< AHB1 bus, second register */
#endif
#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32G0)
    AHB,            /**< AHB bus */
    IOP,            /**< IOP bus */
#elif defined(CPU_FAM_STM32L1) || defined(CPU_FAM_STM32F1) || \
      defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F3)
    AHB,            /**< AHB bus */
#elif defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) || \
      defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32F7) || \
      defined(CPU_FAM_STM32WB) || defined(CPU_FAM_STM32G4) || \
      defined(CPU_FAM_STM32L5) || defined(CPU_FAM_STM32U5) || \
      defined(CPU_FAM_STM32WL)
    AHB1,           /**< AHB1 bus */
    AHB2,           /**< AHB2 bus */
#if defined(CPU_FAM_STM32U5)
    AHB22,          /**< AHB2 bus, second register */
#endif
    AHB3,           /**< AHB3 bus */
#elif defined(CPU_FAM_STM32MP1)
    AHB1,           /**< AHB1 bus */
    AHB2,           /**< AHB2 bus */
    AHB3,           /**< AHB3 bus */
#else
#warning "unsupported stm32XX family"
#endif
#if defined(CPU_FAM_STM32WB) || defined(CPU_FAM_STM32MP1)
    AHB4,           /**< AHB4 bus */
#endif
} bus_t;

/**
 * @brief   Get the actual bus clock frequency for the APB buses
 *
 * @param[in] bus       target APBx bus
 *
 * @return              bus clock frequency in Hz
 */
uint32_t periph_apb_clk(uint8_t bus);

/**
 * @brief   Get the actual timer clock frequency
 *
 * @param[in] bus       corresponding APBx bus
 *
 * @return              timer clock frequency in Hz
 */
uint32_t periph_timer_clk(uint8_t bus);

/**
 * @brief   Enable the given peripheral clock
 *
 * @param[in] bus       bus the peripheral is connected to
 * @param[in] mask      bit in the RCC enable register
 */
void periph_clk_en(bus_t bus, uint32_t mask);

/**
 * @brief   Disable the given peripheral clock
 *
 * @param[in] bus       bus the peripheral is connected to
 * @param[in] mask      bit in the RCC enable register
 */
void periph_lpclk_dis(bus_t bus, uint32_t mask);

/**
 * @brief   Enable the given peripheral clock in low power mode
 *
 * @param[in] bus       bus the peripheral is connected to
 * @param[in] mask      bit in the RCC enable register
 */
void periph_lpclk_en(bus_t bus, uint32_t mask);

/**
 * @brief   Disable the given peripheral clock in low power mode
 *
 * @param[in] bus       bus the peripheral is connected to
 * @param[in] mask      bit in the RCC enable register
 */
void periph_clk_dis(bus_t bus, uint32_t mask);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_COMMON_H */
/** @} */
