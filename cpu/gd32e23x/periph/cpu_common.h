/*
 * Copyright (C) 2023 BISSELL Homecare, Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_gd32e23x
 * @{
 *
 * @file
 * @brief       Common CPU definitions for the GD32E23x family
 *
 * @author      Jason Parker <jason.parker@bissell.com>
 */

#ifndef CPU_COMMON_H
#define CPU_COMMON_H

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
#define CLOCK_LSI           (40000U)

/**
 * @brief   Available peripheral buses
 */
typedef enum {
    APB1,           /**< APB1 bus */
    APB2,           /**< APB2 bus */
    AHB,            /**< AHB bus */
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

#endif /* CPU_COMMON_H */
/** @} */
