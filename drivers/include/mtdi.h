/*
 * Copyright (C) 2016  OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mtdi Memory Technology Device
 * @ingroup     drivers
 * @brief       Low level Memory Technology Device interface
 *
 * Generic memory technology device interface
 *
 * @{
 *
 * @file
 *
 * @author      Aurelien Gonce <aurelien.gonce@altran.com>
 */

#ifndef __MTDI_H_
#define __MTDI_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Results of Memory Functions */
typedef enum {
    MTDI_RES_OK = 0,     /**< 0: Successful */
    MTDI_RES_ERROR,      /**< 1: R/W Error */
    MTDI_RES_WRPRT,      /**< 2: Write Protected */
    MTDI_RES_NOTRDY,     /**< 3: Not Ready */
    MTDI_RES_PARERR      /**< 4: Invalid Parameter */
} mtdi_result_t;

/**
 * @name Disk Status Bits
 * @{
 */
typedef enum {
    MTDI_STA_INIT    = 0x00, /**< Drive initialized */
    MTDI_STA_NOINIT  = 0x01, /**< Drive not initialized */
    MTDI_STA_NODISK  = 0x02, /**< No medium in the drive */
    MTDI_STA_PROTECT = 0x04  /**< Write protected */
} mtdi_sta_t;
/** @} */

/**
 * @name Command code for disk_ioctrl function
 * @{
 */

/**
 * @name Generic ioctl command
 * @{
 */
#define MTDI_CTRL_SYNC           0   /**< Flush disk cache (for write functions) */
#define MTDI_GET_PAGE_COUNT      1   /**< Get media size (for only f_mkfs()) */
#define MTDI_GET_PAGE_SIZE       2   /**< Get page size (for multiple sector size (_MAX_SS >= 1024)) */
#define MTDI_GET_SECTOR_COUNT    3   /**< Get media size (for only f_mkfs()) */
#define MTDI_GET_SECTOR_SIZE     4   /**< Get sector size (for multiple sector size (_MAX_SS >= 1024)) */
#define MTDI_GET_BLOCK_SIZE      5   /**< Get erase block size (for only f_mkfs()) */
#define MTDI_CTRL_ERASE_BLOCK    6   /**< Force erased a block of blocks (for only _USE_ERASE) */
#define MTDI_CTRL_POWER          7   /**< Get/Set power status */
#define MTDI_CTRL_LOCK           8   /**< Lock/Unlock media removal */
#define MTDI_CTRL_EJECT          9   /**< Eject media */
/** @} */

/**
 * @name specific ioctl command
 * @{
 */
#define MTDI_MMC_GET_TYPE        15  /**< Get memory type - NAND / NOR */
#define MTDI_MMC_GET_SDSTAT      16  /**< Get memory status */
/** @} */

/**
 * @name Memory type
 * @{
 */
#define MEMORY_TYPE_NOR     20
#define MEMORY_TYPE_NNAND   21
/** @} */

/** @} */

typedef struct mtdi_desc mtdi_desc_t;

typedef struct {
    const mtdi_desc_t *driver;
} mtdi_dev_t;

struct mtdi_desc {
    /**
     * @brief Initialize Memory Technology Device (MTD)
     *
     * @param[in] mtdi_dev  Pointer to the selected driver
     *
     * @returns 0 on success
     * @returns any other @ref mtdi_sta_t value on error
     */
    mtdi_sta_t (*mtdi_init)(mtdi_dev_t *mtdi_dev);


    /**
     * @brief Get the status of the Memory Technology Device (MTD)
     *
     * @param[in] mtdi_dev  Pointer to the selected driver
     *
     * @returns 0 on success
     * @returns any other @ref mtdi_sta_t value on error
     */
    mtdi_sta_t (*mtdi_status)(mtdi_dev_t *mtdi_dev);


    /**
     * @brief Read pages over the Memory Technology Device (MTD)
     *
     * @param[in] mtdi_dev  Pointer to the selected driver
     * @param[out] buff     Pointer to the data buffer to store read data
     * @param[in]  addr     Starting address
     * @param[in]  size     Number of bytes
     *
     * @return @ref MTDI_RES_OK on success
     * @return any other @ref mtdi_result_t value on error
     */
    mtdi_result_t (*mtdi_read)(mtdi_dev_t *mtdi_dev,
                               unsigned char *buff,
                               unsigned long addr,
                               unsigned long size);


    /**
     * @brief Write pages over the Memory Technology Device (MTD)
     *
     * @param[in] mtdi_dev  Pointer to the selected driver
     * @param[in] buff      Pointer to the data to be written
     * @param[in] addr      Starting address
     * @param[in] size      Number of bytes
     *
     * @return @ref MTDI_RES_OK on success
     * @return any other @ref mtdi_result_t value on error
     */
    mtdi_result_t (*mtdi_write)(mtdi_dev_t *mtdi_dev,
                                const unsigned char *buff,
                                unsigned long addr,
                                unsigned long size);

    /**
     * @brief Erase sector over the Memory Technology Device (MTD)
     *
     * @param[in] mtdi_dev  Pointer to the selected driver
     * @param[in] addr      Starting address
     * @param[in] size      Number of bytes
     *
     * @return @ref MTDI_RES_OK on success
     * @return any other @ref mtdi_result_t value on error
     */
    mtdi_result_t (*mtdi_erase)(mtdi_dev_t *mtdi_dev,
                                unsigned long addr,
                                unsigned long size);

    /**
     * @brief IOCTL functions for the Memory Technology Device (MTD)
     *
     * @param[in] mtdi_dev  Pointer to the selected driver
     * @param[in]     ctrl  Control code
     * @param[in,out] buff  Buffer to send/receive data block
     *
     * @return @ref MTDI_RES_OK on success
     * @return any other @ref mtdi_result_t value on error
     */
    mtdi_result_t (*mtdi_ioctl)(mtdi_dev_t *mtdi_dev,
                                unsigned char ctrl,
                                void *buff);
};


#ifdef __cplusplus
}
#endif

/** @} */
#endif /* __MTDI_H_ */
