/*
 * Copyright(C) 2016,2017, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */
#include <assert.h>

#include "board.h"
#include "eic_irq.h"

void eic_irq_configure(int irq_num)
{
    (void)irq_num;
    /* Only timer interrupt supported currently */
    assert(irq_num == EIC_IRQ_TIMER);

    /* Enable IRQ0 CPU Timer Interrupt */
    IEC0SET = _IEC0_CTIE_MASK;

    /* Set IRQ 0 to priority 1.0 */
    IPC0SET = 1 << _IPC0_CTIP_POSITION | 0 << _IPC0_CTIS_POSITION;
}

void eic_irq_enable(int irq_num)
{
    (void)irq_num;
    /* Only timer interrupt supported currently */
    assert(irq_num == EIC_IRQ_TIMER);

    /* Enable IRQ0 CPU Timer Interrupt */
    IEC0SET = _IEC0_CTIE_MASK;
}

void eic_irq_disable(int irq_num)
{
    (void)irq_num;
    /* Only timer interrupt supported currently */
    assert(irq_num == EIC_IRQ_TIMER);

    /* Disable IRQ0 CPU Timer Interrupt */
    IEC0CLR = _IEC0_CTIE_MASK;
}

void eic_irq_ack(int irq_num)
{
    (void)irq_num;
    /* Only timer interrupt supported currently */
    assert(irq_num == EIC_IRQ_TIMER);

    /* Ack the timer interrupt */
    IFS0CLR =_IFS0_CTIF_MASK;
}
