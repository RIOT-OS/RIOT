/*
 * Copyright (C) 2023 Bissell
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
 * @brief       Shared CPU specific function for the GD32 M23 Cortex CPU family
 *
 * @author      Jason Parker <jason.parker@bissell.com>
 *
 * @}
 */

#include "cpu.h"
#include "periph_conf.h"
#include "periph_cpu.h"
#include "cpu_common.h"
// #include "gd32e23x_rcu.h"
#include "gd32e23x_periph.h"
#include "cfg_clock_default.h"

#define ENABLE_DEBUG 0
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
#ifdef CLOCK_APB2
    if (bus == APB2) {
        return CLOCK_APB2;
    }
#else
    (void)bus;
#endif
    return CLOCK_APB1;
}

uint32_t periph_timer_clk(uint8_t bus)
{
    return periph_apb_clk(bus) * apbmul[bus];
}

void periph_clk_en(bus_t bus, uint32_t mask)
{
    switch (bus) {
        case APB1:
            RCU->APB1EN |= mask;
            break;
        case APB2:
            RCU->APB2EN |= mask;
            break;
        case AHB:
            RCU->AHBEN |= mask;
            break;
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
            RCU->APB1EN &= ~(mask);
            break;
        case APB2:
            RCU->APB2EN &= ~(mask);
            break;
        case AHB:
            RCU->AHBEN &= ~(mask);
            break;
        default:
            DEBUG("unsupported bus %d\n", (int)bus);
            break;
    }
}
