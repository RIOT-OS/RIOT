/*
 * Copyright (c) 2010, Mariano Alvira <mar@devl.org> and other contributors
 * to the MC1322x project (http://mc1322x.devl.org) and Contiki.
 *
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
 * This file is part of the Contiki OS.
 *
 *
 */

	
/*
The following lincence is for all parts of this code done by 
Martin Thomas. Code from others used here may have other license terms.

Copyright (C) 2004 Martin THOMAS

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

! The above copyright notice and this permission notice shall be included in all
! copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
	
// Stack Sizes
        .set  UND_STACK_SIZE, 0x00000004
        .set  ABT_STACK_SIZE, 0x00000004
        .set  FIQ_STACK_SIZE, 0x00000004
        .set  IRQ_STACK_SIZE, 0X00000080
        .set  SVC_STACK_SIZE, 0x00000004

// Standard definitions of Mode bits and Interrupt (I & F) flags in PSRs
        .set  MODE_USR, 0x10            // User Mode
        .set  MODE_FIQ, 0x11            // FIQ Mode
        .set  MODE_IRQ, 0x12            // IRQ Mode
        .set  MODE_SVC, 0x13            // Supervisor Mode
        .set  MODE_ABT, 0x17            // Abort Mode
        .set  MODE_UND, 0x1B            // Undefined Mode
        .set  MODE_SYS, 0x1F            // System Mode

        .equ  I_BIT, 0x80               // when I bit is set, IRQ is disabled
        .equ  F_BIT, 0x40               // when F bit is set, FIQ is disabled

       .section .start
	
       .set _rom_data_init, 0x108d0
       .global _start
_start:
        b     _begin                    // reset - _start
        ldr   pc,_undf                  // undefined 
        ldr   pc,_swi                   // SWI 
        ldr   pc,_pabt                  // program abort 
        ldr   pc,_dabt                  // data abort 
        nop                             // reserved
        ldr   pc,_irq                   // IRQ 
        ldr   pc,_fiq                   // FIQ 

 /* these vectors are used for rom patching */	
.org 0x20
.code 16
_RPTV_0_START:
 	bx lr /* do nothing */

.org 0x60
_RPTV_1_START:
 	bx lr /* do nothing */

.org 0xa0
_RPTV_2_START:
 	bx lr /* do nothing */

.org 0xe0
_RPTV_3_START:
 	bx lr /* do nothing */

.org 0x120
ROM_var_start: .word 0
.org 0x7ff
ROM_var_end:   .word 0

.code 32
.align			
_begin:	
	/* FIQ mode stack */
	msr	CPSR_c, #(MODE_FIQ | I_BIT | F_BIT)
	ldr	sp, =__fiq_stack_top__	/* set the FIQ stack pointer */

	/* IRQ mode stack */
	msr	CPSR_c, #(MODE_IRQ | I_BIT | F_BIT)
	ldr	sp, =__irq_stack_top__	/* set the IRQ stack pointer */

	/* Supervisor mode stack */
	msr	CPSR_c, #(MODE_SVC | I_BIT | F_BIT)
	ldr	sp, =__svc_stack_top__	/* set the SVC stack pointer */

	/* Undefined mode stack */
	msr	CPSR_c, #(MODE_UND | I_BIT | F_BIT)
	ldr	sp, =__und_stack_top__	/* set the UND stack pointer */

	/* Abort mode stack */
	msr	CPSR_c, #(MODE_ABT | I_BIT | F_BIT)
	ldr	sp, =__abt_stack_top__	/* set the ABT stack pointer */

	/* System mode stack */
	msr	CPSR_c, #(MODE_SYS | I_BIT | F_BIT)
	ldr	sp, =__sys_stack_top__	/* set the SYS stack pointer */

	/* call the rom_data_init function in ROM */
	/* initializes ROM_var space defined by ROM_var_start and ROM_var_end */
	/* this area is used by ROM functions (e.g. nvm_read) */
 	ldr r12,=_rom_data_init
 	mov lr,pc
 	bx  r12

 	msr	CPSR_c, #(MODE_SYS)

        /* Clear BSS */
clear_bss:
        ldr     r0, _bss_start          /* find start of bss segment        */
        ldr     r1, _bss_end            /* stop here                        */
        mov     r2, #0x00000000         /* clear                            */
clbss_l:
        str     r2, [r0]                /* clear loop...                    */
        add     r0, r0, #4
        cmp     r0, r1
        blt     clbss_l

        b main

_undf:  .word __undf                    // undefined
_swi:   .word __swi                     // SWI
_pabt:  .word __pabt                    // program abort
_dabt:  .word __dabt                    // data abort
_irq:   .word irq                       // IRQ
_fiq:   .word __fiq                     // FIQ
	
__undf: b     .                         // undefined
__swi:  b     .                         // SWI
__pabt: b     .                         // program abort
__dabt: b     .                         // data abort
/* IRQ handler set in isr.c */
//__irq:  b     .                         // IRQ
__fiq:  b     .                         // FIQ

/*
 * These are defined in the board-specific linker script.
 */
.globl _bss_start
_bss_start:
        .word __bss_start

	.globl _bss_end
_bss_end:
        .word _end
