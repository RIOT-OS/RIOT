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
 * @ingroup     boards_x86-multiboot
 * @{
 *
 * @file
 * @brief       Startup code to be loaded by multiboot enabled bootloaders.
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include <cpu.h>
#include <multiboot.h>
#include <x86_kernel_memory.h>
#include <x86_memory.h>
#include <x86_reboot.h>
#include <x86_videoram.h>

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef BREAK_STARTUP
#   define BREAK_STARTUP (0)
#endif

static void __attribute__((noreturn)) startup(uint32_t multiboot_magic, const multiboot_info_t *multiboot_info);
void __attribute__((noreturn, optimize("Os", "omit-frame-pointer"), no_instrument_function)) _start(void);

extern const multiboot_header_t multiboot_header __attribute__((section("._multiboot_header")));
const multiboot_header_t multiboot_header = {
    .magic = MULTIBOOT_HEADER_MAGIC,
    .flags = MULTIBOOT_HEADER_FLAGS,
    .checksum = (-(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)),
    .header_addr = (uintptr_t) &multiboot_header,
    .load_addr = (uintptr_t) &_kernel_memory_start,
    .load_end_addr = (uintptr_t) &_section_data_end,
    .bss_end_addr = (uintptr_t) &_section_bss_end,
    .entry_addr = (uintptr_t) &_start,
};

void __attribute__((noreturn, optimize("Os", "omit-frame-pointer"), no_instrument_function)) _start(void)
{
    __asm__ volatile ("xor %ebp, %ebp");
    __asm__ volatile ("push %ebp");
    __asm__ volatile ("push %ebx");
    __asm__ volatile ("push %eax");
    __asm__ volatile ("push %ebp");
    __asm__ volatile ("jmp *%0" :: "r"(&startup));
    __builtin_unreachable();
}

static const multiboot_info_t *multiboot_info;

bool x86_get_memory_region(uint64_t *start, uint64_t *len, unsigned long *pos)
{
    while (1) {
        if (*pos >= multiboot_info->mmap_length) {
            return false;
        }

        const multiboot_memory_map_t *mmap = (void *)(multiboot_info->mmap_addr + *pos);
        *pos += mmap->size + 4;

        *start = mmap->addr;
        *len = mmap->len;

        uint64_t end = *start + *len;
        printf("  %08lx%08lx - %08lx%08lx ", (long) (*start >> 32), (long) *start, (long) (end >> 32), (long) end);

        if (mmap->type != 1) {
            // not free (ignore reclaimable RAM)
            const char *msg;
            switch (mmap->type) {
                case 2: msg = "reseved"; break;
                case 3: msg = "ACPI [reclaimable]"; break;
                case 4: msg = "ACPI [NVS]"; break;
                case 5: msg = "bad memory"; break;
                default: msg = "unknown";
            }
            printf("(unusable: %s)\r\n", msg);
        }
        else {
            puts("(usable)");
            return true;
        }
    }
}

static void have_a_break(void)
{
    volatile bool cnt = false;
    while (!cnt) {
        __asm__ volatile ("pause");
    }
}

static void __attribute__((noreturn)) startup(uint32_t multiboot_magic, const multiboot_info_t *multiboot_info_)
{
    x86_init_gdt(); /* load GDT early */
    x86_load_empty_idt(); /* just a safeguard to cause a tripple fault, not really needed */

#if BREAK_STARTUP
    have_a_break();
#else
    (void) have_a_break;
#endif

    memset(&_section_bss_start, 0, &_section_bss_end - &_section_bss_start + 1);

    videoram_puts(" Booting RIOT \r\n");
    if (multiboot_magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        videoram_puts(" Multiboot magic is wrong \r\n");
        x86_hlt();
    }
    else if (!(multiboot_info->flags && MULTIBOOT_INFO_MEM_MAP)) {
        videoram_puts(" Multiboot is lacking memory map information \r\n");
        x86_hlt();
    }

    multiboot_info = multiboot_info_;
    x86_startup();
}
