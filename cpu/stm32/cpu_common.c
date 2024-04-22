/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       Shared CPU specific function for the STM32 CPU family
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Joshua DeWeese <jdeweese@primecontrols.com>
 *
 * @}
 */

#include <assert.h>

#include "compiler_hints.h"
#include "modules.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#if     defined(CPU_FAM_STM32F2) || \
        defined(CPU_FAM_STM32F4) || \
        defined(CPU_FAM_STM32F7) || \
        defined(CPU_FAM_STM32L1)
            #define APB1_PERIPH_LP_EN           RCC->APB1LPENR
            #define APB2_PERIPH_LP_EN           RCC->APB2LPENR
            #ifdef AHB_PERIPH_EN
                #define AHB_PERIPH_LP_EN        RCC->AHBLPENR
            #endif
            #ifdef AHB1_PERIPH_EN
                #define AHB1_PERIPH_LP_EN       RCC->AHB1LPENR
            #endif
            #ifdef AHB2_PERIPH_EN
                #define AHB2_PERIPH_LP_EN       RCC->AHB2LPENR
            #endif
            #ifdef AHB3_PERIPH_EN
                #define AHB3_PERIPH_LP_EN       RCC->AHB3LPENR
            #endif
#endif

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
#if defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32WB) || \
    defined(CPU_FAM_STM32G0) || defined(CPU_FAM_STM32G4) || \
    defined(CPU_FAM_STM32L5) || defined(CPU_FAM_STM32U5) || \
    defined(CPU_FAM_STM32WL) || defined(CPU_FAM_STM32C0)
    [APB12] = 2,
#endif
#else
    [APB1] = 1,
#if defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32WB) || \
    defined(CPU_FAM_STM32G0) || defined(CPU_FAM_STM32G4) || \
    defined(CPU_FAM_STM32L5) || defined(CPU_FAM_STM32U5) || \
    defined(CPU_FAM_STM32WL) || defined(CPU_FAM_STM32C0)
    [APB12] = 1,
#endif
#endif
#if defined(APB2_PERIPH_EN)
#if (CLOCK_APB2 < CLOCK_CORECLOCK)
    [APB2] = 2
#else
    [APB2] = 1
#endif
#endif
};

static volatile uint32_t* _rcc_en_reg(bus_t bus)
{
    switch (bus) {
#ifdef APB1_PERIPH_EN
    case APB1:
        return &APB1_PERIPH_EN;
#endif
#ifdef APB12_PERIPH_EN
    case APB12:
        return &APB12_PERIPH_EN;
#endif
#ifdef APB2_PERIPH_EN
    case APB2:
        return &APB2_PERIPH_EN;
#endif
#ifdef APB22_PERIPH_EN
    case APB22:
        return &APB22_PERIPH_EN;
#endif
#ifdef APB3_PERIPH_EN
    case APB3:
        return &APB3_PERIPH_EN;
#endif
#ifdef APB4_PERIPH_EN
    case APB4:
        return &APB4_PERIPH_EN;
#endif
#ifdef AHB_PERIPH_EN
    case AHB:
        return &AHB_PERIPH_EN;
#endif
#ifdef AHB1_PERIPH_EN
    case AHB1:
        return &AHB1_PERIPH_EN;
#endif
#ifdef AHB2_PERIPH_EN
    case AHB2:
        return &AHB2_PERIPH_EN;
#endif
#ifdef AHB22_PERIPH_EN
    case AHB22:
        return &AHB22_PERIPH_EN;
#endif
#ifdef AHB3_PERIPH_EN
    case AHB3:
        return &AHB3_PERIPH_EN;
#endif
#ifdef AHB4_PERIPH_EN
    case AHB4:
        return &AHB4_PERIPH_EN;
#endif
#ifdef IOP_PERIPH_EN
    case IOP:
        return &IOP_PERIPH_EN;
#endif

    case BUS_NUMOF:
        assert(false);
        return NULL;
    }

    assert(false);
    return NULL;
}

static volatile uint32_t* _rcc_dis_reg(bus_t bus)
{
    /* Note this switch case is designed in such a way that a default is only
       provided when all other cases are *not* provided. This is to ensure that
       either all cases or no cases are provided. Anything else will cause the
       compiler to emit a warning. */
    switch (bus) {
#ifdef APB1_PERIPH_DIS
    #define RCC_REG_IS_ATOMIC 1
    case APB1:
        return &APB1_PERIPH_DIS;
    #endif
#ifdef APB12_PERIPH_DIS
    #define RCC_REG_IS_ATOMIC 1
    case APB12:
        return &APB12_PERIPH_DIS;
#endif
#ifdef APB2_PERIPH_DIS
    #define RCC_REG_IS_ATOMIC 1
    case APB2:
        return &APB2_PERIPH_DIS;
#endif
#ifdef APB22_PERIPH_DIS
    #define RCC_REG_IS_ATOMIC 1
    case APB22:
        return &APB22_PERIPH_DIS;
#endif
#ifdef APB3_PERIPH_DIS
    #define RCC_REG_IS_ATOMIC 1
    case APB3:
        return &APB3_PERIPH_DIS;
#endif
#ifdef APB4_PERIPH_DIS
    #define RCC_REG_IS_ATOMIC 1
    case APB4:
        return &APB4_PERIPH_DIS;
#endif
#ifdef AHB_PERIPH_DIS
    #define RCC_REG_IS_ATOMIC 1
    case AHB:
        return &AHB_PERIPH_DIS;
#endif
#ifdef AHB1_PERIPH_DIS
    #define RCC_REG_IS_ATOMIC 1
    case AHB1:
        return &AHB1_PERIPH_DIS;
#endif
#ifdef AHB2_PERIPH_DIS
    #define RCC_REG_IS_ATOMIC 1
    case AHB2:
        return &AHB2_PERIPH_DIS;
#endif
#ifdef AHB22_PERIPH_DIS
    #define RCC_REG_IS_ATOMIC 1
    case AHB22:
        return &AHB22_PERIPH_DIS;
#endif
#ifdef AHB3_PERIPH_DIS
    #define RCC_REG_IS_ATOMIC 1
    case AHB3:
        return &AHB3_PERIPH_DIS;
#endif
#ifdef AHB4_PERIPH_DIS
    #define RCC_REG_IS_ATOMIC 1
    case AHB4:
        return &AHB4_PERIPH_DIS;
#endif
#ifdef IOP_PERIPH_DIS
    #define RCC_REG_IS_ATOMIC 1
    case IOP:
        return &IOP_PERIPH_DIS;
#endif

    /* If MCU does not have separate set/clear bits. */
#if RCC_REG_IS_ATOMIC == 0
    default:
        return _rcc_en_reg(bus);
#else
    case BUS_NUMOF:
        assert(false);
        return NULL;
#endif
    }

    assert(false);
    return NULL;
}

static volatile uint32_t* _rcc_lp_en_reg(bus_t bus)
{
    /* Note this switch case is designed in such a way that a default is only
       provided when all other cases are *not* provided. This is to ensure that
       either all cases or no cases are provided. Anything else will cause the
       compiler to emit a warning. */
    switch (bus) {
#ifdef APB1_PERIPH_LP_EN
    #define HAS_LP_MODE 1
    case APB1:
        return &APB1_PERIPH_LP_EN;
#endif
#ifdef APB12_PERIPH_LP_EN
    #define HAS_LP_MODE 1
    case APB12:
        return &APB12_PERIPH_LP_EN;
#endif
#ifdef APB2_PERIPH_LP_EN
    #define HAS_LP_MODE 1
    case APB2:
        return &APB2_PERIPH_LP_EN;
#endif
#ifdef APB22_PERIPH_LP_EN
    #define HAS_LP_MODE 1
    case APB22:
        return &APB22_PERIPH_LP_EN;
#endif
#ifdef APB3_PERIPH_LP_EN
    #define HAS_LP_MODE 1
    case APB3:
        return &APB3_PERIPH_LP_EN;
#endif
#ifdef APB4_PERIPH_LP_EN
    #define HAS_LP_MODE 1
    case APB4:
        return &APB4_PERIPH_LP_EN;
#endif
#ifdef AHB_PERIPH_LP_EN
    #define HAS_LP_MODE 1
    case AHB:
        return &AHB_PERIPH_LP_EN;
#endif
#ifdef AHB1_PERIPH_LP_EN
    #define HAS_LP_MODE 1
    case AHB1:
        return &AHB1_PERIPH_LP_EN;
#endif
#ifdef AHB2_PERIPH_LP_EN
    #define HAS_LP_MODE 1
    case AHB2:
        return &AHB2_PERIPH_LP_EN;
#endif
#ifdef AHB22_PERIPH_LP_EN
    #define HAS_LP_MODE 1
    case AHB22:
        return &AHB22_PERIPH_LP_EN;
#endif
#ifdef AHB3_PERIPH_LP_EN
    #define HAS_LP_MODE 1
    case AHB3:
        return &AHB3_PERIPH_LP_EN;
#endif
#ifdef AHB4_PERIPH_LP_EN
    #define HAS_LP_MODE 1
    case AHB4:
        return &AHB4_PERIPH_LP_EN;
#endif
#ifdef IOP_PERIPH_LP_EN
    #define HAS_LP_MODE 1
    case IOP:
        return &IOP_PERIPH_LP_EN;
#endif

#if HAS_LP_MODE == 0
    default:
        /* fall through */
#endif
    case BUS_NUMOF:
        assert(false);
        return NULL;
    }

    assert(false);
    return NULL;
}

static volatile uint32_t* _rcc_lp_dis_reg(bus_t bus)
{
#if RCC_REG_IS_ATOMIC && HAS_LP_MODE
    #error "Atomic disable of periph-in-low-power-mode not implemented yet."
#endif

    return _rcc_lp_en_reg(bus);
}

static void _rcc_reg_set(volatile uint32_t *reg, uint32_t mask)
{
    assert(reg);

    if (IS_ACTIVE(RCC_REG_IS_ATOMIC)) {
        *reg = mask;
    }
    else {
        const int irq_state = irq_disable();
        *reg |= mask;
        irq_restore(irq_state);
    }
}

static void _rcc_reg_clr(volatile uint32_t *reg, uint32_t mask)
{
    assert(reg);

    if (IS_ACTIVE(RCC_REG_IS_ATOMIC)) {
        *reg = mask;
    }
    else {
        const int irq_state = irq_disable();
        *reg &= ~(mask);
        irq_restore(irq_state);
    }
}

uint32_t periph_apb_clk(bus_t bus)
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

uint32_t periph_timer_clk(bus_t bus)
{
    return periph_apb_clk(bus) * apbmul[bus];
}

void periph_clk_en(bus_t bus, uint32_t mask)
{
    assert(bus < BUS_NUMOF);

   _rcc_reg_set(_rcc_en_reg(bus), mask);

    /* stm32xx-errata: Delay after a RCC peripheral clock enable */
    __DSB();
}

void periph_clk_dis(bus_t bus, uint32_t mask)
{
    assert(bus < BUS_NUMOF);
    _rcc_reg_clr(_rcc_dis_reg(bus), mask);
}

MAYBE_UNUSED static inline
void _periph_lpclk_en(bus_t bus, uint32_t mask)
{
    assert(bus < BUS_NUMOF);
    _rcc_reg_set(_rcc_lp_en_reg(bus), mask);
}

MAYBE_UNUSED static inline
void _periph_lpclk_dis(bus_t bus, uint32_t mask)
{
    assert(bus < BUS_NUMOF);
    _rcc_reg_clr(_rcc_lp_dis_reg(bus), mask);
}

#if HAS_LP_MODE
void periph_lpclk_en(bus_t bus, uint32_t mask)
{
    /* call the function's implementation, which is outside the ifdef */
    _periph_lpclk_en(bus, mask);
}
#endif

#if HAS_LP_MODE
void periph_lpclk_dis(bus_t bus, uint32_t mask)
{
    /* call the function's implementation, which is outside the ifdef */
    _periph_lpclk_dis(bus, mask);
}
#endif
