/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cortexm_common
 * @{
 *
 * @file
 * @brief       Shared CPU specific function for the STM32 CPU family
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "periph_cpu_common.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

void periph_clk_en(bus_t bus, uint32_t mask)
{
    if (bus == APB1) {
        RCC->APB1ENR |= mask;
    }
    else if (bus == APB2) {
        RCC->APB2ENR |= mask;
    }
#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1) || defined(CPU_FAM_STM32F1)\
    || defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F3)
    else if (bus == AHB) {
        RCC->AHBENR |= mask;
    }
#elif defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4)
    else if (bus == AHB1) {
        RCC->AHB1ENR |= mask;
    }
    else if (bus == AHB2) {
        RCC->AHB2ENR |= mask;
    }
    else if (bus == AHB3) {
        RCC->AHB3ENR |= mask;
    }
#else
#warning "add support for stm32 family to this function"
#endif
    else {
        DEBUG("unsupported bus %d\n", (int)bus);
    }
    __DSB();
}

void periph_clk_dis(bus_t bus, uint32_t mask)
{
    if (bus == APB1) {
        RCC->APB1ENR &= ~(mask);
    }
    else if (bus == APB2) {
        RCC->APB2ENR &= ~(mask);
    }
#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1) || defined(CPU_FAM_STM32F1)\
    || defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F3)
    else if (bus == AHB) {
        RCC->AHBENR &= ~(mask);
    }
#elif defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4)
    else if (bus == AHB1) {
        RCC->AHB1ENR &= ~(mask);
    }
    else if (bus == AHB2) {
        RCC->AHB2ENR &= ~(mask);
    }
    else if (bus == AHB3) {
        RCC->AHB3ENR &= ~(mask);
    }
#else
#warning "add support for stm32 family to this function"
#endif
    else {
        DEBUG("unsupported bus %d\n", (int)bus);
    }
}
