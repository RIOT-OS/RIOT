/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mtd_spi_nor_sfdp Serial Flash Discoverable Parameters
 * @ingroup     drivers_mtd_spi_nor
 * @brief       Driver for serial NOR flash memory technology devices attached via SPI
 *
 * @{
 *
 * @file
 * @brief       Serial flash discoverable parameter definitions for SPI NOR
 *              Flash devices
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef MTD_SPI_NOR_SFDP_H
#define MTD_SPI_NOR_SFDP_H

#include <inttypes.h>
#include <stddef.h>

#include "bitarithm.h"

#ifdef __cplusplus
extern "C" {
#endif

#define JEDEC_SFDP_SIGNATURE_VAL (0x50444653) /**< "SFDP" signature */

/**
 * @brief   Internal representation of JEDEC serial flash discoverable parameter
 *          table header
 *
 * @see     JEDEC JESD216D
 */
typedef struct __attribute__((packed)) {
    uint32_t signature;     /**< Magic number, always 0x50444653 */
    uint8_t minor;          /**<< Minor version number */
    uint8_t major;          /**< Major version number */
    uint8_t num_headers;    /**< number of parameter headers, 0-based */
    uint8_t protocol;       /**< Access protocol */
} mtd_jedec_sfdp_header_t;

/**
 * @name    SFDP parameter table ID codes
 * @{
 */
#define JEDEC_SFDP_PARAM_TABLE_BASIC_SPI    0xFF00 /**< Basic SPI protocol */
#define JEDEC_SFDP_PARAM_TABLE_4_BYTE_ADDR  0xFF84 /**< Basic SPI protocol */
/** @} */

/**
 * @brief   Internal representation of JEDEC serial flash discoverable parameter
 *          table parameter header
 */
typedef struct __attribute__((packed)) {
    uint8_t id_lsb;         /**< LSB of the parameter ID */
    uint8_t minor;          /**< Parameter table minor version */
    uint8_t major;          /**< Parameter table major version */
    uint8_t length;         /**< Length of the parameter table in 32 bit words*/
    uint32_t table_ptr:24;  /**< Parameter table pointer */
    uint8_t id_msb;         /**< MSB of the parameter ID */
} mtd_jedec_param_header_t;

/**
 * @name    SFDP Basic SPI protocol content offsets in bytes
 * @{
 */
#define JEDEC_SFDP_BASIC_SPI_FLAGS          0x00 /**< Access configuration flags */
#define JEDEC_SFDP_BASIC_SPI_MEM_DENSITY    0x04 /**< Memory density */
#define JEDEC_SFDP_BASIC_SPI_144_114_ACCESS 0x08 /**< 1-4-4 and 1-1-4 access */
#define JEDEC_SFDP_BASIC_SPI_ERASE_12_INFO  0x1C /**< Erase type 1 and 2 info */
#define JEDEC_SFDP_BASIC_SPI_ERASE_34_INFO  0x20 /**< Erase type 3 and 4 info */
#define JEDEC_SFDP_BASIC_SPI_ERASE_TIMINGS  0x24 /**< Erase timings */
#define JEDEC_SFDP_BASIC_SPI_OTHER_TIMINGS  0x28 /**< Chip erase and write timings */
#define JEDEC_SFDP_BASIC_SPI_POWERDOWN_INFO 0x34 /**< Power down support */
/** @} */

/**
 * @name JEDEC serial flash discoverable parameter table basic SPI first word
 * content
 * @{
 */
#define JEDEC_SFDP_PARAM_SUP_114_FAST_READ   (BIT22) /**< 1-1-4 Fast read */
#define JEDEC_SFDP_PARAM_SUP_144_FAST_READ   (BIT21) /**< 1-4-4 Fast read */
#define JEDEC_SFDP_PARAM_SUP_122_FAST_READ   (BIT20) /**< 1-2-2 Fast read */
#define JEDEC_SFDP_PARAM_SUP_DTR_CLOCKING    (BIT19) /**< DTR Clocking */
#define JEDEC_SFDP_PARAM_SUP_ADDRESS_4_ONLY  (BIT18) /**< Only 4 byte addressing */
#define JEDEC_SFDP_PARAM_SUP_ADDRESS_BOTH    (BIT18 | BIT17) /**< Both address types supported */
#define JEDEC_SFDP_PARAM_SUP_ADDRESS_3_ONLY  (BIT17) /**< 3 Byte only */
#define JEDEC_SFDP_PARAM_SUP_112_FAST_READ   (BIT16) /**< 1-1-2 fast read */
#define JEDEC_SFDP_PARAM_SUP_WREN_REQ_STATUS (BIT4) /**< WREN required for status writing */
/** @} */

/**
 * @brief   Internal representation of JEDEC serial flash discoverable parameter
 *          table basic SPI erase commands, sizes and timings
 *
 * Spans word 9 through 11 of the JEDEC SFDP Basic SPI table
 */
typedef struct __attribute__((packed)) {
    struct __attribute__((packed)) {
        uint8_t size;           /**< Power of 2 erase size */
        uint8_t instruction;    /**< Instruction used to erase this size */
    } erase[4]; /**< Erase sizes and instructions supported by the device */

    union {
        struct __attribute__((packed)) {
            uint8_t erase_mult:4;       /**< Max erase multiplier for all erase types */
            uint8_t erase1_time:7;      /**< Typical erase time for type 1 erase */
            uint8_t erase2_time:7;      /**< Typical erase time for type 2 erase */
            uint8_t erase3_time:7;      /**< Typical erase time for type 3 erase */
            uint8_t erase4_time:7;      /**< Typical erase time for type 4 erase */
        };
        uint32_t erase_time;            /**< Erase time as uint32_t */
    };

    uint8_t program_mult:4;     /**< Max program multiplier for all program types */
    uint8_t page_size:4;        /**< Power of 2 page size */
    uint8_t page_time:6;        /**< Typical time for writing a full page */
    uint8_t first_byte_time:5;  /**< Typical program time for first byte */
    uint8_t add_byte_time:5;    /**< Typical program time for additional bytes */
    uint8_t chip_erase_time:7;  /**< Typical erase time for chip erase */
} mtd_jedec_basic_spi_access_info_t;

/**
 * @brief   Internal representation of JEDEC serial flash discoverable parameter
 *          table basic SPI power down information
 *
 * Word 14 of the JEDEC SFDP Basic SPI table
 */
typedef struct __attribute__((packed)) {
    uint8_t reserved:2;     /**< Reserved bits */
    uint8_t busy_reg:6;     /**< Busy flag in the status register */
    uint8_t delay:7;        /**< Exit powerdown to next operation delay */
    uint8_t exit;           /**< Exit Deep powerdown mode instruction */
    uint8_t enter;          /**< Enter Deep powerdown mode instruction */
    uint8_t supported:1;    /**< Deep powerdown supported flag */
} mtd_jedec_basic_spi_powerdown_info_t;

#ifdef __cplusplus
}
#endif

#endif /* MTD_SPI_NOR_SFDP_H */
/** @} */
