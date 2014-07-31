/******************************************************************************
Copyright 2010, Freie Universität Berlin (FUB).
Copyright 2013, INRIA.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is subject to the terms and conditions of the GNU Lesser
General Public License v2.1. See the file LICENSE in the top level
directory for more details.
*******************************************************************************/

/**
 * @ingroup		cc430
 * @file        cc430-gpioint.c
 * @brief		CC430 GPIO Interrupt Multiplexer implementation
 * @author		Oliver Hahm <oliver.hahm@inria.fr>
 */

#include <stdlib.h>
#include <legacymsp430.h>
#include "gpioint.h"
#include "bitarithm.h"
#include "cpu.h"
#include "irq.h"
#include "hwtimer.h"

/** min and max portnumber to generate interrupts */
#define PORTINT_MIN     (1)
#define PORTINT_MAX     (2)

/** amount of interrupt capable ports */
#define INT_PORTS       (2)

/** number of bits per port */
#define BITMASK_SIZE    (8)

/** debouncing port interrupts */
#define DEBOUNCE_TIMEOUT   (250)

/** interrupt callbacks */
fp_irqcb cb[INT_PORTS][BITMASK_SIZE];

/** debounce interrupt flags */
uint8_t debounce_flags[INT_PORTS];

/** debounce interrupt times */
uint16_t debounce_time[INT_PORTS][BITMASK_SIZE];

uint16_t c1 = 0, c2 = 0;

void gpioint_init(void)
{
    uint8_t i, j;

    for (i = 0; i < INT_PORTS; i++) {
        for (j = 0; j < BITMASK_SIZE; j++) {
            cb[i][j] = NULL;
            debounce_time[i][j] = 0;
        }
    }
}

bool gpioint_set(int port, uint32_t bitmask, int flags, fp_irqcb callback)
{
    int8_t base;

    if ((port >= PORTINT_MIN) && (port <= PORTINT_MAX)) {
        /* set the callback function */
        base = bitarithm_msb(bitmask);

        if (base >= 0) {
            cb[port - PORTINT_MIN][base] = callback;
        }
        else {
            return false;
        }

        if (flags & GPIOINT_DEBOUNCE) {
            debounce_flags[port - PORTINT_MIN] |= bitmask;
        }
        else {
            debounce_flags[port - PORTINT_MIN] &= ~bitmask;
        }
    }

    switch(port) {
        case 1:
            /* set port to input */
            P1DIR &= ~bitmask;
            /* enable internal pull-down */
            P1OUT &= ~bitmask;
            P1REN |= bitmask;

            /* reset IRQ flag */
            P1IFG &= ~bitmask;

            /* trigger on rising... */
            if (flags & GPIOINT_RISING_EDGE) {
                P1IES &= bitmask;
            }

            /* ...or falling edge */
            if (flags & GPIOINT_FALLING_EDGE) {
                P1IES |= bitmask;
            }

            /*  disable interrupt */
            if (flags == GPIOINT_DISABLE) {
                P1IE &= ~bitmask;
            }

            /* enable interrupt */
            P1IE |= bitmask;
            break;

        case 2:
            /* set port to input */
            P2DIR &= ~bitmask;
            /* enable internal pull-down */
            P2OUT &= ~bitmask;
            P2REN |= bitmask;

            /* reset IRQ flag */
            P2IFG &= ~bitmask;

            /* trigger on rising... */
            if (flags == GPIOINT_RISING_EDGE) {
                P2IES &= bitmask;
            }
            /* ...or falling edge */
            else if (flags == GPIOINT_FALLING_EDGE) {
                P2IES |= bitmask;
            }
            /* or disable interrupt */
            else {
                P2IE &= ~bitmask;
            }

            /* enable interrupt */
            P2IE |= bitmask;
            break;

        default:
            return false;
    }

    return 1;
}

interrupt(PORT1_VECTOR) __attribute__((naked)) port1_isr(void)
{
    uint8_t int_enable, ifg_num, p1ifg;
    uint16_t p1iv;
    uint16_t diff;
    __enter_isr();

    /* Debounce
     * Disable PORT1 IRQ
     */
    p1ifg = P1IFG;
    p1iv = P1IV;
    int_enable = P1IE;
    P1IE = 0x00;

    ifg_num = (p1iv >> 1) - 1;

    /* check interrupt source */
    if (debounce_flags[0] & p1ifg) {
        /* check if bouncing */
        diff = hwtimer_now() - debounce_time[0][ifg_num];

        if (diff > DEBOUNCE_TIMEOUT) {
            debounce_time[0][ifg_num] = hwtimer_now();

            if (cb[0][ifg_num] != NULL) {
                cb[0][ifg_num]();
            }
        }
        else {
            /* TODO: check for long duration irq */
            asm volatile(" nop ");
        }
    }
    else {
        if (cb[0][ifg_num] != NULL) {
            cb[0][ifg_num]();
        }
    }

    P1IFG = 0x00;
    P1IE  = int_enable;
    __exit_isr();
}

interrupt(PORT2_VECTOR) __attribute__((naked)) port2_isr(void)
{
    uint8_t int_enable, ifg_num, p2ifg;
    uint16_t p2iv;
    uint16_t diff;
    __enter_isr();

    /* Debounce
     * Disable PORT2 IRQ
     */
    p2ifg = P2IFG;
    p2iv = P2IV;
    int_enable = P2IE;
    P2IE = 0x00;

    ifg_num = (p2iv >> 1) - 1;

    /* check interrupt source */
    if (debounce_flags[1] & p2ifg) {
        /* check if bouncing */
        diff = hwtimer_now() - debounce_time[1][ifg_num];

        if (diff > DEBOUNCE_TIMEOUT) {
            debounce_time[1][ifg_num] = hwtimer_now();
            c1++;

            if (cb[1][ifg_num] != NULL) {
                cb[1][ifg_num]();
            }
        }
        else {
            c2++;
            /* TODO: check for long duration irq */
            asm volatile(" nop ");
        }
    }
    else {
        if (cb[1][ifg_num] != NULL) {
            cb[1][ifg_num]();
        }
    }


    P2IFG = 0x00;
    P2IE  = int_enable;
    __exit_isr();
}
