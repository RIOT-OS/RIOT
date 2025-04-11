/*
 * Copyright (C) 2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef SDMMC_SDMMC_H
#define SDMMC_SDMMC_H

/**
 * @defgroup    drivers_sdmmc    SDIO/SD/MMC Device API (SDMMC)
 * @ingroup     drivers_periph
 * @ingroup     drivers_storage
 *
 * @experimental This API is experimental and in an early state - expect
 *               changes!
 *
 * # Overview
 *
 * The SDIO/SD/MMC Device API (SDMMC) implements an SD host controller driver
 * that provides a high-level API using a low-level SDIO/SD/MMC peripheral
 * driver for accessing
 *
 * - MultiMediaCards (MMC) and Embedded MultiMediaCards (eMMC)
 * - SD Memory Cards (SD Cards) with Standard Capacity (SDSC),
 *   High Capacity (SDHC) or Extended Capacity (SDXC).
 *
 * In the context of this API, the term _Card_ refers to either
 * - a removable card such as SD Memory Card, SDIO Card, Combo Card
 *   or MMC Card or
 * - an embedded device such as Embedded SD Memory Device, Embedded SDIO device,
 *   or eMMC device.
 *
 * The term _slot_ refers to the interface for a removable card.
 *
 * The SDIO/SD/MMC device API (SDMMC) is divided into two parts:
 *
 * 1. The high-level API that implements the SD Host Controller driver and
 *    allows
 *    - to initialize and identify different types of cards,
 *    - to access them either blockwise or bytewise,
 *    - to get information about the used card, and
 *    - to send single commands or application specific commands to the card.
 *
 * 2. The low-level SDIO/SD/MMC peripheral driver of type @ref sdmmc_driver_t
 *    implements the low-level functions required by the high-level device API.
 *    It has to be implemented for each MCU. The support of the low-level
 *    SDIO/SD/MMC peripheral driver is indicated by the MCU or board by
 *    the `periph_sdmmc` feature.
 *
 * Currently the SDIO/SD/MMC Device API supports the following card types:
 *
 * <center>
 * | Card Type                       | Support | Remark |
 * |:--------------------------------|:---:|------------|
 * | MMC/eMMC in MultiMediaCard mode | yes | |
 * | MMC/eMMC in SPI mode            | no  | |
 * | SD Memory Card in SD mode       | yes | SDSC, SDHC and SDXC with Default or High Speed |
 * | SD Memory Card in SPI mode      | no  | |
 * | SDIO in SD mode                 | no  | |
 * | SDIO in SPI mode                | no  | |
 * </center>
 *
 * # Limitations:
 *
 * - Only one card per SDIO/SD/MMC device is supported.
 * - eMMCs specific features are not supported.
 * - UHS-I, UHS-II and UHS-III are not supported.
 *
 * # Features and Modules
 *
 * A board that uses an MCU with an SDIO/SD/MMC peripheral and has a card
 * slot or device connected has to indicate this as feature `periph_sdmmc`
 * in order to use the SDIO/SD/MMC API or drivers based on it.
 * Furthermore, specific features of the SDIO/SD/MMC peripheral or
 * configurations of the board have to be defined by corresponding features.
 *
 * These are in detail:
 *
 * - <b>`periph_sdmmc`</b> (`HAS_PERIPH_SDMMC` in Kconfig)
 *   indicates that an SDIO/SD/MMC peripheral is present and used by the
 *   board. This feature shall be provided by the board configuration.
 *
 * - <b>`periph_sdmmc_8bit`</b> (`HAS_PERIPH_SDMMC_8BIT` in Kconfig)
 *   indicates that the SDIO/SD/MMC peripheral supports the 8-bit bus width
 *   and at least one component of the board is connected with 8 data lines.
 *   This feature shall be provided by the board configuration, if available.
 *
 * - <b>`periph_sdmmc_auto_clk`</b> (`HAS_PERIPH_SDMMC_AUTO_CLK` in Kconfig)
 *   indicates that the SDIO/SD/MMC peripheral supports the Auto-CLK
 *   feature, i.e. the automatic activation and deactivation of the SD CLK
 *   signal when required to save power. This function shall be provided by
 *   the MCU if supported.
 *
 * - <b>`periph_sdmmc_auto_cmd12`</b> (`HAS_PERIPH_SDMMC_AUTO_CMD12` in Kconfig)
 *   indicates that the SDIO/SD/MMC peripheral supports the Auto-CMD12
 *   feature, i.e. CMD12 is sent automatically to stop the transmission in
 *   multiple block operations. This feature shall be provided by the MCU
 *   if supported.
 *
 * - <b>`periph_sdmmc_hs`</b> (`HAS_PERIPH_SDMMC_HS` in Kconfig)
 *   indicates that the SDIO/SD/MMC peripheral supports the high speed
 *   access, that is 50 MHz for SD and 52 MHz for MMC. This feature shall be
 *   provided by the MCU if supported.

 * - <b>`periph_sdmmc_mmc`</b> (`HAS_PERIPH_SDMMC_MMC` in Kconfig)
 *   indicates that the SDIO/SD/MMC peripheral supports MMC/eMMCs. This
 *   feature shall be provided by the MCU if supported.
 *
 * Some functionalities of the SDIO/SD/MMC Device API must be explicitly
 * enabled via modules:
 *
 * - <b>`sdmmc_mmc`</b> (`MODULE_SDMMC_MMC` in Kconfig)
 *   enables the support for MMCs/eMMCs.
 *
 * - <b>`periph_sdmmc_8bit`</b> (`MODULE_PERIPH_SDMMC_8BIT` in Kconfig)
 *   enables the 8-bit bus width support. It requires the corresponding
 *   feature of the board.
 *
 * @{
 *
 * @file
 * @brief       SDIO/SD/MMC device API using a low-level peripheral driver
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include <errno.h>

#include "assert.h"
#include "byteorder.h"
#include "macros/units.h"
#include "periph_conf.h"
#include "xfa.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   CPU-specific requirements for SDIO/SD/MMC buffers
 *
 * Can be overridden by periph_cpu if needed by the SDMMC/SDIO peripheral.
 * Example usage:
 * ```
 * SDMMC_CPU_DMA_REQUIREMENTS uint8_t buffer[64];
 * ```
 */
#ifndef SDMMC_CPU_DMA_REQUIREMENTS
#define SDMMC_CPU_DMA_REQUIREMENTS
#endif

/**
 * @brief   Instantiation type for SDIO/SD/MMC buffers
 *
 * Example usage:
 * ```
 * sdmmc_buf_t buffer[SDMMC_SDHC_BLOCK_SIZE];
 * ```
 */
#define sdmmc_buf_t     SDMMC_CPU_DMA_REQUIREMENTS uint8_t

/**
 * @brief   Size of a single data block on SDHC/SDXC Cards in bytes
 *
 * The size of a data block depend on type and mode of the card. SDSC/SDXC
 * use a fix size of 512 bytes.
 */
#define SDMMC_SDHC_BLOCK_SIZE   (512)

/**
 * @name    Command and Response related definitions
 * @{
 */

/**
 * @brief   Command index
 */
#define SDMMC_CMD(n)        (n)

/**
 * @brief   Application specific command index prefix (Bit is 7 used)
 */
#define SDMMC_ACMD_PREFIX   (1 << 7)

/**
 * @brief   Application specific command index
 */
#define SDMMC_ACMD(n)       (SDMMC_ACMD_PREFIX | SDMMC_CMD(n))

/**
 * @brief   SDIO/SD/MMC Commands
 *
 * Commands used by the driver. Command indices are in range [0..63].
 * Application specific command indices are ORed with
 * @ref SDMMC_ACMD_PREFIX (0x80) and thus marked as application specific.
 * @ref SDMMC_ACMD_PREFIX can be used to check whether a given command index
 * specifies an application specific command.
 */
typedef enum {
    SDMMC_CMD0   = SDMMC_CMD(0),   /**< GO_IDLE_STATE */
    SDMMC_CMD1   = SDMMC_CMD(1),   /**< SEND_OP_COND */
    SDMMC_CMD2   = SDMMC_CMD(2),   /**< ALL_SEND_CID */
    SDMMC_CMD3   = SDMMC_CMD(3),   /**< SET_RELATIVE_ADDR */
    SDMMC_CMD4   = SDMMC_CMD(4),   /**< SET_DSR */
    SDMMC_CMD5   = SDMMC_CMD(5),   /**< SD_APP_OP_COND (SDIO only) */
    SDMMC_CMD6   = SDMMC_CMD(6),   /**< SWITCH */
    SDMMC_CMD7   = SDMMC_CMD(7),   /**< SELECT/DESELECT_CARD */
    SDMMC_CMD8   = SDMMC_CMD(8),   /**< SEND_IF_COND (SD), SEND_EXT_CSD (MMC) */
    SDMMC_CMD9   = SDMMC_CMD(9),   /**< SEND_CSD */
    SDMMC_CMD10  = SDMMC_CMD(10),  /**< SEND_CID */
    SDMMC_CMD12  = SDMMC_CMD(12),  /**< STOP_TRANSMISSION */
    SDMMC_CMD13  = SDMMC_CMD(13),  /**< SEND_STATUS */
    SDMMC_CMD16  = SDMMC_CMD(16),  /**< SET_BLOCKLEN */
    SDMMC_CMD17  = SDMMC_CMD(17),  /**< READ_SINGLE_BLOCK */
    SDMMC_CMD18  = SDMMC_CMD(18),  /**< READ_MULTIPLE_BLOCK */
    SDMMC_CMD23  = SDMMC_CMD(23),  /**< SET_BLOCK_COUNT */
    SDMMC_CMD24  = SDMMC_CMD(24),  /**< WRITE_BLOCK */
    SDMMC_CMD25  = SDMMC_CMD(25),  /**< WRITE_MULTIPLE_BLOCK */
    SDMMC_CMD32  = SDMMC_CMD(32),  /**< ERASE_WR_BLK_START */
    SDMMC_CMD33  = SDMMC_CMD(33),  /**< ERASE_WR_BLK_END */
    SDMMC_CMD38  = SDMMC_CMD(38),  /**< ERASE */
    SDMMC_CMD52  = SDMMC_CMD(52),  /**< IO_RW_DIRECT (SDIO only) */
    SDMMC_CMD53  = SDMMC_CMD(53),  /**< IO_RW_EXTENDED (SDIO only) */
    SDMMC_CMD55  = SDMMC_CMD(55),  /**< APP_CMD */
    SDMMC_CMD58  = SDMMC_CMD(58),  /**< READ_OCR (SPI mode only) */
    SDMMC_CMD59  = SDMMC_CMD(59),  /**< CRC_ON_OFF (SPI mode only) */
    SDMMC_ACMD6  = SDMMC_ACMD(6),  /**< SET_BUS_WIDTH */
    SDMMC_ACMD13 = SDMMC_ACMD(13), /**< SD_STATUS */
    SDMMC_ACMD23 = SDMMC_ACMD(23), /**< SET_WR_BLK_ERASE_COUNT */
    SDMMC_ACMD41 = SDMMC_ACMD(41), /**< SD_APP_OP_COND */
    SDMMC_ACMD51 = SDMMC_ACMD(51), /**< SEND_SCR */
} sdmmc_cmd_t;

/** Command argument if no argument is required*/
#define SDMMC_CMD_NO_ARG        (0x00000000UL)

/** Command argument if RCA is used in addressed commands */
#define SDMMC_CMD_ARG_RCA(n)    ((uint32_t)n << 16)

/** Mask to check whether the response type uses CRC7 */
#define SDMMC_RESP_CRC          (1UL << 4)
/** Mask to check whether the response includes busy status from card */
#define SDMMC_RESP_BUSY         (1UL << 5)
/** Mask of response index */
#define SDMMC_RESP_IDX          (0xf)

/**
 * @brief    SDIO/SD/MMC Response types
 *
 * @see Physical Layer Simplified Specification Version 9.00, Section 9.00
 *      [[sdcard.org](https://www.sdcard.org)] and \n
 *      JEDEC Standard No. JESD84-B42, MultiMediaCard (MMC) Electrical
 *      Standard, High Capacity (MMCA, 4.2), Section 7.10 for R4 and R5
 *      [[jedec.org](https://www.jedec.org)]
 */
typedef enum {
    SDMMC_NO_R = 0,                     /**< No response expected */
    SDMMC_R1  = 1 | SDMMC_RESP_CRC,     /**< Normal Response [48 bit (32 bit card status)]*/
    SDMMC_R1B = 1 | SDMMC_RESP_CRC
                  | SDMMC_RESP_BUSY,    /**< Normal Response
                                             [48 bit (R1 with optional busy signal on DAT0)] */
    SDMMC_R2  = 2 | SDMMC_RESP_CRC,     /**< CID/CSD [136 bit (128 bit CID or CSD)] */
    SDMMC_R3  = 3,                      /**< OCR Resister [48 bit (32 bit OCR)] */
    SDMMC_R4  = 4,                      /**< Fast I/O [48 bit (16 bit RCA, 1 bit status,
                                             7 bit addr, 8 bit reg)] */
    SDMMC_R5  = 5 | SDMMC_RESP_CRC,     /**< Interrupt Request
                                             [48 bit (16 bit RCA, 16 bit not defined) */
    SDMMC_R6  = 6 | SDMMC_RESP_CRC,     /**< Published RCA Response [48 bit (16 bit RCA,
                                             16 bit card status) */
    SDMMC_R7  = 7 | SDMMC_RESP_CRC,     /**< Card Interface Condition [48 bit] (32 bit data,
                                             see section 4.9.6) */
} sdmmc_resp_t;
/** @} */

/**
 * @name    Definitions used for CMD8 - SEND_IF_COND (SD only)
 *
 * @see Physical Layer Simplified Specification Version 9.00, Section 4.3.13
 *      [[sdcard.org](https://www.sdcard.org)]
 * @{
 */

#define SDMMC_CMD8_CHECK_PATTERN    (0xaa)          /**< Check pattern, 0xAA recommended */
#define SDMMC_CMD8_VHS_27_36V       (0b0001 << 8)   /**< Voltage Supplied by host 2.7-3.6V */
#define SDMMC_CMD8_PCIE_AVAIL       (1 << 12)       /**< PCIe Availability (not yet used) */
#define SDMMC_CMD8_PCIE_12V         (1 << 13)       /**< PCIe 1.2V Support (not yet used) */

/** Command argument used in CMD8 */
#define SDMMC_CMD8_CHECK  (SDMMC_CMD8_VHS_27_36V | SDMMC_CMD8_CHECK_PATTERN)
/** @} */

/**
 * @name OCR Register Definition
 *
 * @see Physical Layer Simplified Specification Version 9.00, Section 5.1,
 *      Table 5-1 [[sdcard.org](https://www.sdcard.org)]
 * @{
 */
#define SDMMC_OCR_18V           (1UL << 7)  /**< Low Voltage Range */
#define SDMMC_OCR_27_28V        (1UL << 15) /**< 2.7V to 2.8V Range */
#define SDMMC_OCR_28_29V        (1UL << 16) /**< 2.8V to 2.9V Range */
#define SDMMC_OCR_29_30V        (1UL << 17) /**< 2.9V to 3.0V Range */
#define SDMMC_OCR_30_31V        (1UL << 18) /**< 3.0V to 3.1V Range */
#define SDMMC_OCR_31_32V        (1UL << 19) /**< 3.1V to 3.2V Range */
#define SDMMC_OCR_32_33V        (1UL << 20) /**< 3.2V to 3.3V Range */
#define SDMMC_OCR_33_34V        (1UL << 21) /**< 3.3V to 3.4V Range */
#define SDMMC_OCR_34_35V        (1UL << 22) /**< 3.4V to 3.5V Range */
#define SDMMC_OCR_35_36V        (1UL << 23) /**< 3.5V to 3.6V Range */

#define SDMMC_OCR_S18A          (1UL << 24) /**< Switching to 1.8V Accepted */
#define SDMMC_OCR_OVER_2TB      (1UL << 27) /**< Over 2TB support status (CO2T) */
#define SDMMC_OCR_UHS_II        (1UL << 29) /**< UHS-II Card Status */
#define SDMMC_OCR_CCS           (1UL << 30) /**< Card Capacity Status (CCS) */
#define SDMMC_OCR_POWER_UP      (1UL << 31) /**< Card power up status bit (busy) */

/**
 * @brief   Voltage profile for the range 2.7-3.6V as defined for MMC
 *
 * @see JEDEC Standard No. JESD84-B42, MultiMediaCard (MMC) Electrical
 *      Standard, High Capacity (MMCA, 4.2), Section 8, Table 29
 *      [[jedec.org](https://www.jedec.org)]
 */
#define SDMMC_OCR_ALL_VOLTAGES  (SDMMC_OCR_27_28V | SDMMC_OCR_28_29V | \
                                 SDMMC_OCR_29_30V | SDMMC_OCR_30_31V | \
                                 SDMMC_OCR_31_32V | SDMMC_OCR_32_33V | \
                                 SDMMC_OCR_33_34V | SDMMC_OCR_34_35V | \
                                 SDMMC_OCR_35_36V)
/** @} */

/**
 * @brief   SDIO/SD/MMC Card types
 *
 * To be able to specify a Combo card (combined SD Memory and SDIO card),
 * the enumeration values are defined bitwise so that they can be ORed to
 * represent a Combo card with different versions of the SD Memory part. For
 * example, `SDMMC_CARD_TYPE_SDIO | SDMMC_CARD_TYPE_SDSC_V2_V3` represents a
 * Combo card with SDIO function and SD Memory Card Standard Capacity (SDSC)
 * Version 2.x+.
 */
typedef enum {
    SDMMC_CARD_TYPE_UNKNOWN = 0x00,    /**< Card type unknown */
    SDMMC_CARD_TYPE_SDSC_V1 = 0x01,    /**< SD Memory Card Standard Capacity (SDSC) Version 1.x */
    SDMMC_CARD_TYPE_SDSC_V2_V3 = 0x02, /**< SD Memory Card Standard Capacity (SDSC) Version 2.x+ */
    SDMMC_CARD_TYPE_SDHC_SDXC = 0x04,  /**< SD Memory Card High or Extended Capacity (SDHC/SDXC) */
    /* bits 3..5 are left unused for later extensions of SD Memory Card types */
    SDMMC_CARD_TYPE_SDIO = 0x40,       /**< SDIO Card */
    SDMMC_CARD_TYPE_MMC = 0x80,        /**< MultiMedia Card */
} sdmmc_card_type_t;

/**
 *  @brief  Mask for any type of SD Memory card
 */
#define SDMMC_CARD_TYPE_SD  (SDMMC_CARD_TYPE_SDSC_V1 | \
                             SDMMC_CARD_TYPE_SDSC_V2_V3 | \
                             SDMMC_CARD_TYPE_SDHC_SDXC)

/**
 * @brief   SDIO/SD/MMC Card data bus widths
 *
 * Possible bus widths are given as integer values as defined in SD Status
 * register and the SCR register for SD Cards.
 *
 * @warning The values differ from the bus widths used in ACMD6 for SD Cards
 *          and the CCC register in SDIO Cards.
 *
 * @see Physical Layer Simplified Specification Version 9.00, 4.10.2,
 *      Table 4-44, SD Status [[sdcard.org](https://www.sdcard.org)] \n
 */
typedef enum {
    SDMMC_BUS_WIDTH_1BIT = 1,   /**< Data bus width is 1 bit (default) */
    SDMMC_BUS_WIDTH_4BIT = 4,   /**< Data bus width is 4 bit */
    SDMMC_BUS_WIDTH_8BIT = 8,   /**< Data bus width is 8 bit */
} sdmmc_bus_width_t;

/**
 * @brief   SDIO/SD/MMC Card clock rate types
 *
 * Identifies the clock frequency to be used. The clock frequency in
 * identification mode f_OD (Open Drain mode) is fixed and is f_OD = 400 kHz.
 * The actual clock frequency in data transfer mode f_PP (Push-Pull mode)
 * depends on the SDIO/SD/MMC device and the card used.
 * The low-level SDIO/SD/MMC peripheral driver sets the actual clock rate
 * in function sdmmc_driver_t::set_clock_rate.
 */
typedef enum {
    SDMMC_CLK_400K = KHZ(400), /**< Identification Mode f_OD (400 kHz) */
    SDMMC_CLK_20M  = MHZ(20),  /**< MMC Card in Data Transfer Mode (Backward Compatibility) */
    SDMMC_CLK_25M  = MHZ(25),  /**< SD/SDIO Card in Data Transfer Mode (Default Speed) */
    SDMMC_CLK_26M  = MHZ(26),  /**< MMC/eMMC Card in Data Transfer Mode (Default Speed) */
    SDMMC_CLK_50M  = MHZ(50),  /**< SD/SDIO Card in Data Transfer Mode (High Speed) */
    SDMMC_CLK_52M  = MHZ(52),  /**< MMC/eMMC in Data Transfer Mode (High Speed) */
} sdmmc_clock_rate_t;

/**
 * @name    SDIO/SD/MMC Card status as returned in responses R1 to CMD13 with b[15]=0
 *
 * @see Physical Layer Simplified Specification Version 9.00, Section 4.10.1,
 *      Table 4-42 [[sdcard.org](https://www.sdcard.org)] \n
 *      SDIO Simplified Specification Version 3.00, 4.10.8, Table 4-7,
 *      [[sdcard.org](https://www.sdcard.org)] \n
 *      JEDEC Standard No. JESD84-B42, MultiMediaCard (MMC) Electrical
 *      Standard, High Capacity (MMCA, 4.2), Section 7.11, Table 25
 *      [[jedec.org](https://www.jedec.org)]
 * @{
 */
#define SDMMC_CARD_STATUS_OUT_OF_RANGE          (1UL << 31)     /**< SD/SDIO and MMC */
#define SDMMC_CARD_STATUS_ADDRESS_ERROR         (1UL << 30)     /**< SD and MMC */
#define SDMMC_CARD_STATUS_BLOCK_LEN_ERROR       (1UL << 29)     /**< SD and MMC */
#define SDMMC_CARD_STATUS_ERASE_SEQ_ERROR       (1UL << 28)     /**< SD and MMC */
#define SDMMC_CARD_STATUS_ERASE_PARAM           (1UL << 27)     /**< SD and MMC */
#define SDMMC_CARD_STATUS_WP_VIOLATION          (1UL << 26)     /**< SD and MMC */
#define SDMMC_CARD_STATUS_CARD_IS_LOCKED        (1UL << 25)     /**< SD and MMC */
#define SDMMC_CARD_STATUS_LOCK_UNLOCK_FAILED    (1UL << 24)     /**< SD/SDIO and MMC */
#define SDMMC_CARD_STATUS_COM_CRC_ERROR         (1UL << 23)     /**< SD/SDIO and MMC */
#define SDMMC_CARD_STATUS_ILLEGAL_COMMAND       (1UL << 22)     /**< SD and MMC */
#define SDMMC_CARD_STATUS_CARD_ECC_FAILED       (1UL << 21)     /**< SD and MMC */
#define SDMMC_CARD_STATUS_CC_ERROR              (1UL << 20)     /**< SD and MMC */
#define SDMMC_CARD_STATUS_ERROR                 (1UL << 19)     /**< SD/SDIO and MMC */
#define SDMMC_CARD_STATUS_UNDERRUN              (1UL << 18)     /**< MMC only */
#define SDMMC_CARD_STATUS_OVERRUN               (1UL << 17)     /**< MMC only */
#define SDMMC_CARD_STATUS_CSD_OVERWRITE         (1UL << 16)     /**< SD (CSD), MMC (CSD and CID) */
#define SDMMC_CARD_STATUS_WP_ERASE_SKIP         (1UL << 15)     /**< SD and MMC */
#define SDMMC_CARD_STATUS_CARD_ECC_DISABLED     (1UL << 14)     /**< SD only */
#define SDMMC_CARD_STATUS_ERASE_RESET           (1UL << 13)     /**< SD and MMC */
#define SDMMC_CARD_STATUS_READY_FOR_DATA        (1UL << 8)      /**< SD and MMC */
#define SDMMC_CARD_STATUS_SWITCH_ERROR          (1UL << 7)      /**< MMC only */
#define SDMMC_CARD_STATUS_FX_EVENT              (1UL << 6)      /**< SD only */
#define SDMMC_CARD_STATUS_APP_CMD               (1UL << 5)      /**< SD and MMC */
#define SDMMC_CARD_STATUS_AKE_SEQ_ERROR         (1UL << 3)      /**< SD only */

/** SD/MMC Card status mask for error flags */
#define SDMMC_CARD_STATUS_ERRORS   (SDMMC_CARD_STATUS_OUT_OF_RANGE | \
                                    SDMMC_CARD_STATUS_ADDRESS_ERROR | \
                                    SDMMC_CARD_STATUS_BLOCK_LEN_ERROR | \
                                    SDMMC_CARD_STATUS_ERASE_SEQ_ERROR | \
                                    SDMMC_CARD_STATUS_ERASE_PARAM | \
                                    SDMMC_CARD_STATUS_WP_VIOLATION | \
                                    SDMMC_CARD_STATUS_LOCK_UNLOCK_FAILED | \
                                    SDMMC_CARD_STATUS_COM_CRC_ERROR | \
                                    SDMMC_CARD_STATUS_ILLEGAL_COMMAND | \
                                    SDMMC_CARD_STATUS_CARD_ECC_FAILED | \
                                    SDMMC_CARD_STATUS_CC_ERROR | \
                                    SDMMC_CARD_STATUS_ERROR | \
                                    SDMMC_CARD_STATUS_UNDERRUN | \
                                    SDMMC_CARD_STATUS_OVERRUN | \
                                    SDMMC_CARD_STATUS_CSD_OVERWRITE | \
                                    SDMMC_CARD_STATUS_WP_ERASE_SKIP | \
                                    SDMMC_CARD_STATUS_SWITCH_ERROR | \
                                    SDMMC_CARD_STATUS_AKE_SEQ_ERROR)

/** SD/MMC Card status mask for current state */
#define SDMMC_CARD_STATUS_CURRENT_STATE(n)  (((n) >> SDMMC_CARD_STATUS_CURRENT_STATE_Pos) & 0x0f)
#define SDMMC_CARD_STATUS_CURRENT_STATE_Pos (9) /**< CURRENT_STATE position */

/**
 * @brief    SD/MMC Card states
 */
enum {
    SDMMC_CARD_STATE_IDLE = 0,      /**< Idle */
    SDMMC_CARD_STATE_READY = 1,     /**< Ready */
    SDMMC_CARD_STATE_IDENT = 2,     /**< Identification */
    SDMMC_CARD_STATE_STBY = 3,      /**< Stand-by */
    SDMMC_CARD_STATE_TRAN = 4,      /**< Transfer */
    SDMMC_CARD_STATE_DATA = 5,      /**< Data */
    SDMMC_CARD_STATE_RCV = 6,       /**< Receive */
    SDMMC_CARD_STATE_PRG = 7,       /**< Programming */
    SDMMC_CARD_STATE_DIS = 8,       /**< Disconnect */
    SDMMC_CARD_STATE_BTST = 9,      /**< Bus Test */
};

/**
 * @brief    SD/MMC Card status as structure
 */
typedef union {
    struct {
        uint32_t reserved0         :2;  /**< [1:0] Reserved for appl. specific commands */
        uint32_t reserved2         :1;  /**< [2] Reserved for appl. specific commands */
        uint32_t AKE_SEQ_ERROR     :1;  /**< [3] SD only, Reserved in MMC */
        uint32_t reserved4         :1;  /**< [4] Reserved */
        uint32_t APP_CMD           :1;  /**< [5] SD and MMC */
        uint32_t FX_EVENT          :1;  /**< [6] SD only, Reserved in MMC */
        uint32_t SWITCH_ERROR      :1;  /**< [7] MMC only, reserved in SD/SDIO */
        uint32_t READY_FOR_DATA    :1;  /**< [8] SD and MMC */
        uint32_t CURRENT_STATE     :4;  /**< [12:9] SD and MMC */
        uint32_t ERASE_RESET       :1;  /**< [13] SD and MMC */
        uint32_t CARD_ECC_DISABLED :1;  /**< [14] SD only, reserved in MMC */
        uint32_t WP_ERASE_SKIP     :1;  /**< [15] SD and MMC */
        uint32_t CSD_OVERWRITE     :1;  /**< [16] SD (CSD), MMC (CSD and CID) */
        uint32_t OVERRUN           :1;  /**< [17] MMC only, DEFERRED_RESPONSE in SD */
        uint32_t UNDERRUN          :1;  /**< [18] MMC only, reserved in SD */
        uint32_t ERROR             :1;  /**< [19] SD/SDIO and MMC */
        uint32_t CC_ERROR          :1;  /**< [20] SD and MMC */
        uint32_t CARD_ECC_FAILED   :1;  /**< [21] SD and MMC */
        uint32_t ILLEGAL_COMMAND   :1;  /**< [22] SD/SDIO and MMC */
        uint32_t COM_CRC_ERROR     :1;  /**< [23] SD/SDIO and MMC */
        uint32_t LOCK_UNLOCK_FAILED:1;  /**< [24] SD and MMC */
        uint32_t CARD_IS_LOCKED    :1;  /**< [25] SD and MMC */
        uint32_t WP_VIOLATION      :1;  /**< [26] SD and MMC */
        uint32_t ERASE_PARAM       :1;  /**< [27] SD and MMC */
        uint32_t ERASE_SEQ_ERROR   :1;  /**< [28] SD and MMC */
        uint32_t BLOCK_LEN_ERROR   :1;  /**< [29] SD and MMC */
        uint32_t ADDRESS_ERROR     :1;  /**< [30] SD and MMC */
        uint32_t OUT_OF_RANGE      :1;  /**< [31] SD/SDIO and MMC */
    };
    uint32_t value; /**< SD/MMC Card status as 32 bit value */
} sdmmc_card_status_t;

/** @} */

/**
 * @name SD Status register (SD Memory Cards)
 * @{
 */

/** SD status register size */
#define SDMMC_SD_STATUS_SIZE    (64)

/**
 * @brief   SD Status (SD Memory Card only)
 *
 * SD Status has a size of 512 bits and contains the status of SD Memory Card
 * features. It is not supported by MMCs and SDIO Cards. In Combo Cards, it
 * reflects the status of the SD Memory Card portion.
 *
 * Members are sorted by types to reduce the required RAM size.
 *
 * @see Physical Layer Simplified Specification Version 9.00, Section 4.10.2,
 *      Table 4-44 [[sdcard.org](https://www.sdcard.org)]
 */
typedef struct __attribute__((packed)) {
    uint32_t SIZE_OF_PROTECTED_AREA:32; /**< [479:448] Section 4.10.2.1 */
    uint32_t SUS_ADDR:22;               /**< [367:346] Section 4.10.2.12, Table 4-57 */
    uint32_t VSC_AU_SIZE:10;            /**< [377:368] Section 4.10.2.11, Table 4-56 */
    uint16_t SD_CARD_TYPE:16;           /**< [495:480] Section 4.10.2, Table 4-44 */
    uint16_t ERASE_SIZE:16;             /**< [423:408] Section 4.10.2.5, Table 4-49 */
    uint8_t  SPEED_CLASS:8;             /**< [447:440] Section 4.10.2.2, Table 4-45 */
    uint8_t  PERFORMANCE_MOVE:8;        /**< [439:432] Section 4.10.2.3, Table 4-46 */
    uint8_t  VIDEO_SPEED_CLASS:8;       /**< [391:384] Section 4.10.2.10, Table 4-54 */
    uint8_t  ERASE_TIMEOUT:6;           /**< [407:402] Section 4.10.2.6, Table 4-50 */
    uint8_t  ERASE_OFFSET:2;            /**< [401:400] Section 4.10.2.7, Table 4-51 */
    uint8_t  UHS_SPEED_GRADE:4;         /**< [399:396] Section 4.10.2.8, Table 4-52 */
    uint8_t  UHS_AU_SIZE:4;             /**< [395:392] Section 4.10.2.9, Table 4-3 */
    uint8_t  AU_SIZE:4;                 /**< [431:428] Section 4.10.2.4, Table 4-47 */
    uint8_t  DAT_BUS_WIDTH:2;           /**< [511:510] Section 4.10.2, Table 4-44 */
    uint8_t  SECURED_MODE:1;            /**< [509] Section 4.10.2, Table 4-44 */
} sdmmc_sd_status_t;

/** @} */

/**
 * @name CID register (SD Memory and MMC Cards)
 * @{
 */

/** CID register size in bytes (CID is 128 bit) */
#define SDMMC_CID_REG_SIZE      (16)

#define SDMMC_CID_OID_SIZE_SD   (2)   /**< OID (OEM/Application ID) size in byte (SD) */
#define SDMMC_CID_PNM_SIZE_SD   (5)   /**< PNM (Product name) size in byte (SD) */
#define SDMMC_CID_PNM_SIZE_MMC  (6)   /**< PNM (Product name) size in byte (MMC) */

/**
 * @brief   CID register structure (SD Memory Cards)
 *
 * @see Physical Layer Simplified Specification Version 9.00, Section 5.2,
 *      Table 5.2 [[sdcard.org](https://www.sdcard.org)]
 */
typedef struct __attribute__((packed)) {
    uint8_t MID;                      /**< Manufacturer ID */
    char OID[SDMMC_CID_OID_SIZE_SD];  /**< OEM/Application ID */
    char PNM[SDMMC_CID_PNM_SIZE_SD];  /**< Product name */
    uint8_t PRV;                      /**< Product revision */
    be_uint32_t PSN;                  /**< Product serial number in big-endian order */
    be_uint16_t MDT;                  /**< Manufacturing date in big-endian order */
    uint8_t CID_CRC;                  /**< CRC7 checksum including bit 0 */
} sdmmc_cid_sd_t;

/**
 * @brief   CID register structure (MMC)
 *
 * @see JEDEC Standard No. JESD84-B42, MultiMediaCard (MMC) Electrical
 *      Standard, High Capacity (MMCA, 4.2), Section 8.2, Table 30
 *      [[jedec.org](https://www.jedec.org)]
 */
typedef struct __attribute__((packed)) {
    uint8_t MID;                      /**< Manufacturer ID */
    be_uint16_t OID;                  /**< OEM/Application ID in big-endian order */
    char PNM[SDMMC_CID_PNM_SIZE_MMC]; /**< Product name */
    uint8_t PRV;                      /**< Product revision */
    be_uint32_t PSN;                  /**< Product serial number in big-endian order */
    uint8_t MDT;                      /**< Manufacturing date */
    uint8_t CID_CRC;                  /**< CRC7 checksum including bit 0 */
} sdmmc_cid_mmc_t;

/**
 * @brief   CID register structure (SD Memory and MMC Cards)
 */
typedef union {
    sdmmc_cid_sd_t  sd;     /**< CID register of SD Memory Cards */
    sdmmc_cid_mmc_t mmc;    /**< CID register of MMC */
} sdmmc_cid_t;

/** @} */

/**
 * @name CSD Register Definitions
 *
 * @see Physical Layer Simplified Specification Version 9.00, Section 5.3,
 *      [[sdcard.org](https://www.sdcard.org)] \n
 *      JEDEC Standard No. JESD84-B42, MultiMediaCard (MMC) Electrical
 *      Standard, High Capacity (MMCA, 4.2), Section 8.3, Table 35
 *      [[jedec.org](https://www.jedec.org)]
 * @{
 */

/**
 * @brief   CSD Register Versions (SD Memory Card and MMC)
 */
typedef enum {
    SDMMC_CSD_V1 = 0,   /**< CSD Version 1.0 (SDSC and MMC) */
    SDMMC_CSD_V2 = 1,   /**< CSD Version 2.0 (SDHC/SDXC) or Version 1.1 (MMC) */
    SDMMC_CSD_V3 = 2,   /**< CSD Version 3.0 (SDUC) or Version 1.2 (MMC) */
    SDMMC_CSD_Vx = 3,   /**< Reserved (SD) or Version in EXT_CSD (MMC) */
} sdmmc_csd_version_t;

/** CSD register size in byte (CSD is 128 bit) */
#define SDMMC_CSD_REG_SIZE  (16)

/**
 * @brief   CSD register structure Version 1.0
 *
 * @see Physical Layer Simplified Specification Version 9.00, Section 5.3.2,
 *      Table 5.4 [[sdcard.org](https://www.sdcard.org)]
 */
typedef struct __attribute__((packed)) {
    uint32_t CSD_CRC:8;             /**< CRC including End bit 1b [7:0] */
    uint32_t reserved5:1;           /**< reserved [8] */
    uint32_t WP_UPC:1;              /**< write protection until power cycle [9] */
    uint32_t FILE_FORMAT:2;         /**< File format [11:10] */
    uint32_t TMP_WRITE_PROTECT:1;   /**< temporary write protection [12] */
    uint32_t PERM_WRITE_PROTECT:1;  /**< permanent write protection [13] */
    uint32_t COPY:1;                /**< copy flag [14] */
    uint32_t FILE_FORMAT_GRP:1;     /**< File format group [15] */
    uint32_t reserved4:5;           /**< reserved [20:16] */
    uint32_t WRITE_BL_PARTIAL:1;    /**< partial blocks for write allowed [21] */
    uint32_t WRITE_BL_LEN:4;        /**< max. write data block length [25:22] */
    uint32_t R2W_FACTOR:3;          /**< write speed factor [28:26] */
    uint32_t reserved3:2;           /**< reserved [30:29] */
    uint32_t WP_GRP_ENABLE:1;       /**< write protect group enable [31] */
    uint32_t WP_GRP_SIZE:7;         /**< write protect group size [38:32] */
    uint32_t SECTOR_SIZE:7;         /**< erase sector size [45:39] */
    uint32_t ERASE_BLK_EN:1;        /**< erase single block enable [46] */
    uint32_t C_SIZE_MULT:3;         /**< device size multiplier [49:47] */
    uint32_t VDD_W_CURR_MAX:3;      /**< max. write current VDD max [52:50] */
    uint32_t VDD_W_CURR_MIN:3;      /**< max. write current VDD min [55:53] */
    uint32_t VDD_R_CURR_MAX:3;      /**< max. read current VDD max [58:56] */
    uint32_t VDD_R_CURR_MIN:3;      /**< max. read current VDD min [61:59] */
    uint32_t C_SIZE:12;             /**< device size [73:62] */
    uint32_t reserved2:2;           /**< reserved [75:74] */
    uint32_t DSR_IMP:1;             /**< DSR implemented [76] */
    uint32_t READ_BLK_MISALIGN:1;   /**< read block misalignment [77] */
    uint32_t WRITE_BLK_MISALIGN:1;  /**< write block misalignment [78] */
    uint32_t READ_BL_PARTIAL:1;     /**< partial blocks for read allowed [79] */
    uint32_t READ_BL_LEN:4;         /**< max. read data block length [83:80] */
    uint32_t CCC:12;                /**< card command classes [95:84] */
    uint32_t TRAN_SPEED:8;          /**< max. data transfer rate [103:96] */
    uint32_t NSAC:8;                /**< data read access-time-2 in CLK cycles [111:104] */
    uint32_t TAAC:8;                /**< data read access-time-1 [119:112] */
    uint32_t reserved1:6;           /**< reserved [125:120] */
    uint32_t CSD_STRUCTURE:2;       /**< CSD structure [127:126] */
} sdmmc_csd_v1_t;

/**
 * @brief   CSD register structure Version 2.0 and Version 3.0
 *
 * A combined format is used for CSD Version 2.0 and 3.0 to reduce the code
 * size. The only difference is the bit length of `C_SIZE`.
 *
 * @see Physical Layer Simplified Specification Version 9.00
 *      [[sdcard.org](https://www.sdcard.org)] \n
 *      - Version 2.0: Section 5.3.3, Table 5-16 \n
 *      - Version 3.0: Section 5.3.4, Table 5.3.4-1
 */
typedef struct __attribute__((packed)) {
    uint32_t CSD_CRC:8;             /**< CRC including End bit 1b [7:0] */
    uint32_t reserved5:1;           /**< reserved [8] */
    uint32_t WP_UPC:1;              /**< write protection until power cycle [9] */
    uint32_t FILE_FORMAT:2;         /**< File format [11:10] */
    uint32_t TMP_WRITE_PROTECT:1;   /**< temporary write protection [12] */
    uint32_t PERM_WRITE_PROTECT:1;  /**< permanent write protection [13] */
    uint32_t COPY:1;                /**< copy flag [14] */
    uint32_t FILE_FORMAT_GRP:1;     /**< File format group [15] */
    uint32_t reserved4:5;           /**< reserved [20:16] */
    uint32_t WRITE_BL_PARTIAL:1;    /**< partial blocks for write allowed [21] */
    uint32_t WRITE_BL_LEN:4;        /**< max. write data block length [25:22] */
    uint32_t R2W_FACTOR:3;          /**< write speed factor [28:26] */
    uint32_t reserved3:2;           /**< reserved [30:29] */
    uint32_t WP_GRP_ENABLE:1;       /**< write protect group enable [31] */
    uint32_t WP_GRP_SIZE:7;         /**< write protect group size [38:32] */
    uint32_t SECTOR_SIZE:7;         /**< erase sector size [45:39] */
    uint32_t ERASE_BLK_EN:1;        /**< erase single block enable [46] */
    uint32_t reserved2:1;           /**< reserved [47] */
    uint32_t C_SIZE:28;             /**< device size v2.0 [69:48], v3.0 [75:48] */
    uint32_t DSR_IMP:1;             /**< DSR implemented [76] */
    uint32_t READ_BLK_MISALIGN:1;   /**< read block misalignment [77] */
    uint32_t WRITE_BLK_MISALIGN:1;  /**< write block misalignment [78] */
    uint32_t READ_BL_PARTIAL:1;     /**< partial blocks for read allowed [79] */
    uint32_t READ_BL_LEN:4;         /**< max. read data block length [83:80] */
    uint32_t CCC:12;                /**< card command classes [95:84] */
    uint32_t TRAN_SPEED:8;          /**< max. data transfer rate [103:96] */
    uint32_t NSAC:8;                /**< data read access-time-2 in CLK cycles [111:104] */
    uint32_t TAAC:8;                /**< data read access-time-1 [119:112] */
    uint32_t reserved1:6;           /**< reserved [125:120] */
    uint32_t CSD_STRUCTURE:2;       /**< CSD structure [127:126] */
} sdmmc_csd_v2_t;

/**
 * @brief   CSD register structure for MMC
 *
 * @see JEDEC Standard No. JESD84-B42, MultiMediaCard (MMC) Electrical
 *      Standard, High Capacity (MMCA, 4.2), Section 8.3, Table 35
 *      [[jedec.org](https://www.jedec.org)]
 */
typedef struct __attribute__((packed)) {
    uint32_t CSD_CRC:8;             /**< CRC including End bit 1b [7:0] */
    uint32_t ECC:2;                 /**< ECC code [9:8] */
    uint32_t FILE_FORMAT:2;         /**< File format [11:10] */
    uint32_t TMP_WRITE_PROTECT:1;   /**< Temporary write protection [12] */
    uint32_t PERM_WRITE_PROTECT:1;  /**< Permanent write protection [13] */
    uint32_t COPY:1;                /**< Copy flag [14] */
    uint32_t FILE_FORMAT_GRP:1;     /**< File format group [15] */
    uint32_t CONTENT_PROT_APP:1;    /**< Content protection application [16] */
    uint32_t reserved4:4;           /**< reserved [20:17] */
    uint32_t WRITE_BL_PARTIAL:1;    /**< partial blocks for write allowed [21] */
    uint32_t WRITE_BL_LEN:4;        /**< Max. write data block length [25:22] */
    uint32_t R2W_FACTOR:3;          /**< Write speed factor [28:26] */
    uint32_t DEFAULT_ECC:2;         /**< Manufacturer default ECC [30:29] */
    uint32_t WP_GRP_ENABLE:1;       /**< write protect group enable [31] */
    uint32_t WP_GRP_SIZE:5;         /**< Write protect group size [36:32] */
    uint32_t ERASE_GRP_MULT:5;      /**< Erase group size multiplier [41:37] */
    uint32_t ERASE_GRP_SIZE:5;      /**< Erase group size [46:42] */
    uint32_t C_SIZE_MULT:3;         /**< Device size multiplier [49:47] */
    uint32_t VDD_W_CURR_MAX:3;      /**< Max. write current VDD max [52:50] */
    uint32_t VDD_W_CURR_MIN:3;      /**< Max. write current VDD min [55:53] */
    uint32_t VDD_R_CURR_MAX:3;      /**< Max. read current VDD max [58:56] */
    uint32_t VDD_R_CURR_MIN:3;      /**< Max. read current VDD min [61:59] */
    uint16_t C_SIZE:12;             /**< Device size [73:62] */
    uint32_t reserved2:2;           /**< reserved [75:74] */
    uint32_t DSR_IMP:1;             /**< DSR implemented [76] */
    uint32_t READ_BLK_MISALIGN:1;   /**< Read block misalignment [77] */
    uint32_t WRITE_BLK_MISALIGN:1;  /**< Write block misalignment [78] */
    uint32_t READ_BL_PARTIAL:1;     /**< Partial blocks for read allowed [79] */
    uint32_t READ_BL_LEN:4;         /**< Max. read data block length [83:80] */
    uint32_t CCC:12;                /**< Card command classes [95:84] */
    uint32_t TRAN_SPEED:8;          /**< Max. bus clock frequency [103:96] */
    uint32_t NSAC:8;                /**< Data read access-time-2 in CLK cycles [111:104] */
    uint32_t TAAC:8;                /**< Data read access-time-1 [119:112] */
    uint32_t reserved1:2;           /**< reserved [121:120] */
    uint32_t SPEC_VERS:4;           /**< Specification version [125:122] */
    uint32_t CSD_STRUCTURE:2;       /**< CSD structure [127:126] */
} sdmmc_csd_mmc_t;

/**
 * @brief   CSD register (SD Memory Card and MMC)
 */
typedef union {
    sdmmc_csd_v1_t v1;      /**< CSD Version 1.0 */
    sdmmc_csd_v2_t v2;      /**< CSD Version 2.0 and 3.0 */
    sdmmc_csd_mmc_t mmc;    /**< CSD Version for MMC */
} sdmmc_csd_t;

/** Extended CSD (EXT_CSD) register size in byte (EXT_CSD is 512 byte) */
#define SDMMC_EXT_CSD_REG_SIZE  (512)

/**
 * @brief   Extended CSD (EXT_CSD) register structure (MMC only)
 *
 * Since the Extended CSD register is a complete 512 byte block which is only
 * sparsely used, only the elements that are absolutely needed are stored in
 * the extended CSD register structure.
 *
 * @see JEDEC Standard No. JESD84-B42, MultiMediaCard (MMC) Electrical
 *      Standard, High Capacity (MMCA, 4.2), Section 8.4, Table 46
 *      [[jedec.org](https://www.jedec.org)]
 */
typedef struct {
    uint32_t SEC_COUNT;                 /**< Sector Count [215:212] */
    uint8_t CARD_TYPE;                  /**< Card Type [196] */
    uint8_t CSD_STRUCTURE;              /**< CSD Structure Version [194] */
    uint8_t BUS_WIDTH;                  /**< Bus Width Mode [183] */
} sdmmc_ext_csd_t;

/** @} */

/**
 * @name SCR Register Definitions (SD Memory Card only)
 *
 * @see Physical Layer Simplified Specification Version 9.00, Section 5.6,
 *      Table 5-17 [[sdcard.org](https://www.sdcard.org)]
 * @{
 */

/** SCR register size in byte (SCR is 64 bit) */
#define SDMMC_SCR_REG_SIZE          (8)

/**
 * @brief   SCR register structure (SD Memory Card only)
 *
 * @see Physical Layer Simplified Specification Version 9.00, Section 5.6,
 *      Table 5-17 [[sdcard.org](https://www.sdcard.org)]
 */
typedef struct {
    /* first 32-bit word */
    uint32_t reserved0;                 /**< Reserved for manufacturer usage [31:0] */
    /* second 32-bit word */
    union {
        struct {
            uint32_t CMD_SUPPORT:5;             /**< Command Support bits [36:32] */
            uint32_t reserved37:1;              /**< Reserved [37] */
            uint32_t SD_SPECX:4;                /**< Spec. Version 5.00 or higher [41:38] */
            uint32_t SD_SPEC4:1;                /**< Spec. Version 4.00 or higher [42] */
            uint32_t EX_SECURITY:4;             /**< Extended Security Support [46:43] */
            uint32_t SD_SPEC3:1;                /**< Spec. Version 3.00 or higher [47] */
            uint32_t SD_BUS_WIDTHS:4;           /**< DAT Bus widths supported [51:48] */
            uint32_t SD_SECURITY:3;             /**< CPRM Security Support [54:52] */
            uint32_t DATA_STAT_AFTER_ERASE:1;   /**< Data status after erases [55] */
            uint32_t SD_SPEC:4;                 /**< SD Memory Card - Spec. Version [59:56] */
            uint32_t SCR_STRUCTURE:4;           /**< SCR Structure [63:60] */
        };
        uint32_t value;
    };
} sdmmc_scr_t;

#define SDMMC_SCR_ACMD_53_54_SUPPORT    (0b10000)   /**< @ref sdmmc_scr_t::CMD_SUPPORT
                                                         Secure Receive/Send supported */
#define SDMMC_SCR_ACMD_58_59_SUPPORT    (0b01000)   /**< @ref sdmmc_scr_t::CMD_SUPPORT
                                                         Extension Register Multi-Block */
#define SDMMC_SCR_ACMD_48_49_SUPPORT    (0b00100)   /**< @ref sdmmc_scr_t::CMD_SUPPORT
                                                         Extension Register Single-Block */
#define SDMMC_SCR_ACMD_23_SUPPORT       (0b00010)   /**< @ref sdmmc_scr_t::CMD_SUPPORT
                                                         Set Block Count */
#define SDMMC_SCR_ACMD_20_SUPPORT       (0b00001)   /**< @ref sdmmc_scr_t::CMD_SUPPORT
                                                         Speed Class Control */

/**
 * @brief   Get Physical Layer Specification Version value from SCR value
 *
 * @param   scr   SCR register value of type sdmmc_scr_t
 *
 * | Value | Physical Layer Specification Version Number |
 * |---|:---------------------------------|
 * | 0 | Version 1.0 and 1.01 |
 * | 1 | Version 1.10 |
 * | 2 | Version 2.00 |
 * | 3 | Version 3.0X |
 * | 4 | Version 4.XX |
 * | 5 | Version 5.XX |
 * | 6 | Version 6.XX |
 * | 7 | Version 7.XX |
 * | 8 | Version 8.XX |
 * | 9 | Version 9.XX |
 */
#define SDMMC_SCR_SD_SPEC(scr)  \
    (scr.SD_SPEC + scr.SD_SPEC3 + (scr.SD_SPECX ? scr.SD_SPECX + 1 : scr.SD_SPEC4))

/** @} */

/**
 * @brief   SD/MMC device access macro
 * @{
 */
#define SDMMC_DEV(x)          (sdmmc_get_dev(x))
/** @} */

/**
 * @brief   sdmmc_dev_t forward declaration
 */
typedef struct sdmmc_dev sdmmc_dev_t;

/**
 * @brief   Data transfer types
 */
typedef enum {
    SDMMC_MULTIBYTE, /**< Transfer 1 to READ_BL_LEN/WRITE_BL_LEN bytes */
    SDMMC_BLOCK,     /**< Transfer block with size of READ_BL_LEN/WRITE_BL_LEN */
    SDMMC_STREAM,    /**< Transfer until CMD12 is sent (MMC and 1-bit bus only) */
} sdmmc_xfer_type_t;

/**
 * @brief   Transfer descriptor
 *
 * The transfer descriptor hold all information about a transfer such as the
 * block size and the number of block.
 */
typedef struct {
    sdmmc_xfer_type_t type;     /**< Type of the transfer */
    bool write;                 /**< Indicate a write transfer */
    sdmmc_cmd_t cmd_idx;        /**< Command index used for the transfer */
    sdmmc_resp_t resp_type;     /**< Type of expected response for the transfer */
    uint32_t arg;               /**< Command argument used for the transfer */
    uint16_t block_size;        /**< Size of a block or number of bytes for Byte transfer */
    uint16_t block_num;         /**< Number of blocks to be transferred, 1 for Byte transfer */
} sdmmc_xfer_desc_t;

/**
 * @brief   Auto Command features supported by the SDIO/SD/MMC peripheral
 *
 * Most SDIO/SD/MMC peripherals support the Auto-Command feature for
 * CMD12 and CMD23. The low-level SDIO/SD/MMC peripheral driver defines
 * in @ref sdmmc_driver_t::init which Auto-Command features are supported
 * by the peripheral. To be able to specify a combination of these
 * features, the enumeration values are defined bitwise.
 */
typedef enum {
    SDMMC_AUTO_CMD_NONE = 0x00,
    SDMMC_AUTO_CMD12 = 0x01,
    SDMMC_AUTO_CMD23 = 0x02,
    SDMMC_AUTO_CMD_BOTH = 0x03,
} sdmmc_auto_cmd_t;

/**
 * @brief   Events generated by SDIO/SD/MMC high level API
 */
typedef enum {
    SDMMC_EVENT_CARD_INSERTED,
    SDMMC_EVENT_CARD_REMOVED,
} sdmmc_event_t;

/**
 * @brief   Event callback function type
 *
 * @param[in]   dev     SDIO/SD/MMC device to be used
 * @param[in]   event   Event
 *
 * @warning The function is called in the ISR context. Do not do anything
 *          comprehensive or time-consuming. Instead, use `thread_flags`,
 *          `event_queue` or `msg` mechanism to inform a thread about the event,
 *          which then handles the event asynchronously in thread context.
 */
typedef void (*sdmmc_event_cb_t)(sdmmc_dev_t *dev, sdmmc_event_t event);

/**
 * @brief    Low-level SDIO/SD/MMC peripheral driver
 *
 * The low-level SDIO/SD/MMC peripheral driver interface as used by
 * high-level SDIO/SD/MMC device API functions. It has to be implemented
 * for a specific CPU.
 *
 * @note High-level functions such as @ref sdmmc_read_blocks are provided
 *       by the SDIO/SD/MMC API and should be used instead. Directly calling
 *       these functions is not recommended.
 */
typedef struct {
    /**
     * @brief   Basic initialization of the given SDIO/SD/MMC device
     *
     * Low-level SDIO/SD/MMC peripheral driver function for basic initialization
     * of the peripheral including pin configuration of used pins. It is
     * called by @ref sdmmc_init during the board initialization to prepare
     * the SDIO/SD/MMC peripheral for further usage. Specific initialization
     * parameters required for this initialization are defined in the board's
     * `periph_conf.h`.
     *
     * Errors like configuration parameter problems are not signaled by
     * return values, but by using the `assert()`.
     *
     * @see @ref sdmmc_init
     * @pre @p dev must not be `NULL`.
     *
     * @param[in]   dev     SDIO/SD/MMC device to initialize
     */
    void (*init)(sdmmc_dev_t *dev);

    /**
     * @brief   Send command to SDIO/SD/MMC Card and optionally wait for response
     *
     * Low-level SDIO/SD/MMC peripheral driver function to send command
     * @p cmd_idx with argument @p arg to the SDIO/SD/MMC card. @p resp_type
     * specifies the type of the response expected.
     *
     * @warning This function must not be used for application specific
     *          commands. The high-level SDIO/SD/MMC device API function
     *          @ref sdmmc_send_acmd is used instead.
     *
     * To ensure that the @ref sdmmc_send_acmd function is used for
     * application specific commands, the function must fail if the
     * command @p cmd_idx is an application specific command (ACMDx).
     * Use
     * ```
     * assert((cmd_idx & SDMMC_ACMD_PREFIX) == 0);
     * ```
     * in the implementation of this function for that purpose.
     *
     * The response has to be stored word-wise in host byte order in the buffer
     * provided by @p resp as follows:
     *
     * - @p resp_type = SDMMC_NO_R (No Response): @p resp can be NULL
     * - @p resp_tpye = SDMMC_R2 (Long Response):
     *                  @p resp = { R[127:96], R[95:64], R[63:32], R[31:0] }
     * - @p resp_type = anything else (Short Response): @p resp = { R[39:8] }
     *
     * The buffer provided by @p resp can be NULL if the response is not
     * needed. However, the low-level SDIO/SD/MMC peripheral driver must
     * receive the expected response, but does not store it in @p resp
     * in that case.
     *
     * @note R3 does not use CRC7 in CRC field but a fixed value of `0b111111`.
     *       The driver must check the CRC field for this response.
     *
     * @see @ref sdmmc_send_cmd and sdmmc_send_acmd
     * @pre @p dev must not be `NULL`.
     *
     * @param[in]   dev         SDIO/SD/MMC device to be used
     * @param[in]   cmd_idx     Command index
     * @param[in]   arg         Command argument
     * @param[in]   resp_type   Type of response expected
     * @param[out]  resp        Buffer of 32-bit words to store the response if
     *                          needed, otherwise NULL
     *
     * @retval  0           on success
     * @retval  -ENODEV     if card is not present or not usable
     * @retval  -ENOTSUP    if card does not support the command or is in wrong state
     * @retval  -EFAULT     on card status error
     * @retval  -ETIMEDOUT  on timeout condition
     * @retval  -EBADMSG    on CRC7 error in response
     * @retval  -EIO        on not further specified error incl. hardware errors
     */
    int (*send_cmd)(sdmmc_dev_t *dev, sdmmc_cmd_t cmd_idx, uint32_t arg,
                    sdmmc_resp_t resp_type, uint32_t *resp);

    /**
     * @brief Card Initialization and Identification
     *
     * @warning Usually the standard procedure implemented by the high-level
     *          SDIO/SD/MMC device API function @ref sdmmc_card_init is used
     *          for card initialization and identification (recommended).
     *          The low-level SDIO/SD/MMC peripheral driver should implement
     *          its own @ref sdmmc_driver_t::card_init function only in very
     *          special cases, e.g. when special hardware handling is required.
     *          Therefore, this driver function should be set to NULL, that
     *          is the card initialization and identification function is
     *          not implemented by low-level SDIO/SD/MMC peripheral driver
     *
     * @see @ref sdmmc_card_init
     * @pre @p dev must not be `NULL`.
     *
     * @param[in]   dev     SDIO/SD/MMC device to be used
     *
     * @retval  0           on success
     * @retval  -ENODEV     if card is not present or not usable
     * @retval  -ENOTSUP    if card is not supported or can't operate under supplied voltage
     * @retval  -EFAULT     on card status error
     * @retval  -ETIMEDOUT  on card initialization and identification timeout
     * @retval  -EBADMSG    on CRC7 error
     * @retval  -EIO        on not further specified error incl. hardware errors
     */
    int (*card_init)(sdmmc_dev_t *dev);

    /**
     * @brief   Set data bus width
     *
     * Set the data bus width used by the SDIO/SD/MMC peripheral. The
     * function is called by the high-level SDIO/SD/MMC device function
     * @ref sdmmc_card_init at the end of the card initialization and
     * identification procedure to change the data bus width used by the
     * SDIO/SD/MMC peripheral. The data bus width of the card has been
     * already changed at that time by sending the ACMD6 (`SET_BUS_WIDTH`)
     * command.
     *
     * Supported data bus width depend on
     *
     * - the card type,
     * - the mode in which the card operates and
     * - the SDIO/SD/MMC peripheral.
     *
     * In identification mode, always 1-bit data bus width is used. When
     * switching from the identification mode to the data transfer mode,
     * the data bus width is changed. In data transfer mode,
     * the data bus width depends on the type of the used card:
     *
     * - MMCs V3.x support 4-bit data bus width
     * - MMCs V4.x support 8-bit data bus width
     * - SD Memory Cards support 4-bit data bus width
     * - SDIO Cards support 4-bit data bus width
     * - Embedded SDIO devices support 8-bit data bus width
     *
     * The data bus width @p width is the minimum of the data bus width
     * supported by the identified card and the data bus width
     * @ref sdmmc_dev_t::bus_width configured for the SDIO/SD/MMC
     * peripheral. The low-level SDIO/SD/MMC peripheral driver is
     * responsible for correctly setting @ref sdmmc_dev_t::bus_width in the
     * @ref sdmmc_driver_t::init function.
     *
     * @note The @ref set_bus_width function must not change
     *       @ref sdmmc_dev_t::bus_width because it is the configured data
     *       bus width which may be larger than the supported data bus width
     *       of the identified card.
     *
     * @pre @p dev must not be `NULL`.
     *
     * @param[in]   dev     SDIO/SD/MMC device to be used
     * @param[in]   width   Data bus width to be set
     *
     * @retval  0           on success
     * @retval  -ENOTSUP    if SDIO/SD/MMC peripheral does not support the width
     * @retval  -EIO        on not further specified error incl. hardware errors
     */
    int (*set_bus_width)(sdmmc_dev_t *dev, sdmmc_bus_width_t width);

    /**
     * @brief   Set SD CLK signal rate
     *
     * Set the SD CLK rate used by the SDIO/SD/MMC peripheral. The clock
     * frequency in identification mode f_OD is fixed and is 400 kHz. The
     * actual clock frequency in data transfer mode f_PP depends on the
     * SDIO/SD/MMC device and the card used.
     *
     * The function is called by the high-level SDIO/SD/MMC device API function
     * @ref sdmmc_card_init at the end of the card identification procedure
     * with @p rate set to any of the clock rates defined in
     * @ref sdmmc_clock_rate_t. The low-level SDIO/SD/MMC peripheral
     * is responsible to set the clock rate to the highest value it supports
     * for the identified card type.
     *
     * @pre @p dev must not be `NULL`.
     *
     * @param[in]   dev     SDIO/SD/MMC device to be used
     * @param[in]   rate    Clock rate to be set
     *
     * @retval  0           on success
     * @retval  -ENOTSUP    if SDIO/SD/MMC peripheral does not support the clock rate
     * @retval  -EIO        on not further specified error incl. hardware errors
     */
    int (*set_clock_rate)(sdmmc_dev_t *dev, sdmmc_clock_rate_t rate);

#if !IS_USED(MODULE_PERIPH_SDMMC_AUTO_CLK) || DOXYGEN
    /**
     * @brief   Enable or disable the SD CLK signal
     *
     * The function is used by the SDIO/SD/MMC device API functions to enable
     * or disable the SD CLK signal if the SDIO/SD/MMC peripheral driver does
     * not support the Auto CLK feature (periph_sdmmc_auto_clk). It can be
     * left NULL if the SDIO/SD/MMC peripheral driver does not support
     * enabling or disabling the SD CLK signal.
     *
     * @param[in]   dev     SDIO/SD/MMC device to be used
     * @param[in]   enable  enable SD CLK signal on true or disable the
     *                      SD CLK signal on false
     *
     * @retval  0           on success
     * @retval  -EIO        if the SD CLK signal could not be enabled or disabled
     */
    int (*enable_clock)(sdmmc_dev_t *dev, bool enable);
#endif

    /**
     * @brief   Prepare a data transfer
     *
     * Low-level SDIO/SD/MMC peripheral driver function to prepare a data
     * transfer (read or write) in the SDIO/SD/MMC peripheral.
     *
     * It is called by the high-level SDIO/SD/MMC device API function
     * @ref sdmmc_xfer before it sends a block-oriented, stream or
     * byte/multi-byte command to the card to start the transfer from or
     * to the card.
     *
     * A typical activity of this function is the configuration of
     * the DMA transfer.
     *
     * @see @ref sdmmc_xfer
     * @pre @p dev and @p xfer must not be `NULL`.
     *
     * @param[in]   dev     SDIO/SD/MMC device to be used
     * @param[in]   xfer    Transfer descriptor of type @ref sdmmc_xfer_desc_t
     *
     * @retval  0           on success
     * @retval  -EINVAL     on invalid transfer parameters
     * @retval  -EIO        on not further specified error incl. hardware errors
     */
    int (*xfer_prepare)(sdmmc_dev_t *dev, sdmmc_xfer_desc_t *xfer);

    /**
     * @brief   Execute the data transfer
     *
     * Low-level SDIO/SD/MMC peripheral driver function to transfer the data
     * (read or write) by the SDIO/SD/MMC peripheral.
     *
     * It is called by the high-level SDIO/SD/MMC device API function
     * @ref sdmmc_xfer after it sent the block-oriented, stream or
     * byte/multi-byte command to start the transfer from or to the card.
     *
     * The function returns the number of transferred blocks in @p done.
     * Most SDIO/SD/MMC peripherals use a block or byte counter when
     * transferring data, which can be used for this purpose.
     *
     * @note If the SDIO/SD/MMC peripheral does not allow to determine
     *       the number of transferred blocks, the function must return `0`
     *       in @p done in case of an error or @ref sdmmc_xfer_desc_t::block_num
     *       on success.
     *
     * @see @ref sdmmc_xfer
     * @pre @p dev and @p xfer must not be `NULL`. \n
     *      @p data_rd must not be `NULL` for read transfers and \n
     *      @p data_wr must not be `NULL` for write transfers.
     *
     * @param[in]   dev         SDIO/SD/MMC device to be used
     * @param[in]   xfer        Transfer descriptor of type @ref sdmmc_xfer_desc_t
     * @param[in]   data_wr     Buffer with data to write in write transfers, NULL otherwise
     * @param[out]  data_rd     Buffer for data to read in read transfers, NULL otherwise
     * @param[out]  done        Number of blocks transferred, can be `NULL`
     *
     * @retval  0           on success
     * @retval  -ENODEV     if card is not present or not usable
     * @retval  -ENOTSUP    if card does not support a used command or is in wrong state
     * @retval  -EBUSY      if card is busy
     * @retval  -EFAULT     on card status error
     * @retval  -ETIMEDOUT  on timeout condition
     * @retval  -EINVAL     on invalid transfer parameters
     * @retval  -EBADMSG    on CRC7 error in data
     * @retval  -ENOMEM     on RX FIFO overflow or TX FIFO underrun error
     * @retval  -EIO        on not further specified error incl. hardware errors
     */
    int (*xfer_execute)(sdmmc_dev_t *dev, sdmmc_xfer_desc_t *xfer,
                        const void *data_wr, void *data_rd,
                        uint16_t *done);

    /**
     * @brief   Finish the data transfer
     *
     * Low-level SDIO/SD/MMC peripheral driver function to terminate a data
     * transfer (read or write) in the SDIO/SD/MMC peripheral.
     *
     * It is called by the high-level SDIO/SD/MMC device API function
     * @ref sdmmc_xfer after the data transfer has been executed and the
     * stop command (CMD12) has been sent if necessary.
     *
     * @see @ref sdmmc_xfer
     * @pre @p dev and @p xfer must not be `NULL`.
     *
     * @param[in]   dev     SDIO/SD/MMC device to be used
     * @param[in]   xfer    Transfer descriptor of type @ref sdmmc_xfer_desc_t
     *
     * @retval  0           on success
     * @retval  -EIO        on not further specified error incl. hardware errors
     */
    int (*xfer_finish)(sdmmc_dev_t *dev, sdmmc_xfer_desc_t *xfer);

} sdmmc_driver_t;

/**
 * @brief   SDIO/SD/MMC device descriptor
 *
 * The device descriptor holds all required information about the SDIO/SD/MMC
 * device and the card that is used by this decive.
 *
 * @note Most of the information is determined or collected during the card
 *       initialization and identification procedure by the @ref sdmmc_card_init
 *       function. However, some information must be determined and collected
 *       by the low-level SDIO/SD/MMC peripheral driver. These are
 *       - @ref sdmmc_dev_t::driver
 *       - @ref sdmmc_dev_t::present
 *       - @ref sdmmc_dev_t::status
 *       - @ref sdmmc_dev_t::bus_width
 *       - @ref sdmmc_dev_t::spi_mode
 */
typedef struct sdmmc_dev {
    /**
     * Low-level SDIO/SD/MMC peripheral driver. It has to be set by the
     * low-level SDIO/SD/MMC driver during the initialization.
     */
    const sdmmc_driver_t *driver;

    /**
     * The application can register an event callback function of type
     * @ref sdmmc_event_cb_t which is called when one of the defined
     * events (@ref sdmmc_event_t) is generated by the SDIO/SD/MMC driver.
     *
     * @warning The function is called in the ISR context. Do not do anything
     *          comprehensive or time-consuming. Instead, use `thread_flags`,
     *          `event_queue` or `msg` mechanism to inform a thread about
     *          the event, which then handles the event asynchronously in
     *          thread context.
     */
    sdmmc_event_cb_t event_cb;

    /**
     * CID register of the SD/MMC Card, read during the initialization and
     * identification procedure in the @ref sdmmc_card_init function. It is not
     * supported by SDIO cards and reflects the CID of the memory portion in
     * case of Combo cards.
     */
    sdmmc_cid_t cid;

    /**
     * SCR register of the SD Card, read during the initialization and
     * identification procedure in the @ref sdmmc_card_init function. It is not
     * supported by SDIO and MMC cards. It reflects the SCR of the memory
     * portion in case of Combo cards.
     */
    sdmmc_scr_t scr;

    /**
     * CSD register of the SD/MMC Card, read during the initialization and
     * identification procedure in the @ref sdmmc_card_init function. It is not
     * supported by SDIO cards. It reflects the CSD of the memory portion in
     * case of Combo cards.
     */
    sdmmc_csd_t csd;

#if IS_USED(MODULE_SDMMC_MMC)
    /**
     * EXT_CSD register of the MMC Card, read during the initialization and
     * identification procedure int the @ref sdmmc_card_init function.
     */
    sdmmc_ext_csd_t ext_csd;
#endif

    /**
     * Last SDIO/SD/MMC Card status reported in R1 response. It is
     * set by the low-level SDIO/SD/MMC peripheral driver function
     * @ref sdmmc_driver_t::send_cmd.
     */
    uint32_t status;

    /**
     * Relative Card Address (RCA) of the SDIO/SD/MMC Card as determined
     * during the initialization and identification procedure in the
     * @ref sdmmc_card_init function.
     */
    uint16_t rca;

    /**
     * Type of the SDIO/SD/MMC Card as identified during the initialization and
     * identification procedure in the @ref sdmmc_card_init function
     * (default @ref SDMMC_CARD_TYPE_UNKNOWN).
     */
    sdmmc_card_type_t type;

    /**
     * Data bus width supported by the SDIO/SD/MMC device
     * (default @ref SDMMC_BUS_WIDTH_1BIT). It has to be set by the low-level
     * SDIO/SD/MMC peripheral driver function @ref sdmmc_driver_t::init.
     * It is either hard-configured for the SDIO/SD/MMC device or detected
     * during its initialization.
     */
    sdmmc_bus_width_t bus_width;

    /**
     * Indicates whether a card is present. It has to be set by the low-level
     * SDIO/SD/MMC peripheral driver in function @ref sdmmc_driver_t::init and
     * if the card is removed or inserted.
     * Either the CD signal is used if available, or it must be set to
     * `true` by default.
     */
    bool present;

    /**
     * Indicates whether the card is initialized (default `false`).
     * It is set by the initialization and identification procedure in
     * function @ref sdmmc_card_init.
     */
    bool init_done;

    /**
     * Indicates whether the SDIO/SD/MMC peripheral supports the switching
     * to 1.8V (default `false`). It has to be set by the low-level SDIO/SD/MMC
     * peripheral driver function @ref sdmmc_driver_t::init if supported.
     */
    bool s18v_support;

    /**
     * Indicates whether the card supports the switching to 1.8V (default
     * `false`). It is set during the initialization and identification
     * procedure in function @ref sdmmc_card_init.
     */
    bool s18v_allowed;

    /**
     * Indicates whether SPI mode is used by the SDIO/SD/MMC device (default
     * `false`). It has to be set by the low-level SDIO/SD/MMC peripheral driver
     * function @ref sdmmc_driver_t::init.
     */
    bool spi_mode;

} sdmmc_dev_t;

/**
 * @brief   SDIO/SD/MMC device descriptor references as read-only XFA
 *
 * The array contains the references to all SDIO/SD/MMC device descriptors.
 * The i-th device in this array can then be accessed with `sdmmc_devs[i]`.
 * The number of SDIO/SD/MMC device descriptor references defined in this array
 * is `XFA_LEN(sdmmc_devs)`, see @ref SDMMC_NUMOF.
 *
 * @warning To ensure to have the references to all SDIO/SD/MMC device
 *          descriptors in this array, the low-level SDIO/SD/MMC peripheral
 *          drivers must define the references to their SDIO/SD/MMC device
 *          descriptors as XFA members by using the macro
 *          `XFA_CONST(sdmmc_dev_t *, sdmmc_devs, 0)` as shown in the example
 *          below.
 *
 * For example, if the low-level
 * SDIO/SD/MMC peripheral driver defines an MCU-specific SDIO/SD/MMC
 * device descriptor structure `_mcu_sdmmc_dev_t` and defines the device
 * descriptors in an array `_mcu_sdmmc_dev`, it must define the references
 * to them as members of the XFA `sdmmc_devs` as follows:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * typedef struct {
 *     sdmmc_dev_t sdmmc_dev;      // Inherited sdmmc_dev_t struct
 *     const sdmmc_conf_t *config; // SDIO/SD/MMC peripheral config
 *     ...
 * } _mcu_sdmmc_dev_t;
 *
 * static _mcu_sdmmc_dev_t _mcu_sdmmc_devs[] = {
 *     {
 *         ...
 *     },
 *     {
 *         ...
 *     },
 * };
 *
 * XFA_CONST(sdmmc_dev_t * const, sdmmc_devs, 0, _sdmmc_1) = (sdmmc_dev_t * const)&_mcu_sdmmc_devs[0];
 * XFA_CONST(sdmmc_dev_t * const, sdmmc_devs, 0, _sdmmc_2) = (sdmmc_dev_t * const)&_mcu_sdmmc_devs[1];
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 */
#if !DOXYGEN
XFA_USE_CONST(sdmmc_dev_t * const, sdmmc_devs);
#else
sdmmc_dev_t sdmmc_devs[];
#endif

/**
 * @brief   Number of SDIO/SD/MMC devices defined
 */
#define SDMMC_NUMOF   XFA_LEN(sdmmc_dev_t *, sdmmc_devs)

/**
 * @brief   Retrieve SDIO/SD/MMC device descriptor reference from device index
 *
 * The function converts the device index to the corresponding SDIO/SD/MMC
 * device descriptor. See also @ref sdmmc_devs.
 *
 * @param[in]   num   SDIO/SD/MMC peripheral index
 *
 * @retval  pointer to the SDIO/SD/MMC device descriptor at index @p num on success
 * @retval  NULL if @p num is greater than the number of SDIO/SD/MMC device
 *          descriptors
 */
static inline sdmmc_dev_t *sdmmc_get_dev(unsigned num)
{
    return (num < SDMMC_NUMOF) ? sdmmc_devs[num] : NULL;
}

/**
 * @brief   Basic initialization of the given SDIO/SD/MMC device
 *
 * The function calls the low-level SDIO/SD/MMC peripheral driver function
 * @ref sdmmc_driver_t::init for the basic initialization of the SDIO/SD/MMC
 * peripheral including the configuration of used pins.
 *
 * If the `auto_init` module is enabled, it is called automatically during the
 * startup. Otherwise, it has to be called before the SDIO/SD/MMC device is
 * used for the first time.
 *
 * Errors like configuration parameter problems are not signaled by
 * return values, but by using the `assert()`.
 *
 * @see @ref sdmmc_driver_t::init
 * @pre @p dev must not be `NULL`.
 *
 * @param[in]   dev     SDIO/SD/MMC device to initialize
 */
static inline void sdmmc_init(sdmmc_dev_t *dev)
{
    assert(dev);
    assert(dev->driver);

    dev->driver->init(dev);
}

/**
 * @brief   Send command to SDIO/SD/MMC Card and optionally wait for response
 *
 * Send the command @p cmd_idx with argument @p arg to the SDIO/SD/MMC
 * card. @p resp_type specifies the type of the response expected. Addressed
 * (point-to-point) commands are sent to the card that is currently selected.
 *
 * This function just calls the low-level SDIO/SD/MMC peripheral driver function
 * @ref sdmmc_driver_t::send_cmd.
 *
 * @note @ref sdmmc_card_init is called implicitly if necessary.
 *
 * @warning For application specific commands, the @ref sdmmc_send_acmd function
 *          MUST to be used.
 *
 * The response is stored word-wise in host byte order in the buffer
 * provided by @p resp as follows:
 *
 * - @p resp_type = SDMMC_NO_R (No Response): @p resp can be NULL
 * - @p resp_tpye = SDMMC_R2 (Long Response):
 *                  @p resp = { R[127:96], R[95:64], R[63:32], R[31:0] }
 * - @p resp_type = anything else (Short Response): @p resp = { R[39:8] }
 *
 * The buffer provided by @p resp can be NULL if the response is not
 * needed.
 *
 * @note R3 does not use CRC7 in CRC field but a fixed value of `0b111111`.
 *       The low-level SDIO/SD/MMC peripheral driver must not check the CRC
 *       field for this response.
 *
 * @see @ref sdmmc_driver_t::send_cmd
 * @pre @p dev must not be `NULL`.
 *
 * @param[in]   dev         SDIO/SD/MMC device to be used
 * @param[in]   cmd_idx     Command index
 * @param[in]   arg         Command argument
 * @param[in]   resp_type   Type of response expected
 * @param[out]  resp        Buffer of 32-bit words to store the response if
 *                          needed, otherwise NULL
 *
 * @retval  -ENODEV     if card is not present or not usable
 * @retval  -ENOTSUP    if card does not support the command or is in wrong state
 * @retval  -EFAULT     on card status error
 * @retval  -ETIMEDOUT  on timeout condition
 * @retval  -EBADMSG    on CRC7 error in response
 * @retval  -EIO        on not further specified error incl. hardware errors
 */
int sdmmc_send_cmd(sdmmc_dev_t *dev, sdmmc_cmd_t cmd_idx, uint32_t arg,
                   sdmmc_resp_t resp_type, uint32_t *resp);

/**
 * @brief   Send application specific command optionally wait for response
 *
 * Send an application specific command @p cmd_idx with argument @p arg
 * to the SDIO/SD/MMC Card. This function is a convenience function that
 * uses the low-level SDIO/SD/MMC peripheral driver function
 * @ref sdmmc_driver_t::send_cmd. It just sends CMD55 before sending the
 * command with index @p cmd_idx to the card. Addressed (point-to-point)
 * commands are sent to the card that is currently selected.
 *
 * Instead of using this function, the following could also be used:
 * ```c
 * sdmmc_send_cmd(dev, SDMMC_CMD55, RCA, SDMMC_R1, NULL);
 * sdmmc_send_cmd(dev, cmd_idx & ~SDMMC_ACMD_PREFIX, arg, resp_type, resp);
 * ```
 *
 * @note @ref sdmmc_card_init is called implicitly if necessary.
 *
 * @warning The command index must be an ACMD command index, i.e. the command
 *          index ORed with @ref SDMMC_ACMD_PREFIX. Otherwise the function
 *          fails because of an assertion.
 *
 * @p resp_type specifies the response expected.
 *
 * The response is stored word-wise in host byte order in the buffer
 * provided by @p resp as follows:
 *
 * - @p resp_type = SDMMC_NO_R (No Response): @p resp can be NULL
 * - @p resp_tpye = SDMMC_R2 (Long Response):
 *                  @p resp = { R[127:96], R[95:64], R[63:32], R[31:0] }
 * - @p resp_type = anything else (Short Response): @p resp = { R[39:8] }
 *
 * The buffer provided by @p resp can be NULL if the response is not
 * needed. However, the low-level SDIO/SD/MMC peripheral driver must
 * receive the expected response, but does not store it in @p resp
 * in that case.
 *
 * @note R3 does not use CRC7 in CRC field but a fixed value of `0b111111`.
 *       The low-level SDIO/SD/MMC peripheral driver must not check the CRC
 *       field for this response.
 *
 * @pre @p dev must not be `NULL`. @p cmd_idx must be in range 0 to 63.
 *
 * @param[in]   dev         SDIO/SD/MMC device to be used
 * @param[in]   cmd_idx     Command index of an application specific command
 * @param[in]   arg         Command argument
 * @param[in]   resp_type   Type of response expected
 * @param[out]  resp        Buffer of 32-bit words to store the response
 *
 * @retval  -ENODEV     if card is not present or not usable
 * @retval  -ENOTSUP    if card does not support the command or is in wrong state
 * @retval  -EFAULT     on card status error
 * @retval  -ETIMEDOUT  on timeout condition
 * @retval  -EBADMSG    on CRC7 error in response
 * @retval  -EIO        on not further specified error incl. hardware errors
 */
int sdmmc_send_acmd(sdmmc_dev_t *dev, sdmmc_cmd_t cmd_idx, uint32_t arg,
                    sdmmc_resp_t resp_type, uint32_t *resp);

/**
 * @brief Card Initialization and Identification
 *
 * This function identifies the type of used SDIO/SD/MMC Cards and
 * initializes them according to the standardized procedure specified in:
 *
 * - Physical Layer Simplified Specification Version 9.00,
 *   Section 4.2.3 Card Initialization and Identification Process, pp. 43
 *   [[sdcard.org](https://www.sdcard.org)]
 * - SDIO Simplified Specification Version 3.00,
 *   Section 3.1.2 Initialization by I/O Aware Host, pp. 17
 *   [[sdcard.org](https://www.sdcard.org)]
 * - SD Host Controller Simplified Specification Version 4.20,
 *   Section 3.6 Card Initialization and Identification (for SD I/F), pp. 160
 *   [[sdcard.org](https://www.sdcard.org)]
 * - JEDEC Standard No. JESD84-B42, MultiMediaCard (MMC) Electrical
 *   Standard, High Capacity (MMCA, 4.2),
 *   Section 7.2 Card Identification Mode, p. 24 and Figure 16 on p. 16
 *   [[jedec.org](https://www.jedec.org)]
 *
 * Identified card types are:
 * - SD Memory Cards with Standard Capacity (SDSC) Version 1.x, Version 2.x+
 * - SD Memory Cards with High or Extended Capacity (SDHC/SDXC)
 * - SDIO Cards, not supported yet
 * - Combined SDIO/SD Memory Cards (Combo Cards), SDIO part not supported yet
 * - MultiMedia Cards (MMC) and Embedded Multimedia Cards (eMMC)
 *
 * @warning  If the low-level SDIO/SD/MMC peripheral driver defines its own
 *           @ref sdmmc_driver_t::card_init function, this function is used
 *           instead.\n
 *           However, the low-level SDIO/SD/MMC peripheral driver should
 *           define its own @ref sdmmc_driver_t::card_init function only in
 *           very special cases, e.g. when special hardware handling is
 *           required. Otherwise it is strongly recommended to set
 *           sdmmc_driver_t::card_init to `NULL` and to use the default
 *           procedure implemented by @ref sdmmc_send_acmd.
 *
 * @see @ref sdmmc_driver_t::card_init
 * @pre @p dev must not be `NULL`.
 *
 * @param[in]   dev     SDIO/SD/MMC device to be used
 *
 * @retval  0           on success
 * @retval  -ENODEV     if card is not present or not usable
 * @retval  -ENOTSUP    if card is not supported or can't operate under supplied voltage
 * @retval  -EFAULT     on card status error
 * @retval  -ETIMEDOUT  on card initialization and identification timeout
 * @retval  -EBADMSG    on CRC7 error
 * @retval  -EIO        on not further specified error incl. hardware errors
 */
int sdmmc_card_init(sdmmc_dev_t *dev);

/**
 * @brief   Perform a data transfer with the selected card or embedded device
 *
 * This high-level SDIO/SD/MMC device API function performs a read or write data
 * transfer either
 *
 * - block-oriented with a number of blocks of the same size (SD and MMC)
 * - as stream until the stop command (CMD12) is sent by the host (MMC only)
 * - in byte or multi-byte mode (SDIO only)
 *
 * The possible block sizes @p block_size depend on the card and mode
 * used. For block-oriented data transfers, the block size is usually
 * 512 bytes, but may differ for MMCs and SD Cards with Standard Capacity
 * Version 1.x (SDSC):
 *
 * - MMC: block size can be 1, 2, 4, ..., 4096 bytes (as power of 2)
 * - SDSC V1.x: block size can be 512, 1024 or 2048 bytes
 * - SDSC V2.x and later, SDSC/SDHC: block size is 512 bytes
 *
 * The block size for SDIO transfers in byte or multi-byte mode, if
 * supported, can be in the range of 1 to 512 bytes. The number of
 * blocks @p block_num MUST be 1 in this case.
 *
 * For a data transfer as a stream, the block size @p block_size and the
 * number of blocks @p block_num are 0.
 *
 * @note Some block-oriented data transfers such as reading the SD Card
 *       Configuration register (SCR) with ACMD51 may use the
 *       @ref sdmmc_xfer function with smaller block size, for example
 *       only 8 bytes. The low-level SDIO/SD/MMC peripheral driver MUST
 *       support block sizes smaller than the usual block size used
 *       for block-oriented operations.
 *
 * The @ref sdmmc_xfer function uses the low-level SDIO/SD/MMC peripheral
 * driver functions @ref sdmmc_driver_t::xfer_prepare to prepare the data
 * transfer, @ref sdmmc_driver_t::xfer_execute to perform the data transfer,
 * and @ref sdmmc_driver_t::xfer_finish to complete the data transfer.
 * In detail:
 * ```c
 * _wait_for_ready(dev);
 * if (cmd_idx & SDMMC_ACMD_PREFIX) {
 *     sdmmc_send_cmd(dev, SDMMC_CMD55, ...);
 * }
 * dev->driver->xfer_prepare(dev, xfer);
 * sdmmc_send_cmd(dev, cmd_idx, ...);
 * dev->driver->xfer_execute(dev, xfer, ...);
 * if (block_num > 1) {
 *    sdmmc_send_cmd(dev, SDMMC_CMD12, ...);
 * }
 * dev->driver->xfer_finish(dev, xfer);
 * ```
 * @p xfer is the transfer descriptor of type @ref sdmmc_xfer_desc_t that
 * contains all transfer parameters.
 *
 * If the parameter @p done is not `NULL`, the function returns the number of
 * transferred blocks.
 *
 * @warning
 * The buffers specified by @p data_wr and @p data_rd may need to be
 * word-aligned depending on CPU-specific requirements. @ref sdmmc_buf_t
 * or @ref SDMMC_CPU_DMA_REQUIREMENTS have to be used to define such buffers:
 * ```c
 * sdmmc_buf_t buffer[SDMMC_SDHC_BLOCK_SIZE];
 * ...
 * SDMMC_CPU_DMA_REQUIREMENTS uint8_t buffer[SDMMC_SDHC_BLOCK_SIZE];
 * ```
 *
 * @note @ref sdmmc_card_init is called implicitly if necessary.
 *
 * @see @ref sdmmc_driver_t::xfer_prepare, @ref sdmmc_driver_t::xfer_execute
 *       and @ref sdmmc_driver_t::xfer_finish
 * @pre @p dev must not be `NULL`. \n
 *      @p data_rd must not be `NULL` for read transfers and
 *      @p data_wr must not be `NULL` for write transfers.
 *
 * @param[in]   dev         SDIO/SD/MMC device to be used
 * @param[in]   cmd_idx     Command index or application specific command index
 * @param[in]   arg         Command argument
 * @param[in]   block_size  Block size dependent on card and mode used
 *                          - 512 bytes for block-oriented transfers (SDSC V2.x/SDHC/SDXC)
 *                          - 1, 2, 4, ... 4096 for block-oriented transfers (MMC)
 *                          - 512, 1024, 2048 for block-oriented transfers (SDSC V1.x)
 *                          - 1...512 bytes in byte/multibyte mode (SDIO)
 *                          - 0 in stream mode (SDIO)
 * @param[in]   block_num   Number of blocks:
 *                          - 1, ... for block-oriented transfers (SD Card/MMC)
 *                          - 1 in byte/multibyte mode (SDIO)
 *                          - 0 in stream mode (MMC)
 * @param[in]   data_wr     Buffer with data to write in write transfers, NULL otherwise
 * @param[out]  data_rd     Buffer for data to read in read transfers, NULL otherwise
 * @param[out]  done        Number of transferred blocks, can be `NULL`
 *
 * @retval  0           on success
 * @retval  -ENODEV     if card is not present or not usable
 * @retval  -ENOTSUP    if card does not support a used command or is in wrong state
 * @retval  -EBUSY      if card is busy
 * @retval  -EFAULT     on card status error
 * @retval  -ETIMEDOUT  on timeout condition
 * @retval  -EINVAL     on invalid transfer parameters
 * @retval  -EBADMSG    on CRC7 error
 * @retval  -ENOMEM     on RX FIFO overflow or TX FIFO underrun error
 * @retval  -EIO        on not further specified error incl. hardware errors
 */
int sdmmc_xfer(sdmmc_dev_t *dev, sdmmc_cmd_t cmd_idx, uint32_t arg,
               uint16_t block_size, uint16_t block_num,
               const void *data_wr, void *data_rd, uint16_t *done);

/**
 * @brief   Read a number of blocks
 *
 * Read @p block_num blocks with size @p block_size from @p dev starting at
 * block address @p block_addr to buffer @p data.
 *
 * The starting block address is always specified as block address independent
 * on the actual addressing scheme of used card. The driver takes care of
 * mapping to byte addressing if needed.
 *
 * If the parameter @p done is not `NULL`, the function returns the number of
 * read blocks.
 *
 * @warning
 * The buffer @p data may need to be word-aligned depending on CPU-specific
 * requirements. @ref sdmmc_buf_t or @ref SDMMC_CPU_DMA_REQUIREMENTS have to
 * be used to define the buffer:
 * ```c
 * sdmmc_buf_t buffer[SDMMC_SDHC_BLOCK_SIZE];
 * ...
 * SDMMC_CPU_DMA_REQUIREMENTS uint8_t buffer[SDMMC_SDHC_BLOCK_SIZE];
 * ```
 *
 * @note @ref sdmmc_card_init is called implicitly if necessary.
 *
 * @pre @p dev and @p data must not be `NULL`.
 *
 * @param[in]   dev         SDIO/SD/MMC device to be used
 * @param[in]   block_addr  Start address to read from given as block address
 * @param[in]   block_size  Block size dependent on card and mode used
 *                          - 512 bytes for block-oriented transfers (SDSC V2.x/SDHC/SDXC)
 *                          - 1, 2, 4, ... 4096 for block-oriented transfers (MMC)
 *                          - 512, 1024, 2048 for block-oriented transfers (SDSC V1.x)
 *                          - 1...512 bytes in byte/multibyte mode (SDIO only)
 *                          - 0 in stream mode (MMC only)
 * @param[in]   block_num   Number of blocks:
 *                          - 1, ... for block-oriented transfers (SD Card/MMC)
 *                          - 1 in byte/multibyte mode (SDIO only)
 *                          - 0 in stream mode (MMC only)
 * @param[out]  data        Buffer for read data
 * @param[out]  done        Number of read blocks, can be `NULL`
 *
 * @retval  0           on success, @p block_num blocks were read successfully
 * @retval  -ENODEV     if card is not present or not usable
 * @retval  -ENOTSUP    if card does not support a used command or is in wrong state
 * @retval  -EBUSY      if card is busy
 * @retval  -EFAULT     on card status error
 * @retval  -ETIMEDOUT  on timeout condition
 * @retval  -EINVAL     on invalid transfer parameters
 * @retval  -EBADMSG    on CRC7 error
 * @retval  -ENOMEM     on RX FIFO overflow error
 * @retval  -EIO        on not further specified error incl. hardware errors
 */
int sdmmc_read_blocks(sdmmc_dev_t *dev,
                      uint32_t block_addr, uint16_t block_size,
                      uint16_t block_num, void *data, uint16_t *done);

/**
 * @brief   Write a number of blocks
 *
 * Write @p block_num blocks with size @p block_size to @p dev starting at
 * block address @p block_addr from buffer @p data.
 *
 * The starting block address is always specified as block address independent
 * on the actual addressing scheme of used card. The driver takes care of
 * mapping to byte addressing if needed.
 *
 * If the parameter @p done is not `NULL`, the function returns the number of
 * written blocks.
 *
 * @warning
 * The buffer @p data may need to be word-aligned depending on CPU-specific
 * requirements. @ref sdmmc_buf_t or @ref SDMMC_CPU_DMA_REQUIREMENTS have to
 * be used to define the buffer:
 * ```c
 * sdmmc_buf_t buffer[SDMMC_SDHC_BLOCK_SIZE];
 * ...
 * SDMMC_CPU_DMA_REQUIREMENTS uint8_t buffer[SDMMC_SDHC_BLOCK_SIZE];
 * ```
 *
 * @note @ref sdmmc_card_init is called implicitly if necessary.
 *
 * @pre @p dev and @p data must not be `NULL`.
 *
 * @param[in]   dev         SDIO/SD/MMC device to be used
 * @param[in]   block_addr  Start address to write to given as block address
 * @param[in]   block_size  Block size dependent on card and mode used
 *                          - 512 bytes for block-oriented transfers (SDSC V2.x/SDHC/SDXC)
 *                          - 1, 2, 4, ... 4096 for block-oriented transfers (MMC)
 *                          - 512, 1024, 2048 for block-oriented transfers (SDSC V1.x)
 *                          - 1...512 bytes in byte/multibyte mode (SDIO only)
 *                          - 0 in stream mode (MMC only)
 * @param[in]   block_num   Number of blocks:
 *                          - 1, ... for block-oriented transfers (SD and MMC)
 *                          - 1 in byte/multibyte mode (SDIO only)
 *                          - 0 in stream mode (MMC only)
 * @param[in]   data        Buffer with data to write
 * @param[out]  done        Number of blocks written, can be `NULL`
 *
 * @retval  0           on success, @p block_num blocks were read successfully
 * @retval  -ENODEV     if card is not present or not usable
 * @retval  -ENOTSUP    if card does not support a used command or is in wrong state
 * @retval  -EBUSY      if card is busy
 * @retval  -EFAULT     on card status error
 * @retval  -ETIMEDOUT  on timeout condition
 * @retval  -EINVAL     on invalid transfer parameters
 * @retval  -EBADMSG    on CRC7 error
 * @retval  -ENOMEM     on TX FIFO underrun error
 * @retval  -EIO        on not further specified error incl. hardware errors
 */
int sdmmc_write_blocks(sdmmc_dev_t *dev, uint32_t block_addr,
                       uint16_t block_size, uint16_t block_num,
                       const void *data, uint16_t *done);

/**
 * @brief   Erase a number of blocks
 *
 * Erase @p block_num blocks starting at block address @p block_addr on
 * SD/MMC Card device.
 *
 * The starting block address is always specified as block address independent
 * on the actual addressing scheme of used card. The driver takes care of
 * mapping to byte addressing if needed.
 *
 * @note    This function is only available for SD Memory Cards, MMC Cards
 *          or the SD Memory Card portion of a combined SDIO/SD Memory Card
 *          (Combo Card). Calling this function for a SDIO only card returns
 *          the `-ENOTSUP` error.
 *
 * @note @ref sdmmc_card_init is called implicitly if necessary.
 *
 * @pre @p dev must not be `NULL`. @p block_num has to be greater than 0.
 *
 * @param[in]   dev         SD/MMC device to be used
 * @param[in]   block_addr  Start address for erase given as block address
 * @param[in]   block_num   Number of blocks to be erased
 *
 * @retval  0           on success, @p block_num blocks were erased successfully
 * @retval  -ENODEV     if card is not present or not usable
 * @retval  -ENOTSUP    if card does not support erase operation or a command used
 * @retval  -EBUSY      if card is busy
 * @retval  -EFAULT     on card status error
 * @retval  -ETIMEDOUT  on timeout condition
 * @retval  -EBADMSG    on CRC7 error in response
 * @retval  -EINVAL     on invalid erase parameters
 * @retval  -EIO        on not further specified error incl. hardware errors
 */
int sdmmc_erase_blocks(sdmmc_dev_t *dev,
                       uint32_t block_addr, uint16_t block_num);

/**
 * @brief   Read SD Status Register
 *
 * Read the SD Status register of a SD Memory Card using ACMD13 and stores
 * the results in the @p sds of type @ref sdmmc_sd_status_t.
 *
 * @note    This function is only available for SD Memory Cards
 *          or the SD Memory Card portion of a combined SDIO/SD Memory Card
 *          (Combo Card). Calling this function for a SDIO only card returns
 *          the `-ENOTSUP` error.
 *
 * @note @ref sdmmc_card_init is called implicitly if necessary.
 *
 * @pre @p dev and @p sds must not be `NULL`.
 *
 * @param[in]   dev     SD device to be used
 * @param[out]  sds     SD Status register content
 *
 * @retval  0           on success
 * @retval  -ENODEV     if card is not present or not usable
 * @retval  -ENOTSUP    if card does not support the operation
 * @retval  -EBUSY      if card is busy
 * @retval  -EFAULT     on card status error
 * @retval  -ETIMEDOUT  on timeout condition
 * @retval  -EINVAL     on invalid transfer parameters
 * @retval  -EBADMSG    on CRC7 error
 * @retval  -ENOMEM     on RX FIFO overflow error
 * @retval  -EIO        on not further specified error incl. hardware errors
 */
int sdmmc_read_sds(sdmmc_dev_t *dev, sdmmc_sd_status_t *sds);

/**
 * @brief   Get Capacity of SD/MMC Card
 *
 * Get the capacity of a SD/MMC Card device.
 *
 * @note    This function is only available for SD Memory Cards, MMC Cards
 *          or the SD Memory Card portion of a combined SDIO/SD Memory Card
 *          (Combo Card). Calling this function for a SDIO only card returns
 *          the `-ENOTSUP` error.
 *
 * @note @ref sdmmc_card_init is called implicitly if necessary.
 *
 * @pre @p dev must not be `NULL`.
 *
 * @param[in]   dev     SD/MMC device to be used
 *
 * @return  the capacity in in byte or 0 on error
 */
uint64_t sdmmc_get_capacity(sdmmc_dev_t *dev);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* SDMMC_SDMMC_H */
