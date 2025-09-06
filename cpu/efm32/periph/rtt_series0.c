/*
 * SPDX-FileCopyrightText: 2016-2017 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_efm32
 * @ingroup     drivers_periph_rtt
 * @{
 *
 * @file
 * @brief       RTT peripheral driver implementation
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 * @}
 */

#include "cpu.h"

#include "periph_conf.h"
#include "periph/rtt.h"

#include "em_cmu.h"
#include "em_rtc.h"

typedef struct {
    rtt_cb_t alarm_cb;              /**< callback called from RTC alarm */
    void *alarm_arg;                /**< argument passed to the callback */
    rtt_cb_t overflow_cb;           /**< callback called when RTC overflows */
    void *overflow_arg;             /**< argument passed to the callback */
} rtt_state_t;

static rtt_state_t rtt_state;

/* prescaler of 32768 = 1 s of resolution and overflow each 194 days */
#if RTT_FREQUENCY == 1
#define RTT_CMU_CLK_DIV cmuClkDiv_32768
#elif RTT_FREQUENCY == 2
#define RTT_CMU_CLK_DIV cmuClkDiv_16384
#elif RTT_FREQUENCY == 4
#define RTT_CMU_CLK_DIV cmuClkDiv_8192
#elif RTT_FREQUENCY == 8
#define RTT_CMU_CLK_DIV cmuClkDiv_4096
#elif RTT_FREQUENCY == 16
#define RTT_CMU_CLK_DIV cmuClkDiv_2048
#elif RTT_FREQUENCY == 32
#define RTT_CMU_CLK_DIV cmuClkDiv_1024
#elif RTT_FREQUENCY == 64
#define RTT_CMU_CLK_DIV cmuClkDiv_512
#elif RTT_FREQUENCY == 128
#define RTT_CMU_CLK_DIV cmuClkDiv_256
#elif RTT_FREQUENCY == 256
#define RTT_CMU_CLK_DIV cmuClkDiv_128
#elif RTT_FREQUENCY == 512
#define RTT_CMU_CLK_DIV cmuClkDiv_64
#elif RTT_FREQUENCY == 1024
#define RTT_CMU_CLK_DIV cmuClkDiv_32
#elif RTT_FREQUENCY == 2048
#define RTT_CMU_CLK_DIV cmuClkDiv_16
#elif RTT_FREQUENCY == 4096
#define RTT_CMU_CLK_DIV cmuClkDiv_8
#elif RTT_FREQUENCY == 8192
#define RTT_CMU_CLK_DIV cmuClkDiv_4
#elif RTT_FREQUENCY == 16384
#define RTT_CMU_CLK_DIV cmuClkDiv_2
#elif RTT_FREQUENCY == 32768
#define RTT_CMU_CLK_DIV cmuClkDiv_1
#else
#warning "no matching prescaler for RTT_FREQUENCY"
#endif

void rtt_init(void)
{
    /* setup prescaler */
    CMU_ClockDivSet(cmuClock_RTC, RTT_CMU_CLK_DIV);

    /* enable clocks */
    CMU_ClockEnable(cmuClock_CORELE, true);
    CMU_ClockEnable(cmuClock_RTC, true);

    /* reset and initialize peripheral */
    RTC_Init_TypeDef init = RTC_INIT_DEFAULT;

    init.enable = false;
    init.comp0Top = false;

    RTC_Reset();
    RTC_Init(&init);

    /* enable interrupts */
    RTC_IntEnable(RTC_IEN_OF);

    NVIC_ClearPendingIRQ(RTC_IRQn);
    NVIC_EnableIRQ(RTC_IRQn);

    /* enable peripheral */
    RTC_Enable(true);
}

void rtt_set_overflow_cb(rtt_cb_t cb, void *arg)
{
    rtt_state.overflow_cb = cb;
    rtt_state.overflow_arg = arg;

    RTC_IntClear(RTC_IFC_OF);
    RTC_IntEnable(RTC_IEN_OF);
}

void rtt_clear_overflow_cb(void)
{
    rtt_state.overflow_cb = NULL;
    rtt_state.overflow_arg = NULL;

    RTC_IntDisable(RTC_IEN_OF);
}

uint32_t rtt_get_counter(void)
{
    return RTC_CounterGet();
}

void rtt_set_counter(uint32_t counter)
{
    RTC->CNT = counter & RTT_MAX_VALUE;
}

void rtt_set_alarm(uint32_t alarm, rtt_cb_t cb, void *arg)
{
    rtt_state.alarm_cb = cb;
    rtt_state.alarm_arg = arg;

    /* disable interrupt so it doesn't accidentally trigger */
    RTC_IntDisable(RTC_IEN_COMP0);

    /* set compare register */
    RTC_CompareSet(0, alarm & RTT_MAX_VALUE);

    /* enable the interrupt */
    RTC_IntClear(RTC_IFC_COMP0);
    RTC_IntEnable(RTC_IEN_COMP0);
}

uint32_t rtt_get_alarm(void)
{
    return RTC_CompareGet(0);
}

void rtt_clear_alarm(void)
{
    rtt_state.alarm_cb = NULL;
    rtt_state.alarm_arg = NULL;

    /* disable the interrupt */
    RTC_IntDisable(RTC_IEN_COMP0);
}

void rtt_poweron(void)
{
    CMU_ClockEnable(cmuClock_RTC, true);
}

void rtt_poweroff(void)
{
    CMU_ClockEnable(cmuClock_RTC, false);
}

void isr_rtc(void)
{
    if ((RTC_IntGet() & RTC_IF_COMP0)) {
        if (rtt_state.alarm_cb != NULL) {
            rtt_state.alarm_cb(rtt_state.alarm_arg);
        }

        /* clear interrupt */
        RTC_IntClear(RTC_IFC_COMP0);
    }
    if (RTC_IntGet() & RTC_IF_OF) {
        if (rtt_state.overflow_cb != NULL) {
            rtt_state.overflow_cb(rtt_state.overflow_arg);
        }

        /* clear interrupt */
        RTC_IntClear(RTC_IFC_OF);
    }
    cortexm_isr_end();
}
