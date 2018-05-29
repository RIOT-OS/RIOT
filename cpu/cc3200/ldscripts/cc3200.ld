/*****************************************************************************
* cc3200.ld
*
* GCC Linker script for blinky application.
*
* Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
*
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/


HEAP_SIZE = 0x00010000;

MEMORY
{
    /* SRAM size of 240KB for cc3200 ES 1.33 device onward */
    SRAM (rwx) : ORIGIN = 0x20004000, LENGTH = 0x3C000
}

SECTIONS
{
    .text :
    {
        _text = .;
        KEEP(*(.vectors))
        *(.text*)
        *(.rodata*)
	*(.ARM.extab* .gnu.linkonce.armextab.*)
	. = ALIGN(8);
        _etext = .;
    } > SRAM

    .ARM : {
    __exidx_start = .;
      *(.ARM.exidx*)
      __exidx_end = .;
    } > SRAM

    __init_data = .;
    _etext = .;

    .data : AT(__init_data)
    {
        _srelocate = .;
        *(.data*)
        KEEP (*(.openocd .openocd.*))
        . = ALIGN (8);
        _erelocate = .;
    } > SRAM

    .bss :
    {
        _szero = .;
        *(.bss*)
        *(COMMON)
        _ezero = .;
    } > SRAM

    .heap :
    {
	_sheap = .;
	. = . + HEAP_SIZE;
	. = ALIGN(8);
	_eheap = .;

    }

    /* stack section */
    .stack (NOLOAD):
    {
        . = ALIGN(8);
        _sstack = .;
        . = ORIGIN(SRAM) + LENGTH(SRAM);
        . = ALIGN(8);
        _estack = .;
    } > SRAM

    /* Populate information abour ram size */
    _sram = ORIGIN(SRAM);
    _eram = ORIGIN(SRAM) + LENGTH(SRAM);

}
