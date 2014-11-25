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
 * The virtual memory management of x86 boards.
 *
 * @ingroup x86
 * @{
 * @file
 * @author  René Kijewski <rene.kijewski@fu-berlin.de>
 */

#ifndef CPU__X86__MEMORY__H__
#define CPU__X86__MEMORY__H__

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum static size of the kernel.
 *
 * Each page table can hold 2MB.
 * As the low memory is ignored, 1MB of virtual memory is wasted.
 * Bad things will happen if this value is set too small.
 * The kernel must not exceed the 10MB mark, unless you are sure that your board has no ISA hole.
 * (It is unlikely that you have an ISA hole, though.)
 */
#define X86_NUM_STATIC_PT (4)
#define X86_NUM_STATIC_PD ((X86_NUM_STATIC_PT+511) / 512) /* 1GB=512 PTs each */

/**
 * @brief   Initialize virtual memory management.
 *
 * This function is called during initialization by x86_startup().
 * You must not call this function on your own accord.
 */
void x86_init_memory(void);

/**
 * @brief   Datum for the Global Descriptor Table Register.
 */
struct gdtr_t {
    uint16_t size_minus_one;
    uint32_t offset;
} __attribute__((packed));

/**
 * @brief   One entry in the Global Descriptor Table.
 */
struct gdt_entry {
    uint16_t limit_0_15;
    uint16_t base_0_15;
    uint8_t base_16_23;
    uint8_t access_byte;
    uint8_t limit_16_19_and_flags;
    uint8_t base_24_31;
} __attribute__((packed));

#define GDT_AB_AC     (1u << 0) /**< Access byte (set to one by CPU when accessed) */
#define GDT_AB_RW     (1u << 1) /**< If CS: read access allowed. If DS: write access allowed. */
#define GDT_AB_DC     (1u << 2) /**< Direction bit */
#define GDT_AB_EX     (1u << 3) /**< 1 = segment is CS */
#define GDT_AB_S      (1u << 4) /**< 1 = segment is CS or DS; 0 = segment is gate or TSS */
#define GDT_AB_RING_0 (0u << 5) /**< Privilege level: ring 0 */
#define GDT_AB_RING_1 (1u << 5) /**< Privilege level: ring 1 */
#define GDT_AB_RING_2 (2u << 5) /**< Privilege level: ring 2 */
#define GDT_AB_RING_3 (3u << 5) /**< Privilege level: ring 3 */
#define GDT_AB_PR     (1u << 7) /**< Segment present */

#define GDT_FLAG_USR4 (1u << 4) /**< bit for free use */
#define GDT_FLAG_L    (1u << 5) /**< 1 = long mode */
#define GDT_FLAG_SZ   (1u << 6) /**< 0 = 16/64 bit selector; 1 = 32 bit selector */
#define GDT_FLAG_GR   (1u << 7) /**< 0 = limit given in 1 b blocks; 1 = limit given in 4k blocks */

typedef uint64_t pae_page_directory_pointer_table_t[512] __attribute__((aligned(0x1000)));
typedef uint64_t pae_page_directory_t[512] __attribute__((aligned(0x1000)));
typedef uint64_t pae_page_table_t[512] __attribute__((aligned(0x1000)));

#define PT_P     (1ull << 0)  /**< 1 = page present */
#define PT_RW    (1ull << 1)  /**< 1 = page writable */
#define PT_US    (1ull << 2)  /**< 1 = don't restrict to ring 0 */
#define PT_PWT   (1ull << 3)  /**< 1 = use write-through; 0 = use write-back */
#define PT_PCD   (1ull << 4)  /**< 1 = disable caching */
#define PT_A     (1ull << 5)  /**< 1 = page was accessed (set by the CPU, used for swapping) */
#define PT_D     (1ull << 6)  /**< 1 = dirty (set by the CPU when written to) */
#define PT_S     (1ull << 7)  /**< 1 = 4MB pages */
#define PT_PAT   (1ull << 7)  /**< ? */
#define PT_G     (1ull << 8)  /**< 1 = global (ignored for page directory entries) */
#define PT_USR9  (1ull << 9)  /**< bit for free use */
#define PT_USR10 (1ull << 10) /**< bit for free use */
#define PT_USR11 (1ull << 11) /**< bit for free use */
#define PT_XD    (1ull << 63) /**< 1 = no execute */
#define PT_ADDR_MASK (((1ull << 48) - 1) & ~((1ull << 12) - 1))

#ifdef DEBUG_READ_BEFORE_WRITE
#   define PT_HEAP_BIT PT_USR9
#endif

#define PF_EC_P (1u << 0) /**< 1 = page protection violation; 0 = page not present */
#define PF_EC_W (1u << 1) /**< 1 = accessed writingly; 0 = readingly */
#define PF_EC_U (1u << 2) /**< 1 = ring 3 */
#define PF_EC_R (1u << 3) /**< ? */
#define PF_EC_I (1u << 4) /**< 1 = exception in CS; 0 = exception in DS */

/**
 * @brief   Error value for x86_get_pte().
 *
 * All bits are set but PT_P.
 * Most likely you are interested only if the page table is present.
 * Since the PTE bits 48 to 62 are restricted, this value cannot occur in an actual PTE.
 */
#define NO_PTE (~PT_P)
/**
 * @brief     Read page table entry for addr.
 * @param     addr   Does not have to be aligned.
 * @returns   PTE iff addr has an entry in the PDPT and PD, otherwise NO_PTE.
 */
uint64_t x86_get_pte(uint32_t addr);

/**
 * @brief     Maps a range of physical pages at a new virtual address.
 * @param     physical_start   Start address of the physical page. Must be aligned.
 * @param     pages            Number of physical pages.
 * @param     bits             Flags of the PTE for each page (do not forget PT_P).
 * @returns   Start of the virtual address range, aligned of course.
 * @returns   NULL if the memory was exhausted.
 *
 * This function is to be used by hardware drivers.
 */
void *x86_map_physical_pages(uint64_t physical_start, unsigned pages, uint64_t bits);

/**
 * @brief     Allocate a range of pages.
 * @param     pages   Number of pages to allocate.
 * @param     bits    Flags of the PTE for each page (do not forget PT_P).
 * @returns   Start of the virtual address range, aligned of course.
 *            Use x86_get_pte() to determine the start of the physical address range.
 */
void *x86_get_virtual_pages(unsigned pages, uint64_t bits);

/**
 * @brief   Deallocate a range of bytes that were allocated with x86_get_virtual_pages()
 * @param   virtual_start   The return value of x86_get_virtual_pages().
 * @param   pages           Must match the original call to x86_get_virtual_pages().
 */
void x86_release_virtual_pages(uint32_t virtual_start, unsigned pages);

/**
 * @brief     Intializes the global descriptor table.
 * @details   This should be the very first call in the startup routine.
 *            Changing the segmentation confuses the JTAG.
 * @details   0x0000 is an invalid descriptor.
 *            0x0008 is the code segment, from 0x00000000 to 0xFFFFFFFF, executable, read-only.
 *            0x0010 is the data segment, from 0x00000000 to 0xFFFFFFFF, non-executable, writable.
 */
void x86_init_gdt(void);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
