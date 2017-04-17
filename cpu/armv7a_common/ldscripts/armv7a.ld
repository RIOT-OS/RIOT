/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*  Copyright (C) 2017                                                         */
/*                                                                             */
/*  Module      : armv7a_common                                                */
/*  File        : armv7a.ld                                                    */
/*  Description : armv7a common linker script                                  */
/*  Author      : Dang Minh Phuong (kamejoko80@yahoo.com)                      */
/*  History     :                                                              */
/*-----------------------------------------------------------------------------*/

OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)

INCLUDE cpu_model.ld

/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __end__
 *   end
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 */

SECTIONS
{
    .text :
    {
        KEEP(*(.isr_vector))
        *(.text*)

        KEEP(*(.init))
        KEEP(*(.fini))

        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)

        /* .dtors */
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)

        *(.rodata*)

        KEEP(*(.eh_frame*))
    } > ROM

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > ROM

    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > ROM
    __exidx_end = .;

    __etext = .;

    .data : AT (__etext)
    {
        __data_start__ = .;
        *(vtable)
        *(.data*)

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

        . = ALIGN(4);
        /* All data end */
        __data_end__ = .;

    } > RAM

    .bss (NOLOAD):
    {
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        __bss_end__ = .;
    } > RAM

    .heap (NOLOAD) : {
        . = ALIGN(16);
        _heap = .;
        _sheap = .;
        HeapBase = .;
        _heap_start = .;
        . += HEAP_SIZE;
        _heap_end = .;
        _eheap = .;
        HeapLimit = .;
    } > RAM
 
    .stack (NOLOAD):
    {
        . = ALIGN(8);
        PROVIDE (__stack_start = .);
        PROVIDE (__stack_fiq_start = .);
        . += STACK_FIQ_SIZE;
        PROVIDE (__stack_fiq_end = .);
        . = ALIGN(8);
        PROVIDE (__stack_irq_start = .);
        . += STACK_IRQ_SIZE;
        PROVIDE (__stack_irq_end = .);
        . = ALIGN(8);
        PROVIDE (__stack_abt_start = .);
        . += STACK_ABT_SIZE;
        PROVIDE (__stack_abt_end = .);
        . = ALIGN(8);
        PROVIDE (__stack_und_start = .);
        . += STACK_UND_SIZE;
        PROVIDE (__stack_und_end = .);
        . = ALIGN(8);
        PROVIDE (__stack_srv_start = .);
        . += STACK_SRV_SIZE;
        PROVIDE (__stack_srv_end = .);
        . = ALIGN(8);
        PROVIDE (__stack_sys_start = .);
        . += STACK_SYS_SIZE;
        PROVIDE (__stack_sys_end = .);

        . = ALIGN(8);
        PROVIDE (__stack_end = .);
    } > RAM
}
