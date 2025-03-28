/*
 * Copyright (C) 2016 Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef SDCARD_SPI_H
#define SDCARD_SPI_H

/**
 * @defgroup    drivers_sdcard_spi SPI SD Card driver
 * @ingroup     drivers_storage
 * @brief       Driver for reading and writing SD Cards using the SPI.
 * @anchor      drivers_sdcard_spi
 * @{
 *
 * @file
 * @brief       Public interface for the sdcard_spi driver.
 *
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "periph/spi.h"
#include "periph/gpio.h"
#include "stdbool.h"

#define SD_HC_BLOCK_SIZE      (512)  /**< size of a single block on SDHC cards */
#define SDCARD_SPI_INIT_ERROR (-1)   /**< returned on failed init */
#define SDCARD_SPI_OK         (0)    /**< returned on successful init */

#define SD_SIZE_OF_OID 2 /**< OID (OEM/application ID field in CID reg) */
#define SD_SIZE_OF_PNM 5 /**< PNM (product name field in CID reg) */

/**
 * @brief   CID register see section 5.2 in SD-Spec v5.00
 */
typedef struct {
    uint8_t MID;              /**< Manufacturer ID */
    char OID[SD_SIZE_OF_OID]; /**< OEM/Application ID*/
    char PNM[SD_SIZE_OF_PNM]; /**< Product name */
    uint8_t PRV;              /**< Product revision */
    uint32_t PSN;             /**< Product serial number */
    uint16_t MDT;             /**< Manufacturing date */
    uint8_t CID_CRC;          /**< CRC7 checksum */
} cid_t;

/**
 * @brief   CSD register with csd structure version 1.0
 *          see section 5.3.2 in SD-Spec v5.00
 */
typedef struct {
    uint8_t CSD_STRUCTURE : 2;        /**< see section 5.3.2 in SD-Spec v5.00 */
    uint8_t TAAC : 8;                 /**< see section 5.3.2 in SD-Spec v5.00 */
    uint8_t NSAC : 8;                 /**< see section 5.3.2 in SD-Spec v5.00 */
    uint8_t TRAN_SPEED : 8;           /**< see section 5.3.2 in SD-Spec v5.00 */
    uint16_t CCC : 12;                /**< see section 5.3.2 in SD-Spec v5.00 */
    uint8_t READ_BL_LEN : 4;          /**< see section 5.3.2 in SD-Spec v5.00 */
    uint8_t READ_BL_PARTIAL : 1;      /**< see section 5.3.2 in SD-Spec v5.00 */
    uint8_t WRITE_BLK_MISALIGN : 1;   /**< see section 5.3.2 in SD-Spec v5.00 */
    uint8_t READ_BLK_MISALIGN : 1;    /**< see section 5.3.2 in SD-Spec v5.00 */
    uint8_t DSR_IMP : 1;              /**< see section 5.3.2 in SD-Spec v5.00 */
    uint16_t C_SIZE : 12;             /**< see section 5.3.2 in SD-Spec v5.00 */
    uint8_t VDD_R_CURR_MIN : 3;       /**< see section 5.3.2 in SD-Spec v5.00 */
    uint8_t VDD_R_CURR_MAX : 3;       /**< see section 5.3.2 in SD-Spec v5.00 */
    uint8_t VDD_W_CURR_MIN : 3;       /**< see section 5.3.2 in SD-Spec v5.00 */
    uint8_t VDD_W_CURR_MAX : 3;       /**< see section 5.3.2 in SD-Spec v5.00 */
    uint8_t C_SIZE_MULT : 3;          /**< see section 5.3.2 in SD-Spec v5.00 */
    uint8_t ERASE_BLK_EN : 1;         /**< see section 5.3.2 in SD-Spec v5.00 */
    uint8_t SECTOR_SIZE : 7;          /**< see section 5.3.2 in SD-Spec v5.00 */
    uint8_t WP_GRP_SIZE : 7;          /**< see section 5.3.2 in SD-Spec v5.00 */
    uint8_t WP_GRP_ENABLE : 1;        /**< see section 5.3.2 in SD-Spec v5.00 */
    uint8_t R2W_FACTOR : 3;           /**< see section 5.3.2 in SD-Spec v5.00 */
    uint8_t WRITE_BL_LEN : 4;         /**< see section 5.3.2 in SD-Spec v5.00 */
    uint8_t WRITE_BL_PARTIAL : 1;     /**< see section 5.3.2 in SD-Spec v5.00 */
    uint8_t FILE_FORMAT_GRP : 1;      /**< see section 5.3.2 in SD-Spec v5.00 */
    uint8_t COPY : 1;                 /**< see section 5.3.2 in SD-Spec v5.00 */
    uint8_t PERM_WRITE_PROTECT : 1;   /**< see section 5.3.2 in SD-Spec v5.00 */
    uint8_t TMP_WRITE_PROTECT : 1;    /**< see section 5.3.2 in SD-Spec v5.00 */
    uint8_t FILE_FORMAT : 2;          /**< see section 5.3.2 in SD-Spec v5.00 */
    uint8_t CSD_CRC : 8;              /**< see section 5.3.2 in SD-Spec v5.00 */
} csd_v1_t;

/**
 * @brief   CSD register with csd structure version 2.0
 *          see section 5.3.3 in SD-Spec v5.00
 */
typedef struct {
    uint8_t CSD_STRUCTURE : 2;        /**< see section 5.3.3 in SD-Spec v5.00 */
    uint8_t TAAC : 8;                 /**< see section 5.3.3 in SD-Spec v5.00 */
    uint8_t NSAC : 8;                 /**< see section 5.3.3 in SD-Spec v5.00 */
    uint8_t TRAN_SPEED : 8;           /**< see section 5.3.3 in SD-Spec v5.00 */
    uint16_t CCC : 12;                /**< see section 5.3.3 in SD-Spec v5.00 */
    uint8_t READ_BL_LEN : 4;          /**< see section 5.3.3 in SD-Spec v5.00 */
    uint8_t READ_BL_PARTIAL : 1;      /**< see section 5.3.3 in SD-Spec v5.00 */
    uint8_t WRITE_BLK_MISALIGN : 1;   /**< see section 5.3.3 in SD-Spec v5.00 */
    uint8_t READ_BLK_MISALIGN : 1;    /**< see section 5.3.3 in SD-Spec v5.00 */
    uint8_t DSR_IMP : 1;              /**< see section 5.3.3 in SD-Spec v5.00 */
    uint32_t C_SIZE : 22;             /**< see section 5.3.3 in SD-Spec v5.00 */
    uint8_t ERASE_BLK_EN : 1;         /**< see section 5.3.3 in SD-Spec v5.00 */
    uint8_t SECTOR_SIZE : 7;          /**< see section 5.3.3 in SD-Spec v5.00 */
    uint8_t WP_GRP_SIZE : 7;          /**< see section 5.3.3 in SD-Spec v5.00 */
    uint8_t WP_GRP_ENABLE : 1;        /**< see section 5.3.3 in SD-Spec v5.00 */
    uint8_t R2W_FACTOR : 3;           /**< see section 5.3.3 in SD-Spec v5.00 */
    uint8_t WRITE_BL_LEN : 4;         /**< see section 5.3.3 in SD-Spec v5.00 */
    uint8_t WRITE_BL_PARTIAL : 1;     /**< see section 5.3.3 in SD-Spec v5.00 */
    uint8_t FILE_FORMAT_GRP : 1;      /**< see section 5.3.3 in SD-Spec v5.00 */
    uint8_t COPY : 1;                 /**< see section 5.3.3 in SD-Spec v5.00 */
    uint8_t PERM_WRITE_PROTECT : 1;   /**< see section 5.3.3 in SD-Spec v5.00 */
    uint8_t TMP_WRITE_PROTECT : 1;    /**< see section 5.3.3 in SD-Spec v5.00 */
    uint8_t FILE_FORMAT : 2;          /**< see section 5.3.3 in SD-Spec v5.00 */
    uint8_t CSD_CRC : 8;              /**< see section 5.3.3 in SD-Spec v5.00 */
} csd_v2_t;

/**
 * @brief   CSD register (see section 5.3 in SD-Spec v5.00)
 */
typedef union {
    csd_v1_t v1;   /**< see section 5.3.2 in SD-Spec v5.00 */
    csd_v2_t v2;   /**< see section 5.3.3 in SD-Spec v5.00 */
} csd_t;

/**
 * @brief   SD status register (see section 4.10.2 in SD-Spec v5.00)
 */
typedef struct {
    uint32_t SIZE_OF_PROTECTED_AREA : 32;   /**< see section 4.10.2 in SD-Spec v5.00 */
    uint32_t SUS_ADDR : 22;                 /**< see section 4.10.2.12 in SD-Spec v5.00 */
    uint32_t VSC_AU_SIZE : 10;              /**< see section 4.10.2.11 in SD-Spec v5.00 */
    uint16_t SD_CARD_TYPE : 16;             /**< see section 4.10.2 in SD-Spec v5.00 */
    uint16_t ERASE_SIZE : 16;               /**< see section 4.10.2.5 in SD-Spec v5.00 */
    uint8_t  SPEED_CLASS : 8;               /**< see section 4.10.2.2 in SD-Spec v5.00 */
    uint8_t  PERFORMANCE_MOVE : 8;          /**< see section 4.10.2.3 in SD-Spec v5.00 */
    uint8_t  VIDEO_SPEED_CLASS : 8;         /**< see section 4.10.2.10 in SD-Spec v5.00 */
    uint8_t  ERASE_TIMEOUT : 6;             /**< see section 4.10.2.6 in SD-Spec v5.00 */
    uint8_t  ERASE_OFFSET : 2;              /**< see section 4.10.2.7 in SD-Spec v5.00 */
    uint8_t  UHS_SPEED_GRADE : 4;           /**< see section 4.10.2.8 in SD-Spec v5.00 */
    uint8_t  UHS_AU_SIZE : 4;               /**< see section 4.10.2.9 in SD-Spec v5.00 */
    uint8_t  AU_SIZE : 4;                   /**< see section 4.10.2.4 in SD-Spec v5.00 */
    uint8_t  DAT_BUS_WIDTH : 2;             /**< see section 4.10.2 in SD-Spec v5.00 */
    uint8_t  SECURED_MODE : 1;              /**< see section 4.10.2 in SD-Spec v5.00 */
} sd_status_t;

/**
 * @brief   version type of SD-card
 */
typedef enum {
    SD_V2,                 /**< SD version 2  */
    SD_V1,                 /**< SD version 1  */
    MMC_V3,                /**< MMC version 3 */
    SD_UNKNOWN             /**< SD-version unknown */
} sd_version_t;

/**
 * @brief   sdcard_spi r/w-operation return values
 */
typedef enum {
    SD_RW_OK = 0,           /**< no error */
    SD_RW_NO_TOKEN,         /**< no token was received (on block read) */
    SD_RW_TIMEOUT,          /**< cmd timed out (not-busy-state wasn't entered) */
    SD_RW_RX_TX_ERROR,      /**< error while performing SPI read/write */
    SD_RW_WRITE_ERROR,      /**< data-packet response indicates error */
    SD_RW_CRC_MISMATCH,     /**< CRC-mismatch of received data */
    SD_RW_NOT_SUPPORTED     /**< operation not supported on used card */
} sd_rw_response_t;

/**
 * @brief   sdcard_spi device params
 */
typedef struct {
    spi_t spi_dev;          /**< SPI bus used */
    gpio_t cs;              /**< pin connected to the DAT3 sd pad */
    gpio_t clk;             /**< pin connected to the CLK sd pad */
    gpio_t mosi;            /**< pin connected to the CMD sd pad*/
    gpio_t miso;            /**< pin connected to the DAT0 sd pad*/
    gpio_t power;           /**< pin that controls sd power circuit*/
    bool power_act_high;    /**< true if card power is enabled by 'power'-pin HIGH*/
} sdcard_spi_params_t;

/**
 * @brief   Device descriptor for sdcard_spi
 */
typedef struct {
    const sdcard_spi_params_t *params;    /**< parameters for pin and spi config */
    spi_clk_t spi_clk;      /**< active SPI clock speed */
    bool use_block_addr;    /**< true if block addressing (vs. byte addressing) is used */
    bool init_done;         /**< set to true once the init procedure completed successfully */
    sd_version_t card_type; /**< version of SD-card */
    int csd_structure;      /**< version of the CSD register structure */
    cid_t cid;              /**< CID register */
    csd_t csd;              /**< CSD register */
} sdcard_spi_t;

/**
 * @brief  Initializes the sd-card with the given parameters in sdcard_spi_t structure.
 *
 * The init procedure also takes care of initializing the spi peripheral to
 * master mode and performing all necessary steps to set the sd-card to spi-mode.
 * Reading the CID and CSD registers is also done within this routine and their
 * values are copied to the given sdcard_spi_t struct.
 *
 * @param[out] card    the device descriptor
 * @param[in]  params  parameters for this device (pins and spi device are
 *                     initialized by this driver)
 *
 * @return             0 if the card could be initialized successfully
 * @return             false if an error occurred while initializing the card
 */
int sdcard_spi_init(sdcard_spi_t *card, const sdcard_spi_params_t *params);

/**
 * @brief   Reads data blocks (usually multiples of 512 Bytes) from card to buffer.
 *
 * @param[in] card        Initialized sd-card struct
 * @param[in] blockaddr   Start address to read from. Independent of the actual
 *                        addressing scheme of the used card the address needs
 *                        to be given as block address (e.g. 0, 1, 2...
 *                        NOT: 0, 512... ). The driver takes care of mapping
 *                        to byte addressing if needed.
 * @param[out] data       Buffer to store the read data in. The user is
 *                        responsible for providing a suitable buffer size.
 * @param[in]  blocksize  Size of data blocks. For now only 512 byte blocks
 *                        are supported because only older (SDSC) cards support
 *                        variable blocksizes anyway. With SDHC/SDXC-cards this
 *                        is always fixed to 512 bytes. SDSC cards are
 *                        automatically forced to use 512 byte as blocksize by
 *                        the init procedure.
 * @param[in]  nblocks    Number of blocks to read
 * @param[out] state      Contains information about the error state if
 *                        something went wrong (if return value is lower than nblocks).
 *
 * @return  number of successfully read blocks (0 if no block was read).
 */
int sdcard_spi_read_blocks(sdcard_spi_t *card, uint32_t blockaddr,
                           void *data, uint16_t blocksize,
                           uint16_t nblocks, sd_rw_response_t *state);

/**
 * @brief   Writes data blocks (usually multiples of 512 Bytes) from buffer to card.
 *
 * @param[in] card        Initialized sd-card struct
 * @param[in] blockaddr   Start address to read from. Independent of the actual
 *                        addressing scheme of the used card the address needs
 *                        to be given as block address (e.g. 0, 1, 2...
 *                        NOT: 0, 512... ). The driver takes care of mapping to
 *                        byte addressing if needed.
 * @param[out] data       Buffer that contains the data to be sent.
 * @param[in]  blocksize  Size of data blocks. For now only 512 byte blocks
 *                        are supported because only older (SDSC) cards support
 *                        variable blocksizes anyway. With SDHC/SDXC-cards this
 *                        is always fixed to 512 bytes. SDSC cards are
 *                        automatically forced to use 512 byte as blocksize by
 *                        the init procedure.
 * @param[in]  nblocks    Number of blocks to write
 * @param[out] state      Contains information about the error state if
 *                        something went wrong
 *                         (if return value is lower than nblocks).
 *
 * @return  number of successfully written blocks (0 if no block was written).
 */
int sdcard_spi_write_blocks(sdcard_spi_t *card, uint32_t blockaddr,
                            const void *data, uint16_t blocksize,
                            uint16_t nblocks, sd_rw_response_t *state);

/**
 * @brief                 Gets the capacity of the card.
 *
 * @param[in] card        Initialized sd-card struct
 *
 * @return                capacity of the card in bytes
 */
uint64_t sdcard_spi_get_capacity(sdcard_spi_t *card);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* SDCARD_SPI_H */
