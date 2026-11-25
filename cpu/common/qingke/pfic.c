/*
 * SPDX-FileCopyrightText: 2026 Klim Evdokimov <klimevdokimov@mail.ru>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup         cpu_riscv_common
 * @{
 *
 * @file
 * @brief           WCH QingKe PFIC interrupt controller implementation
 *
 * @author          Klim Evdokimov <klimevdokimov@mail.ru>
 * @}
 */

#include <stdint.h>
#include <assert.h>

#include "panic.h"
#include "pfic.h"
#include "vendor/riscv_csr.h"

#define PFIC_IRQ_REG_INDEX(irq)  (((uint32_t)(irq)) >> 5)
#define PFIC_IRQ_BIT_MASK(irq)   (1UL << (((uint32_t)(irq)) & 0x1F))

/* PFIC ISR function list */
static pfic_isr_cb_t _ext_isrs[PFIC_NUM_INTERRUPTS];

void pfic_init(void)
{
    /* QingKe INTSYSCR (0x804): bit 0 enables the hardware prologue/epilogue
     * (HPE), bit 1 enables interrupt nesting. Both must be off, as the common
     * trap entry saves the context itself and does not support nested
     * interrupts. */
    write_csr(0x804, 0);

    for (unsigned i = 0; i < 8; i++) {
        PFIC->IRER[i] = UINT32_MAX;
        PFIC->IPRR[i] = UINT32_MAX;
    }
    /* Do not mask any interrupt by priority */
    PFIC->ITHRESDR = 0;
}

void pfic_enable_interrupt(unsigned irq, unsigned priority)
{
    assert(irq < PFIC_NUM_INTERRUPTS);
    pfic_set_priority(irq, priority);
    PFIC->IENR[PFIC_IRQ_REG_INDEX(irq)] = PFIC_IRQ_BIT_MASK(irq);
}

void pfic_disable_interrupt(unsigned irq)
{
    assert(irq < PFIC_NUM_INTERRUPTS);
    PFIC->IRER[PFIC_IRQ_REG_INDEX(irq)] = PFIC_IRQ_BIT_MASK(irq);
}

void pfic_set_priority(unsigned irq, unsigned priority)
{
    assert(irq < PFIC_NUM_INTERRUPTS);
    PFIC->IPRIOR[irq] = priority;
}

void pfic_set_pending(unsigned irq)
{
    assert(irq < PFIC_NUM_INTERRUPTS);
    PFIC->IPSR[PFIC_IRQ_REG_INDEX(irq)] = PFIC_IRQ_BIT_MASK(irq);
}

void pfic_clear_pending(unsigned irq)
{
    assert(irq < PFIC_NUM_INTERRUPTS);
    PFIC->IPRR[PFIC_IRQ_REG_INDEX(irq)] = PFIC_IRQ_BIT_MASK(irq);
}

void pfic_set_handler(unsigned irq, pfic_isr_cb_t cb)
{
    assert(irq < PFIC_NUM_INTERRUPTS);
    _ext_isrs[irq] = cb;
}

void pfic_isr_handler(uint32_t irq)
{
    if ((irq >= PFIC_NUM_INTERRUPTS) || !_ext_isrs[irq]) {
        core_panic(PANIC_GENERAL_ERROR, "Unhandled PFIC interrupt");
    }
    _ext_isrs[irq](irq);
}
