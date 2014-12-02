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

/**
 * Layout of the kernel memory. The symbols are set in the linker.ld.
 *
 * @ingroup x86
 * @{
 * @file
 * @author  René Kijewski <rene.kijewski@fu-berlin.de>
 */

#ifndef KERNEL_MEMORY_H__
#define KERNEL_MEMORY_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   First byte inside the elf segments.
 *
 * Expected to be the start of the high memory, i.e. 1MB.
 */
extern char _kernel_memory_start;

/**
 * @brief   Start of first byte of code inside the .text segment.
 *
 * This symbol does not have to be aligned.
 */
extern char _section_text_start;

/**
 * @brief   First byte after the code.
 *
 * This symbol does not have to be aligned.
 */
extern char _section_text_end;

/**
 * @brief   First byte inside the .rodata segment.
 *
 * This symbol is expected to be aligned.
 */
extern char _section_rodata_start;

/**
 * @brief   First byte after the .rodata segment.
 *
 * This symbol does not have to be aligned.
 */
extern char _section_rodata_end;

/**
 * @brief   First byte inside the .data segment.
 *
 * This symbol is expected to be aligned.
 */
extern char _section_data_start;

/**
 * @brief   First byte after the .data segment.
 *
 * This symbol does not have to be aligned, since it shares the same previleges as the .bss segment.
 */
extern char _section_data_end;

/**
 * @brief   Start of the .bss segment.
 *
 * Probably unaligned.
 * The .bss segment must follow the .data segment with no other segment inbetween.
 * It's likely that `&_section_data_end == &_section_bss_start`.
 */
extern char _section_bss_start;

/**
 * @brief   First byte after the .bss segment.
 *
 * This symbol should not to be aligned, because RAM cycles would be wasted to clear unused bytes otherwise.
 */
extern char _section_bss_end;

/**
 * @brief   First byte after the last of the common elf segments.
 *
 * This symbol is expected to be aligned.
 * The "common elf segments" are .text, .rodata, .data and .bss.
 * All the (physical) memory beyond this point will be overwritten at the pleasure of the kernel.
 */
extern char _kernel_memory_end;

/**
 * @brief   First byte that may be mapped as the heap
 *
 * This symbol is expected to be aligned.
 * Must be bigger than _kernel_memory_end.
 * There should be a space between _kernel_memory_end and _heap_start, but it does not have to.
 */
extern char _heap_start;

#ifdef __cplusplus
}
#endif

#endif

/** @} */
