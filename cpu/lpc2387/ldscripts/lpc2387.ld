/*
 * Copyright 2013, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/* specify the LPC2387 memory areas (see LPC2387 datasheet page 15)  */
MEMORY
{
    flash               : ORIGIN = 0,          LENGTH = 504K    /* FLASH ROM 512kByte without ISP bootloader*/
    infomem             : ORIGIN = 0x0007D000, LENGTH = 4K      /* Last sector in FLASH ROM for config data */
    ram_battery         : ORIGIN = 0xE0084000, LENGTH = 2K      /* Battery RAM                              */
    ram                 : ORIGIN = 0x40000000, LENGTH = 64K     /* LOCAL ON-CHIP STATIC RAM                 */
    ram_usb             : ORIGIN = 0x7FD00000, LENGTH = 16K     /* USB RAM  !!! first 1024 bytes are occupied from GPDMA for MCI */
    ram_ethernet        : ORIGIN = 0x7FE00000, LENGTH = 16K     /* ethernet RAM                             */
}

__stack_und_size = 4;           /* stack for "undefined instruction" interrupts             */
__stack_abt_size = 4;           /* stack for "abort" interrupts                             */
__stack_fiq_size = 64;          /* stack for "FIQ" interrupts                               */
__stack_irq_size = 400;         /* stack for "IRQ" normal interrupts                        */
__stack_svc_size = 400;         /* stack for "SVC" supervisor mode                          */
__stack_usr_size = 4096;        /* stack for user operation (kernel init)                   */
__stack_size = __stack_und_size + __stack_abt_size + __stack_fiq_size + __stack_irq_size + __stack_svc_size + __stack_usr_size;

/* now define the output sections  */
SECTIONS
{
    .text :                             /* collect all sections that should go into FLASH after startup  */
    {
        KEEP(*(.vectors))               /* Exception Vectors and branch table >= 64 bytes */
        . = ALIGN(64);
        KEEP(*(.init))
        KEEP(*(.init0))                 /* Start here after reset.  */
        KEEP(*(.init1))
        KEEP(*(.init2))                 /* Copy data loop  */
        KEEP(*(.init3))
        KEEP(*(.init4))                 /* Clear bss  */
        KEEP(*(.init5))
        KEEP(*(.init6))                 /* C++ constructors.  */
        KEEP(*(.init7))
        KEEP(*(.init8))
        KEEP(*(.init9))                 /* Call main().  */
        *(.text)                        /* all .text sections (code)  */
        *(.text.*)
        *(.gnu.linkonce.t.*)

        . = ALIGN(4);
        __commands_start = .;
        KEEP(*(.commands))              /* command table */
        __commands_end = .;
        . = ALIGN(4);
        __cfgspec_start = .;
        KEEP(*(.cfgspec))               /* configuration spec table */
        __cfgspec_end = .;
        . = ALIGN(4);

        __ctors_start = .;
            PROVIDE(_os_ctor_start = .);
            KEEP(*(.ctors));
            KEEP(*(.init_array))
            PROVIDE(_os_ctor_end = .);
        __ctors_end = .;
            KEEP(*(.dtors));
            LONG (0);


        *(.rodata .rodata.*)            /* all .rodata sections (constants, strings, etc.)  */
        *(.gnu.linkonce.r.*)
        *(.glue_7)                      /* all .glue_7 sections, see https://gcc.gnu.org/ml/gcc-help/2009-03/msg00306.html */
        *(.glue_7t)                     /* all .glue_7t sections */

        KEEP(*(.fini9))
        KEEP(*(.fini8))
        KEEP(*(.fini7))
        KEEP(*(.fini6))                     /* C++ destructors.  */
        KEEP(*(.fini5))
        KEEP(*(.fini4))
        KEEP(*(.fini3))
        KEEP(*(.fini2))
        KEEP(*(.fini1))
        KEEP(*(.fini0))                     /* Infinite loop after program termination.  */
        KEEP(*(.fini))

        *(.gcc_except_table)

    } >flash                            /* put all the above into FLASH */
    . = ALIGN(4);

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

/*
    .eh_frame       : ONLY_IF_RW { KEEP (*(.eh_frame)) } > sram_u AT > flash
    .gcc_except_table   : ONLY_IF_RW { *(.gcc_except_table .gcc_except_table.*) } > sram_u AT > flash
*/

    _etext = . ;                        /* define a global symbol _etext just after the last code byte */

    .config :
    {
        *(.configmem)
        . = ALIGN(256);
    } >infomem
    . = ALIGN(4);

    /**************************************************************************
     * RAM
     **************************************************************************/


    .ctors (NOLOAD) :
    {
        . = ALIGN(4096);
        start_ctors = .;
        *(.init_array);
        *(.ctors);
        end_ctors = .;
    } >ram

    .dtors (NOLOAD) :
    {
        . = ALIGN(4096);
        start_dtors = .;
        *(.fini_array);
        *(.dtors);
        end_dtors = .;
    } >ram

    /*
     * collect all uninitialized sections that go into RAM
     */
    .noinit (NOLOAD) :
    {
        __noinit_start = .;
        PROVIDE(__fiq_handler = .);
        *(.fiq)
        *(.noinit)
    }  > ram
    . = ALIGN(4);
    __noinit_end = .;

    /*
     * collect all zero initialized sections that go into RAM
     */
    .bss (NOLOAD) :
    {
        . = ALIGN(4);                   /* ensure data is aligned so relocation can use 4-byte operations */
        __bss_start = .;                /* define a global symbol marking the start of the .bss section */
        *(.bss*)                        /* all .bss sections  */
        *(COMMON)
    } > ram                             /* put all the above in RAM (it will be cleared in the startup code */
    . = ALIGN(4);                       /* ensure data is aligned so relocation can use 4-byte operations */
    __bss_end = . ;                     /* define a global symbol marking the end of the .bss section */

    /*
     * collect all initialized .data sections that go into RAM
     * initial values get placed at the end of .text in flash
     */
    .data : AT (_etext)
    {
        . = ALIGN(4);                   /* ensure data is aligned so relocation can use 4-byte operations */
        _data = .;                      /* create a global symbol marking the start of the .data section  */
        *(.data .data.*)                /* all .data sections  */
        *(.gnu.linkonce.d*)
        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
            PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
            KEEP(*(.init_array))
            PROVIDE_HIDDEN (__init_array_end = .);

        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
            KEEP(*(.fini_array))
            PROVIDE_HIDDEN (__fini_array_end = .);
    } >ram                              /* put all the above into RAM (but load the LMA copy into FLASH) */
    . = ALIGN(4);                       /* ensure data is aligned so relocation can use 4-byte operations */
    _edata = .;                         /* define a global symbol marking the end of the .data section  */


    /*
     * Exception frames (newer linker versions generate these but they use of
     * most of the RAM.
     */
    /DISCARD/ :                         /* discard exception frames */
    {
        *(.eh_*)
    }

    /* to enable exception frames */
    /*
    .eh_frame :
    {
         KEEP (*(.eh_frame))
    } > ram
    . = ALIGN(4);
    */

    _end = .;                           /* define a global symbol marking the end of application RAM */

    __heap1_size = ORIGIN(ram) + LENGTH(ram) - . - __stack_size;
    .heap1 (NOLOAD) :
    {
        PROVIDE(_sheap = .);
        . = . + __heap1_size;
        PROVIDE(_eheap = .);
    } > ram

    /*
     * Stacks
     */
    .stack (NOLOAD) :
    {
        PROVIDE(__stack_start = .);

        . = . + __stack_usr_size;
        __stack_usr_start = .;
        . = . + __stack_und_size;
        __stack_und_start = .;
        . = . + __stack_fiq_size;
        __stack_fiq_start = .;
        . = . + __stack_irq_size;
        __stack_irq_start = .;
        . = . + __stack_abt_size;
        __stack_abt_start = .;
        . = . + __stack_svc_size;
        __stack_svc_start = .;

        PROVIDE(__stack_end = .);
    } > ram

    __heap2_size = LENGTH(ram_ethernet);
    .heap2 (NOLOAD) :
    {
        PROVIDE(__heap2_start = . );
        . = . + __heap2_size;
        PROVIDE(__heap2_max = .);       /* _heap shall always be < _heap_max */
    } > ram_ethernet

    . = ORIGIN(ram_usb);
    .usbdata (NOLOAD) :                 /* USB RAM section, may be used otherwise if USB is disabled */
    {
        *(.usbdata)
    } > ram_usb

    .heap3 ALIGN(0x1000) (NOLOAD) :
    {
        __heap3_size = ORIGIN(ram_usb) + LENGTH(ram_usb) - ABSOLUTE(.);
        PROVIDE(__heap3_start = . );
        . += __heap3_size;
        PROVIDE(__heap3_max = .);
    } > ram_usb
    __heap_size = SIZEOF(.heap3);


    .batteryram (NOLOAD) :              /* battery ram stays on during powerdown but needs to be handled specially */
    {
        *(.batteryram)
    } > ram_battery
}
