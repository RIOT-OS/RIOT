/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_fe310
 * @{
 *
 * @file
 * @brief       Platform-Level interrupt controller driver
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @}
 */

#include <assert.h>

#include "vendor/encoding.h"
#include "vendor/platform.h"
#include "cpu.h"
#include "plic.h"

/* PLIC external ISR function list */
static plic_isr_cb_t _ext_isrs[PLIC_NUM_INTERRUPTS];

static inline volatile uint32_t *_get_claim_complete_addr(void)
{
    uint32_t hart_id = read_csr(mhartid);

    /* Construct the claim address */
    return &PLIC_REG(PLIC_CLAIM_OFFSET +
                     (hart_id << PLIC_CLAIM_SHIFT_PER_TARGET));
}

static inline volatile uint32_t *_get_threshold_addr(void)
{
    uint32_t hart_id = read_csr(mhartid);

    /* Construct the claim address */
    return &PLIC_REG(PLIC_THRESHOLD_OFFSET +
                     (hart_id << PLIC_THRESHOLD_SHIFT_PER_TARGET));
}

static inline volatile uint32_t *_get_irq_reg(unsigned irq)
{
    uint32_t hart_id = read_csr(mhartid);

    return &PLIC_REG(PLIC_ENABLE_OFFSET +
                     (hart_id << PLIC_ENABLE_SHIFT_PER_TARGET)) +
           (irq >> 5);           /* Intentionally outside the PLIC_REG macro */
}

void plic_enable_interrupt(unsigned irq)
{
    volatile uint32_t *irq_reg = _get_irq_reg(irq);

    __atomic_fetch_or(irq_reg, 1 << (irq & 0x1f), __ATOMIC_RELAXED);
}

void plic_disable_interrupt(unsigned irq)
{
    volatile uint32_t *irq_reg = _get_irq_reg(irq);

    __atomic_fetch_and(irq_reg, ~(1 << (irq & 0x1f)), __ATOMIC_RELAXED);
}

void plic_set_threshold(unsigned threshold)
{
    volatile uint32_t *plic_threshold = _get_threshold_addr();

    *plic_threshold = threshold;

}

void plic_set_priority(unsigned irq, unsigned priority)
{
    assert(irq <= PLIC_NUM_INTERRUPTS);
    assert(irq != 0);
    *(&PLIC_REG(PLIC_PRIORITY_OFFSET) + irq) = priority;
}

static void plic_complete_interrupt(unsigned irq)
{
    volatile uint32_t *complete_addr = _get_claim_complete_addr();

    *complete_addr = irq;
}

static unsigned plic_claim_interrupt(void)
{
    return *_get_claim_complete_addr();
}

void plic_set_isr_cb(unsigned irq, plic_isr_cb_t cb)
{
    assert(irq <= PLIC_NUM_INTERRUPTS);
    assert(irq != 0);
    _ext_isrs[irq] = cb;
}

void plic_init(void)
{
    for (unsigned i = 1; i <= PLIC_NUM_INTERRUPTS; i++) {
        plic_disable_interrupt(i);
        plic_set_priority(i, 0);
    }

    plic_set_threshold(0);
}

void plic_isr_handler(void)
{
    unsigned irq = plic_claim_interrupt();

    /* Don't check here, just crash hard if no handler is available */
    _ext_isrs[irq](irq);

    plic_complete_interrupt(irq);
}
