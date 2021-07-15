/*
 * Copyright (C) 2019 Mesotic SAS
 *
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_imx8-m4
 * @ingroup     drivers_periph_timer
 * @{
 *
 * @file        timer.c
 * @brief       Low-level General Purpose Timer driver implementation
 *
 * @author      Dylan Laduranty <dylanladuranty@mesotic.com>
 *
 * @}
 */

#include <stdlib.h>
#include <stdio.h>

#include "board.h"
#include "cpu.h"

#include "periph/timer.h"
#include "periph_conf.h"

#include "clk_conf.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/**
 * @brief Timer state memory
 */
static timer_isr_ctx_t config[TIMER_NUMOF];

static inline GPT_Type *dev(tim_t tim)
{
    return timer_config[tim].dev;
}

/**
 * @brief Setup the given timer
 */
int timer_init(tim_t tim, unsigned long freq, timer_cb_t cb, void *arg)
{
    (void) freq;

    ccm_set_root_clk(CCM_ROOT_CLK_GPT1, 4, 5, 3);
    ccm_set_ccgr(CCM_CCGR_GPT1, CLK_ENABLE_ALL_DOMAINS);
    ccm_en_root_clk(CCM_ROOT_CLK_GPT1, 1);

    /* Disable timer */
    dev(tim)->CR = 0;
    /* Issue a software reset of peripheral */
    dev(tim)->CR |= GPT_CR_SWR(1);
    while(dev(tim)->CR & GPT_CR_SWR_MASK) {}
    /* Select clock source to feed GPT */
    dev(tim)->CR |= GPT_CR_CLKSRC(timer_config[tim].clksrc);
    /* Clear status register */
    dev(tim)->SR = 0x3F;
    /* Reset CNT value */
    dev(tim)->CR |= GPT_CR_ENMOD_MASK | GPT_CR_STOPEN_MASK |
                    GPT_CR_WAITEN_MASK |  GPT_CR_FRR_MASK;

    /* Register callback */
    config[tim].cb = cb;
    config[tim].arg = arg;
    /* Start timer */
    dev(tim)->CR |= GPT_CR_EN_MASK;
    /* Enable interrupt */
    NVIC_EnableIRQ(timer_config[tim].irq);
    return 0;
}
int timer_set_absolute(tim_t tim, int channel, unsigned int value)
{
   (void) tim;
   (void) channel;

    dev(tim)->IR = GPT_IR_OF1IE_MASK;
    dev(tim)->OCR[0] = value;
    return 1;
}

unsigned int timer_read(tim_t tim)
{
    return dev(tim)->CNT;
}

static inline void gpt_isr(tim_t tim)
{
    /* Clear status flag */
    dev(tim)->SR = GPT_SR_OF1_MASK;
    dev(tim)->IR &= ~GPT_IR_OF1IE_MASK;
    /* call stored callback */
    if (config[tim].cb) {
        config[tim].cb(config[tim].arg, 0);
    }
}

#ifdef GPT_0_ISR
void GPT_0_ISR(void)
{
    gpt_isr(0);
    cortexm_isr_end();
}
#endif
#ifdef GPT_1_ISR
void GPT_1_ISR(void)
{
    gpt_isr(1);
    cortexm_isr_end();
}
#endif
#ifdef GPT_2_ISR
void GPT_2_ISR(void)
{
    gpt_isr(2);
    cortexm_isr_end();
}
#endif
#ifdef GPT_3_ISR
void GPT_3_ISR(void)
{
    gpt_isr(3);
    cortexm_isr_end();
}
#endif
#ifdef GPT_4_ISR
void GPT_4_ISR(void)
{
    gpt_isr(4);
    cortexm_isr_end();
}
#endif
#ifdef GPT_5_ISR
void GPT_5_ISR(void)
{
    gpt_isr(5);
    cortexm_isr_end();
}
#endif
