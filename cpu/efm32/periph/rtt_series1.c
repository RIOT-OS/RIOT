/*
 * Copyright (C) 2016-2017 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_efm32
 * @ingroup     drivers_periph_rtt
 * @{
 *
 * @file
 * @brief       RTT peripheral driver implementation for EFM32 Series 1 MCUs
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 * @}
 */

#include "cpu.h"

#include "periph_conf.h"
#include "periph/rtt.h"

#include "em_cmu.h"
#include "em_rtcc.h"

typedef struct {
    rtt_cb_t alarm_cb;              /**< callback called from RTC alarm */
    void *alarm_arg;                /**< argument passed to the callback */
    rtt_cb_t overflow_cb;           /**< callback called when RTC overflows */
    void *overflow_arg;             /**< argument passed to the callback */
} rtt_state_t;

static rtt_state_t rtt_state;

/* prescaler of 32768 = 1 s of resolution and overflow each 194 days */
#if RTT_FREQUENCY == 1
#define RTT_CMU_CLK_DIV rtccCntPresc_32768
#elif RTT_FREQUENCY == 2
#define RTT_CMU_CLK_DIV rtccCntPresc_16384
#elif RTT_FREQUENCY == 4
#define RTT_CMU_CLK_DIV rtccCntPresc_8192
#elif RTT_FREQUENCY == 8
#define RTT_CMU_CLK_DIV rtccCntPresc_4096
#elif RTT_FREQUENCY == 16
#define RTT_CMU_CLK_DIV rtccCntPresc_2048
#elif RTT_FREQUENCY == 32
#define RTT_CMU_CLK_DIV rtccCntPresc_1024
#elif RTT_FREQUENCY == 64
#define RTT_CMU_CLK_DIV rtccCntPresc_512
#elif RTT_FREQUENCY == 128
#define RTT_CMU_CLK_DIV rtccCntPresc_256
#elif RTT_FREQUENCY == 256
#define RTT_CMU_CLK_DIV rtccCntPresc_128
#elif RTT_FREQUENCY == 512
#define RTT_CMU_CLK_DIV rtccCntPresc_64
#elif RTT_FREQUENCY == 1024
#define RTT_CMU_CLK_DIV rtccCntPresc_32
#elif RTT_FREQUENCY == 2048
#define RTT_CMU_CLK_DIV rtccCntPresc_16
#elif RTT_FREQUENCY == 4096
#define RTT_CMU_CLK_DIV rtccCntPresc_8
#elif RTT_FREQUENCY == 8192
#define RTT_CMU_CLK_DIV rtccCntPresc_4
#elif RTT_FREQUENCY == 16384
#define RTT_CMU_CLK_DIV rtccCntPresc_2
#elif RTT_FREQUENCY == 32768
#define RTT_CMU_CLK_DIV rtccCntPresc_1
#else
#warning "no matching prescaler for RTT_FREQUENCY"
#endif

void rtt_init(void)
{
    /* enable clocks */
    CMU_ClockEnable(cmuClock_CORELE, true);
    CMU_ClockEnable(cmuClock_RTCC, true);

    /* reset and initialize peripheral */
    RTCC_Init_TypeDef init = RTCC_INIT_DEFAULT;

    init.enable = false;
    init.presc = RTT_CMU_CLK_DIV;

    RTCC_Reset();
    RTCC_Init(&init);

    /* initialize alarm channel */
    RTCC_CCChConf_TypeDef init_channel = RTCC_CH_INIT_COMPARE_DEFAULT;

    RTCC_ChannelInit(0, &init_channel);

    /* enable interrupts */
    NVIC_ClearPendingIRQ(RTCC_IRQn);
    NVIC_EnableIRQ(RTCC_IRQn);

    /* enable peripheral */
    RTCC_Enable(true);
}

void rtt_set_overflow_cb(rtt_cb_t cb, void *arg)
{
    rtt_state.overflow_cb = cb;
    rtt_state.overflow_arg = arg;

    RTCC_IntClear(RTCC_IFC_OF);
    RTCC_IntEnable(RTCC_IEN_OF);
}

void rtt_clear_overflow_cb(void)
{
    rtt_state.overflow_cb = NULL;
    rtt_state.overflow_arg = NULL;

    RTCC_IntDisable(RTCC_IEN_OF);
}

uint32_t rtt_get_counter(void)
{
    return RTCC_CounterGet();
}

void rtt_set_counter(uint32_t counter)
{
    RTCC->CNT = counter & RTT_MAX_VALUE;
}

void rtt_set_alarm(uint32_t alarm, rtt_cb_t cb, void *arg)
{
    rtt_state.alarm_cb = cb;
    rtt_state.alarm_arg = arg;

    /* disable interrupt so it doesn't accidentally trigger */
    RTCC_IntDisable(RTCC_IEN_CC0);

    /* set compare register */
    RTCC_ChannelCCVSet(0, alarm & RTT_MAX_VALUE);

    /* enable the interrupt */
    RTCC_IntClear(RTCC_IFC_CC0);
    RTCC_IntEnable(RTCC_IEN_CC0);
}

uint32_t rtt_get_alarm(void)
{
    return RTCC_ChannelCCVGet(0);
}

void rtt_clear_alarm(void)
{
    rtt_state.alarm_cb = NULL;
    rtt_state.alarm_arg = NULL;

    /* disable the interrupt */
    RTCC_IntDisable(RTCC_IEN_CC0);
}

void rtt_poweron(void)
{
    CMU_ClockEnable(cmuClock_RTCC, true);
}

void rtt_poweroff(void)
{
    CMU_ClockEnable(cmuClock_RTCC, false);
}

void isr_rtcc(void)
{
    if ((RTCC_IntGet() & RTCC_IF_CC0)) {
        if (rtt_state.alarm_cb != NULL) {
            rtt_state.alarm_cb(rtt_state.alarm_arg);
        }

        /* clear interrupt */
        RTCC_IntClear(RTCC_IFC_CC0);
    }
    if (RTCC_IntGet() & RTCC_IF_OF) {
        if (rtt_state.overflow_cb != NULL) {
            rtt_state.overflow_cb(rtt_state.overflow_arg);
        }

        /* clear interrupt */
        RTCC_IntClear(RTCC_IFC_OF);
    }
    cortexm_isr_end();
}
