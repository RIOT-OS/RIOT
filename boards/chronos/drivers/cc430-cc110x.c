/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup chronos
 * @{
 */

/**
 * @file
 * @brief       eZ430-chronos radio driver (board dependent part)
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 */

#include <stdint.h>
#include <stdint.h>
#include <cc430f6137.h>

#include "cpu.h"
#include "irq.h"
#include "cc110x_legacy.h"

#define CC1100_GDO0     (RF1AIN & BIT0)
#define CC1100_GDO1     (RF1AIN & BIT1)
#define CC1100_GDO2     (RF1AIN & BIT2)

int cc110x_get_gdo0(void)
{
    return  CC1100_GDO0;
}

int cc110x_get_gdo1(void)
{
    return  CC1100_GDO1;
}

int cc110x_get_gdo2(void)
{
    return  CC1100_GDO2;
}

void cc110x_before_send(void)
{
    // Disable GDO2 interrupt before sending packet
    cc110x_gdo2_disable();
}

void cc110x_after_send(void)
{
    // Enable GDO2 interrupt after sending packet
    cc110x_gdo2_enable();
}

void cc110x_gdo0_enable(void)
{
    RF1AIFG &= ~BIT0;
    RF1AIE  |= BIT0;
}

void cc110x_gdo0_disable(void)
{
    RF1AIE  &= ~BIT0;
    RF1AIFG &= ~BIT0;
}

void cc110x_gdo2_disable(void)
{
    RF1AIFG &= ~BIT2;                         // Clear a pending interrupt
    RF1AIE  &= ~BIT2;                          // Disable the interrupt
}

void cc110x_gdo2_enable(void)
{
    RF1AIFG &= ~BIT2;                         // Clear a pending interrupt
    RF1AIE  |= BIT2;                          // Enable the interrupt
}

void cc110x_init_interrupts(void)
{
    uint8_t state = disableIRQ(); /* Disable all interrupts */
    cc110x_gdo2_enable();
    cc110x_gdo0_disable();
    restoreIRQ(state);  /* Enable all interrupts */
}

interrupt(CC1101_VECTOR) __attribute__((naked)) cc110x_isr(void)
{
    __enter_isr();

    /* Check IFG */
    if (RF1AIV == RF1AIV_RFIFG2) {
        while (RF1AIN & BIT2);

        /* discard all further interrupts */
        RF1AIV = 0;
        cc110x_gdo2_irq();
    }

    if (RF1AIV == RF1AIV_RFIFG0) {
        cc110x_gdo0_irq();
        RF1AIE &= ~BIT0;
    }

    __exit_isr();
}
