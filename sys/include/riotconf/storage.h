/*
 * Copyright (C) 2024 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_riotconf_storage    RIOT configuration storage API
 * @ingroup     sys
 * @{
 *
 * @file
 * @brief       RIOT configuration storage API
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 *
 * @}
 */

#ifndef RIOTCONF_STORAGE_H
#define RIOTCONF_STORAGE_H

#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>

#include "mtd.h"
#include "riotconf/hdr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Make sure that the sector buffer passed in @ref riotconf_storage_init()
 *          is aligned to the riotconf header
 */
#define  RIOTCONF_BUF_ALIGN __attribute__((aligned(alignof(riotconf_hdr_t))));

/**
 * @brief   RIOT configuration storage
 */
typedef struct riotconf_storage {
    mtd_dev_t *dev;             /**< based on any MTD */
    uint32_t sector_offset;     /**< sector of configuration header */
    void *sector_buf;           /**< buffer for sector read/write */
    size_t sector_size;         /**< size of sector (>= x * MTD sector size) */
} riotconf_storage_t;

/**
 * @brief   Get access to a storage device that stores the provided configuration slot
 *
 * @param[in]   slot    Configuration slot
 *
 * @return  Pointer to storage device
 */
riotconf_storage_t *riotconf_storage_get(riotconf_slot_t slot);

/**
 * @brief   Initialize a configuration slot with a storage device
 *
 * @pre The storage device @p dev must have been initialized with @ref mtd_init()
 * @pre The size @p sector_size must align with the physical sector size of the MTD
 *
 * @param[in]   slot            Configuration slot
 * @param[in]   dev             MTD device
 * @param[in]   sector_offset   Number of sectors to skip on the MTD before the configuration header
 * @param[in]   sector_buf      Buffer provided for read/write operations
 * @param[in]   sector_size     Size of the provided buffer
 */
void riotconf_storage_init(riotconf_slot_t slot, mtd_dev_t *dev, uint32_t sector_offset, void *sector_buf, size_t sector_size);

/**
 * @brief   Open the storage device for reading configuration
 *
 * @param[out]  dev     Storage device
 * @param[out]  buf     Returns a pointer to an internal sector buffer
                        to relieve the caller from allocating memory
 * @param[out]  size    Size of the returned buffer
 */
void riotconf_storage_start_read(riotconf_storage_t *dev, void **buf, size_t *size);

/**
 * @brief   Read data from the storage device
 *
 * @param[in]   dev     Storage device
 * @param[out]  buf     Buffer to store the read data
 *                      which can be the one obtained from @ref riotconf_storage_start_read()
 * @param[in]   offset  Offset in the storage device
 * @param[in]   size    Number of bytes to read
 *
 * @return  0 on success
 * @return  <0 on error
 */
int riotconf_storage_read(riotconf_storage_t *dev, void *buf, size_t offset, size_t size);

/**
 * @brief   Finish configuration reading from the storage device
 *
 * @param[in]   dev     Storage device
 */
void riotconf_storage_finish_read(riotconf_storage_t *dev);

/**
 * @brief   Erase a sector on the storage device
 *
 * @param[in]   dev     Storage device
 * @param[in]   sector  Start sector to erase
 * @param[in]   count   Number of sectors to erase
 *
 * @return  0 on success
 * @return  <0 on error
 */
int riotconf_storage_erase_sector(riotconf_storage_t *dev, uint32_t sector, uint32_t count);

/**
 * @brief   Open the storage device for writing configuration
 *
 * The first sector containing the header is saved before erase.
 *
 * @warning The configuration on the device is invalidated to not have valid but partially written data
 *
 * @param[in]   dev     Storage device
 *
 * @return  0 on success
 * @return  <0 on error
 */
int riotconf_storage_start_write(riotconf_storage_t *dev);

/**
 * @brief   Write to the storage device
 *
 * @param[in]   dev     Storage device
 * @param[in]   data    Data to write
 * @param[in]   offset  Offset in the storage device
 * @param[in]   size    Number of bytes to write
 *
 * @return 0 on success
 * @return <0 on error
 */
int riotconf_storage_write(riotconf_storage_t *dev, const void *data, size_t offset, size_t size);

/**
 * @brief   Finish configuration writing to the storage device
 *
 * @param dev   Storage device
 * @param hdr   Configuration header to be written to the device
 */
void riotconf_storage_finish_write(riotconf_storage_t *dev, const riotconf_hdr_t *hdr);

#ifdef __cplusplus
}
#endif
#endif /* RIOTCONF_STORAGE_H */
