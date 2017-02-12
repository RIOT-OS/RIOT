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

#include "periph_conf.h"
#include "periph_cpu_common.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

uint32_t periph_apb_clk(uint8_t bus)
{
    if (bus == APB1) {
        return CLOCK_APB1;
    }
    else {
        return CLOCK_APB2;
    }
}

void periph_clk_en(bus_t bus, uint32_t mask)
{
    switch (bus) {
        case APB1:
            RCC->APB1ENR |= mask;
            break;
        case APB2:
            RCC->APB2ENR |= mask;
            break;
#if defined(CPU_FAM_STM32L0)
        case AHB:
            RCC->AHBENR |= mask;
            break;
        case IOP:
            RCC->IOPENR |= mask;
            break;
#elif defined(CPU_FAM_STM32L1) || defined(CPU_FAM_STM32F1) \
            || defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F3)
        case AHB:
            RCC->AHBENR |= mask;
            break;
#elif defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4)
        case AHB1:
            RCC->AHB1ENR |= mask;
            break;
        case AHB2:
            RCC->AHB2ENR |= mask;
            break;
        case AHB3:
            RCC->AHB3ENR |= mask;
            break;
#endif
        default:
            DEBUG("unsupported bus %d\n", (int)bus);
            break;
    }
    /* stm32xx-errata: Delay after a RCC peripheral clock enable */
    __DSB();
}

void periph_clk_dis(bus_t bus, uint32_t mask)
{
    switch (bus) {
        case APB1:
            RCC->APB1ENR &= ~(mask);
            break;
        case APB2:
            RCC->APB2ENR &= ~(mask);
            break;
#if defined(CPU_FAM_STM32L0)
        case AHB:
            RCC->AHBENR &= ~(mask);
            break;
        case IOP:
            RCC->IOPENR &= ~(mask);
            break;
#elif defined(CPU_FAM_STM32L1) || defined(CPU_FAM_STM32F1) \
            || defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F3)
        case AHB:
            RCC->AHBENR &= ~(mask);
            break;
#elif defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4)
        case AHB1:
            RCC->AHB1ENR &= ~(mask);
            break;
        case AHB2:
            RCC->AHB2ENR &= ~(mask);
            break;
        case AHB3:
            RCC->AHB3ENR &= ~(mask);
            break;
#endif
        default:
            DEBUG("unsupported bus %d\n", (int)bus);
            break;
    }
}
