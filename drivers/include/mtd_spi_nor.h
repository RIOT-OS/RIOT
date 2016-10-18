/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mtd_spi_nor Serial NOR flash
 * @ingroup     drivers_memory
 * @brief       Driver for serial NOR flash memory technology devices attached via SPI
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the serial flash memory driver
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef MTD_SPI_NOR_H_
#define MTD_SPI_NOR_H_

#include <stdint.h>
#include <stdbool.h>
#include "periph_conf.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include "mtd.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief SPI NOR flash opcode table
 */
typedef struct {
    uint8_t rdid;         /**< Read identification (JEDEC ID) */
    uint8_t wren;         /**< Write enable */
    uint8_t rdsr;         /**< Read status register */
    uint8_t wrsr;         /**< Write status register */
    uint8_t read;         /**< Read data bytes, 3 byte address */
    uint8_t read_fast;    /**< Read data bytes, 3 byte address, at higher speed */
    uint8_t page_program; /**< Page program */
    uint8_t sector_erase; /**< Block erase 4 KiB */
    uint8_t chip_erase;   /**< Chip erase */
    uint8_t sleep;        /**< Deep power down */
    uint8_t wake;         /**< Release from deep power down */
    /* TODO: enter 4 byte address mode for large memories */
} mtd_spi_nor_opcode_t;

/**
 * @brief Internal representation of JEDEC memory ID codes.
 *
 * @see http://www.jedec.org/standards-documents/results/jep106
 */
typedef struct __attribute__((packed)) {
    uint8_t bank;   /**< Manufacturer ID bank number, 1 through 10, see JEP106 */
    uint8_t manuf;  /**< Manufacturer ID, 1 byte */
    uint8_t device[2]; /**< Device ID, 2 bytes */
} mtd_jedec_id_t;

/**
 * @brief Byte to signal increment bank number when reading manufacturer ID
 *
 * @see http://www.jedec.org/standards-documents/results/jep106
 */
#define JEDEC_NEXT_BANK (0x7f)

/**
 * @brief Device descriptor for serial flash memory devices
 *
 * This is an extension of the @c mtd_dev_t struct
 */
typedef struct {
    mtd_dev_t base;          /**< inherit from mtd_dev_t object */
    const mtd_spi_nor_opcode_t *opcode; /**< Opcode table for the device */
    spi_t spi;               /**< SPI bus the device is connected to */
    gpio_t cs;               /**< CS pin GPIO handle */
    mtd_jedec_id_t jedec_id; /**< JEDEC ID of the chip */
    /**
     * @brief bitmask to corresponding to the page address
     *
     * Computed by mtd_spi_nor_init, no need to touch outside the driver.
     */
    uint32_t page_addr_mask;
    /**
     * @brief bitmask to corresponding to the sector address
     *
     * Computed by mtd_spi_nor_init, no need to touch outside the driver.
     */
    uint32_t sec_addr_mask;
    uint8_t addr_width;      /**< Number of bytes in addresses, usually 3 for small devices */
    /**
     * @brief number of right shifts to get the address to the start of the page
     *
     * Computed by mtd_spi_nor_init, no need to touch outside the driver.
     */
    uint8_t page_addr_shift;
    /**
     * @brief number of right shifts to get the address to the start of the sector
     *
     * Computed by mtd_spi_nor_init, no need to touch outside the driver.
     */
    uint8_t sec_addr_shift;
} mtd_spi_nor_t;

/**
 * @brief NOR flash SPI MTD device operations table
 */
extern const mtd_desc_t mtd_spi_nor_driver;

/* Available opcode tables for known devices */
/* Defined in mtd_spi_nor_configs.c */
/**
 * @brief Default command opcodes
 *
 * The numbers were taken from Micron M25P16, but the same opcodes can
 * be found in Macronix MX25L25735E, and multiple other data sheets for
 * different devices, as well as in the Linux kernel, so they seem quite
 * sensible for default values. */
extern const mtd_spi_nor_opcode_t mtd_spi_nor_opcode_default;

#ifdef __cplusplus
}
#endif

#endif /* ADT7310_H_ */
/** @} */
