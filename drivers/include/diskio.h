/*
 * Copyright 2010 ChaN
 * Copyright 2016 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_diskio Disk IO Driver
 * @ingroup     drivers_storage
 * @brief       Low level disk interface
 *
 * @{
 *
 * @file
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */

#ifndef DISKIO_H
#define DISKIO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* These functions are defined in asmfunc.S */

/**
 * @brief   Copy aligned to unaligned
 *
 * @param[out] dst   Pointer to unaligned destination address
 * @param[in]  src   Pointer to aligned source address
 * @param[in]  count Number of bytes to copy
 */
void copy_al2un(unsigned char *dst, const unsigned long *src, int count);

/**
 * @brief   Copy unaligned to aligned
 *
 * @param[out] dst   Pointer to unaligned destination address
 * @param[in]  src   Pointer to aligned source address
 * @param[in]  count Number of bytes to copy
 */
void copy_un2al(unsigned long *dst, const unsigned char *src, int count);

/** Results of Disk Functions */
typedef enum {
    DISKIO_RES_OK = 0,     /**< 0: Successful */
    DISKIO_RES_ERROR,      /**< 1: R/W Error */
    DISKIO_RES_WRPRT,      /**< 2: Write Protected */
    DISKIO_RES_NOTRDY,     /**< 3: Not Ready */
    DISKIO_RES_PARERR      /**< 4: Invalid Parameter */
} diskio_result_t;

/**
 * @brief   Disk Status Bits
 */
typedef enum {
    DISKIO_STA_NOINIT  = 0x01, /**< Drive not initialized */
    DISKIO_STA_NODISK  = 0x02, /**< No medium in the drive */
    DISKIO_STA_PROTECT = 0x04  /**< Write protected */
} diskio_sta_t;

/**
 * @name    Command code for disk_ioctrl function
 * @{
 */

/**
 * @name Generic ioctl command (defined for FatFs)
 * @{
 */
#define CTRL_SYNC           0   /**< Flush disk cache (for write functions) */
#define GET_SECTOR_COUNT    1   /**< Get media size (for only f_mkfs()) */
#define GET_SECTOR_SIZE     2   /**< Get sector size (for multiple sector size (_MAX_SS >= 1024)) */
#define GET_BLOCK_SIZE      3   /**< Get erase block size (for only f_mkfs()) */
#define CTRL_ERASE_SECTOR   4   /**< Force erased a block of sectors (for only _USE_ERASE) */
/** @} */

/**
 * @name    Generic ioctl command
 * @{
 */
#define CTRL_POWER          5   /**< Get/Set power status */
#define CTRL_LOCK           6   /**< Lock/Unlock media removal */
#define CTRL_EJECT          7   /**< Eject media */
/** @} */

/**
 * @name    MMC/SDC specific ioctl command
 * @{
 */
#define MMC_GET_TYPE        10  /**< Get card type */
#define MMC_GET_CSD         11  /**< Get CSD */
#define MMC_GET_CID         12  /**< Get CID */
#define MMC_GET_OCR         13  /**< Get OCR */
#define MMC_GET_SDSTAT      14  /**< Get SD status */
/** @} */

/** @} */

/**
 * @brief   Initialize media control interface (MCI)
 *
 * @returns 0 on success
 * @returns a @ref diskio_sta_t value on error
 */
diskio_sta_t mci_initialize(void);

/**
 * @brief   Get the status of the media control interface (MCI)
 *
 * @returns 0 on success
 * @returns a @ref diskio_sta_t value on error
 */
diskio_sta_t mci_status(void);

/**
 * @brief   Read sectors over the media control interface (MCI)
 *
 * @param[out] buff      Pointer to the data buffer to store read data
 * @param[in]  sector    Start sector number (LBA)
 * @param[in]  count     Sector count (1..127)
 *
 * @return @ref DISKIO_RES_OK on success
 * @return any other @ref diskio_result_t value on error
 */
diskio_result_t mci_read(unsigned char *buff, unsigned long sector, unsigned char count);

/**
 * @brief   Write sectors over the media control interface (MCI)
 * @param[in] buff     Pointer to the data to be written
 * @param[in] sector   Start sector number (LBA)
 * @param[in] count    Sector count (1..127)
 *
 * @return @ref DISKIO_RES_OK on success
 * @return any other @ref diskio_result_t value on error
 */
diskio_result_t mci_write(const unsigned char *buff, unsigned long sector, unsigned char count);

/**
 * @brief   IOCTL functions for the media control interface (MCI)
 *
 * @param[in]       ctrl  Control code
 * @param[in,out]   buff  Buffer to send/receive data block
 *
 * @return @ref DISKIO_RES_OK on success
 * @return any other @ref diskio_result_t value on error
 */
diskio_result_t mci_ioctl(unsigned char ctrl, void *buff);

#ifdef __cplusplus
}
#endif

#endif /* DISKIO_H */
/** @} */
