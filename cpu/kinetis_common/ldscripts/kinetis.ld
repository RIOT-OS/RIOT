/* RAM limits */
__sram_start  = ORIGIN(sram);
__sram_length = LENGTH(sram);
__sram_end    = __sram_start + __sram_length;

/* Define the default stack size for interrupt mode. */
STACK_SIZE = DEFINED(STACK_SIZE) ? STACK_SIZE : 2048;

RAMVECT_SIZE = DEFINED(RAMVECT_SIZE) ? RAMVECT_SIZE : 0;

SECTIONS
{
    /* Interrupt vectors 0x00-0x3ff. */
    .vector_table :
    {
        _vector_rom = .;
        KEEP(*(.vector_table))
    } > vectors
    ASSERT (SIZEOF(.vector_table) == 0x400, "Interrupt vector table of invalid size.")
    ASSERT (ADDR(.vector_table) == 0x00000000, "Interrupt vector table at invalid location (linker-script error?)")
    ASSERT (LOADADDR(.vector_table) == 0x00000000, "Interrupt vector table at invalid location (linker-script error?)")

    /* Flash configuration field, very important in order to not accidentally lock the device */
    /* Flash configuration field 0x400-0x40f. */
    .fcfield :
    {
        . = ALIGN(4);
        KEEP(*(.fcfield))
        . = ALIGN(4);
    } > flashsec
    ASSERT (SIZEOF(.fcfield) == 0x10, "Flash configuration field of invalid size (linker-script error?)")
    ASSERT (ADDR(.fcfield) == 0x400, "Flash configuration field at invalid position (linker-script error?)")
    ASSERT (LOADADDR(.fcfield) == 0x400, "Flash configuration field at invalid position (linker-script error?)")

    /* Program code 0x410-. */
    .text :
    {
        . = ALIGN(4);
        _text_load = LOADADDR(.text);
        _text_start = .;
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(SORT(.preinit_array.*)))
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);
        . = ALIGN(4);

        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);
        . = ALIGN(4);

        /* fini data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);
        . = ALIGN(4);

        KEEP (*(SORT_NONE(.init)))
        KEEP (*(SORT_NONE(.fini)))
        /* Default ISR handlers */
        KEEP(*(.default_handlers))
        *(.text.unlikely .text.*_unlikely .text.unlikely.*)
        *(.text.exit .text.exit.*)
        *(.text.startup .text.startup.*)
        *(.text.hot .text.hot.*)
        *(.text .stub .text.* .gnu.linkonce.t.*)

        /* gcc uses crtbegin.o to find the start of
           the constructors, so we make sure it is
           first.  Because this is a wildcard, it
           doesn't matter if the user does not
           actually link against crtbegin.o; the
           linker won't look for a file to match a
           wildcard.  The wildcard also means that it
           doesn't matter which directory crtbegin.o
           is in.  */
        KEEP (*crtbegin.o(.ctors))
        KEEP (*crtbegin?.o(.ctors))
        KEEP (*crtbeginTS.o(.ctors))
        /* We don't want to include the .ctor section from
           the crtend.o file until after the sorted ctors.
           The .ctor section from the crtend file contains the
           end of ctors marker and it must be last */
        KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*(.ctors))

        KEEP (*crtbegin.o(.dtors))
        KEEP (*crtbegin?.o(.dtors))
        KEEP (*crtbeginTS.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*(.dtors))
        . = ALIGN(4);
        _rodata_start = .;
        *(.rodata .rodata* .gnu.linkonce.r.*)
        . = ALIGN(4);
        _rodata_end = .;
        _text_end = .;
    } > flash

    /* The .extab, .exidx sections are used for C++ exception handling */
    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > flash

    PROVIDE_HIDDEN (__exidx_start = .);
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > flash
    PROVIDE_HIDDEN (__exidx_end = .);

    .eh_frame_hdr :
    {
        *(.eh_frame_hdr)
    } > flash

    .eh_frame : ONLY_IF_RO
    {
        KEEP (*(.eh_frame))
    } > flash

    .gcc_except_table : ONLY_IF_RO
    {
        *(.gcc_except_table .gcc_except_table.*)
    } > flash

    . = ALIGN(4);
    _etext = .;

    /*
     * Allocate space for interrupt vector in RAM
     */
    .ramvect :
    {
        . = ALIGN(1024);
        _vector_ram_start = .;
        . = _vector_ram_start + RAMVECT_SIZE;
        . = ALIGN(4);
        _vector_ram_end = .;
    } > sram

    /* Program data, values stored in flash and loaded upon init. */
    .relocate : AT (_etext)
    {
        . = ALIGN(4);
        _data_load  = LOADADDR(.relocate);
        _data_start = .;
        _srelocate = .;
        *(.ramfunc .ramfunc.*);
        *(.data .data.*);
        . = ALIGN(4);
        _erelocate = .;
        _data_end = .;
    } > sram

    /* .bss section, zeroed out during init. */
    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        _sbss = . ;
        __bss_start = .;
        _szero = .;
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = . ;
        __bss_end = .;
        _ezero = .;
    } > sram

    /* stack section */
    .stack (NOLOAD):
    {
        . = ALIGN(8);
        _sstack = .;
        . = . + STACK_SIZE;
        . = ALIGN(8);
        _estack = .;
    } > sram

    /* heap section */
    . = ALIGN(8);

    /* end is used by newlib's default sbrk_r implementations to find the start
     * of the heap */
    _end = .;
    __end = .;
    __end__ = .;
    PROVIDE(end = .);

    /* _sheap and _eheap is used by RIOT's sbrk_r implementation */
    _sheap = . ;
    _eheap = ORIGIN(sram) + LENGTH(sram);

    /* Populate information abour ram size */
    _sram = ORIGIN(sram);
    _eram = ORIGIN(sram) + LENGTH(sram);

    /* Any debugging sections */
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
    /* DWARF 1 */
    .debug          0 : { *(.debug) }
    .line           0 : { *(.line) }
    /* GNU DWARF 1 extensions */
    .debug_srcinfo  0 : { *(.debug_srcinfo) }
    .debug_sfnames  0 : { *(.debug_sfnames) }
    /* DWARF 1.1 and DWARF 2 */
    .debug_aranges  0 : { *(.debug_aranges) }
    .debug_pubnames 0 : { *(.debug_pubnames) }
    /* DWARF 2 */
    .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
    .debug_abbrev   0 : { *(.debug_abbrev) }
    .debug_line     0 : { *(.debug_line .debug_line.* .debug_line_end ) }
    .debug_frame    0 : { *(.debug_frame) }
    .debug_str      0 : { *(.debug_str) }
    .debug_loc      0 : { *(.debug_loc) }
    .debug_macinfo  0 : { *(.debug_macinfo) }
    /* SGI/MIPS DWARF 2 extensions */
    .debug_weaknames 0 : { *(.debug_weaknames) }
    .debug_funcnames 0 : { *(.debug_funcnames) }
    .debug_typenames 0 : { *(.debug_typenames) }
    .debug_varnames  0 : { *(.debug_varnames) }
    /* DWARF 3 */
    .debug_pubtypes 0 : { *(.debug_pubtypes) }
    .debug_ranges   0 : { *(.debug_ranges) }
    /* DWARF Extension.  */
    .debug_macro    0 : { *(.debug_macro) }

    /* XXX: what is the purpose of these sections? */
    .ARM.attributes 0 : { KEEP (*(.ARM.attributes)) KEEP (*(.gnu.attributes)) }
    .note.gnu.arm.ident 0 : { KEEP (*(.note.gnu.arm.ident)) }
    /DISCARD/ : { *(.note.GNU-stack) *(.gnu_debuglink) *(.gnu.lto_*) }
}
