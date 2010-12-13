/******************************************************************************
Copyright 2010, Freie Universität Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is part of µkleos.

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

FeuerWare is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see http://www.gnu.org/licenses/ .
--------------------------------------------------------------------------------
For further information and questions please use the web site
	http://scatterweb.mi.fu-berlin.de
and the mailinglist (subscription via web site)
	scatterweb@lists.spline.inf.fu-berlin.de
*******************************************************************************/

#include <stdlib.h>
#include <signal.h>
#include <gpioint.h>
#include <cpu.h>
#include <irq.h>
#include <hwtimer.h>
#include <cc430x613x.h>

/** min and max portnumber to generate interrupts */
#define PORTINT_MIN     (1)
#define PORTINT_MAX     (2)

/** amount of interrupt capable ports */
#define INT_PORTS       (2)

/** number of bits per port */
#define BITMASK_SIZE    (8)

/** debouncing port interrupts */
#define DEBOUNCE_TIMEOUT   (50)

/** interrupt callbacks */
fp_irqcb cb[INT_PORTS][BITMASK_SIZE];

static int8_t calc_log2(uint8_t power);

void gpioint_init(void) {
    uint8_t i, j;
    for (i = 0; i < INT_PORTS; i++) {
        for (j = 0; j < BITMASK_SIZE; j++) {
            cb[i][j] = NULL;
        }
    }
}

bool gpioint_set(int port, uint32_t bitmask, int flags, fp_irqcb callback) {
    int8_t base;

    if ((port >= PORTINT_MIN) && (port <= PORTINT_MAX)) {
       /* set the callback function */
        base = calc_log2(bitmask);
        if (base >= 0) {
            cb[port - PORTINT_MIN][calc_log2(bitmask)] = callback;
        }
        else {
            return false;
        }
     }

     switch (port) {
        case 1:
            /* set port to input */
            P1DIR &= ~bitmask;
            /* enable internal pull-down */
            P1OUT &= ~bitmask;
            P1REN |= bitmask;
            
            /* reset IRQ flag */
            P1IFG &= ~bitmask;

            /* trigger on rising... */
            if (flags == GPIOINT_RISING_EDGE) {
                P1IES &= bitmask;
            }
            /* ...or falling edge */
            else if (flags == GPIOINT_FALLING_EDGE) {
                P1IES |= bitmask;
            }
            /* or disable interrupt */
            else {
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

static int8_t calc_log2(uint8_t power) {
    int8_t i;
    for (i = 7; i >= 0; i--) {
        if ((power >> i) & 1) {
            return i;
        }
    }
    return -1;
}

interrupt (PORT1_VECTOR) __attribute__ ((naked)) port1_isr(void) {
    uint8_t int_enable, istate;
    uint16_t p1iv;
    __enter_isr();

    /* Debounce
     * Disable PORT1 IRQ 
     */
    p1iv = P1IV;
    istate = disableIRQ();
    int_enable = P1IE;
    restoreIRQ(istate);
    P1IE = 0x00; 
    hwtimer_wait(DEBOUNCE_TIMEOUT);

    switch (p1iv) {
        case P1IV_P1IFG0:
            if ((P1IN & P1IV_P1IFG0) & P1IV_P1IFG0) {
                cb[0][0]();
            }
            break;
        case P1IV_P1IFG1:
            if ((P1IN & P1IV_P1IFG1) & P1IV_P1IFG1) {
                cb[0][1]();
            }
            break;
        case P1IV_P1IFG2:
            if ((P1IN & P1IV_P1IFG2) & P1IV_P1IFG2) {
                cb[0][2]();
            }
            break;
        case P1IV_P1IFG3:
            if ((P1IN & P1IV_P1IFG3) & P1IV_P1IFG3) {
                cb[0][3]();
            }
            break;
        case P1IV_P1IFG4:
            if ((P1IN & P1IV_P1IFG4) & P1IV_P1IFG4) {
                cb[0][4]();
            }
            break;
        case P1IV_P1IFG5:
            if ((P1IN & P1IV_P1IFG5) & P1IV_P1IFG5) {
                cb[0][5]();
            }
            break;
        case P1IV_P1IFG6:
            if ((P1IN & P1IV_P1IFG6) & P1IV_P1IFG6) {
                cb[0][6]();
            }
            break;
        case P1IV_P1IFG7:
            if ((P1IN & P1IV_P1IFG7) & P1IV_P1IFG7) {
                cb[0][7]();
            }
            break;
    }
	P1IFG = 0x00; 	
    istate = disableIRQ();
    P1IE  = int_enable; 	
    restoreIRQ(istate);
    __exit_isr();
}

interrupt (PORT2_VECTOR) __attribute__ ((naked)) port2_isr(void) {
    uint8_t int_enable, istate;
    uint16_t p2iv;
    __enter_isr();

    /* Debounce
     * Disable PORT2 IRQ 
     */
    p2iv = P2IV;
    istate = disableIRQ();
    int_enable = P2IE;
    restoreIRQ(istate);
    P2IE = 0x00; 
    hwtimer_wait(DEBOUNCE_TIMEOUT);

    switch (p2iv) {
        case P2IV_P2IFG0:
            if ((P2IN & P2IV_P2IFG0) & P2IV_P2IFG0) {
                cb[1][0]();
            }
            break;
        case P2IV_P2IFG1:
            if ((P2IN & P2IV_P2IFG1) & P2IV_P2IFG1) {
                cb[1][1]();
            }
            break;
        case P2IV_P2IFG2:
            if ((P2IN & P2IV_P2IFG2) & P2IV_P2IFG2) {
                cb[1][2]();
            }
            break;
        case P2IV_P2IFG3:
            if ((P2IN & P2IV_P2IFG3) & P2IV_P2IFG3) {
                cb[1][3]();
            }
            break;
        case P2IV_P2IFG4:
            if ((P2IN & P2IV_P2IFG4) & P2IV_P2IFG4) {
                cb[1][4]();
            }
            break;
        case P2IV_P2IFG5:
            if ((P2IN & P2IV_P2IFG5) & P2IV_P2IFG5) {
                cb[1][5]();
            }
            break;
        case P2IV_P2IFG6:
            if ((P2IN & P2IV_P2IFG6) & P2IV_P2IFG6) {
                cb[1][6]();
            }
            break;
        case P2IV_P2IFG7:
            if ((P2IN & P2IV_P2IFG7) & P2IV_P2IFG7) {
                cb[1][7]();
            }
            break;
    }
	P2IFG = 0x00; 	
    istate = disableIRQ();
    P2IE  = int_enable; 	
    restoreIRQ(istate);
    __exit_isr();
}

