/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_cc2538
 * @{
 *
 * @file
 * @brief           Linker script for the CC2538SF53
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */
 
_rom_offset = DEFINED( _rom_offset ) ? _rom_offset : 0x0;
_fw_rom_length = DEFINED( _fw_rom_length ) ? _fw_rom_length : _rom_length - _rom_offset;

ASSERT((_fw_rom_length <= _rom_length - _rom_offset), "Specified firmware size does not fit in ROM");

/* Memory Space Definitions: */
MEMORY
{
    rom (rx)    : ORIGIN = _rom_start_addr + _rom_offset, LENGTH = _fw_rom_length
    cca         : ORIGIN = 0x0027ffd4, LENGTH = 44
    ram (w!rx)  : ORIGIN = _ram_start_addr,               LENGTH = _ram_length
}

/* MCU Specific Section Definitions */
SECTIONS
{
    .flashcca :
    {
        KEEP(*(.flashcca))
    } > cca
}

INCLUDE cortexm_base.ld

/* @} */
