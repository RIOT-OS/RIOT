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

INCLUDE cortexm_rom_offset.ld

_cca_length = 44;

/* Memory Space Definitions: */
MEMORY
{
    rom (rx)    : ORIGIN = _rom_start_addr + _rom_offset, LENGTH = _fw_rom_length - _cca_length
    cca         : ORIGIN = 0x0027ffd4, LENGTH = _cca_length
    sram0       : ORIGIN = 0x20000000, LENGTH = 16K /* Lost in PM2 and PM3 */
    sram1       : ORIGIN = 0x20004000, LENGTH = 16K
    ram (w!rx)  : ORIGIN = _ram_start_addr, LENGTH = _ram_length
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
