/* Copyright (C) 2014-2021 Free Software Foundation, Inc.
   Copying and distribution of this script, with or without modification,
   are permitted in any medium without royalty provided the copyright
   notice and this notice are preserved.  */
SECTIONS
{
  /* Read-only sections, merged into text segment: */
  .hash          : { *(.hash)		}
  .dynsym        : { *(.dynsym)		}
  .dynstr        : { *(.dynstr)		}
  .gnu.version   : { *(.gnu.version)	}
  .gnu.version_d   : { *(.gnu.version_d)	}
  .gnu.version_r   : { *(.gnu.version_r)	}
  .rel.init      : { *(.rel.init)		}
  .rela.init     : { *(.rela.init)	}
  .rel.text      :
    {
      *(.rel.text)
      *(.rel.text.*)
      *(.rel.gnu.linkonce.t*)
    }
  .rela.text     :
    {
      *(.rela.text)
      *(.rela.text.*)
      *(.rela.gnu.linkonce.t*)
    }
  .rel.fini      : { *(.rel.fini)		}
  .rela.fini     : { *(.rela.fini)	}
  .rel.rodata    :
    {
      *(.rel.rodata)
      *(.rel.rodata.*)
      *(.rel.gnu.linkonce.r*)
    }
  .rela.rodata   :
    {
      *(.rela.rodata)
      *(.rela.rodata.*)
      *(.rela.gnu.linkonce.r*)
    }
  .rel.data      :
    {
      *(.rel.data)
      *(.rel.data.*)
      *(.rel.gnu.linkonce.d*)
    }
  .rela.data     :
    {
      *(.rela.data)
      *(.rela.data.*)
      *(.rela.gnu.linkonce.d*)
    }
  .rel.ctors     : { *(.rel.ctors)	}
  .rela.ctors    : { *(.rela.ctors)	}
  .rel.dtors     : { *(.rel.dtors)	}
  .rela.dtors    : { *(.rela.dtors)	}
  .rel.got       : { *(.rel.got)		}
  .rela.got      : { *(.rela.got)		}
  .rel.bss       : { *(.rel.bss)		}
  .rela.bss      : { *(.rela.bss)		}
  .rel.plt       : { *(.rel.plt)		}
  .rela.plt      : { *(.rela.plt)		}
  /* Internal text space or external memory.  */
  .text   :
  {
    *(.vectors)
    KEEP(*(.vectors))
    /* For data that needs to reside in the lower 64k of progmem.  */
    *(.progmem.gcc*)
    /* PR 13812: Placing the trampolines here gives a better chance
       that they will be in range of the code that uses them.  */
    . = ALIGN(2);
    __trampolines_start = . ;
    /* The jump trampolines for the 16-bit limited relocs will reside here.  */
    *(.trampolines)
    *(.trampolines*)
    __trampolines_end = . ;
    /* avr-libc expects these data to reside in lower 64K. */
    *libprintf_flt.a:*(.progmem.data)
    *libc.a:*(.progmem.data)
    *(.progmem.*)
    . = ALIGN(2);
    /* For code that needs to reside in the lower 128k progmem.  */
    *(.lowtext)
    *(.lowtext*)
     __ctors_start = . ;
     *(.ctors)
     __ctors_end = . ;
     __dtors_start = . ;
     *(.dtors)
     __dtors_end = . ;
    KEEP(SORT(*)(.ctors))
    KEEP(SORT(*)(.dtors))
    /* From this point on, we do not bother about whether the insns are
       below or above the 16 bits boundary.  */
    *(.init0)  /* Start here after reset.  */
    KEEP (*(.init0))
    *(.init1)
    KEEP (*(.init1))
    *(.init2)  /* Clear __zero_reg__, set up stack pointer.  */
    KEEP (*(.init2))
    *(.init3)
    KEEP (*(.init3))
    *(.init4)  /* Initialize data and BSS.  */
    KEEP (*(.init4))
    *(.init5)
    KEEP (*(.init5))
    *(.init6)  /* C++ constructors.  */
    KEEP (*(.init6))
    *(.init7)
    KEEP (*(.init7))
    *(.init8)
    KEEP (*(.init8))
    *(.init9)  /* Call main().  */
    KEEP (*(.init9))
    *(.text)
    . = ALIGN(2);
    *(.text.*)
    . = ALIGN(2);
    *(.fini9)  /* _exit() starts here.  */
    KEEP (*(.fini9))
    *(.fini8)
    KEEP (*(.fini8))
    *(.fini7)
    KEEP (*(.fini7))
    *(.fini6)  /* C++ destructors.  */
    KEEP (*(.fini6))
    *(.fini5)
    KEEP (*(.fini5))
    *(.fini4)
    KEEP (*(.fini4))
    *(.fini3)
    KEEP (*(.fini3))
    *(.fini2)
    KEEP (*(.fini2))
    *(.fini1)
    KEEP (*(.fini1))
    *(.fini0)  /* Infinite loop after program termination.  */
    KEEP (*(.fini0))
    /* For code that needs not to reside in the lower progmem.  */
    *(.hightext)
    *(.hightext*)
    *(.progmemx.*)
    . = ALIGN(2);
    /* For tablejump instruction arrays.  We do not relax
       JMP / CALL instructions within these sections.  */
    *(.jumptables)
    *(.jumptables*)
    _etext = . ;
  }  > text
  .data          :
  {
     PROVIDE (__data_start = .) ;
    *(.data)
    *(.data*)
    /* Changes for XFA */
    KEEP (*(SORT(.roxfa.*)))
    KEEP (*(SORT(.xfa.*)))
    /* End of Changes for XFA*/
    *(.gnu.linkonce.d*)
    *(.rodata)  /* We need to include .rodata here if gcc is used */
    *(.rodata*) /* with -fdata-sections.  */
    *(.gnu.linkonce.r*)
    . = ALIGN(2);
     _edata = . ;
     PROVIDE (__data_end = .) ;
  }  > data AT> text
  .bss  ADDR(.data) + SIZEOF (.data)   : AT (ADDR (.bss))
  {
     PROVIDE (__bss_start = .) ;
    *(.bss)
     *(.bss*)
     *(COMMON)
     PROVIDE (__bss_end = .) ;
  }  > data
   __data_load_start = LOADADDR(.data);
   __data_load_end = __data_load_start + SIZEOF(.data);
  /* Global data not cleared after reset.  */
  .noinit  ADDR(.bss) + SIZEOF (.bss)  :  AT (ADDR (.noinit))
  {
     PROVIDE (__noinit_start = .) ;
    *(.noinit .noinit.* .gnu.linkonce.n.*)
     PROVIDE (__noinit_end = .) ;
     _end = . ;
     PROVIDE (__heap_start = .) ;
  }  > data
  .eeprom  :
  {
    /* See .data above...  */
    KEEP(*(.eeprom*))
     __eeprom_end = . ;
  }  > eeprom
  .fuse  :
  {
    KEEP(*(.fuse))
    KEEP(*(.lfuse))
    KEEP(*(.hfuse))
    KEEP(*(.efuse))
  }  > fuse
  .lock  :
  {
    KEEP(*(.lock*))
  }  > lock
  .signature  :
  {
    KEEP(*(.signature*))
  }  > signature
  /* Stabs debugging sections.  */
  .stab 0 : { *(.stab) }
  .stabstr 0 : { *(.stabstr) }
  .stab.excl 0 : { *(.stab.excl) }
  .stab.exclstr 0 : { *(.stab.exclstr) }
  .stab.index 0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment 0 : { *(.comment) }
  .note.gnu.build-id   : { *(.note.gnu.build-id) }
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
  .debug_line     0 : { *(.debug_line .debug_line.* .debug_line_end) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
  /* SGI/MIPS DWARF 2 extensions.  */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }
  /* DWARF 3.  */
  .debug_pubtypes 0 : { *(.debug_pubtypes) }
  .debug_ranges   0 : { *(.debug_ranges) }
  /* DWARF 5.  */
  .debug_addr     0 : { *(.debug_addr) }
  .debug_line_str 0 : { *(.debug_line_str) }
  .debug_loclists 0 : { *(.debug_loclists) }
  .debug_macro    0 : { *(.debug_macro) }
  .debug_names    0 : { *(.debug_names) }
  .debug_rnglists 0 : { *(.debug_rnglists) }
  .debug_str_offsets 0 : { *(.debug_str_offsets) }
  .debug_sup      0 : { *(.debug_sup) }
}
