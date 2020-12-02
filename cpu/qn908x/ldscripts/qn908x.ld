/*
 * Copyright (C) 2020 iosabi
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_qn908x
 * @{
 *
 * @file
 * @brief           Sections definitions for the NXP QN908x MCUs
 *
 * @author          iosabi <iosabi@protonmail.com>
 *
 * This linker script organizes the flash headers to generate a "Legacy" image
 * as described in the "Boot Process" section of the QN908x user manual. A
 * legacy image contains an "Image vector table" which is the standard ARM
 * vector table with some special values in the reserved fields. In particular,
 * this needs a to have a valid checksum at address 0x1c to be considered a
 * valid image by the bootloader, which is not set by the build process.
 *
 * @}
 */
OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)

_vectors_length = 0x114;

/* The Flash lock and protect descriptor occupies the last flash page of 0x800
 * bytes. See "Flash lock and protection" protection section. */
_flash_lock_length = 0x800;

INCLUDE cortexm_rom_offset.ld

MEMORY
{
    /* Note: What we call "rom" here is the flash region for consistency with
     * the rest of the RIOT build system naming. There is a 256 kB ROM memory in
     * the QN908x holding the bootloader and Bluetooth stack that can't be
     * modified by the user.
     */
    vectors    : ORIGIN = _rom_start_addr + _rom_offset, LENGTH = _vectors_length
    rom (rx)   : ORIGIN = _rom_start_addr + _rom_offset + _vectors_length, LENGTH = _fw_rom_length - _vectors_length - _flash_lock_length
    ram (!rx)  : ORIGIN = _ram_start_addr, LENGTH = _ram_length
}

SECTIONS
{
    /* "Image vector table" 0x000-0x114, defined in the "Boot process" section,
     * must have exactly this size, otherwise we configured something wrong.
     */
    .vectors :
    {
        PROVIDE(_isr_vectors = .);
        KEEP(*(SORT(.vector*)))
    } > vectors
    ASSERT (SIZEOF(.vectors) == _vectors_length,
            "Image vector table size mismatch.")
    ASSERT (ADDR(.vectors) == _rom_start_addr + _rom_offset,
            "Image vector table must start at the beginning of the flash")
    ASSERT (LOADADDR(.vectors) == _rom_start_addr + _rom_offset,
            "Image vector table must start at the beginning of the flash")

}

INCLUDE cortexm_base.ld
