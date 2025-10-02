/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_stm32
 * @ingroup     drivers_periph_rtt
 * @{
 *
 * @file
 * @brief       RTT implementation using LPTIM1
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Andres Diaz    <andres.diaz@andeselectronics.cl>
 * @}
 */

#include <assert.h>

#include "cpu.h"
#include "irq.h"
#include "periph/rtt.h"
#include "stmclk.h"

/* this driver is only valid for STM CPUs that provide LPTIMERs */
#if defined(LPTIM1)

/* figure out the used pre-scaler */
#if (RTT_FREQUENCY == 32768)
#define PRE                 (0)
#elif (RTT_FREQUENCY == 16384)
#define PRE                 (LPTIM_CFGR_PRESC_0)
#elif (RTT_FREQUENCY == 8192)
#define PRE                 (LPTIM_CFGR_PRESC_1)
#elif (RTT_FREQUENCY == 4096)
#define PRE                 (LPTIM_CFGR_PRESC_1 | LPTIM_CFGR_PRESC_0)
#elif (RTT_FREQUENCY == 2048)
#define PRE                 (LPTIM_CFGR_PRESC_2)
#elif (RTT_FREQUENCY == 1024)
#define PRE                 (LPTIM_CFGR_PRESC_2 | LPTIM_CFGR_PRESC_0)
#elif (RTT_FREQUENCY == 512)
#define PRE                 (LPTIM_CFGR_PRESC_2 | LPTIM_CFGR_PRESC_1)
#elif (RTT_FREQUENCY == 256)
#define PRE                 (LPTIM_CFGR_PRESC)
#else
#error "RTT config: RTT_FREQUENCY not configured or invalid for your board"
#endif

#if defined(CPU_FAM_STM32F4) || defined(CPU_FAM_STM32F7)
#define CLOCK_SRC_REG       RCC->DCKCFGR2
#define CLOCK_SRC_MASK      RCC_DCKCFGR2_LPTIM1SEL
#if IS_ACTIVE(CONFIG_BOARD_HAS_LSE)
#define CLOCK_SRC_CFG       (RCC_DCKCFGR2_LPTIM1SEL_1 | RCC_DCKCFGR2_LPTIM1SEL_0)
#else
#define CLOCK_SRC_CFG       (RCC_DCKCFGR2_LPTIM1SEL_0)
#endif
#elif defined(CPU_FAM_STM32L5)
#define CLOCK_SRC_REG       RCC->CCIPR1
#define CLOCK_SRC_MASK      RCC_CCIPR1_LPTIM1SEL
#if IS_ACTIVE(CONFIG_BOARD_HAS_LSE)
#define CLOCK_SRC_CFG       (RCC_CCIPR1_LPTIM1SEL_1 | RCC_CCIPR1_LPTIM1SEL_0)
#else
#define CLOCK_SRC_CFG       (RCC_CCIPR1_LPTIM1SEL_0)
#endif
#else
#define CLOCK_SRC_REG       RCC->CCIPR
#define CLOCK_SRC_MASK      RCC_CCIPR_LPTIM1SEL
#if IS_ACTIVE(CONFIG_BOARD_HAS_LSE)
#define CLOCK_SRC_CFG       (RCC_CCIPR_LPTIM1SEL_1 | RCC_CCIPR_LPTIM1SEL_0)
#else
#define CLOCK_SRC_CFG       (RCC_CCIPR_LPTIM1SEL_0)
#endif
#endif

#if defined(CPU_FAM_STM32WB)
/* IM32 is the interrupt line used to wakeup the CPU on WB but is not defined
in the CMSIS. According to the reference manual, this is the first bit in the
register. */
#define EXTI_IMR2_IM32      (1 << 0)
#endif

#if defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32WB) || defined(CPU_FAM_STM32L5)
#define IMR_REG             IMR2
#define EXTI_IMR_BIT        EXTI_IMR2_IM32
#elif defined(CPU_FAM_STM32G0) || defined(CPU_FAM_STM32WL) || \
    defined(CPU_FAM_STM32C0)
#define IMR_REG             IMR1
#define EXTI_IMR_BIT        EXTI_IMR1_IM29
#elif defined(CPU_FAM_STM32G4)
#define IMR_REG             IMR2
#define EXTI_IMR_BIT        EXTI_IMR2_IM37
#elif defined(CPU_FAM_STM32L0)
#define IMR_REG             IMR
#define EXTI_IMR_BIT        EXTI_IMR_IM29
#else
#define IMR_REG             IMR
#define FTSR_REG            FTSR
#define RTSR_REG            RTSR
#define PR_REG              PR
#define EXTI_FTSR_BIT       EXTI_FTSR_TR23
#define EXTI_RTSR_BIT       EXTI_RTSR_TR23
#define EXTI_IMR_BIT        EXTI_IMR_MR23
#define EXTI_PR_BIT         EXTI_PR_PR23
#endif

/* allocate memory for overflow and alarm callbacks + args */
static rtt_cb_t ovf_cb = NULL;
static void *ovf_arg;
static rtt_cb_t to_cb = NULL;
static void *to_arg;

void rtt_init(void)
{
    /* Enable the low speed clock (LSE) */
    stmclk_enable_lfclk();

    /* power on the selected LPTIMER */
    rtt_poweron();

    /* stop the timer and reset configuration */
    LPTIM1->CR = 0;

    /* select low speed clock (LSI or LSE) */
    CLOCK_SRC_REG &= ~(CLOCK_SRC_MASK);
    CLOCK_SRC_REG |= CLOCK_SRC_CFG;

    /* set configuration: prescale factor and external clock (LSI or LSE) */
    LPTIM1->CFGR = PRE;
    /* enable overflow and compare interrupts */
    LPTIM1->IER = (LPTIM_IER_ARRMIE | LPTIM_IER_CMPMIE);
    /* configure the EXTI channel, as RTT interrupts are routed through it.
     * Needs to be configured to trigger on rising edges. */
    EXTI->IMR_REG |= EXTI_IMR_BIT;
#if !defined(CPU_FAM_STM32L4) && !defined(CPU_FAM_STM32L0) && \
    !defined(CPU_FAM_STM32WB) && !defined(CPU_FAM_STM32G4) && \
    !defined(CPU_FAM_STM32G0) && !defined(CPU_FAM_STM32WL) && \
    !defined(CPU_FAM_STM32L5) && !defined(CPU_FAM_STM32C0)
    EXTI->FTSR_REG &= ~(EXTI_FTSR_BIT);
    EXTI->RTSR_REG |= EXTI_RTSR_BIT;
    EXTI->PR_REG = EXTI_PR_BIT;
#endif
#if defined(TIM6_DAC_LPTIM1_SHARED_IRQ)
    NVIC_EnableIRQ(TIM6_DAC_LPTIM1_IRQn);
#else
    NVIC_EnableIRQ(LPTIM1_IRQn);
#endif
    /* enable timer */
    LPTIM1->CR = LPTIM_CR_ENABLE;
    /* set auto-reload value (timer needs to be enabled for this) */
    LPTIM1->ICR = LPTIM_ICR_ARROKCF;
    LPTIM1->ARR = RTT_MAX_VALUE;
    while (!(LPTIM1->ISR & LPTIM_ISR_ARROK)) {}
    /* start the timer */
    LPTIM1->CR |= LPTIM_CR_CNTSTRT;
}

uint32_t rtt_get_counter(void)
{
    uint32_t cnt;
    do {
        cnt = LPTIM1->CNT;
    } while (cnt != LPTIM1->CNT);
    return cnt;
}

void rtt_set_overflow_cb(rtt_cb_t cb, void *arg)
{
    assert(cb);

    unsigned is = irq_disable();
    ovf_cb  = cb;
    ovf_arg = arg;
    irq_restore(is);
}

void rtt_clear_overflow_cb(void)
{
    ovf_cb = NULL;
}

void rtt_set_alarm(uint32_t alarm, rtt_cb_t cb, void *arg)
{
    assert(cb && !(alarm & ~RTT_MAX_VALUE));

    unsigned is = irq_disable();
    LPTIM1->ICR = LPTIM_ICR_CMPOKCF;
    to_cb  = cb;
    to_arg = arg;
    LPTIM1->CMP = (uint16_t)alarm;
    while (!(LPTIM1->ISR & LPTIM_ISR_CMPOK)) {}
    irq_restore(is);
}

uint32_t rtt_get_alarm(void)
{
    return LPTIM1->CMP;
}

void rtt_clear_alarm(void)
{
    to_cb = NULL;
}

void rtt_poweron(void)
{
#ifdef RCC_APB1ENR1_LPTIM1EN
    periph_clk_en(APB1, RCC_APB1ENR1_LPTIM1EN);
#elif defined(RCC_APBENR1_LPTIM1EN)
    periph_clk_en(APB1, RCC_APBENR1_LPTIM1EN);
#else
    periph_clk_en(APB1, RCC_APB1ENR_LPTIM1EN);
#endif
}

void rtt_poweroff(void)
{
#ifdef RCC_APB1ENR1_LPTIM1EN
    periph_clk_dis(APB1, RCC_APB1ENR1_LPTIM1EN);
#elif defined(RCC_APBENR1_LPTIM1EN)
    periph_clk_dis(APB1, RCC_APBENR1_LPTIM1EN);
#else
    periph_clk_dis(APB1, RCC_APB1ENR_LPTIM1EN);
#endif
}

#if defined(CPU_FAM_STM32G0)
void isr_tim6_dac_lptim1(void)
#else
void isr_lptim1(void)
#endif
{
    if (LPTIM1->ISR & LPTIM_ISR_CMPM) {
        if (to_cb) {
            /* 'consume' the callback (as it might be set again in the cb) */
            rtt_cb_t tmp = to_cb;
            to_cb = NULL;
            tmp(to_arg);
        }
    }
    if (LPTIM1->ISR & LPTIM_ISR_ARRM) {
        if (ovf_cb) {
            ovf_cb(ovf_arg);
        }
    }
    LPTIM1->ICR = (LPTIM_ICR_ARRMCF | LPTIM_ICR_CMPMCF);
#if !defined(CPU_FAM_STM32L4) && !defined(CPU_FAM_STM32L0) && \
    !defined(CPU_FAM_STM32WB) && !defined(CPU_FAM_STM32G4) && \
    !defined(CPU_FAM_STM32G0) && !defined(CPU_FAM_STM32WL) && \
    !defined(CPU_FAM_STM32L5) && !defined(CPU_FAM_STM32C0)
    EXTI->PR_REG = EXTI_PR_BIT; /* only clear the associated bit */
#endif

    cortexm_isr_end();
}

#endif /* LPTIM1 */
