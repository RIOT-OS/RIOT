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

/**
 * @brief   Timer specific additional bus clock prescaler
 *
 * This prescale factor is dependent on the actual APBx bus clock divider, if
 * the APBx presacler is != 1, it is set to 2, if the APBx prescaler is == 1, it
 * is set to 1.
 *
 * See reference manuals section 'reset and clock control'.
 */
static const uint8_t apbmul[] = {
#if (CLOCK_APB1 < CLOCK_CORECLOCK)
    [APB1] = 2,
#else
    [APB1] = 1,
#endif
#if (CLOCK_APB2 < CLOCK_CORECLOCK)
    [APB2] = 2
#else
    [APB2] = 1
#endif
};

uint32_t periph_apb_clk(uint8_t bus)
{
    if (bus == APB1) {
        return CLOCK_APB1;
    }
    else {
        return CLOCK_APB2;
    }
}

uint32_t periph_timer_clk(uint8_t bus)
{
    return periph_apb_clk(bus) * apbmul[bus];
}

void periph_clk_en(bus_t bus, uint32_t mask)
{
    switch (bus) {
        case APB1:
#if defined(CPU_FAM_STM32L4)
            RCC->APB1ENR1 |= mask;
#else
            RCC->APB1ENR |= mask;
#endif
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
#elif defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) \
      || defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32F7)
        case AHB1:
            RCC->AHB1ENR |= mask;
            break;
/* STM32F410 RCC doesn't provide AHB2 and AHB3 */
#if !defined(CPU_LINE_STM32F410Rx)
        case AHB2:
            RCC->AHB2ENR |= mask;
            break;
        case AHB3:
            RCC->AHB3ENR |= mask;
            break;
#endif
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
#if defined(CPU_FAM_STM32L4)
            RCC->APB1ENR1 &= ~(mask);
#else
            RCC->APB1ENR &= ~(mask);
#endif
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
#elif defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) \
      || defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32F7)
        case AHB1:
            RCC->AHB1ENR &= ~(mask);
            break;
/* STM32F410 RCC doesn't provide AHB2 and AHB3 */
#if !defined(CPU_LINE_STM32F410Rx)
        case AHB2:
            RCC->AHB2ENR &= ~(mask);
            break;
        case AHB3:
            RCC->AHB3ENR &= ~(mask);
            break;
#endif
#endif
        default:
            DEBUG("unsupported bus %d\n", (int)bus);
            break;
    }
}
