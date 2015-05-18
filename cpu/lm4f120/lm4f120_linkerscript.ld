/*
* Copyright (c) 2012, Rakendra Thapa
* All rights reserved.
*
* File:			lm4f120_linkerscript.ld
* Author:		Rakendra Thapa <rakendrathapa@gmail.com>.
* Description:	Linker description file for LM4FXXX microcontrollers.
*/

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
SEARCH_DIR(.)

MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 256K
    RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 32K
}

_stack_size = 4K;

/*
* Sections definitions:
*
* .text 		- machine instructions.
* .relocate    -  uninitialized data defined in the program. Data copied from FLASH to RAM
* .bss  		- un-initialized global and static variables (to be initialized to 0 before starting main).
* .stack
*/

SECTIONS
{	
	/* This section it's the code, containing the NVIC Vector table that must start at 0x0
	*   Look at the LM4F120H5QR datasheet for details. 
	*/
    .text : 
    {
		. = ALIGN(4);
    	_start_text = .;	
    	KEEP(*(.isr_vector)) 
        *(.text .text.* .gnu.linkonce.t.*)
        *(.glue_7t) *(.glue_7)
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

        . = ALIGN(4);
        _efixed = .;            /* End of fixed text section */
    } > FLASH

	/* .ARM.exidx is sorted, so has to go in its own output section.  */
    PROVIDE_HIDDEN (__exidx_start = .);
    .ARM.exidx :
    {
      *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FLASH
    PROVIDE_HIDDEN (__exidx_end = .);

	. = ALIGN(4);
    _end_text = .;	

    .relocate : AT (_end_text) 
    {
		. = ALIGN(4);
    	_srelocate = .;	
        *(.ramfunc .ramfunc.*);
        *(.data .data.*);
        . = ALIGN(4);
        _erelocate = .;		/* And another index to the end of .data segment. */
    } > RAM

	/* .bss contains the unitialized variables and must be set as 0x0 during runtime.*/
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
    } > RAM

    /* heap section */
    . = ALIGN(4);
    _sheap = . ;
    _eheap = ORIGIN(RAM) + LENGTH(RAM) - _stack_size;

    . = ALIGN(8);
	_stack_bottom = _eheap; 
	_stack_top = ORIGIN(RAM) + LENGTH(RAM);			/* An index to the end of the stack */
	
}
