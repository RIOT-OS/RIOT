/*
 * Copyright (C) 2023 BISSELL Homecare, Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_gd32f3x0
 * @{
 *
 * @file
 * @brief       Shared CPU specific function for the GD32 M4 Cortex CPU family
 *
 * @author      Jason Parker <jason.parker@bissell.com>
 *
 * @}
 */

// #include "kernel_defines.h"
// #include "irq.h"
#include "cpu.h"
#include "periph_conf.h"


//TODO need to define settings  below
// first section taken from gd32v clock.h implementation
// second section taken from previous cfg_clock_default.h from stm32f0 implementation


// #define CLOCK_AHB_DIV        0          /* Max speed at 108 MHz */
// #define CLOCK_APB1_DIV       (0x04 | 0) /* Max speed at 54 MHz */
// #define CLOCK_APB2_DIV       (0x0 | 0)  /* Max speed at 108 MHz */

// #define CLOCK_AHB_DIV_CONF   (CLOCK_AHB_DIV << RCU_CFG0_AHBPSC_Pos)
// #define CLOCK_APB1_DIV_CONF  (CLOCK_APB1_DIV << RCU_CFG0_APB1PSC_Pos)
// #define CLOCK_APB2_DIV_CONF  (CLOCK_APB2_DIV << RCU_CFG0_APB2PSC_Pos)

// #define PREDV0_CONF          1  /* Divide by 2 */
// #ifdef CONFIG_BOARD_HAS_HXTAL
// #define PLL_MULT_FACTOR      (CLOCK_CORECLOCK / (CONFIG_CLOCK_HXTAL / (PREDV0_CONF + 1)) - 1)
// #else
// #define PLL_MULT_FACTOR      (CLOCK_CORECLOCK / (MHZ(8) / 2 ) - 1)
// #endif

// #define RCU_CFG0_SCS_IRC8    (0 << RCU_CFG0_SCS_Pos)
// #define RCU_CFG0_SCS_HXTAL   (1 << RCU_CFG0_SCS_Pos)
// #define RCU_CFG0_SCS_PLL     (2 << RCU_CFG0_SCS_Pos)

// #define RCU_CFG0_SCSS_IRC8   (0 << RCU_CFG0_SCSS_Pos)
// #define RCU_CFG0_SCSS_HXTAL  (1 << RCU_CFG0_SCSS_Pos)
// #define RCU_CFG0_SCSS_PLL    (2 << RCU_CFG0_SCSS_Pos)


/**
 * @name    GD32F3x0 clock settings
 * @{
 */

// #ifndef CONFIG_CLOCK_PLL_PREDIV
// #define CONFIG_CLOCK_PLL_PREDIV         (1)
// #endif

// #ifndef CONFIG_CLOCK_PLL_MUL
// #define CONFIG_CLOCK_PLL_MUL            (12)
// #endif

// #if IS_ACTIVE(CONFIG_USE_CLOCK_HSI)
// #define CLOCK_CORECLOCK                 (CLOCK_HSI)

// #elif IS_ACTIVE(CONFIG_USE_CLOCK_HSE)
// #if !IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
// #error "The board doesn't provide an HSE oscillator"
// #endif
// #define CLOCK_CORECLOCK                 (CLOCK_HSE)

// #elif IS_ACTIVE(CONFIG_USE_CLOCK_PLL)
// #if IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
// #define CLOCK_PLL_SRC                   (CLOCK_HSE)
// #else /* CLOCK_HSI */
// #define CLOCK_PLL_SRC                   (CLOCK_HSI)
// #endif
/* PLL configuration: make sure your values are legit!
 *
 * compute by: CORECLOCK = ((PLL_IN / PLL_PREDIV) * PLL_MUL)
 * with:
 * PLL_IN:          input clock is HSE if available or HSI otherwise
 * PLL_PREDIV :     pre-divider,  allowed range: [1:16]
 * PLL_MUL:         multiplier,   allowed range: [2:16]
 * CORECLOCK        -> 48MHz Max on F0, 72MHz MAX on F1/F3!
 */
// #define CLOCK_CORECLOCK                 ((CLOCK_PLL_SRC / CONFIG_CLOCK_PLL_PREDIV) * CONFIG_CLOCK_PLL_MUL)
// #if CLOCK_CORECLOCK > MHZ(72)
// #error "SYSCLK cannot exceed 72MHz"
// #endif
// #endif /* CONFIG_USE_CLOCK_PLL */

// #define CLOCK_AHB                       CLOCK_CORECLOCK

// #ifndef CONFIG_CLOCK_APB1_DIV
// #define CONFIG_CLOCK_APB1_DIV           (1)
// #endif
// #define CLOCK_APB1                      (CLOCK_AHB / CONFIG_CLOCK_APB1_DIV)

// #ifndef CONFIG_CLOCK_APB2_DIV
// #define CONFIG_CLOCK_APB2_DIV           (1)
// #endif
// #define CLOCK_APB2                      (CLOCK_AHB / CONFIG_CLOCK_APB2_DIV)
/** @} */

#define ENABLE_DEBUG 0
#include "debug.h"

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
