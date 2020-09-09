/*
 * Copyright (c) 2005, Swedish Institute of Computer Science
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Modified by Kaspar Schleiser
 */

/**
 * @ingroup     cpu
 * @{
 *
 * @file
 * @brief       MSP430 CPU initialization
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include "cpu.h"
#include "irq.h"
#include "periph/init.h"

/*---------------------------------------------------------------------------*/
static void
init_ports(void)
{
    /* Turn everything off, device drivers enable what is needed. */

    /* All configured for digital I/O */
#ifdef P1SEL
    P1SEL = 0;
#endif
#ifdef P2SEL
    P2SEL = 0;
#endif
#ifdef P3SEL
    P3SEL = 0;
#endif
#ifdef P4SEL
    P4SEL = 0;
#endif
#ifdef P5SEL
    P5SEL = 0;
#endif
#ifdef P6SEL
    P6SEL = 0;
#endif

    /* All available inputs */
#ifdef P1DIR
    P1DIR = 0;
    P1OUT = 0;
#endif
#ifdef P2DIR
    P2DIR = 0;
    P2OUT = 0;
#endif
#ifdef P3DIR
    P3DIR = 0;
    P3OUT = 0;
#endif
#ifdef P4DIR
    P4DIR = 0;
    P4OUT = 0;
#endif

#ifdef P5DIR
    P5DIR = 0;
    P5OUT = 0;
#endif

#ifdef P6DIR
    P6DIR = 0;
    P6OUT = 0;
#endif

    P1IE = 0;
    P2IE = 0;
}

void msp430_cpu_init(void)
{
    irq_disable();
    init_ports();
    irq_enable();
}
/*---------------------------------------------------------------------------*/
#define asmv(arg) __asm__ __volatile__(arg)


/*---------------------------------------------------------------------------*/
/*
 * Mask all interrupts that can be masked.
 */
int
splhigh_(void)
{
    /* Clear the GIE (General Interrupt Enable) flag. */
    int sr;
    asmv("mov r2, %0" : "=r"(sr));
    asmv("bic %0, r2" : : "i"(GIE));
    asmv("nop");
    return sr & GIE;        /* Ignore other sr bits. */
}
/*---------------------------------------------------------------------------*/
/*
 * Restore previous interrupt mask.
 */
void
splx_(int sr)
{
    sr &= GIE;
    /* If GIE was set, restore it. */
    asmv("bis %0, r2" : : "r"(sr));
    asmv("nop");
}

extern void board_init(void);
