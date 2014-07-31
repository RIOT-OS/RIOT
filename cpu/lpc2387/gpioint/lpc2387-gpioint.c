/******************************************************************************
Copyright 2009, Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is subject to the terms and conditions of the GNU Lesser
General Public License v2.1. See the file LICENSE in the top level
directory for more details.
*******************************************************************************/

/**
 * @ingroup     lpc2387
 * @{
 */

/**
 * @file
 * @brief       LPC2387 GPIO Interrupt Multiplexer implementation
 *
 * @author      Michael Baar <michael.baar@fu-berlin.de>
 * @version     $Revision: 1508 $
 *
 * @note        $Id: lpc2387-gpioint.c 1508 2009-10-26 15:10:02Z baar $
 * @see         dev_gpioint
 */

#include <stdio.h>
#include "lpc2387.h"
#include "gpioint.h"
#include "cpu.h"
#include "irq.h"

struct irq_callback_t {
    fp_irqcb    callback;
};

static struct irq_callback_t gpioint0[32];
static struct irq_callback_t gpioint2[32];


void gpioint_init(void)
{
    extern void GPIO_IRQHandler(void);

    /* GPIO Init */
    INTWAKE |= GPIO0WAKE | GPIO2WAKE;                       /* allow GPIO to wake up from power down */
    install_irq(GPIO_INT, &GPIO_IRQHandler, IRQP_GPIO);     /* install irq handler */
}

/*---------------------------------------------------------------------------*/
bool
gpioint_set(int port, uint32_t bitmask, int flags, fp_irqcb callback)
{
    struct irq_callback_t   *cbdata;
    unsigned long bit;
    volatile unsigned long *en_f;
    volatile unsigned long *en_r;
    volatile unsigned long *en_clr;

    /* lookup registers */
    bit = bitarithm_msb(bitmask);         /* get irq mapping table index */

    switch(port) {
        case 0:                                                 /* PORT0 */
            cbdata = gpioint0;
            en_f = &IO0_INT_EN_F;
            en_r = &IO0_INT_EN_R;
            en_clr = &IO0_INT_CLR;
            break;

        case 2:                                                 /* PORT2 */
            cbdata = gpioint2;
            en_f = &IO2_INT_EN_F;
            en_r = &IO2_INT_EN_R;
            en_clr = &IO2_INT_CLR;
            break;

        default:                                                /* unsupported */
            return false;                                       /* fail */
    }

    /* reconfigure irq */
    unsigned long cpsr = disableIRQ();
    *en_clr |= bitmask;                                         /* clear interrupt */

    if ((flags & GPIOINT_FALLING_EDGE) != 0) {
        *en_f |= bitmask;                                       /* enable falling edge */
    }
    else {
        *en_f &= ~bitmask;                                      /* disable falling edge */
    }

    if ((flags & GPIOINT_RISING_EDGE) != 0) {
        *en_r |= bitmask;                                       /* enable rising edge */
    }
    else {
        *en_r &= ~bitmask;                                      /* disable rising edge */
    }

    if (((flags & (GPIOINT_FALLING_EDGE | GPIOINT_RISING_EDGE)) != 0)) {
        cbdata[bit].callback = callback;

    }
    else {
        cbdata[bit].callback = NULL;                            /* remove from interrupt mapping table */
    }

    restoreIRQ(cpsr);

    return true;                                                /* success */
}
/*---------------------------------------------------------------------------*/
static void __attribute__((__no_instrument_function__)) test_irq(int port, unsigned long f_mask, unsigned long r_mask, struct irq_callback_t *pcb)
{
    (void) port;

    /* Test each bit of rising and falling masks, if set trigger interrupt
     * on corresponding device */
    do {
        if ((pcb->callback != NULL)) {
            if ((r_mask & 1) | (f_mask & 1)) {
                pcb->callback();            /* pass to handler */
            }
        }

        f_mask >>= 1UL;
        r_mask >>= 1UL;
        pcb++;
    }
    while ((f_mask != 0) || (r_mask != 0));
}
/*---------------------------------------------------------------------------*/
void GPIO_IRQHandler(void) __attribute__((interrupt("IRQ")));
/**
 * @brief   GPIO Interrupt handler function
 * @internal
 *
 * Invoked whenever an activated gpio interrupt is triggered by a rising
 * or falling edge.
 */
void __attribute__((__no_instrument_function__)) GPIO_IRQHandler(void)
{
    if (IO_INT_STAT & BIT0) {                                       /* interrupt(s) on PORT0 pending */
        unsigned long int_stat_f = IO0_INT_STAT_F;                  /* save content */
        unsigned long int_stat_r = IO0_INT_STAT_R;                  /* save content */

        IO0_INT_CLR = int_stat_f;                                   /* clear flags of fallen pins */
        IO0_INT_CLR = int_stat_r;                                   /* clear flags of risen pins */

        test_irq(0, int_stat_f, int_stat_r, gpioint0);
    }

    if (IO_INT_STAT & BIT2) {                                       /* interrupt(s) on PORT2 pending */
        unsigned long int_stat_f = IO2_INT_STAT_F;                  /* save content */
        unsigned long int_stat_r = IO2_INT_STAT_R;                  /* save content */

        IO2_INT_CLR = int_stat_f;                                   /* clear flags of fallen pins */
        IO2_INT_CLR = int_stat_r;                                   /* clear flags of risen pins */

        test_irq(2, int_stat_f, int_stat_r, gpioint2);
    }

    VICVectAddr = 0;                                                /* Acknowledge Interrupt */
}
/*---------------------------------------------------------------------------*/
/** @} */
