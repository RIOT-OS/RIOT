/*
 * Copyright (C) 2021 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_rp2040
 * @{
 *
 * @file
 * @brief           Memory definitions for the RP2040 MCU
 *
 * @author          Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

INCLUDE cortexm.ld

MEMORY
{
    boot_loader (rx)    : ORIGIN = 0x10000000, LENGTH = 0x100
}

SECTIONS {
    .boot_loader :
    {
        __boot_loader_start__ = .;
        KEEP(*(.boot2));
        __boot_loader_end__ = .;
        KEEP(*(.boot2));
    } > boot_loader

    ASSERT(__boot_loader_end__ - __boot_loader_start__ == 256,
           "ERROR: RP2040 second stage bootloader must be 256 bytes in size")

    /* Reserve 16 KiB of RAM for virtual CMSIS-DAP on-chip debugging adapter
     * running on the second core */
    .pico-debug 0x2003C000:
    {
        . = . + 16K;
    } > ram
}
