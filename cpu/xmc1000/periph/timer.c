/*
 * Copyright (C) 2015 Sebastian Sontberg <sebastian@sontberg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_xmc1000
 * @{
 *
 * @file
 * @brief       Low-level timer driver implementation
 *
 * @author      Sebastian Sontberg <sebastian@sontberg.de>
 *
 * @}
 */

#include <stdlib.h>
#include <stdio.h>

#include "cpu.h"
#include "board.h"
#include "sched.h"
#include "thread.h"
#include "periph_conf.h"
#include "periph/timer.h"
#include "periph/gating.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

#define CMC_LSB         (0)
/* CCUy is concatenated to CCUy-1 */
#define CMC_MSB         (CCU4_CC4_CMC_TCE_Msk)
/* External start activated through Event 0 */
#define CMC_START       (1 << CCU4_CC4_CMC_STRTS_Pos)
/* External stop activated through Event 0 */
#define CMC_STOP        (1 << CCU4_CC4_CMC_ENDS_Pos)

/**
 * Timer state memory
 */
static void (*callback)(int);
static unsigned int timer_value;

static inline void _slice_irq_enable(CCU4_CC4_TypeDef *slice)
{
    slice->INTE = (1 << CCU4_CC4_INTE_CMUE_Pos);
}

static inline void _slice_irq_disable(CCU4_CC4_TypeDef *slice)
{
    slice->INTE = 0;
}

static inline void _shadow_transfer(void)
{
    /* Global Channel Set, request shadow transfer for all slices */
    CCU40->GCSS = 0xffff;
}

static inline void _init_slice(CCU4_CC4_TypeDef *slice)
{
    /* Prescaler Control, set to PCLK/64 = 1Mhz */
    slice->PSC = 6UL;
    /* Timer Shadow Period Value, set to max */
    slice->PRS = 0xffff;
    /* Timer clear */
    slice->TCCLR = 1;
    /* External start/stop triggers through SCU */
    slice->INS =
        /* Select CCUCON as input for Events 0 & 1 */
        ((8 << CCU4_CC4_INS_EV0IS_Pos) | (8 << CCU4_CC4_INS_EV1IS_Pos) |
        /* Active start on HIGH and active stop on LOW level */
         (0 << CCU4_CC4_INS_EV0LM_Pos) | (0 << CCU4_CC4_INS_EV1LM_Pos) |
        /* Signal on rising edge */
         (1 << CCU4_CC4_INS_EV0EM_Pos) | (2 << CCU4_CC4_INS_EV1EM_Pos));

    slice->TC  =
        /* External stop clears run bit (stop) */
        ((0 << CCU4_CC4_TC_ENDM_Pos) |
        /* External start sets run bit and clears timer (flush/start) */
         (1 << CCU4_CC4_TC_STRM_Pos) |
        /* Enable shadow transfer on timer clear (aka on STRM) */
         (1 << CCU4_CC4_TC_CLST_Pos));
}

int timer_init(tim_t dev, unsigned int ticks_per_us, void (*cb)(int))
{
    if ((dev == TIMER_UNDEFINED) || (ticks_per_us != 1)) {
        return -1;
    }

    CCU4_GLOBAL_TypeDef *unit;

    /* save callback reference */
    callback = cb;

    /* Steps as in chapter 16.6.1 "Initialization Sequence" */

    /* To be expanded for devices with more than one CAPCOM unit */
    unit = CCU40;
    /* 1st step: "Enable the CCU4 clock" */
    GATING_CLEAR(CCU40);

    /* 2nd step: "Enable the prescaler block" */
    unit->GIDLC = CCU4_GIDLC_SPRB_Msk;

    /* 3rd step: Configure the global CCU4 register GCTRL */
    unit->GCTRL = 0;

    /* 4th step: Configure all the registers related to the required
     * Timer Slice(s) functions, including the interrupt/service
     * request configuration. */
    _init_slice(CCU40_CC40);
    _init_slice(CCU40_CC41);
    _init_slice(CCU40_CC42);
    _init_slice(CCU40_CC43);

    /* Setup counter concatenation */
    CCU40_CC41->CMC = CCU4_CC4_CMC_TCE_Msk;
    CCU40_CC43->CMC = CCU4_CC4_CMC_TCE_Msk;

    /* Wire service request msb[X] -> isr_ccuX*/
    CCU40_CC41->SRS  = 0 << CCU4_CC4_SRS_CMSR_Pos;
    CCU40_CC43->SRS  = 1 << CCU4_CC4_SRS_CMSR_Pos;

    /* enable NVIC interrupts */
    NVIC_SetPriority(CCU40_0_IRQn, TIMER_IRQ_PRIO);
    NVIC_SetPriority(CCU40_1_IRQn, TIMER_IRQ_PRIO);
    NVIC_EnableIRQ(CCU40_0_IRQn);
    NVIC_EnableIRQ(CCU40_1_IRQn);

    /* 5th step: (not applicable) */
    /* 6th step: Enable the specific timer slice(s) */
    unit->GIDLC = (CCU4_GIDLC_CS0I_Msk | CCU4_GIDLC_CS1I_Msk |
                   CCU4_GIDLC_CS2I_Msk | CCU4_GIDLC_CS3I_Msk);

    _shadow_transfer();

    /* Synchronous start of all counters */
    timer_start(dev);

    return 0;
}

int timer_set_absolute(tim_t dev, int channel, unsigned int ticks)
{
    unsigned int now = timer_read(dev);

    if (ticks > now) {
        return timer_set(dev, channel, ticks - now);
    }
    else {
        DEBUG("ticks: %u <= now: %u, diff: %u\n", ticks, now, now - ticks);
        callback(channel);
        return 1;
    }
}

int timer_set(tim_t dev, int ch, unsigned int timeout)
{
    uint16_t msb_crs = (timeout & 0xffff0000) >> 16;
    uint16_t lsb_crs = (timeout & 0x0000ffff);

    /* Halt timer. This way both channels stay in sync */
    SCU_GENERAL->CCUCON = 0;

    switch (ch) {
        case 0:
            CCU40_CC40->CRS = lsb_crs;
            CCU40_CC41->CRS = msb_crs;
            _slice_irq_enable(CCU40_CC41);
            break;
        case 1:
            CCU40_CC42->CRS = lsb_crs;
            CCU40_CC43->CRS = msb_crs;
            _slice_irq_enable(CCU40_CC43);
            break;
        default:
            return 0;
    }

    CCU40_CC40->CMC = CMC_LSB | CMC_STOP | CMC_START;
    CCU40_CC41->CMC = CMC_MSB | CMC_STOP | CMC_START;
    CCU40_CC42->CMC = CMC_LSB | CMC_STOP | CMC_START;
    CCU40_CC43->CMC = CMC_MSB | CMC_STOP | CMC_START;

    /* Backup current timer value */
    timer_value += (CCU40_CC41->TIMER << 16) | CCU40_CC40->TIMER;

    _shadow_transfer();

    /* Send start signal. Will flush counter, do a shadow transfer and
     * resume running the timer */
    SCU_GENERAL->CCUCON = 1;

    return 1;
}

int timer_clear(tim_t dev, int ch)
{
    switch (ch) {
        case 0:
            CCU40_CC41->INTE = 0;
            return 1;
        case 1:
            CCU40_CC43->INTE = 0;
            return 1;
        default:
            return 0;
    }
}

unsigned int timer_read(tim_t dev)
{
    return timer_value + ((CCU40_CC41->TIMER << 16) | CCU40_CC40->TIMER);
}


void timer_start(tim_t dev)
{
    CCU40_CC40->CMC = CMC_LSB | CMC_START;
    CCU40_CC41->CMC = CMC_MSB | CMC_START;

    CCU40_CC42->CMC = CMC_LSB | CMC_START;
    CCU40_CC43->CMC = CMC_MSB | CMC_START;

    SCU_GENERAL->CCUCON = 1;
}

void timer_stop(tim_t dev)
{
    CCU40_CC40->CMC = CMC_LSB | CMC_STOP;
    CCU40_CC41->CMC = CMC_MSB | CMC_STOP;

    CCU40_CC42->CMC = CMC_LSB | CMC_STOP;
    CCU40_CC43->CMC = CMC_MSB | CMC_STOP;

    SCU_GENERAL->CCUCON = 0;
}

void timer_irq_enable(tim_t dev)
{
    _slice_irq_enable(CCU40_CC41);
    _slice_irq_enable(CCU40_CC43);
}


void timer_irq_disable(tim_t dev)
{
    _slice_irq_disable(CCU40_CC41);
    _slice_irq_disable(CCU40_CC43);
}

void timer_reset(tim_t dev)
{
    timer_value = 0;
    SCU_GENERAL->CCUCON = 0;
    SCU_GENERAL->CCUCON = 1;
}

void isr_ccu0(void)
{
    /* execute callback  */
    callback(0);

    if (sched_context_switch_request) {
        thread_yield();
    }
}

void isr_ccu1(void)
{
    /* execute callback */
    callback(1);

    if (sched_context_switch_request) {
        thread_yield();
    }
}
