/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/* Section Definitions */
SECTIONS
{
    /* This is just for crt0.S and interrupt handlers.  */
    .lowtext           :
    {
        PROVIDE (_start = .);
        . = ALIGN(2);
        KEEP (*(SORT(.crt_*)))
        KEEP (*(.lowtext))
    } > ROM

    .text :
    {
        . = ALIGN(4);
        _sfixed = .;
        KEEP(*(.vectors .vectors.*))
        . = ALIGN(2);
        *(.lower.text.* .lower.text)

        . = ALIGN(2);
        *(.text .stub .text.* .gnu.linkonce.t.* .text:*)
        KEEP (*(.text.*personality*))
        *(.rodata .rodata* .gnu.linkonce.r.* .const .const:*)

        *(.eh_frame_hdr)
        KEEP (*(.eh_frame))
        KEEP (*(.gcc_except_table)) *(.gcc_except_table.*)
        /* Support C constructors, and C destructors in both user code
           and the C library. This also provides support for C++ code. */
        . = ALIGN(4);
        KEEP(*(.init))
        . = ALIGN(4);
        __preinit_array_start = .;
        KEEP (*(.preinit_array))
        __preinit_array_end = .;

        . = ALIGN(4);
        __init_array_start = .;
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array))
        __init_array_end = .;

        . = ALIGN(4);
        KEEP (*crtbegin.o(.ctors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*crtend.o(.ctors))

        . = ALIGN(4);
        KEEP(*(.fini))

        . = ALIGN(4);
        __fini_array_start = .;
        KEEP (*(.fini_array))
        KEEP (*(SORT(.fini_array.*)))
        __fini_array_end = .;

        KEEP (*crtbegin.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*crtend.o(.dtors))

        . = ALIGN(4);
        _efixed = .;            /* End of text section */
    } > ROM

    _etext = .;

    /* stack section */
    .stack (NOLOAD):
    {
        . = ALIGN(8);
        PROVIDE (__stack = .);
        _sstack = .;
        KEEP (*(.isr_stack))
        . = ALIGN(8);
        _estack = .;
    } > RAM

    .relocate : AT (_etext)
    {
        . = ALIGN(4);
        _srelocate = .;
        PROVIDE(__datastart = .);
        *(.ramfunc .ramfunc.*);
        *(.lower.data.* .lower.data)
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
        . = ALIGN(4);
        /* We want the small data sections together, so single-instruction offsets
           can access them all, and initialized data all before uninitialized, so
           we can shorten the on-disk segment size.  */
        . = ALIGN(2);
        *(.sdata .sdata.* .gnu.linkonce.s.* D_2 D_1)

        . = ALIGN(2);
        _erelocate = .;
    } > RAM

    /* MSPGCC libc compatibility  */
    PROVIDE(__romdatastart = LOADADDR(.relocate));
    PROVIDE (__romdatacopysize = SIZEOF(.relocate));

    /* .bss section which is used for uninitialized data */
    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        _sbss = . ;
        _szero = .;
        PROVIDE(__bssstart = .);
        *(.lower.bss.* .lower.bss)
        . = ALIGN(2);
        *(.either.bss.* .either.bss)
        *(.dynbss)
        *(.sbss .sbss.*)
        *(.bss .bss.* .gnu.linkonce.b.*)
        . = ALIGN(2);
        *(COMMON)
        . = ALIGN(4);
        _ebss = . ;
        _ezero = .;
    } > RAM
    PROVIDE (__bsssize = SIZEOF(.bss));

    /* heap section */
    . = ALIGN(4);
    _sheap = . ;
    _eheap = ORIGIN(RAM) + LENGTH(RAM);

    /* Populate information abour RAM size */
    _sram = ORIGIN(RAM);
    _eram = ORIGIN(RAM) + LENGTH(RAM);

    .infoA     : {} > INFOA              /* MSP430 INFO FLASH MEMORY SEGMENTS */
    .infoB     : {} > INFOB

    /* Make sure that upper data sections are not used.  */
    .upper :
    {
        *(.upper.bss.* .upper.bss)
        *(.upper.data.* .upper.data)
        ASSERT (SIZEOF(.upper) == 0, "This MCU does not support placing read/write data into high memory");
    }

    /* The rest are all not normally part of the runtime image.  */

    .MP430.attributes 0 :
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
