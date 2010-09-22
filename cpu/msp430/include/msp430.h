/*
 * Copyright (c) 2007, Swedish Institute of Computer Science.
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
 * This file is part of the Contiki operating system.
 *
 * $Id$
 */

#ifndef __MSP430_H__
#define __MSP430_H__

/**
 * @ingroup		msp430
 * @{
 */

/**
 * \file
 *         MSP430 definitions
 * \author
 *         Adam Dunkels <adam@sics.se>
 *         Modified by Kaspar Schleiser
 */

#include <stdint.h>
#include <stdbool.h>

#include "board.h"

#ifndef MSP430_INITIAL_CPU_SPEED
#define MSP430_INITIAL_CPU_SPEED 2457600uL
#warning MSP430_INITIAL_CPU_SPEED undefined. MSP430 initial dco speed set to 2.457600 MHz.
#endif

#ifndef MSP430_HAS_DCOR
#define MSP430_HAS_DCOR 0
#endif

#ifndef MSP430_HAS_EXTERNAL_CRYSTAL
#define MSP430_HAS_EXTERNAL_CRYSTAL 0
#endif

#define HAVE_STDINT_H

#define CC_CONF_REGISTER_ARGS          1
#define CC_CONF_FUNCTION_POINTER_ARGS  1
#define CC_CONF_INLINE inline
#define CC_CONF_VA_ARGS                1
//#define CC_CONF_USED					// is ignored my mspgcc
#define CC_CONF_NONNULL(...)			__attribute__((nonnull(__VA_ARGS__)))
#define CC_CONF_WARN_UNUSED_RESULT		__attribute__((warn_unused_result))

#if (MSP430_INITIAL_CPU_SPEED > 5000000) && (MSP430_HAS_DCOR != 1) || (MSP430_INITIAL_CPU_SPEED > 10000000) && (MSP430_HAS_DCOR == 1)
#warning MSP430_INITIAL_CPU_SPEED too high
#endif

extern volatile uint32_t cpu_speed;
#define RTIMER_ARCH_SECOND		(32767 / 4)		///< frequency of the rtimer
#define CLOCK_CONF_SECOND		(32767 / 8)		///< frequency of the software timers

void msp430_set_cpu_speed(uint32_t speed);

void msp430_cpu_init(void);	/* Rename to cpu_init() later! */
#define cpu_init() msp430_cpu_init()

void msp430_init_dco();

void   *sbrk(int);

typedef int spl_t;
void    splx_(spl_t);
spl_t   splhigh_(void);

#define splhigh() splhigh_()
#define splx(sr) __asm__ __volatile__("bis %0, r2" : : "r" (sr))

void msp430_set_cpu_speed(uint32_t speed);
void msp430_cpu_init(void);

/** @} */
#endif /* __MSP430_H__ */
