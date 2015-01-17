/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     kinetis
 * @{
 *
 * @file
 * @brief       Periodic Interrupt Timer driver implementation
 *
 * @author      Finn Wilke <finn.wilke@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */


#include <stdlib.h>
#include <stdbool.h>

#include "cpu.h"
#include "board.h"
#include "sched.h"
#include "thread.h"
#include "periph_conf.h"
#include "periph/timer.h"
#include "kinetis_sim.h"

#include "MK20DZ10.h"

/** The number of PIT timers present in the system */
#define PIT_TIMER_COUNT 4

/** Unified IRQ handler for all timers */
static inline void isr_timer_pit_handler(short timer);

/**
 * @brief Type for timer state
 */
typedef struct {
    bool is_enabled;
    bool is_continuous;
    void (*callback)(short);
    void *callback_data;
} pit_timer_conf_t;

/**
 * @brief Timer state memory
 */
static pit_timer_conf_t config[PIT_TIMER_COUNT];

/**
 * @brief Returns the IRQ number of the timer
 *
 * @return      IRQ number of the timer or INT_MAX if the timer was invalid
 * @{
 */
static int _pit_get_irqn(short timer)
{
    switch (timer) {
    case 0:
        return PIT_Channel_0_IRQn;
    case 1:
        return PIT_Channel_1_IRQn;
    case 2:
        return PIT_Channel_2_IRQn;
    case 3:
        return PIT_Channel_3_IRQn;
    default:
        return INT_MAX;
    }
}
/** @} */

/*---------------------------------------------------------------------------*/

int kinetis_pit_timer_irq_enable(short timer)
{
    if (timer >= PIT_TIMER_COUNT) {
        return -1;
    }

    int irqn = _pit_get_irqn(timer);

    NVIC_EnableIRQ(irqn);
    NVIC_SetPriority(irqn, TIMER_IRQ_PRIO);

    /* Starts the timer interrupt */
    PIT_TCTRL(timer) |= (1 << PIT_TCTRL_TIE_SHIFT);
    return 0;
}

int kinetis_pit_timer_irq_disable(short timer)
{
    if (timer >= PIT_TIMER_COUNT) {
        return -1;
    }

    /* Stop the timer interrupt */
    PIT_TCTRL(timer) &= ~(1 << PIT_TCTRL_TIE_SHIFT);

    NVIC_DisableIRQ(_pit_get_irqn(timer));

    return 0;
}

/*---------------------------------------------------------------------------*/

/**
 * @brief Clear timer, stop interrupt
 *
 * @param[in]   timer   The timer to clear
 * @return      0 if successful, -1 if the timer number is invalid
 * @{
 */
static int _pit_timer_clear(short timer)
{
    if (timer >= PIT_TIMER_COUNT) {
        return -1;
    }

    PIT_TCTRL(timer) = 0;
    PIT_CVAL(timer) = 0;
    PIT_TFLG(timer) = (1 << PIT_TFLG_TIF_SHIFT);

    return kinetis_pit_timer_irq_disable(timer);
}
/** @} */

/**
 * @brief Start timer, enable interrupt
 *
 * @param[in]   timer   The timer to start
 * @return      0 if successful, -1 if the timer number is invalid
 * @{
 */
static int _pit_timer_begin(short timer, uint32_t ldval, bool enable_interrupt)
{
    if (timer >= PIT_TIMER_COUNT) {
        return -1;
    }

    PIT_LDVAL(timer) = ldval;

    if (enable_interrupt) {
        kinetis_pit_timer_irq_enable(timer);
    }

    PIT_TCTRL(timer) |= (1 << PIT_TCTRL_TEN_SHIFT);
    return 0;
}

/*---------------------------------------------------------------------------*/

void kinetis_pit_timers_start(void)
{
    /* Start all timers */
    PIT_MCR &= ~(1 << PIT_MCR_MDIS_SHIFT);
}

void kinetis_pit_timers_stop(void)
{
    /* Stop all timers */
    PIT_MCR |= (1 << PIT_MCR_MDIS_SHIFT);
}

/*---------------------------------------------------------------------------*/

void kinetis_pit_timers_irq_enable(void)
{
    for (short i = 0; i < PIT_TIMER_COUNT; i++) {
        kinetis_pit_timer_irq_enable(i);
    }
}

void kinetis_pit_timers_irq_disable(void)
{
    for (short i = 0; i < PIT_TIMER_COUNT; i++) {
        kinetis_pit_timer_irq_disable(i);
    }
}

/*---------------------------------------------------------------------------*/

void kinetis_pit_timers_init(void)
{
    /* Enable the clock gate */
    kinetis_clock_gate_enable(K20_CGATE_PIT);

    /* start the timers */
    kinetis_pit_timers_start();
}

void kinetis_pit_timers_deinit(void)
{
    /* Stop all timers */
    void kinetis_pit_timers_stop(void);

    for (short i = 0; i < PIT_TIMER_COUNT; i++) {
        _pit_timer_clear(0);
    }

    /* Disable the clock gate */
    kinetis_clock_gate_disable(K20_CGATE_PIT);
}

/*---------------------------------------------------------------------------*/

int kinetis_pit_timer_set(short timer, uint32_t ticks, bool is_continuous,
                          void (*callback)(short), void *callback_data)
{
    if (timer >= PIT_TIMER_COUNT || config[timer].is_enabled) {
        /* Return -1 if a timer is already configured */

        return -1;
    }

    /* Disable timer */
    _pit_timer_clear(timer);

    /* Configure timer */
    config[timer].is_enabled = true;
    config[timer].is_continuous = is_continuous;
    config[timer].callback = callback;
    config[timer].callback_data = callback_data;

    /* Enable timer */
    _pit_timer_begin(timer, ticks, true);

    return 0;
}

uint32_t kinetis_pit_timer_read(short timer)
{
    if (timer >= PIT_TIMER_COUNT) {
        return -1;
    }

    return *(uint32_t *)PIT_CVAL(timer);
}

int kinetis_pit_timer_stop(short timer)
{
    if (timer >= PIT_TIMER_COUNT || config[timer].is_enabled == false) {
        return -1;
    }

    _pit_timer_clear(timer);

    config[timer].is_enabled = false;
    return 0;
}

/*---------------------------------------------------------------------------*/

/**
 * @brief The combined interrupt handler for the PIT module
 *
 * @param[in]   timer       The timer to handle
 * @{
 */
static void isr_timer_pit_handler(short timer)
{
    if (!config[timer].is_enabled) {
        // this isr shouldn't have fired
        return;
    }

    if (!config[timer].is_continuous) {
        // A PIT timer always continues until stopped
        kinetis_pit_timer_stop(timer);
    }

    if (config[timer].callback != NULL) {
        config[timer].callback(*(short *)(config[timer].callback_data));
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}
/** @} */

/**
 * @brief Interrupt handler for timer 0
 * @{
 */
void isr_timer_pit0(void)
{
    isr_timer_pit_handler(0);
}
/** @} */

/**
 * @brief Interrupt handler for timer 1
 * @{
 */
void isr_timer_pit1(void)
{
    isr_timer_pit_handler(1);
}
/** @} */

/**
 * @brief Interrupt handler for timer 2
 * @{
 */
void isr_timer_pit2(void)
{
    isr_timer_pit_handler(2);
}
/** @} */

/**
 * @brief Interrupt handler for timer 3
 * @{
 */
void isr_timer_pit3(void)
{
    isr_timer_pit_handler(3);
}
