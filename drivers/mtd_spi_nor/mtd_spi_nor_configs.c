/*
 * SPDX-FileCopyrightText: 2016 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_mtd_spi_nor
 * @{
 *
 * @file
 * @brief       Configurations for some known serial flash memory devices
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#include <stdint.h>
#include "mtd_spi_nor.h"

/* Define opcode tables for SPI NOR flash memory devices here. */

/* Linker garbage collection (gcc -fdata-sections -Wl,--gc-sections) should ensure
 * that only the tables that are actually used by the application will take up
 * space in the .rodata section in program ROM. */

const mtd_spi_nor_opcode_t mtd_spi_nor_opcode_default = {
    .rdid            = 0x9f,
    .wren            = 0x06,
    .rdsr            = 0x05,
    .wrsr            = 0x01,
    .read            = 0x03,
    .read_fast       = 0x0b,
    .page_program    = 0x02,
    .sector_erase    = 0x20,
    .block_erase_32k = 0x52,
    .block_erase_64k = 0xd8,
    .chip_erase      = 0xc7,
    .sleep           = 0xb9,
    .wake            = 0xab,
};

const mtd_spi_nor_opcode_t mtd_spi_nor_opcode_default_4bytes = {
    .rdid            = 0x9f,
    .wren            = 0x06,
    .rdsr            = 0x05,
    .wrsr            = 0x01,
    .read            = 0x13,
    .read_fast       = 0x0c,
    .page_program    = 0x12,
    .sector_erase    = 0x21,
    .block_erase_32k = 0x5c,
    .block_erase_64k = 0xdc,
    .chip_erase      = 0xc7,
    .sleep           = 0xb9,
    .wake            = 0xab,
};

/** @} */
