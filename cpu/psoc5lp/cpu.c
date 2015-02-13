/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_psoc5lp
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Murat Cakmak <mail@muratcakmak.net>
 * @}
 */

#include "cpu.h"
#include "CyLib.h"

/**
 * @brief Default handler is called in case no interrupt handler was defined
 */
void dummy_handler(void)
{
    while (1) {asm ("nop");}
}

void isr_svc(void)            __attribute__ ((weak, alias("dummy_handler")));
void isr_pendsv(void)         __attribute__ ((weak, alias("dummy_handler")));
void isr_systick(void)        __attribute__ ((weak, alias("dummy_handler")));

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* 
     * IMP: Following Cortex-M Specific interrupts are being managed by RIOT but 
     * PSoC Creator sets interrupt vectors with a dummy handlers internally and
     * provides APIs to associate interupts and handler methods. 
     */
    CyIntSetSysVector(CY_INT_SVCALL_IRQN, isr_svc);
    CyIntSetSysVector(CY_INT_PEND_SV_IRQN, isr_pendsv);
    CyIntSetSysVector(CY_INT_SYSTICK_IRQN, isr_systick);
    
    
    /* set pendSV interrupt to lowest possible priority */
    NVIC_SetPriority(PendSV_IRQn, 0xff);
}
