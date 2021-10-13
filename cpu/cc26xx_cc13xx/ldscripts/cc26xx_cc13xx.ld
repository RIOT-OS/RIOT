/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_cc26x2_cc13x2
 * @{
 *
 * @file
 * @brief           Linker script for the CC26x2/CC13x2 MCUs
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 */

INCLUDE cortexm_rom_offset.ld

/* Memory Space Definitions: */
MEMORY
{
    rom      (rx)   : ORIGIN = _rom_start_addr + _rom_offset, LENGTH = _fw_rom_length
    /* CCFG starts at the end of ROM */
    ccfg     (rx)   : ORIGIN = _rom_length - 88,              LENGTH = 88
    /* GPRAM is only available when cache is disabled. When GPRAM is enabled it
     * is used as a backup RAM at the expense of slower CPU execution time */
    gpram           : ORIGIN = 0x11000000,                    LENGTH = 8K
    ram      (w!rx) : ORIGIN = _ram_start_addr,               LENGTH = _ram_length
}

/* MCU Sepcific Section Definitions */
SECTIONS
{
    .ccfg :
    {
        KEEP(*(.ccfg))
    } > ccfg

    .gpram :
    {
    } > gpram

    .heap_gpram (NOLOAD) : ALIGN(4)
    {
        _sheap1 = . ;
        _eheap1 = ORIGIN(gpram) + LENGTH(gpram);
    } > gpram
}

INCLUDE cortexm_base.ld

/* @} */
