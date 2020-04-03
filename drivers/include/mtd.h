/*
 * Copyright (C) 2016  OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mtd Memory Technology Device
 * @ingroup     drivers_storage
 * @{
 * @brief       Low level Memory Technology Device interface
 *
 * Generic memory technology device interface
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
 * @brief   MTD power states
 */
enum mtd_power_state {
    MTD_POWER_UP,    /**< Power up */
    MTD_POWER_DOWN,  /**< Power down */
};

/**
 * @brief   MTD driver interface
 *
 * This define the functions to access a MTD.
 *
 * A MTD is composed of pages combined into sectors. A sector is the smallest erasable unit.
 * The number of pages in a sector must be constant for the whole MTD.
 *
 * The erase operation is available only for entire sectors.
 */
typedef struct mtd_desc mtd_desc_t;

/**
 * @brief   MTD device descriptor
 */
typedef struct {
    const mtd_desc_t *driver;  /**< MTD driver */
    uint32_t sector_count;     /**< Number of sector in the MTD */
    uint32_t pages_per_sector; /**< Number of pages by sector in the MTD */
    uint32_t page_size;        /**< Size of the pages in the MTD */
} mtd_dev_t;

/**
 * @brief   MTD driver interface
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
     * @brief   Initialize Memory Technology Device (MTD)
     *
     * @param[in] dev  Pointer to the selected driver
     *
     * @returns 0 on success
     * @returns < 0 value in error
     */
    int (*init)(mtd_dev_t *dev);

    /**
     * @brief   Read from the Memory Technology Device (MTD)
     *
     * No alignment is required on @p addr and @p size.
     *
     * @param[in]  dev      Pointer to the selected driver
     * @param[out] buff     Pointer to the data buffer to store read data
     * @param[in]  addr     Starting address
     * @param[in]  size     Number of bytes
     *
     * @return the number of bytes actually read
     * @return < 0 value on error
     */
    int (*read)(mtd_dev_t *dev,
                void *buff,
                uint32_t addr,
                uint32_t size);

    /**
     * @brief   Write to the Memory Technology Device (MTD)
     *
     * @p addr + @p size must be inside a page boundary. @p addr can be anywhere
     * but the buffer cannot overlap two pages.
     *
     * @param[in] dev       Pointer to the selected driver
     * @param[in] buff      Pointer to the data to be written
     * @param[in] addr      Starting address
     * @param[in] size      Number of bytes
     *
     * @return the number of bytes actually written
     * @return < 0 value on error
     */
    int (*write)(mtd_dev_t *dev,
                 const void *buff,
                 uint32_t addr,
                 uint32_t size);

    /**
     * @brief   Erase sector(s) over the Memory Technology Device (MTD)
     *
     * @p addr must be aligned on a sector boundary. @p size must be a multiple of a sector size.
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
     * @brief   Control power of Memory Technology Device (MTD)
     *
     * @param[in] dev       Pointer to the selected driver
     * @param[in] power     Power state to apply (from @ref mtd_power_state)
     *
     * @return 0 on success
     * @return < 0 value on error
     */
    int (*power)(mtd_dev_t *dev, enum mtd_power_state power);
};

/**
 * @brief   mtd_init Initialize a MTD device
 *
 * @param mtd the device to initialize
 *
 * @return
 */
int mtd_init(mtd_dev_t *mtd);

/**
 * @brief   Read data from a MTD device
 *
 * No alignment is required on @p addr and @p count.
 *
 * @param      mtd   the device to read from
 * @param[out] dest  the buffer to fill in
 * @param[in]  addr  the start address to read from
 * @param[in]  count the number of bytes to read
 *
 * @return the number of byte actually read
 * @return < 0 if an error occurred
 * @return -ENODEV if @p mtd is not a valid device
 * @return -ENOTSUP if operation is not supported on @p mtd
 * @return -EOVERFLOW if @p addr or @p count are not valid, i.e. outside memory
 * @return -EIO if I/O error occurred
 */
int mtd_read(mtd_dev_t *mtd, void *dest, uint32_t addr, uint32_t count);

/**
 * @brief   Write data to a MTD device
 *
 * @p addr + @p count must be inside a page boundary. @p addr can be anywhere
 * but the buffer cannot overlap two pages. Though some devices might enforce alignment
 * on both @p addr and @p buf.
 *
 * @param      mtd   the device to write to
 * @param[in]  src   the buffer to write
 * @param[in]  addr  the start address to write to
 * @param[in]  count the number of bytes to write
 *
 * @return the number of byte actually written
 * @return < 0 if an error occurred
 * @return -ENODEV if @p mtd is not a valid device
 * @return -ENOTSUP if operation is not supported on @p mtd
 * @return -EOVERFLOW if @p addr or @p count are not valid, i.e. outside memory,
 * or overlapping two pages
 * @return -EIO if I/O error occurred
 * @return -EINVAL if parameters are invalid (invalid alignment for instance)
 */
int mtd_write(mtd_dev_t *mtd, const void *src, uint32_t addr, uint32_t count);

/**
 * @brief   Erase sectors of a MTD device
 *
 * @p addr must be aligned on a sector boundary. @p count must be a multiple of a sector size.
 *
 * @param      mtd   the device to erase
 * @param[in]  addr  the address of the first sector to erase
 * @param[in]  count the number of bytes to erase
 *
 * @return 0 if erase successful
 * @return < 0 if an error occurred
 * @return -ENODEV if @p mtd is not a valid device
 * @return -ENOTSUP if operation is not supported on @p mtd
 * @return -EOVERFLOW if @p addr or @p count are not valid, i.e. outside memory
 * @return -EIO if I/O error occurred
 */
int mtd_erase(mtd_dev_t *mtd, uint32_t addr, uint32_t count);

/**
 * @brief   Set power mode on a MTD device
 *
 * @param      mtd   the device to access
 * @param[in]  power the power mode to set
 *
 * @return 0 if power mode successfully set
 * @return < 0 if an error occurred
 * @return -ENODEV if @p mtd is not a valid device
 * @return -ENOTSUP if operation or @p power state is not supported on @p mtd
 * @return -EIO if I/O error occurred
 */
int mtd_power(mtd_dev_t *mtd, enum mtd_power_state power);

#if defined(MODULE_VFS) || defined(DOXYGEN)
/**
 * @brief   MTD driver for VFS
 */
extern const vfs_file_ops_t mtd_vfs_ops;
#endif

#ifdef __cplusplus
}
#endif

#endif /* MTD_H */
/** @} */
