/*
 * Copyright (C) 2015 Freie Universität Berlin
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
 * @brief       Low-Power Timer driver implementation
 *
 * @author      Finn Wilke <finn.wilke@fu-berlin.de>
 *
 * @}
 */

#include "cpu-conf.h"
#include "kinetis_lptmr.h"
#include "kinetis_sim.h"
#include "cpu.h"
#include "periph_conf.h"
#include "MK20DZ10.h"
#include "thread.h"
#include "board.h"

#define ENABLE_DEBUG (1)
#include <debug.h>


/**
 * @brief The configuration for the lptmr timer(s)
 */
typedef struct {
    kinetis_lptmr_cb_t callback;
    void *callback_arg;
    bool has_set_value;
} lptmr_config_t;

static lptmr_config_t lptmr_config[LPTMR_NUM];


/**
 * @brief Returns the memory pointer of the specified lptmr
 *
 * @param[in]   timer       The timer for which to get the memory pointer
 * @return      The pointer of the specified timer
 * @{
 */
__STATIC_INLINE LPTMR_MemMapPtr _get_timer_base(kinetis_lptmr_t timer)
{
    if (timer == KINETIS_LPTMR0) {
        return LPTMR0_BASE_PTR;
    }

    return NULL;
}
/** @} */

/*---------------------------------------------------------------------------*/

void kinetis_lptmr_init(kinetis_lptmr_t timer, kinetis_lptmr_clock_t clock,
                        kinetis_lptmr_prescale_t prescale, bool freerunning)
{
    kinetis_clock_gate_enable(K20_CGATE_LPTMR);

    LPTMR_MemMapPtr dev = _get_timer_base(timer);

    /**
     * CSR *must* be written before doing anything
     */
    dev->CSR = 0;

    if (clock == KINETIS_LPTMR_CLOCK_MCGIRCLK) {
        /* Enable the MCGIRCLK */
        MCG_BASE_PTR->C1 |= MCG_C1_IRCLKEN_MASK;
    }

    /** Set prescaler and clock */
    if (prescale == KINETIS_LPTMR_PRE_DISABLE) {
        DEBUG("Disabling prescale\n");
        dev->PSR = LPTMR_PSR_PBYP_MASK | LPTMR_PSR_PCS(clock);
    } else {
        dev->PSR = LPTMR_PSR_PRESCALE(prescale) | LPTMR_PSR_PCS(clock);
    }

    if (freerunning) {
        dev->CSR |= LPTMR_CSR_TFC_MASK;
    }

    /** Enable the LPTMR */
    dev->CSR |= LPTMR_CSR_TEN_MASK;

    DEBUG("LPTMR initialized. Clock: %X, prescale: %X, freerunning: %s\n", clock,
          prescale, freerunning ? "true": "false");
}

void kinetis_lptmr_set_callback(kinetis_lptmr_t timer, kinetis_lptmr_cb_t callback,
                                void *argument)
{
    lptmr_config[timer].callback = callback;
    lptmr_config[timer].callback_arg = argument;

    DEBUG("LPTMR setting callback: %lX\n", (uint32_t)callback);
}

/*---------------------------------------------------------------------------*/

void kinetis_lptmr_enable_interrupt(kinetis_lptmr_t timer)
{
    LPTMR_MemMapPtr dev = _get_timer_base(timer);

    NVIC_EnableIRQ(Low_Power_Timer_IRQn);
    NVIC_SetPriority(Low_Power_Timer_IRQn, TIMER_IRQ_PRIO);

    dev->CSR |= LPTMR_CSR_TIE_MASK;

    DEBUG("LPTMR enabling interrupt.\n");
}

void kinetis_lptmr_disable_interrupt(kinetis_lptmr_t timer)
{
    LPTMR_MemMapPtr dev = _get_timer_base(timer);

    dev->CSR &= ~LPTMR_CSR_TIE_MASK;

    NVIC_DisableIRQ(Low_Power_Timer_IRQn);
}

/*---------------------------------------------------------------------------*/

uint16_t kinetis_lptmr_read(kinetis_lptmr_t timer)
{
    LPTMR_MemMapPtr dev = _get_timer_base(timer);

    /* On each read of the CNR, software must first write to the CNR with any value. */
    dev->CNR = 0;
    uint16_t value = dev->CNR & LPTMR_CNR_COUNTER_MASK;
    return value;
}

int kinetis_lptmr_get_remaining(kinetis_lptmr_t timer, uint16_t *remaining)
{
    if (lptmr_config[timer].has_set_value) {
        LPTMR_MemMapPtr dev = _get_timer_base(timer);

        uint16_t value = dev->CNR & LPTMR_CNR_COUNTER_MASK;
        uint16_t threshold = dev->CMR & LPTMR_CMR_COMPARE_MASK;

        /* This might overflow but the math still works */
        *remaining = threshold - value;

        return 0;
    }
    else {
        return -1;
    }
}

/*---------------------------------------------------------------------------*/

void kinetis_lptmr_set_absolute(kinetis_lptmr_t timer, uint16_t value)
{
    /**
     * This is sort of difficult. We may only set the CMR value of a running
     * timer when CSR & TCF are set. Otherwise we must save the current value
     * and restart the timer
     */
    LPTMR_MemMapPtr dev = _get_timer_base(timer);

    if (dev->CSR & LPTMR_CSR_TCF_MASK) {
        /** Easy one */
        dev->CMR = value;

        /** Assert the TCF flag so the interrupt fires again */
        dev->CSR |= LPTMR_CSR_TCF_MASK;
    } else {
        /**
         * We already have a timer running. This means we need to stop and
         * restart
         */
         dev->CNR = 0;
         uint16_t cnr = dev->CNR & LPTMR_CNR_COUNTER_MASK;

         dev->CSR &= ~LPTMR_CSR_TEN_MASK;
         dev->CMR = LPTMR_CMR_COMPARE(value - cnr);
         dev->CSR |= LPTMR_CSR_TEN_MASK;
    }

    lptmr_config[timer].has_set_value = true;
}

void kinetis_lptmr_set_offset(kinetis_lptmr_t timer, uint16_t offset)
{
    uint16_t current = kinetis_lptmr_read(timer);
    uint16_t absolute = current + offset;
    kinetis_lptmr_set_absolute(timer, absolute);
}

void kinetis_lptmr_clear(kinetis_lptmr_t timer)
{
    /* This will not disable any interrupts because we are lazy */
    lptmr_config[timer].has_set_value = false;
}

/*---------------------------------------------------------------------------*/

/**
 * @brief The interrupt handler for the lptmr timer
 *
 * @param[in]  timer  The timer for which the interrupt should be handled
 * @{
 */
void isr_lptmr_handler(kinetis_lptmr_t timer)
{
    LPTMR_MemMapPtr dev = _get_timer_base(timer);

    if (lptmr_config[timer].has_set_value) {
        /* only handle the interrupt if we actually want
         * to do something with the timer irq */
        uint16_t value = dev->CNR & LPTMR_CNR_COUNTER_MASK;

        if (lptmr_config[timer].callback != NULL) {
            lptmr_config[timer].callback(value, lptmr_config[timer].callback_arg);
        }

        lptmr_config[timer].has_set_value = false;
    }

    /** Clear interrupt flag */
    dev->CSR |= LPTMR_CSR_TCF_MASK;

    if (sched_context_switch_request) {
        thread_yield();
    }
}
/** @} */

/**
 * @brief The interrupt handler for lptmr0
 * @{
 */
void isr_lptmr(void)
{
    /* Assume the timer is 1, change this if needed for any device */
    isr_lptmr_handler(KINETIS_LPTMR0);

}
/** @} */