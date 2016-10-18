/*
 * Copyright (C) 2016  OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mtd Memory Technology Device
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
 * @author      Vincent Dupont <vincent@otakeys.com>
 */

#ifndef MTD_H
#define MTD_H

#include <stdint.h>
#if MODULE_VFS
#include "vfs.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Disk Status Bits
 * @{
 */
typedef enum {
    MTD_STA_NOINIT  = 0x00, /**< Drive not initialized */
    MTD_STA_INIT    = 0x01, /**< Drive initialized */
    MTD_STA_NODISK  = 0x02, /**< No medium in the drive */
    MTD_STA_PROTECT = 0x04  /**< Write protected */
} mtd_sta_t;
/** @} */

/**
 * @name Command code for disk_ioctrl function
 * @{
 */

/**
 * @name Generic ioctl command
 * @{
 */
#define MTD_CTRL_SYNC           0   /**< Flush disk cache (for write functions) */
#define MTD_CTRL_ERASE_BLOCK    6   /**< Force erased a block of blocks */
#define MTD_CTRL_POWER          7   /**< Get/Set power status */
#define MTD_CTRL_LOCK           8   /**< Lock/Unlock media removal */
#define MTD_CTRL_EJECT          9   /**< Eject media */
/** @} */

/**
 * @name specific ioctl command
 * @{
 */
#define MTD_MMC_GET_TYPE        15  /**< Get memory type - NAND / NOR */
#define MTD_MMC_GET_SDSTAT      16  /**< Get memory status */
/** @} */

/**
 * @name Memory type
 * @{
 */
#define MEMORY_TYPE_NOR     20
#define MEMORY_TYPE_NAND    21
/** @} */

/** @} */

typedef struct mtd_desc mtd_desc_t;

typedef struct {
    const mtd_desc_t *driver;
    uint32_t sector_count;
    uint32_t pages_per_sector;
    uint32_t page_size;
} mtd_dev_t;


/**
 * @brief MTD driver interface
 *
 * This define the functions to access a MTD.
 *
 * A MTD is composed of pages combined into sectors. A sector is the smallest erasable unit.
 * The number of pages in a sector must be constant for the whole MTD.
 *
 * The erase operation is available only for entire sectors.
 */
struct mtd_desc {
    /**
     * @brief Initialize Memory Technology Device (MTD)
     *
     * @param[in] dev  Pointer to the selected driver
     *
     * @returns 0 on success
     * @returns any other @ref mtd_sta_t value on error
     */
    mtd_sta_t (*init)(mtd_dev_t *dev);


    /**
     * @brief Get the status of the Memory Technology Device (MTD)
     *
     * @param[in] dev  Pointer to the selected driver
     *
     * @returns 0 on success
     * @returns any other @ref mtd_sta_t value on error
     */
    mtd_sta_t (*status)(mtd_dev_t *dev);


    /**
     * @brief Read from the Memory Technology Device (MTD)
     *
     * No alignment is required on @p addr and @p size.
     *
     * @param[in]  dev      Pointer to the selected driver
     * @param[out] buff     Pointer to the data buffer to store read data
     * @param[in]  addr     Starting address
     * @param[in]  size     Number of bytes
     *
     * @return 0 on success
     * @return < 0 value on error
     */
    int (*read)(mtd_dev_t *dev,
                void *buff,
                uint32_t addr,
                uint32_t size);


    /**
     * @brief Write to the Memory Technology Device (MTD)
     *
     * @p addr + @p size must be inside a page boundary. @p addr can be anywhere
     * but the buffer cannot overlap two pages.
     *
     * @param[in] dev       Pointer to the selected driver
     * @param[in] buff      Pointer to the data to be written
     * @param[in] addr      Starting address
     * @param[in] size      Number of bytes
     *
     * @return 0 on success
     * @return < 0 value on error
     */
    int (*write)(mtd_dev_t *dev,
                 const void *buff,
                 uint32_t addr,
                 uint32_t size);

    /**
     * @brief Erase sector over the Memory Technology Device (MTD)
     *
     * @p addr must be aligned on a sector boundary. @p size must be a sector size.
     *
     * @param[in] dev       Pointer to the selected driver
     * @param[in] addr      Starting address
     * @param[in] size      Number of bytes
     *
     * @return 0 on success
     * @return < 0 value on error
     */
    int (*erase)(mtd_dev_t *dev,
                 uint32_t addr,
                 uint32_t size);

    /**
     * @brief IOCTL functions for the Memory Technology Device (MTD)
     *
     * @param[in] dev       Pointer to the selected driver
     * @param[in] ctrl      Control code
     * @param[in,out] buff  Buffer to send/receive data block
     *
     * @return 0 on success
     * @return < 0 value on error
     */
    int (*ioctl)(mtd_dev_t *dev,
                 unsigned char ctrl,
                 void *buff);
};

/**
 * @brief mtd_init Initialize a MTD device
 *
 * @param mtd the device to initialize
 *
 * @return
 */
int mtd_init(mtd_dev_t *mtd);

/**
 * @brief mtd_read Read data from a MTD device
 *
 * @param      mtd   the device to read from
 * @param[out] dest  the buffer to fill in
 * @param[in]  addr  the start address to read from
 * @param[in]  count the number of bytes to read
 *
 * @return the number of byte actually read
 * @return < 0 if an error occured
 */
int mtd_read(mtd_dev_t *mtd, void *dest, uint32_t addr, uint32_t count);

/**
 * @brief mtd_read write data to a MTD device
 *
 * @param      mtd   the device to write to
 * @param[in]  src   the buffer to write
 * @param[in]  addr  the start address to write to
 * @param[in]  count the number of bytes to write
 *
 * @return the number of byte actually written
 * @return < 0 if an error occured
 */
int mtd_write(mtd_dev_t *mtd, const void *src, uint32_t addr, uint32_t count);

/**
 * @brief mtd_erase Erase sectors of a MTD device
 *
 * @param      mtd   the device to erase
 * @param[in]  addr  the address of the first sector to erase
 * @param[in]  count the number of bytes to erase
 *
 * @return 0 if erase successful
 * @return < 0 if an error occured
 */
int mtd_erase(mtd_dev_t *mtd, uint32_t addr, uint32_t count);

/**
 * @brief mtd_ioctl IOCTL on a MTD device
 *
 * @param         mtd  the device to access
 * @param[in]     ctrl the value to get/set
 * @param[in,out] buf  the buffer to get/set
 *
 * @return 0 if ioctl successful
 * @return < 0 if an error occured
 */
int mtd_ioctl(mtd_dev_t *mtd, unsigned char ctrl, void *buf);

#if MODULE_VFS
extern const vfs_file_ops_t mtd_vfs_ops;
#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* MTD_H */
