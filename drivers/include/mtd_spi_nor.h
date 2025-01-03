/*
 * Copyright (C) 2016 Eistec AB
 *               2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mtd_spi_nor Serial NOR flash
 * @ingroup     drivers_storage
 * @brief       Driver for serial NOR flash memory technology devices attached via SPI
 *
 *
 * @section mtd_spi_nor_overview Overview
 * Various manufacturers such as ISSI or Macronix offer small SPI NOR Flash which usually
 * come in 8-pin packages and are used for persistent data storage.
 * This driver adds support for these devices with support for RIOT's MTD subsystem.
 *
 * @section mtd_spi_nor_usage Usage
 *
 * The basic functions of all flash devices can be used by just including the
 * mtd_spi_nor module.
 *
 *      USEMODULE += mtd_spi_nor
 *
 * For ISSI and Macronix devices, some data integrity features are provided
 * to check if program or erase operations were successful.
 * These features can be activated by specifying the "SPI_NOR_F_CHECK_INTEGRITY" flag in
 * the "flag" parameter of the mtd_spi_nor_params_t structure.
 *
 * \n
 * Some examples of how to work with the MTD SPI NOR driver can be found in the test for the
 * driver or in some board definitions such as for the nRF52840DK.
 *
 * @{
 * @file
 * @brief       Interface definition for the serial flash memory driver
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Vincent Dupont <vincent@otakeys.com>
 */

#ifndef MTD_SPI_NOR_H
#define MTD_SPI_NOR_H

#include <stdint.h>

#include "periph_conf.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include "mtd.h"
#include "kernel_defines.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   SPI NOR flash opcode table
 *
 * Manufacturer specific opcodes have a short form of the manufacturer as a prefix,
 * for example "MX" for "Macronix".
 */
typedef struct {
    uint8_t rdid;            /**< Read identification (JEDEC ID) */
    uint8_t wren;            /**< Write enable */
    uint8_t rdsr;            /**< Read status register */
    uint8_t wrsr;            /**< Write status register */
    uint8_t read;            /**< Read data bytes, 3 byte address */
    uint8_t read_fast;       /**< Read data bytes, 3 byte address, at higher speed */
    uint8_t page_program;    /**< Page program */
    uint8_t sector_erase;    /**< Block erase 4 KiB */
    uint8_t block_erase_32k; /**< 32KiB block erase */
    uint8_t block_erase_64k; /**< Block erase (usually 64 KiB) */
    uint8_t chip_erase;      /**< Chip erase */
    uint8_t sleep;           /**< Deep power down */
    uint8_t wake;            /**< Release from deep power down */
    uint8_t mx_rdscur;       /**< Read security register (Macronix specific) */
    uint8_t issi_rderp;      /**< Read extended read parameters register (ISSI specific) */
    uint8_t issi_clerp;      /**< Clear extended read parameters register (ISSI specific) */
    uint8_t issi_wrdi;       /**< Write disable (ISSI specific) */
} mtd_spi_nor_opcode_t;

/**
 * @brief   Internal representation of JEDEC memory ID codes.
 *
 * @see http://www.jedec.org/standards-documents/results/jep106
 */
typedef struct __attribute__((packed)) {
    uint8_t bank;      /**< Manufacturer ID bank number, 1 through 10, see JEP106 */
    uint8_t manuf;     /**< Manufacturer ID, 1 byte */
    uint8_t device[2]; /**< Device ID, 2 bytes */
} mtd_jedec_id_t;

/**
 * @brief   Byte to signal increment bank number when reading manufacturer ID
 *
 * @see http://www.jedec.org/standards-documents/results/jep106
 */
#define JEDEC_NEXT_BANK (0x7f)

/**
 * @brief   The highest possible bank number when reading manufacturer ID
 *
 * @see http://www.jedec.org/standards-documents/results/jep106
 */
#define JEDEC_BANK_MAX  (10)

/**
 * @brief   Flag to set when the device support 4KiB sector erase (sector_erase opcode)
 */
#define SPI_NOR_F_SECT_4K   (1)

/**
 * @brief   Flag to set when the device support 32KiB block erase (block_erase_32k opcode)
 */
#define SPI_NOR_F_SECT_32K  (2)

/**
 * @brief   Flag to set when the device support 64KiB block erase (block_erase_32k opcode)
 */
#define SPI_NOR_F_SECT_64K  (4)

/**
 * @brief Enable data integrity checks after program/erase operations
 * for devices that support it.
 */
#define SPI_NOR_F_CHECK_INTEGRETY   (256)

/**
 * @brief Enable functionality that is specific for Macronix devices.
 */
#define SPI_NOR_F_MANUF_MACRONIX    (0xC2)

/**
 * @brief Enable functionality that is specific for ISSI devices.
 */
#define SPI_NOR_F_MANUF_ISSI    (0x9D)

/**
 * @brief Compile-time parameters for a serial flash device
 */
typedef struct {
    const mtd_spi_nor_opcode_t *opcode; /**< Opcode table for the device */
    uint32_t wait_chip_erase;   /**< Full chip erase wait time in µs */
    uint32_t wait_64k_erase;    /**< 64KB page erase wait time in µs */
    uint32_t wait_32k_erase;    /**< 32KB page erase wait time in µs */
    uint32_t wait_sector_erase; /**< 4KB sector erase wait time in µs */
    uint32_t wait_chip_wake_up; /**< Chip wake up time in µs */
    spi_clk_t clk;           /**< SPI clock */
    uint16_t flag;           /**< Config flags */
    spi_t spi;               /**< SPI bus the device is connected to */
    spi_mode_t mode;         /**< SPI mode */
    gpio_t cs;               /**< CS pin GPIO handle */
    gpio_t wp;               /**< Write Protect pin GPIO handle */
    gpio_t hold;             /**< HOLD pin GPIO handle */
} mtd_spi_nor_params_t;

/**
 * @brief   Device descriptor for serial flash memory devices
 *
 * This is an extension of the @c mtd_dev_t struct
 */
typedef struct {
    mtd_dev_t base;          /**< inherit from mtd_dev_t object */
    const mtd_spi_nor_params_t *params; /**< SPI NOR params */
    mtd_jedec_id_t jedec_id; /**< JEDEC ID of the chip */

    /**
     * @brief   bitmask to corresponding to the page address
     *
     * Computed by mtd_spi_nor_init, no need to touch outside the driver.
     */
    uint32_t page_addr_mask;
    /**
     * @brief   bitmask to corresponding to the sector address
     *
     * Computed by mtd_spi_nor_init, no need to touch outside the driver.
     */
    uint32_t sec_addr_mask;
    /**
     * @brief   number of right shifts to get the address to the start of the page
     *
     * Computed by mtd_spi_nor_init, no need to touch outside the driver.
     */
    uint8_t page_addr_shift;
    /**
     * @brief   number of right shifts to get the address to the start of the sector
     *
     * Computed by mtd_spi_nor_init, no need to touch outside the driver.
     */
    uint8_t sec_addr_shift;
    /**
     * @brief   number of address bytes
     *
     * Computed by mtd_spi_nor_init, no need to touch outside the driver.
     */
    uint8_t addr_width;
} mtd_spi_nor_t;

/**
 * @brief   NOR flash SPI MTD device operations table
 */
extern const mtd_desc_t mtd_spi_nor_driver;

/* Available opcode tables for known devices */
/* Defined in mtd_spi_nor_configs.c */
/**
 * @brief   Default command opcodes
 *
 * The numbers were taken from Micron M25P16, but the same opcodes can
 * be found in Macronix MX25L25735E, and multiple other data sheets for
 * different devices, as well as in the Linux kernel, so they seem quite
 * sensible for default values.
 */
extern const mtd_spi_nor_opcode_t mtd_spi_nor_opcode_default;

/**
 * @brief   Default 4-byte addresses opcodes
 *
 * Commands for 4-byte addresses chips (above 128Mb)
 */
extern const mtd_spi_nor_opcode_t mtd_spi_nor_opcode_default_4bytes;

#ifdef __cplusplus
}
#endif

#endif /* MTD_SPI_NOR_H */
/** @} */
