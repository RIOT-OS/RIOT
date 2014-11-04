/* multiboot.h - the header for Multiboot */
/* Copyright (C) 1999, 2001  Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/**
 * Architecture specific definitions for multiboot enabled kernels.
 *
 * @ingroup x86-multiboot
 * @{
 * @file
 */

#ifdef __cplusplus
extern "C" {
#endif

#define MULTIBOOT_HEADER_MAGIC 0x1BADB002 /**< The magic number for the Multiboot header. */
#define MULTIBOOT_HEADER_FLAGS 0x00010003 /**< The flags for the Multiboot header. */
#define MULTIBOOT_HEADER_CHECKSUM (-(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)) /**< The checksum for the Multiboot header.  */
#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002 /**< The magic number passed by a Multiboot-compliant boot loader. */

/**
 * @brief   The Multiboot header.
 *
 * A multiboot enabled kernel needs to contain this header in the first 8kB of the binary file.
 */
typedef struct multiboot_header {
    unsigned long magic;         /**< Set to MULTIBOOT_HEADER_MAGIC. */
    unsigned long flags;         /**< Set to MULTIBOOT_HEADER_FLAGS. */
    unsigned long checksum;      /**< Set to MULTIBOOT_HEADER_CHECKSUM. */
    unsigned long header_addr;   /**< The address of the datum. */
    unsigned long load_addr;     /**< Set to `(uintptr_t) &_kernel_memory_start`. */
    unsigned long load_end_addr; /**< Set to `(uintptr_t) &_section_data_end`. */
    unsigned long bss_end_addr;  /**< Set to `(uintptr_t) &_section_bss_end` */
    unsigned long entry_addr;    /**< Your entry function. */
} multiboot_header_t;

/**
 * @brief   The symbol table for a.out.
 *
 * Do not use!
 */
typedef struct aout_symbol_table {
    unsigned long tabsize;
    unsigned long strsize;
    unsigned long addr;
    unsigned long reserved;
} aout_symbol_table_t;

/**
 * @brief   The section header table for ELF.
 *
 * Do not use!
 */
typedef struct elf_section_header_table {
    unsigned long num;
    unsigned long size;
    unsigned long addr;
    unsigned long shndx;
} elf_section_header_table_t;

/**
 * @brief   The multiboot information provided by the bootloader (e.g. Grub).
 */
typedef struct multiboot_info {
    unsigned long flags;
    unsigned long mem_lower;
    unsigned long mem_upper;
    unsigned long boot_device;
    unsigned long cmdline;
    unsigned long mods_count;
    unsigned long mods_addr;
    union {
        aout_symbol_table_t aout_sym;
        elf_section_header_table_t elf_sec;
    } u;
    unsigned long mmap_length;
    unsigned long mmap_addr;
} multiboot_info_t;

/**
 * @brief   The module structure.
 *
 * Do not use!
 */
typedef struct module {
    unsigned long mod_start;
    unsigned long mod_end;
    unsigned long string;
    unsigned long reserved;
} module_t;

/**
 * @brief   An entry in the memory map table.
 */
typedef struct memory_map {
    unsigned long size;
    unsigned long base_addr_low;
    unsigned long base_addr_high;
    unsigned long length_low;
    unsigned long length_high;
    unsigned long type;
} memory_map_t;

enum multiboot_info_flags {
    MULTIBOOT_INFO_FLAGS_MEM     = 0x01,
    MULTIBOOT_INFO_FLAGS_BOOT    = 0x02,
    MULTIBOOT_INFO_FLAGS_CMDLINE = 0x04,
    MULTIBOOT_INFO_FLAGS_MODS    = 0x08,
    MULTIBOOT_INFO_FLAGS_SYMS1   = 0x10,
    MULTIBOOT_INFO_FLAGS_SYMS2   = 0x20,
    MULTIBOOT_INFO_FLAGS_MMAP    = 0x40,
};

#ifdef __cplusplus
}
#endif

/** @} */
