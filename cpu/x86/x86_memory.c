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
 * @ingroup     x86
 * @{
 *
 * @file
 * @brief       Virtual memory management.
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include "x86_kernel_memory.h"
#include "x86_interrupts.h"
#include "x86_memory.h"
#include "x86_registers.h"
#include "cpu.h"
#include "irq.h"
#include "tlsf-malloc.h"

#include <malloc.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* Compare Figure 44 (p. 99) of "Intel® Quark SoC X1000 Core Developer’s Manual" */
#define PT_CR3_BITS  (0)
#define PT_PDPT_BITS (PT_P)
#define PT_PD_BITS  (PT_P | PT_RW | PT_US)

#ifndef DEBUG_READ_BEFORE_WRITE
#   define PT_HEAP_BITS (PT_P | PT_RW | PT_US | pt_xd)
#else
#   define PT_HEAP_BITS (PT_HEAP_BIT | PT_RW | PT_US | pt_xd)
#endif

static uint64_t pt_xd = PT_XD;

typedef union page {
    char content[4096];
    uint64_t next_page;
    uint64_t indices[512];
} __attribute__((aligned(0x1000))) page_t;

static volatile page_t TEMP_PAGE;
#define TEMP_PAGE_PT (((uintptr_t) &TEMP_PAGE / 0x1000) / 512)
#define TEMP_PAGE_PTE  (((uintptr_t) &TEMP_PAGE / 0x1000) % 512)

void x86_init_gdt(void)
{
    static const struct gdt_entry gdt_entries[3] = {
        [0x0000 / 8] = {
            .limit_0_15            = 0,
            .base_0_15             = 0,
            .base_16_23            = 0,
            .access_byte           = 0,
            .limit_16_19_and_flags = 0,
            .base_24_31            = 0,
        },
        [0x0008 / 8] = {
            .limit_0_15            = 0xFFFF,
            .base_0_15             = 0,
            .base_16_23            = 0,
            .access_byte           = GDT_AB_EX | GDT_AB_S | GDT_AB_RING_0 | GDT_AB_PR,
            .limit_16_19_and_flags = 0xF | GDT_FLAG_SZ | GDT_FLAG_GR,
            .base_24_31            = 0,
        },
        [0x0010 / 8] = {
            .limit_0_15            = 0xFFFF,
            .base_0_15             = 0,
            .base_16_23            = 0,
            .access_byte           = GDT_AB_RW | GDT_AB_S | GDT_AB_RING_0 | GDT_AB_PR,
            .limit_16_19_and_flags = 0xF | GDT_FLAG_SZ | GDT_FLAG_GR,
            .base_24_31            = 0,
        },
    };
    static const struct gdtr_t gdt = {
        .size_minus_one = sizeof gdt_entries - 1,
        .offset = (unsigned long) &gdt_entries[0],
    };

    asm volatile ("" :: "a"(0x0010));

    asm volatile ("lgdt %0" :: "m"(gdt));
    asm volatile ("ljmp $0x0008, $1f\n"
                  "1:");

    asm volatile ("mov %ax, %ds");
    asm volatile ("mov %ax, %es");
    asm volatile ("mov %ax, %fs");
    asm volatile ("mov %ax, %gs");
    asm volatile ("mov %ax, %ss");
}

/* Addresses in PDPT, PD, and PT are linear addresses. */
/* TEMP_PAGE is used to to access these pages. */
static pae_page_table_t static_pts[X86_NUM_STATIC_PT];
static pae_page_directory_t static_pds[X86_NUM_STATIC_PD];
static pae_page_directory_pointer_table_t pdpt;

static void init_elf_static_section(uint64_t bits, void *start_, void *end_)
{
    unsigned long start = ((unsigned long) start_) / 0x1000;
    unsigned long end = (((unsigned long) end_) + 0x1000 - 1) / 0x1000;
    for (unsigned i = start; i < end; ++i) {
        unsigned pt_num = i / 512;
        unsigned pte_num = i % 512;
        static_pts[pt_num][pte_num] = (i * 0x1000) | PT_P | PT_G | bits;
    }
}

static void check_requirements(void)
{
    uint64_t cpuid = cpuid_caps();
    if ((cpuid & CPUID_PAE) == 0) {
        puts("Your CPU does not support PAE! Halting.");
        x86_hlt();
    }
    if ((cpuid & CPUID_PGE) == 0)  {
        puts("Your CPU does not support PGE! Halting.");
        x86_hlt();
    }
    if ((cpuid & CPUID_MSR) == 0) {
        puts("Warning: Your CPU does not support MSR!\n"
             "         Setting PT_XD = 0.");
        pt_xd = 0;
    }
    else {
        /* enable NX bit (if possible) */
        uint64_t efer = msr_read(MSR_EFER);
        efer |= EFER_NXE;
        msr_set(MSR_EFER, efer);
        if (!(msr_read(MSR_EFER) & EFER_NXE)) {
            puts("Warning: Your hardware does not support the NX bit!\n"
                 "         Setting PT_XD = 0.");
            pt_xd = 0;
        }
    }
}

static void init_pagetable(void)
{
    /* identity map tables */
    for (unsigned i = 0; i < X86_NUM_STATIC_PD; ++i) {
        pdpt[i] = ((uintptr_t) &static_pds[i]) | PT_PDPT_BITS;
    }
    for (unsigned i = 0; i < X86_NUM_STATIC_PT; ++i) {
        unsigned pd_num = i / 512;
        unsigned pt_num = i % 512;
        static_pds[pd_num][pt_num] = ((uintptr_t) &static_pts[i]) | PT_PD_BITS;
    }
    init_elf_static_section(PT_RW | pt_xd, (void *) 0, (void *) 0x100000);
    init_elf_static_section(PT_US, &_section_text_start, &_section_text_end);
    init_elf_static_section(PT_US | pt_xd, &_section_rodata_start, &_section_rodata_end);
    init_elf_static_section(PT_US | PT_RW | pt_xd, &_section_data_start, &_section_bss_end);

    /* activate PAE */
    /* FIXME: add x86_init_cr4() */
    uint32_t cr4 = cr4_read();
    cr4 |= CR4_PAE | CR4_MCE | CR4_PGE | CR4_PCE | CR4_OSXMMEXCPT;
    cr4 &= ~(CR4_VME | CR4_PVI | CR4_TSD | CR4_DE | CR4_PSE | CR4_OSFXSR | CR4_SMEP);
    cr4_write(cr4);

    /* load page table */
    cr3_write((uint32_t) &pdpt | PT_CR3_BITS);

    /* activate paging */
    uint32_t cr0 = cr0_read();
    cr0 |= CR0_PE | CR0_NE | CR0_WP | CR0_PG;
    cr0 &= ~(CR0_MP | CR0_EM | CR0_TS | CR0_AM | CR0_NW | CR0_CD);
    cr0_write(cr0);
}

static void set_temp_page(uint64_t addr)
{
    static_pts[TEMP_PAGE_PT][TEMP_PAGE_PTE] = addr != -1ull ? addr | PT_P | PT_RW | pt_xd : 0;
    asm volatile ("invlpg (%0)" :: "r"(&TEMP_PAGE));
}

static inline uint64_t min64(uint64_t a, uint64_t b)
{
    return a <= b ? a : b;
}

static inline uint64_t max64(uint64_t a, uint64_t b)
{
    return a >= b ? a : b;
}

static uint32_t init_free_pages_heap_position = (uintptr_t) &_heap_start;

static uint64_t init_free_pages_sub(uint64_t table, uint64_t bits, unsigned index, uint64_t *start, uint64_t *pos)
{
    set_temp_page(table);
    if (TEMP_PAGE.indices[index] & PT_P) {
        return TEMP_PAGE.indices[index] & PT_ADDR_MASK;
    }

    TEMP_PAGE.indices[index] = *start | bits;

    uint64_t result = *start;
    *start += 0x1000;
    *pos = max64(*start, *pos);
    init_free_pages_heap_position += 0x1000;
    return result;
}

static bool add_pages_to_pool(uint64_t start, uint64_t end)
{
    start += 0xFFF;
    start &= ~0xFFF;
    end &= ~0xFFF;

    start = max64(start, (uintptr_t) &_kernel_memory_end);
    uint64_t pos = start;

    uint32_t addr = init_free_pages_heap_position >> 12;
    unsigned pte_i = addr % 512;
    addr >>= 9;
    unsigned pt_i = addr % 512;
    addr >>= 9;
    unsigned pd_i = addr;

    if (pd_i >= 4) {
        return false;
    }

    while (pos < end) {
        uint64_t table = (uintptr_t) &pdpt;

        table = init_free_pages_sub(table, PT_PDPT_BITS, pd_i, &start, &pos);
        if (pos >= end) {
            break;
        }

        table = init_free_pages_sub(table, PT_PD_BITS, pt_i, &start, &pos);
        if (pos >= end) {
            break;
        }

        set_temp_page(table);
        TEMP_PAGE.indices[pte_i] = pos | PT_HEAP_BITS;
        pos += 0x1000;

        if (++pte_i >= 512) {
            pte_i = 0;
            if (++pt_i >= 512) {
                pt_i = 0;
                if (++pd_i >= 4) {
                    break;
                }
            }
        }
    }

    if (start < end) {
        cr3_write((uint32_t) &pdpt | PT_CR3_BITS); /* flush tlb */
        tlsf_add_pool((void *) init_free_pages_heap_position, end - start);
        init_free_pages_heap_position += end - start;
    }

    return true;
}

static void init_free_pages(void)
{
    printf("Kernel memory: %p - %p\r\n", &_kernel_memory_start, &_kernel_memory_end);
    printf("  .text:   %p - %p\r\n", &_section_text_start, &_section_text_end);
    printf("  .rodata: %p - %p\r\n", &_section_rodata_start, &_section_rodata_end);
    printf("  .data:   %p - %p\r\n", &_section_data_start, &_section_data_end);
    printf("  .bss:    %p - %p\r\n", &_section_bss_start, &_section_bss_end);
    printf("Unmapped memory: %p - %p\r\n", &_kernel_memory_end, &_heap_start);
    printf("Heap start: %p\r\n", &_heap_start);

    unsigned long cnt = 0;
    uint64_t start, len;
    while (x86_get_memory_region(&start, &len, &cnt)) {
        uint64_t end = start + len;
        if (!add_pages_to_pool(start, end)) {
            break;
        }
    }

    unsigned long free_pages_count = (init_free_pages_heap_position - (uintptr_t) &_heap_start) / 4096;
    float mem_amount = free_pages_count * (4096 / 1024);
    const char *mem_unit = "kB";
    if (mem_amount >= 2 * 1024) {
        mem_amount /= 1024;
        mem_unit = "MB";
    }
    if (mem_amount >= 2 * 1024) {
        mem_amount /= 1024;
        mem_unit = "GB";
    }
    printf("There are %lu free pages (%.3f %s) available for the heap.\n", free_pages_count, mem_amount, mem_unit);
}

static unsigned handling_pf;
static void pagefault_handler(uint8_t intr_num, struct x86_pushad *orig_ctx, unsigned long error_code)
{
    (void) intr_num; /* intr_num == X86_INT_PF */

    ++handling_pf;

    switch (handling_pf) {
        case 1:
            break; /* first #PF */

        case 2: /* pagefault while handing a page fault. */
            puts("A page fault occured while handling a page fault!");
            x86_print_registers(orig_ctx, error_code);
            puts("Halting.");
            /* fall through */

        default: /* pagefault while printing #PF message, everything is lost */
            x86_hlt();
    }

#ifdef DEBUG_READ_BEFORE_WRITE
    uint32_t virtual_addr = cr2_read();
    uint64_t pte = x86_get_pte(virtual_addr);
#endif

    if (error_code & PF_EC_I) {
        puts("Page fault while fetching instruction.");
        x86_print_registers(orig_ctx, error_code);
        puts("Halting.");
        x86_hlt();
    }
#ifdef DEBUG_READ_BEFORE_WRITE
    else if ((pte != NO_PTE) && !(pte & PT_P) && (pte & PT_HEAP_BIT)) {
        /* mark as present */
        TEMP_PAGE.indices[(virtual_addr >> 12) % 512] |= PT_P;
        asm volatile ("invlpg (%0)" :: "r"(virtual_addr));

        /* initialize for easier debugging */
        uint32_t *p = (uint32_t *) (virtual_addr & ~0xfff);
        for (unsigned i = 0; i < 0x1000 / 4; ++i) {
            const union {
                char str_value[4];
                uint32_t int_value;
            } debug_init = { .str_value = "RIOT" };
            *p++ = debug_init.int_value;
        }

        /* print a warning if the page was read before written */
        if (!(error_code & PF_EC_W)) {
            unsigned long *sp = (void *) orig_ctx->sp; /* ip, cs, flags */
            printf("DEBUG: Read before write on heap address 0x%08x (physical: 0x%016llx) at 0x%08lx.\n",
                   virtual_addr, pte & PT_ADDR_MASK, sp[0]);
        }
    }
#endif
    else if (error_code & PF_EC_P) {
        printf("Page fault: access violation while %s present page.\n", error_code & PF_EC_W ? "writing to" : "reading from");
        x86_print_registers(orig_ctx, error_code);
        puts("Halting.");
        x86_hlt();
    }
    else {
        printf("Page fault: access violation while %s non-present page.\n", error_code & PF_EC_W ? "writing to" : "reading from");
        x86_print_registers(orig_ctx, error_code);
        puts("Halting.");
        x86_hlt();
    }

    --handling_pf;
}

static void init_pagefault_handler(void)
{
    x86_interrupt_handler_set(X86_INT_PF, &pagefault_handler);
}

void x86_init_memory(void)
{
    check_requirements();

    init_pagetable();
    init_pagefault_handler();
    init_free_pages();

    puts("Virtual memory initialized");
}

uint64_t x86_get_pte(uint32_t addr)
{
    addr >>= 12;
    unsigned pte_i = addr % 512;
    addr >>= 9;
    unsigned pt_i = addr % 512;
    addr >>= 9;
    unsigned pd_i = addr;

    if (pdpt[pd_i] & PT_P) {
        set_temp_page(pdpt[pd_i] & PT_ADDR_MASK);
        if (TEMP_PAGE.indices[pt_i] & PT_P) {
            set_temp_page(TEMP_PAGE.indices[pt_i] & PT_ADDR_MASK);
            return TEMP_PAGE.indices[pte_i];
        }
    }
    return NO_PTE;
}

static void virtual_pages_set_bits(uint32_t virtual_addr, unsigned pages, uint64_t bits)
{
    while (pages-- > 0) {
        unsigned pte_i = (virtual_addr >> 12) % 512;

        uint64_t old_physical_addr = x86_get_pte(virtual_addr) & PT_ADDR_MASK;
        TEMP_PAGE.indices[pte_i] = old_physical_addr | bits;
        asm volatile ("invlpg (%0)" :: "r"(virtual_addr));

        virtual_addr += 0x1000;
    }
}

void *x86_map_physical_pages(uint64_t physical_start, unsigned pages, uint64_t bits)
{
    if (bits & PT_XD) {
        bits &= ~PT_XD;
        bits |= pt_xd;
    }

    /* We use an already set up space, so we are sure that the upper level page tables are allocated. */
    /* We cut out a slice and re-add the physical pages. */
    char *result = memalign(0x1000, pages * 0x1000);
    if (!result) {
        return NULL;
    }

    for (unsigned page = 0; page < pages; ++page) {
        uint64_t physical_addr = physical_start + page * 0x1000;
        uint32_t virtual_addr = (uintptr_t) result + page * 0x1000;
        unsigned pte_i = (virtual_addr >> 12) % 512;

        uint64_t old_pte = x86_get_pte(virtual_addr);
        TEMP_PAGE.indices[pte_i] = physical_addr | bits;

        if (page == 0) {
            uint64_t old_physical_addr = old_pte & PT_ADDR_MASK;

            /* FIXME: does this work? Won't work if TLSF joins different buffers. */
            add_pages_to_pool(old_physical_addr, old_physical_addr + 0x1000 * pages);
        }
    }

    cr3_write((uint32_t) &pdpt | PT_CR3_BITS); /* flush tlb */
    return result;
}

void *x86_get_virtual_pages(unsigned pages, uint64_t bits)
{
    if (bits & PT_XD) {
        bits &= ~PT_XD;
        bits |= pt_xd;
    }

    char *result = memalign(0x1000, pages * 0x1000);
    if (!result) {
        return (void *) -1ul;
    }

    virtual_pages_set_bits((uintptr_t) result, pages, bits);
    return result;
}

void x86_release_virtual_pages(uint32_t virtual_start, unsigned pages)
{
    virtual_pages_set_bits(virtual_start, pages, PT_HEAP_BITS);
    free((void *) virtual_start);
}
