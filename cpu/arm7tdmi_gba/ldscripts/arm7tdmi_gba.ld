/*
 * Copyright 2013, Freie Universitaet Berlin (FUB). All rights reserved.
 *           2024, HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

MEMORY {
    ewram : ORIGIN = 0x2000000, LENGTH = 256K /* on board ram, slower */
    ram (w!rx) : ORIGIN = 0x3000000, LENGTH = 32K /* on chip ram, faster */
    rom (rx)  : ORIGIN = 0x8000000, LENGTH = 32M /* on cartridge */
}


SECTIONS
{
    .text :
    {
        KEEP(*(.gbaheader))
        . = ALIGN(4);
        KEEP(*(.init))
        . = ALIGN(4);
        KEEP(*(.init0))                 /* Start here after reset.  */
        . = ALIGN(4);
        *(.text .text.* .gnu.linkonce.t.*)
        *(.glue_7t) *(.glue_7)
        . = ALIGN(4);
        *(.rodata .rodata* .gnu.linkonce.r.*)
        *(.ARM.extab* .gnu.linkonce.armextab.*)

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

        . = ALIGN(0x4);
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
        KEEP (*(SORT(.roxfa.*)))

        . = ALIGN(4);
        _efixed = .;            /* End of text section */
    } > rom

    /*
     * TLS relocations are offsets relative to the address
     * of the first TLS symbol. That means we just need to
     * allocate them all together so that the TLS region
     * is compact when allocated for each thread.
     */

    /*
     * TLS initialization data is loaded into ROM so that
     * each thread can get its values initialized from there
     * at startup
     */
    .tdata :
    {
         __tdata_start = .;
        *(.tdata .tdata.* .gnu.linkonce.td.*)
        __tdata_end = .;
    } > rom
    __tdata_source = LOADADDR(.tdata);
    __tdata_size = SIZEOF(.tdata);

    /*
     * TLS zeroed data is relocated as if it immediately followed
     * the tdata values. However, the linker 'magically' erases the
     * memory allocation so that no ROM is consumed by this
     * section
     */
    .tbss :
    {
        *(.tbss .tbss.* .gnu.linkonce.tb.*)
        *(.tcommon)
        __tbss_end = .;
    } > rom
    __tls_size = __tbss_end - __tdata_start;
    __tbss_size = __tls_size - __tdata_size;

    /* .ARM.exidx is sorted, so has to go in its own output section.  */
    PROVIDE_HIDDEN (__exidx_start = .);
    .ARM.exidx :
    {
      *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > rom
    PROVIDE_HIDDEN (__exidx_end = .);

    /* exception handling */
    . = ALIGN(4);
    .eh_frame :
    {
        KEEP (*(.eh_frame))
    } > rom

    . = ALIGN(4);
    _etext = .;

/**************************************************************************
 * RAM
 **************************************************************************/

    /*
     * Stacks
     */
    .stack (NOLOAD) :
    {
        PROVIDE(__stack_start = .);
        KEEP (*(.usr_stack))
        __stack_usr_start = .;
        KEEP (*(.und_stack))
        __stack_und_start = .;
        KEEP (*(.fiq_stack))
        __stack_fiq_start = .;
        KEEP (*(.irq_stack))
        __stack_irq_start = .;
        KEEP (*(.abt_stack))
        __stack_abt_start = .;
        KEEP (*(.svc_stack))
        __stack_svc_start = .;

        PROVIDE(__stack_end = .);
    } > ram

    .relocate :
    {
        . = ALIGN(4);
        _srelocate = .;
        *(.ramfunc .ramfunc.*);
        *(.data .data.*);
        KEEP (*(.openocd .openocd.*))
        KEEP (*(SORT(.xfa.*)))
        . = ALIGN(4);
        _erelocate = .;
    } > ram AT> rom

    /* .bss section which is used for uninitialized data */
    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        _sbss = . ;
        _szero = .;
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = . ;
        _ezero = .;
    } > ram

    /*
     * collect all uninitialized sections that go into RAM
     */
    .noinit (NOLOAD) :
    {
        __noinit_start = .;
        PROVIDE(__fiq_handler = .);
        *(.fiq)
        *(.noinit)
        . = ALIGN(4);
        __noinit_end = .;
    }  > ram

    /* heap section */
    . = ALIGN(4);
    _sheap = . ;
    _eheap = ORIGIN(ram) + LENGTH(ram);

    .heap1 ALIGN(4) (NOLOAD) :
    {
        _sheap1 = ORIGIN(ewram);
        _eheap1 = ORIGIN(ewram) + LENGTH(ewram);
    } > ewram

    /* Populate information about ram size */
    _sram = ORIGIN(ram);
    _eram = ORIGIN(ram) + LENGTH(ram);

    /* Populate information about rom size */
    _srom = ORIGIN(rom);
    _erom = ORIGIN(rom) + LENGTH(rom);

    .end_fw (NOLOAD) : ALIGN(4) {
        _end_fw = . ;
    } > rom
}
