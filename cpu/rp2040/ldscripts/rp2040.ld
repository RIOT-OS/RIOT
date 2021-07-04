/*
 * Copyright (C) 2021 Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup cpu_rp2040
 * @{
 *
 * @file rp2040.ld
 * @brief Linker script
 *
 * @author Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 *
 * @}
 */

INCLUDE cortexm_rom_offset.ld

MEMORY {
    ram(rwx): ORIGIN = _ram_start_addr, LENGTH = _ram_length
    rom(rx): ORIGIN = _rom_start_addr, LENGTH = _fw_rom_length
}

SECTIONS {
    /*
     * The second stage bootloader is prepended to the flash image.
     *
     * This bootloader must be 256 bytes, padded and checksummed.
     * The last 4 bytes containing the checksum and everything residing
     * within this 256 bytes.
     *
     * It is built by: rp2040/include/vendor/boot_stage2/Makefile.
     */

    .boot2 : {
        __boot2_start__ = .;
        KEEP (*(.boot2))
        __boot2_end__ = .;
    } > rom

    ASSERT(
        __boot2_end__ - __boot2_start__ == 256,
        "ERROR: Raspberry Pi Pico second stage bootloader must be 256 bytes in size"
    )
}

INCLUDE cortexm_base.ld
