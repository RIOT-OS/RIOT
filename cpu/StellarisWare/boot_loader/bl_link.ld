/******************************************************************************
 *
 * bl_link.ld - Scatter file for Gnu tools
 *
 * Copyright (c) 2007-2012 Texas Instruments Incorporated.  All rights reserved.
 * Software License Agreement
 * 
 * Texas Instruments (TI) is supplying this software for use solely and
 * exclusively on TI's microcontroller products. The software is owned by
 * TI and/or its suppliers, and is protected under applicable copyright
 * laws. You may not combine this software with "viral" open-source
 * software in order to form a larger program.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
 * NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
 * NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
 * DAMAGES, FOR ANY REASON WHATSOEVER.
 * 
 * This is part of revision 9453 of the Stellaris Firmware Development Package.
 *
 *****************************************************************************/

SECTIONS
{
    .text 0x20000000 : AT (0x00000000)
    {
        _text = .;
        KEEP(*(.isr_vector))
        *(.text*)
        *(.rodata*)
        _etext = .;
    }

    .data 0x20000000 + SIZEOF(.text) : AT (SIZEOF(.text))
    {
        _data = .;
        *(.data*)
        _edata = .;
    }

    .bss 0x20000000 + SIZEOF(.text) + SIZEOF(.data) :
        AT (ADDR(.data) + SIZEOF(.data))
    {
        _bss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
    }
}
