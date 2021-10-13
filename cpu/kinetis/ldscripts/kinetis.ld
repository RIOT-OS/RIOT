/*
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 * Copyright (C) 2015-2017 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_kinetis
 * @{
 *
 * @file
 * @brief           Sections definitions for the Freescale Kinetis MCUs
 *
 * @author          Johann Fischer <j.fischer@phytec.de>
 * @author          Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */
OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)

_vectors_length = 0x400;
_flashsec_length = 0x10;

INCLUDE cortexm_rom_offset.ld

_flash_sec_offset = _vectors_length + _rom_offset;

MEMORY
{
    vectors    : ORIGIN = _rom_start_addr + _rom_offset, LENGTH = _vectors_length
    flashsec   : ORIGIN = _rom_start_addr + _vectors_length + _rom_offset, LENGTH = _flashsec_length
    rom (rx)   : ORIGIN = _rom_start_addr + _vectors_length + _flashsec_length + _rom_offset, LENGTH = _fw_rom_length - (_vectors_length + _flashsec_length)
    ram (!rx)  : ORIGIN = _ram_start_addr, LENGTH = _ram_length
}

SECTIONS
{
    /* Interrupt vectors 0x00-0x3ff. */
    .vector :
    {
        _isr_vectors = .;
        KEEP(*(SORT(.vector*)))
    } > vectors
    ASSERT (SIZEOF(.vector) == _vectors_length, "Interrupt vector table of invalid size.")
    ASSERT (ADDR(.vector) == _rom_offset, "Interrupt vector table at invalid location (linker-script error?)")
    ASSERT (LOADADDR(.vector) == _rom_offset, "Interrupt vector table at invalid location (linker-script error?)")

    /* Flash configuration field, very important in order to not accidentally lock the device */
    /* Flash configuration field 0x400-0x40f. */
    .fcfield :
    {
        . = ALIGN(4);
        KEEP(*(.fcfield))
        . = ALIGN(4);
    } > flashsec
    ASSERT (SIZEOF(.fcfield) == _flashsec_length, "Flash configuration field of invalid size (linker-script error?)")
    ASSERT (ADDR(.fcfield) == _flash_sec_offset, "Flash configuration field at invalid position (linker-script error?)")
    ASSERT (LOADADDR(.fcfield) == _flash_sec_offset, "Flash configuration field at invalid position (linker-script error?)")
}

INCLUDE cortexm_base.ld
