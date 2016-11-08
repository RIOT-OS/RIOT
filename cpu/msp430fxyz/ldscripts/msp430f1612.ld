/* ============================================================================ */
/* Copyright (c) 2015, Texas Instruments Incorporated                           */
/*  All rights reserved.                                                        */
/*                                                                              */
/*  Redistribution and use in source and binary forms, with or without          */
/*  modification, are permitted provided that the following conditions          */
/*  are met:                                                                    */
/*                                                                              */
/*  *  Redistributions of source code must retain the above copyright           */
/*     notice, this list of conditions and the following disclaimer.            */
/*                                                                              */
/*  *  Redistributions in binary form must reproduce the above copyright        */
/*     notice, this list of conditions and the following disclaimer in the      */
/*     documentation and/or other materials provided with the distribution.     */
/*                                                                              */
/*  *  Neither the name of Texas Instruments Incorporated nor the names of      */
/*     its contributors may be used to endorse or promote products derived      */
/*     from this software without specific prior written permission.            */
/*                                                                              */
/*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" */
/*  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,       */
/*  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR      */
/*  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR            */
/*  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,       */
/*  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,         */
/*  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; */
/*  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,    */
/*  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR     */
/*  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,              */
/*  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                          */
/* ============================================================================ */

/* This file supports MSP430F2617 devices. */
/* Version: 1.173 */
/* Default linker script, for normal executables */

OUTPUT_FORMAT("elf32-msp430", "elf32-msp430", "elf32-msp430")
OUTPUT_ARCH(msp430)
ENTRY(_start)
SEARCH_DIR(.)

MEMORY {
  SFR              : ORIGIN = 0x0000, LENGTH = 0x0010 /* END=0x0010, size 16 */
  PERIPHERAL_8BIT  : ORIGIN = 0x0010, LENGTH = 0x00F0 /* END=0x0100, size 240 */
  PERIPHERAL_16BIT : ORIGIN = 0x0100, LENGTH = 0x0100 /* END=0x0200, size 256 */
  RAM              : ORIGIN = 0x1100, LENGTH = 0x1400 /* END=0x24FF, size 5120 */
  INFOMEM          : ORIGIN = 0x1000, LENGTH = 0x0100 /* END=0x10FF, size 256 as 2 128-byte segments */
  INFOA            : ORIGIN = 0x1080, LENGTH = 0x0080 /* END=0x10FF, size 128 */
  INFOB            : ORIGIN = 0x1000, LENGTH = 0x0080 /* END=0x107F, size 128 */
  ROM (rx)         : ORIGIN = 0x2500, LENGTH = 0xDAE0 /* END=0xFFDF, size 56032 */
  VECT1            : ORIGIN = 0xFFE0, LENGTH = 0x0002
  VECT2            : ORIGIN = 0xFFE2, LENGTH = 0x0002
  VECT3            : ORIGIN = 0xFFE4, LENGTH = 0x0002
  VECT4            : ORIGIN = 0xFFE6, LENGTH = 0x0002
  VECT5            : ORIGIN = 0xFFE8, LENGTH = 0x0002
  VECT6            : ORIGIN = 0xFFEA, LENGTH = 0x0002
  VECT7            : ORIGIN = 0xFFEC, LENGTH = 0x0002
  VECT8            : ORIGIN = 0xFFEE, LENGTH = 0x0002
  VECT9            : ORIGIN = 0xFFF0, LENGTH = 0x0002
  VECT10           : ORIGIN = 0xFFF2, LENGTH = 0x0002
  VECT11           : ORIGIN = 0xFFF4, LENGTH = 0x0002
  VECT12           : ORIGIN = 0xFFF6, LENGTH = 0x0002
  VECT13           : ORIGIN = 0xFFF8, LENGTH = 0x0002
  VECT14           : ORIGIN = 0xFFFA, LENGTH = 0x0002
  VECT15           : ORIGIN = 0xFFFC, LENGTH = 0x0002
  RESETVEC         : ORIGIN = 0xFFFE, LENGTH = 0x0002
  RAM_MIRROR       : ORIGIN = 0x0200, LENGTH = 0x0800
}

SECTIONS
{
  __interrupt_vector_1   : { KEEP (*(__interrupt_vector_1 )) KEEP (*(__interrupt_vector_dacdma)) } > VECT1
  __interrupt_vector_2   : { KEEP (*(__interrupt_vector_2 )) KEEP (*(__interrupt_vector_port2)) } > VECT2
  __interrupt_vector_3   : { KEEP (*(__interrupt_vector_3 )) KEEP (*(__interrupt_vector_usart1tx)) } > VECT3
  __interrupt_vector_4   : { KEEP (*(__interrupt_vector_4 )) KEEP (*(__interrupt_vector_usart1rx)) } > VECT4
  __interrupt_vector_5   : { KEEP (*(__interrupt_vector_5 )) KEEP (*(__interrupt_vector_port1)) } > VECT5
  __interrupt_vector_6   : { KEEP (*(__interrupt_vector_6 )) KEEP (*(__interrupt_vector_timera1)) } > VECT6
  __interrupt_vector_7   : { KEEP (*(__interrupt_vector_7 )) KEEP (*(__interrupt_vector_timera0)) } > VECT7
  __interrupt_vector_8   : { KEEP (*(__interrupt_vector_8 )) KEEP (*(__interrupt_vector_adc12)) } > VECT8
  __interrupt_vector_9   : { KEEP (*(__interrupt_vector_9 )) KEEP (*(__interrupt_vector_usart0tx)) } > VECT9
  __interrupt_vector_10  : { KEEP (*(__interrupt_vector_10)) KEEP (*(__interrupt_vector_usart0rx)) } > VECT10
  __interrupt_vector_11  : { KEEP (*(__interrupt_vector_11)) KEEP (*(__interrupt_vector_wdt)) } > VECT11
  __interrupt_vector_12  : { KEEP (*(__interrupt_vector_12)) KEEP (*(__interrupt_vector_comparatora)) } > VECT12
  __interrupt_vector_13  : { KEEP (*(__interrupt_vector_13)) KEEP (*(__interrupt_vector_timerb1)) } > VECT13
  __interrupt_vector_14  : { KEEP (*(__interrupt_vector_14)) KEEP (*(__interrupt_vector_timerb0)) } > VECT14
  __interrupt_vector_15  : { KEEP (*(__interrupt_vector_15)) KEEP (*(__interrupt_vector_nmi)) } > VECT15
  __reset_vector :
  {
    KEEP (*(__interrupt_vector_16))
    KEEP (*(__interrupt_vector_reset))
    KEEP (*(.resetvec))
  } > RESETVEC
}

INCLUDE msp430_base.ld
