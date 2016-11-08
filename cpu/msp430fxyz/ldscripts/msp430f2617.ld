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
  RAM              : ORIGIN = 0x1100, LENGTH = 0x2000 /* END=0x30FF, size 8192 */
  INFOMEM          : ORIGIN = 0x1000, LENGTH = 0x0100 /* END=0x10FF, size 256 as 4 64-byte segments */
  INFOA            : ORIGIN = 0x10C0, LENGTH = 0x0040 /* END=0x10FF, size 64 */
  INFOB            : ORIGIN = 0x1080, LENGTH = 0x0040 /* END=0x10BF, size 64 */
  INFOC            : ORIGIN = 0x1040, LENGTH = 0x0040 /* END=0x107F, size 64 */
  INFOD            : ORIGIN = 0x1000, LENGTH = 0x0040 /* END=0x103F, size 64 */
  ROM (rx)         : ORIGIN = 0x3100, LENGTH = 0xCEBE /* END=0xFFBD, size 52926 */
  VECT1            : ORIGIN = 0xFFC0, LENGTH = 0x0002
  VECT2            : ORIGIN = 0xFFC2, LENGTH = 0x0002
  VECT3            : ORIGIN = 0xFFC4, LENGTH = 0x0002
  VECT4            : ORIGIN = 0xFFC6, LENGTH = 0x0002
  VECT5            : ORIGIN = 0xFFC8, LENGTH = 0x0002
  VECT6            : ORIGIN = 0xFFCA, LENGTH = 0x0002
  VECT7            : ORIGIN = 0xFFCC, LENGTH = 0x0002
  VECT8            : ORIGIN = 0xFFCE, LENGTH = 0x0002
  VECT9            : ORIGIN = 0xFFD0, LENGTH = 0x0002
  VECT10           : ORIGIN = 0xFFD2, LENGTH = 0x0002
  VECT11           : ORIGIN = 0xFFD4, LENGTH = 0x0002
  VECT12           : ORIGIN = 0xFFD6, LENGTH = 0x0002
  VECT13           : ORIGIN = 0xFFD8, LENGTH = 0x0002
  VECT14           : ORIGIN = 0xFFDA, LENGTH = 0x0002
  VECT15           : ORIGIN = 0xFFDC, LENGTH = 0x0002
  VECT16           : ORIGIN = 0xFFDE, LENGTH = 0x0002
  VECT17           : ORIGIN = 0xFFE0, LENGTH = 0x0002
  VECT18           : ORIGIN = 0xFFE2, LENGTH = 0x0002
  VECT19           : ORIGIN = 0xFFE4, LENGTH = 0x0002
  VECT20           : ORIGIN = 0xFFE6, LENGTH = 0x0002
  VECT21           : ORIGIN = 0xFFE8, LENGTH = 0x0002
  VECT22           : ORIGIN = 0xFFEA, LENGTH = 0x0002
  VECT23           : ORIGIN = 0xFFEC, LENGTH = 0x0002
  VECT24           : ORIGIN = 0xFFEE, LENGTH = 0x0002
  VECT25           : ORIGIN = 0xFFF0, LENGTH = 0x0002
  VECT26           : ORIGIN = 0xFFF2, LENGTH = 0x0002
  VECT27           : ORIGIN = 0xFFF4, LENGTH = 0x0002
  VECT28           : ORIGIN = 0xFFF6, LENGTH = 0x0002
  VECT29           : ORIGIN = 0xFFF8, LENGTH = 0x0002
  VECT30           : ORIGIN = 0xFFFA, LENGTH = 0x0002
  VECT31           : ORIGIN = 0xFFFC, LENGTH = 0x0002
  RESETVEC         : ORIGIN = 0xFFFE, LENGTH = 0x0002
  RAM_MIRROR       : ORIGIN = 0x0200, LENGTH = 0x0800
  HIROM (rx)       : ORIGIN = 0x00010000, LENGTH = 0x00009FFF
}

SECTIONS
{
  __interrupt_vector_1   : { KEEP (*(__interrupt_vector_1 )) KEEP (*(__interrupt_vector_reserved0)) } > VECT1
  __interrupt_vector_2   : { KEEP (*(__interrupt_vector_2 )) KEEP (*(__interrupt_vector_reserved1)) } > VECT2
  __interrupt_vector_3   : { KEEP (*(__interrupt_vector_3 )) KEEP (*(__interrupt_vector_reserved2)) } > VECT3
  __interrupt_vector_4   : { KEEP (*(__interrupt_vector_4 )) KEEP (*(__interrupt_vector_reserved3)) } > VECT4
  __interrupt_vector_5   : { KEEP (*(__interrupt_vector_5 )) KEEP (*(__interrupt_vector_reserved4)) } > VECT5
  __interrupt_vector_6   : { KEEP (*(__interrupt_vector_6 )) KEEP (*(__interrupt_vector_reserved5)) } > VECT6
  __interrupt_vector_7   : { KEEP (*(__interrupt_vector_7 )) KEEP (*(__interrupt_vector_reserved6)) } > VECT7
  __interrupt_vector_8   : { KEEP (*(__interrupt_vector_8 )) KEEP (*(__interrupt_vector_reserved7)) } > VECT8
  __interrupt_vector_9   : { KEEP (*(__interrupt_vector_9 )) KEEP (*(__interrupt_vector_reserved8)) } > VECT9
  __interrupt_vector_10  : { KEEP (*(__interrupt_vector_10)) KEEP (*(__interrupt_vector_reserved9)) } > VECT10
  __interrupt_vector_11  : { KEEP (*(__interrupt_vector_11)) KEEP (*(__interrupt_vector_reserved10)) } > VECT11
  __interrupt_vector_12  : { KEEP (*(__interrupt_vector_12)) KEEP (*(__interrupt_vector_reserved11)) } > VECT12
  __interrupt_vector_13  : { KEEP (*(__interrupt_vector_13)) KEEP (*(__interrupt_vector_reserved12)) } > VECT13
  __interrupt_vector_14  : { KEEP (*(__interrupt_vector_14)) KEEP (*(__interrupt_vector_reserved13)) } > VECT14
  __interrupt_vector_15  : { KEEP (*(__interrupt_vector_15)) KEEP (*(__interrupt_vector_dac12)) } > VECT15
  __interrupt_vector_16  : { KEEP (*(__interrupt_vector_16)) KEEP (*(__interrupt_vector_dma)) } > VECT16
  __interrupt_vector_17  : { KEEP (*(__interrupt_vector_17)) KEEP (*(__interrupt_vector_usciab1tx)) } > VECT17
  __interrupt_vector_18  : { KEEP (*(__interrupt_vector_18)) KEEP (*(__interrupt_vector_usciab1rx)) } > VECT18
  __interrupt_vector_19  : { KEEP (*(__interrupt_vector_19)) KEEP (*(__interrupt_vector_port1)) } > VECT19
  __interrupt_vector_20  : { KEEP (*(__interrupt_vector_20)) KEEP (*(__interrupt_vector_port2)) } > VECT20
  __interrupt_vector_21  : { KEEP (*(__interrupt_vector_21)) KEEP (*(__interrupt_vector_reserved20)) } > VECT21
  __interrupt_vector_22  : { KEEP (*(__interrupt_vector_22)) KEEP (*(__interrupt_vector_adc12)) } > VECT22
  __interrupt_vector_23  : { KEEP (*(__interrupt_vector_23)) KEEP (*(__interrupt_vector_usciab0tx)) } > VECT23
  __interrupt_vector_24  : { KEEP (*(__interrupt_vector_24)) KEEP (*(__interrupt_vector_usciab0rx)) } > VECT24
  __interrupt_vector_25  : { KEEP (*(__interrupt_vector_25)) KEEP (*(__interrupt_vector_timera1)) } > VECT25
  __interrupt_vector_26  : { KEEP (*(__interrupt_vector_26)) KEEP (*(__interrupt_vector_timera0)) } > VECT26
  __interrupt_vector_27  : { KEEP (*(__interrupt_vector_27)) KEEP (*(__interrupt_vector_wdt)) } > VECT27
  __interrupt_vector_28  : { KEEP (*(__interrupt_vector_28)) KEEP (*(__interrupt_vector_comparatora)) } > VECT28
  __interrupt_vector_29  : { KEEP (*(__interrupt_vector_29)) KEEP (*(__interrupt_vector_timerb1)) } > VECT29
  __interrupt_vector_30  : { KEEP (*(__interrupt_vector_30)) KEEP (*(__interrupt_vector_timerb0)) } > VECT30
  __interrupt_vector_31  : { KEEP (*(__interrupt_vector_31)) KEEP (*(__interrupt_vector_nmi)) } > VECT31
  __reset_vector :
  {
    KEEP (*(__interrupt_vector_32))
    KEEP (*(__interrupt_vector_reset))
    KEEP (*(.resetvec))
  } > RESETVEC
}

INCLUDE msp430_base.ld
INCLUDE msp430_hirom.ld
