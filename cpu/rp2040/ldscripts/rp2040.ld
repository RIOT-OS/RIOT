/*
 * Copyright (C) 2021 Ishraq Ibne Ashraf
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_rp2040
 * @{
 *
 * @file
 * @brief           Sections definitions for the Raspberry Pi rp2040 MCU
 *
 * @author          Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 *
 * @}
 */

INCLUDE cortexm_rom_offset.ld

MEMORY
{
    ram (rwx) : ORIGIN = _ram_start_addr, LENGTH = _ram_length
    rom (rx) : ORIGIN = _rom_start_addr, LENGTH = _fw_rom_length /* XIP flash code. */
}

SECTIONS
{
    /* Second stage bootloader is prepended to the image. It must be 256 bytes big
       and checksummed. It is usually built by the boot_stage2 target
       in the Raspberry Pi Pico SDK
    */

    .boot2 : {
        __boot2_start__ = .;
        KEEP (*(.boot2))
        __boot2_end__ = .;
    } > rom

    ASSERT(
        __boot2_end__ - __boot2_start__ == 256,
        "ERROR: Pico second stage bootloader must be 256 bytes in size"
    )
}

INCLUDE cortexm_base.ld
