/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(_start)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
    /* Begin putting sections at 1 MiB, a conventional place for kernels to be
       loaded at by the bootloader. */
    . = 1M;
    _kernel_memory_start = .;

    /* First put the multiboot header, as it is required to be put very early
       early in the image or the bootloader won't recognize the file format.
       Next we'll put the .text section. */
    ._multiboot_header :
    {
        KEEP(*(._multiboot_header))
    }

    .note.gnu.build-id :
    {
        *(.note.gnu.build-id)
    }

    .text :
    {
        . = ALIGN(4);
        _section_text_start = .;

        *(.text)
        *(.text.*)

        *(.gnu.linkonce.t)
        *(.gnu.linkonce.t.*)

        _section_text_end = .;
    }
    . = ALIGN(0x1000);

    /* Read-only data. */
    .rodata :
    {
        _section_rodata_start = .;

        *(.rodata)
        *(.rodata.*)

        *(.gnu.linkonce.r)
        *(.gnu.linkonce.r.*)

        _section_rodata_end = .;
    }
    . = ALIGN(0x1000);

    /* Read-write data (initialized) */
    .data :
    {
        _section_data_start = .;

        *(.data)
        *(.data.*)

        *(.gnu.linkonce.d)
        *(.gnu.linkonce.d.*)

        . = ALIGN(4);
        _section_data_end = .;
        /* no need to align between .data and .bss */
    }

    /* Read-write data (uninitialized) and stack */
    .bss :
    {
        _section_bss_start = .;

        *(COMMON)
        *(COMMON.*)

        *(.bss)
        *(.bss.*)

        *(.gnu.linkonce.b)
        *(.gnu.linkonce.b.*)

        _section_bss_end = .;
    }

    . = ALIGN(0x1000);
    _kernel_memory_end = .;

    . += 0x1000;
    . = ALIGN(0x10000);
    _heap_start = .;

    /* The compiler may produce other sections, by default it will put them in
       a segment with the same name. Simply add stuff here as needed. */
}
