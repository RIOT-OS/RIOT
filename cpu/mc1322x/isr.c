/*
 * isr.c - mc1322x specific isr
 * Copyright (C) 2013 Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "mc1322x.h"

#define MAX_IRQ_INDEX 10

static void (*isr_funcs[11])(void) = {
    asm_isr,
    uart1_isr,
    uart2_isr,
    crm_isr,
    i2c_isr,
    tmr_isr,
    spif_isr,
    maca_isr,
    ssi_isr,
    adc_isr,
    spi_isr
};

void register_isr(uint8_t interrupt, void (*isr)(void)) {
    if (interrupt <= MAX_IRQ_INDEX) {
        isr_funcs[interrupt] = isr;
    }
}

void isr(void)
{
    /* pending interrupt? */
    while (ITC->NIPEND) {
        /* get interrupt source, range 0-10 */
        /* call corresponding ISR */
        if (isr_funcs[ITC->NIVECTOR]) {
            (isr_funcs[ITC->NIVECTOR])();
        }
    }
}
