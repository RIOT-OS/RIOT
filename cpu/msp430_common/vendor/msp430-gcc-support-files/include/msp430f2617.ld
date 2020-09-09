/* ============================================================================ */
/* Copyright (c) 2020, Texas Instruments Incorporated                           */
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
/* Version: 1.210 */
/* Default linker script, for normal executables */

OUTPUT_ARCH(msp430)
ENTRY(_start)

MEMORY {
  SFR              : ORIGIN = 0x0000, LENGTH = 0x0010 /* END=0x0010, size 16 */
  RAM              : ORIGIN = 0x1100, LENGTH = 0x2000 /* END=0x30FF, size 8192 */
  RAM_MIRROR       : ORIGIN = 0x0200, LENGTH = 0x0800
  INFOMEM          : ORIGIN = 0x1000, LENGTH = 0x0100 /* END=0x10FF, size 256 as 4 64-byte segments */
  INFOA            : ORIGIN = 0x10C0, LENGTH = 0x0040 /* END=0x10FF, size 64 */
  INFOB            : ORIGIN = 0x1080, LENGTH = 0x0040 /* END=0x10BF, size 64 */
  INFOC            : ORIGIN = 0x1040, LENGTH = 0x0040 /* END=0x107F, size 64 */
  INFOD            : ORIGIN = 0x1000, LENGTH = 0x0040 /* END=0x103F, size 64 */
  ROM (rx)         : ORIGIN = 0x3100, LENGTH = 0xCEBE /* END=0xFFBD, size 52926 */
  HIROM (rx)       : ORIGIN = 0x00010000, LENGTH = 0x00009FFF
  BSLSIGNATURE     : ORIGIN = 0xFFBE, LENGTH = 0x0002
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
}

SECTIONS
{
  .bslsignature       : {} > BSLSIGNATURE
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

  .lower.rodata :
  {
    . = ALIGN(2);
    *(.lower.rodata.* .lower.rodata)
  } > ROM

  .rodata :
  {
    . = ALIGN(2);
    *(.plt)
    . = ALIGN(2);
    *(.rodata .rodata.* .gnu.linkonce.r.* .const .const:*)
    *(.rodata1)
    KEEP (*(.gcc_except_table)) *(.gcc_except_table.*)
  } > ROM

  /* Note: This is a separate .rodata section for sections which are
     read only but which older linkers treat as read-write.
     This prevents older linkers from marking the entire .rodata
     section as read-write.  */
  .rodata2 :
  {
    . = ALIGN(2);
    PROVIDE (__preinit_array_start = .);
    KEEP (*(.preinit_array))
    PROVIDE (__preinit_array_end = .);
    . = ALIGN(2);
    PROVIDE (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array))
    PROVIDE (__init_array_end = .);
    . = ALIGN(2);
    PROVIDE (__fini_array_start = .);
    KEEP (*(.fini_array))
    KEEP (*(SORT(.fini_array.*)))
    PROVIDE (__fini_array_end = .);
    . = ALIGN(2);
    *(.eh_frame_hdr)
    KEEP (*(.eh_frame))

    /* gcc uses crtbegin.o to find the start of the constructors, so
       we make sure it is first.  Because this is a wildcard, it
       doesn't matter if the user does not actually link against
       crtbegin.o; the linker won't look for a file to match a
       wildcard.  The wildcard also means that it doesn't matter which
       directory crtbegin.o is in.  */
    KEEP (*crtbegin*.o(.ctors))

    /* We don't want to include the .ctor section from from the
       crtend.o file until after the sorted ctors.  The .ctor section
       from the crtend file contains the end of ctors marker and it
       must be last */
    KEEP (*(EXCLUDE_FILE (*crtend*.o ) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))

    KEEP (*crtbegin*.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend*.o ) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
  } > ROM

  .upper.rodata :
  {
    *(.upper.rodata.* .upper.rodata)
  } > HIROM

  .data :
  {
    . = ALIGN(2);
    PROVIDE (__datastart = .);
    *(.lower.data.* .lower.data)

    . = ALIGN(2);
    *(.either.data.* .either.data)

    . = ALIGN(2);
    KEEP (*(.jcr))
    *(.data.rel.ro.local) *(.data.rel.ro*)
    *(.dynamic)

    . = ALIGN(2);
    *(.data .data.* .gnu.linkonce.d.*)
    KEEP (*(.gnu.linkonce.d.*personality*))
    SORT(CONSTRUCTORS)
    *(.data1)
    *(.got.plt) *(.got)

    /* We want the small data sections together, so single-instruction offsets
       can access them all, and initialized data all before uninitialized, so
       we can shorten the on-disk segment size.  */
    . = ALIGN(2);
    *(.sdata .sdata.* .gnu.linkonce.s.* D_2 D_1)

    . = ALIGN(2);
    _edata = .;
    PROVIDE (edata = .);
    PROVIDE (__dataend = .);
  } > RAM AT> ROM

  /* Note that crt0 assumes this is a multiple of two; all the
     start/stop symbols are also assumed word-aligned.  */
  PROVIDE(__romdatastart = LOADADDR(.data));
  PROVIDE (__romdatacopysize = SIZEOF(.data));

  .bss :
  {
    . = ALIGN(2);
    PROVIDE (__bssstart = .);
    *(.lower.bss.* .lower.bss)
    . = ALIGN(2);
    *(.either.bss.* .either.bss)
    *(.dynbss)
    *(.sbss .sbss.*)
    *(.bss .bss.* .gnu.linkonce.b.*)
    . = ALIGN(2);
    *(COMMON)
    PROVIDE (__bssend = .);
  } > RAM
  PROVIDE (__bsssize = SIZEOF(.bss));

  /* This section contains data that is not initialised during load
     or application reset.  */
  .noinit (NOLOAD) :
  {
    . = ALIGN(2);
    PROVIDE (__noinit_start = .);
    *(.noinit)
    . = ALIGN(2);
    PROVIDE (__noinit_end = .);
  } > RAM

  /* We create this section so that "end" will always be in the
     RAM region (matching .stack below), even if the .bss
     section is empty.  */
  .heap (NOLOAD) :
  {
    . = ALIGN(2);
    __heap_start__ = .;
    _end = __heap_start__;
    PROVIDE (end = .);
    KEEP (*(.heap))
    _end = .;
    PROVIDE (end = .);
    /* This word is here so that the section is not empty, and thus
       not discarded by the linker.  The actual value does not matter
       and is ignored.  */
    LONG(0);
    __heap_end__ = .;
    __HeapLimit = __heap_end__;
  } > RAM
  /* WARNING: Do not place anything in RAM here.
     The heap section must be the last section in RAM and the stack
     section must be placed at the very end of the RAM region.  */

  .stack (ORIGIN (RAM) + LENGTH(RAM)) :
  {
    PROVIDE (__stack = .);
    *(.stack)
  }

  /* This is just for crt0.S and interrupt handlers.  */
  .lowtext           :
  {
    PROVIDE (_start = .);
    . = ALIGN(2);
    KEEP (*(SORT(.crt_*)))
    KEEP (*(.lowtext))
  } > ROM

  .lower.text :
  {
    . = ALIGN(2);
    *(.lower.text.* .lower.text)
  } > ROM

  .text :
  {
    . = ALIGN(2);
    *(.text .stub .text.* .gnu.linkonce.t.* .text:*)

    KEEP (*(.text.*personality*))
    /* .gnu.warning sections are handled specially by elf32.em.  */
    *(.gnu.warning)
    *(.interp .hash .dynsym .dynstr .gnu.version*)
    PROVIDE (__etext = .);
    PROVIDE (_etext = .);
    PROVIDE (etext = .);
    . = ALIGN(2);
    KEEP (*(.init))
    KEEP (*(.fini))
    KEEP (*(.tm_clone_table))
  } > ROM

  .upper.text :
  {
    . = ALIGN(2);
    *(.upper.text.* .upper.text)
  } > HIROM

  .infoA     : {} > INFOA              /* MSP430 INFO FLASH MEMORY SEGMENTS */
  .infoB     : {} > INFOB
  .infoC     : {} > INFOC
  .infoD     : {} > INFOD

  /* Make sure that upper data sections are not used.  */
  .upper :
  {
    *(.upper.bss.* .upper.bss)
    *(.upper.data.* .upper.data)
    ASSERT (SIZEOF(.upper) == 0, "This MCU does not support placing read/write data into high memory");
  } > HIROM

  /* The rest are all not normally part of the runtime image.  */

  .MSP430.attributes 0 :
  {
    KEEP (*(.MSP430.attributes))
    KEEP (*(.gnu.attributes))
    KEEP (*(__TI_build_attributes))
  }

  /* Stabs debugging sections.  */
  .stab          0 : { *(.stab) }
  .stabstr       0 : { *(.stabstr) }
  .stab.excl     0 : { *(.stab.excl) }
  .stab.exclstr  0 : { *(.stab.exclstr) }
  .stab.index    0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment       0 : { *(.comment) }
  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */
  /* DWARF 1.  */
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }
  /* GNU DWARF 1 extensions.  */
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }
  /* DWARF 1.1 and DWARF 2.  */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  /* DWARF 2.  */
  .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line .debug_line.* .debug_line_end ) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
  /* SGI/MIPS DWARF 2 extensions.  */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }
  /* DWARF 3 */
  .debug_pubtypes 0 : { *(.debug_pubtypes) }
  .debug_ranges   0 : { *(.debug_ranges) }
  /* DWARF Extension.  */
  .debug_macro    0 : { *(.debug_macro) }

  /DISCARD/ : { *(.note.GNU-stack) }
}


/****************************************************************************/
/* Include peripherals memory map                                           */
/****************************************************************************/

INCLUDE msp430f2617_symbols.ld

