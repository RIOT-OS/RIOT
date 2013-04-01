/*
 * hwtimer_cpu.c - architecture dependent hardware timer functionality
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This source code is licensed under the GNU General Public License,
 * Version 3.  See the file LICENSE for more details.
 *
 * This file is part of RIOT.
 */

#include <stdint.h>
#include "mc1322x.h"

/* TODO: do scaling voodoo */
#define COUNT_MODE 1      /* use rising edge of primary source */
#define PRIME_SRC  0xf    /* Perip. clock with 128 prescale (for 24Mhz = 187500Hz)*/
#define SEC_SRC    0      /* don't need this */
#define ONCE       0      /* keep counting */
#define LEN        0      /* continue counting */
#define DIR        0      /* count up */
#define CO_INIT    0      /* other counters cannot force a re-initialization of this counter */
#define OUT_MODE   0      /* OFLAG is asserted while counter is active */

void hwtimer_arch_init(void (*handler)(int), uint32_t fcpu) {
    /* Reset the timer */
    TMR_ENBL = 0;
    /* Clear status */
    TMR0_SCTRL = 0;
    /* disable interrupt */
    TMR0_CSCTRL =0x0000;
    /* Reload/initialize to zero */
    TMR0_LOAD = 0;

    /* disable comparison */
    TMR0_COMP_UP = 0;
    TMR0_CMPLD1 = 0;
    
    /* set counter to zero */
    TMR0_CNTR = 0;

    TMR0_CTRL = (COUNT_MODE<<13) | (PRIME_SRC<<9) | (SEC_SRC<<7) | (ONCE<<6) | (LEN<<5) | (DIR<<4) | (CO_INIT<<3) | (OUT_MODE);
    TMR_ENBL = 0xf;                   /* enable all the timers --- why not? */

}
