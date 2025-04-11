/*
 * Copyright (C) 2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef MTD_EMULATED_H
#define MTD_EMULATED_H
/**
 * @defgroup    drivers_mtd_emulated MTD wrapper for emulated MTD devices
 * @ingroup     drivers_mtd
 * @{
 * @brief       MTD device that is emulated in RAM for test purposes
 *
 * Helpers for using emulated MTDs.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include <stdbool.h>

#include "board.h"
#include "macros/utils.h"
#include "mtd.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Macro to define an emulated MTD
 *
 * This macro creates a MTD device that is emulated in RAM. For example, using
 * ```
 * MTD_EMULATED_DEV(0, 16, 4, 64)
 * ```
 * creates the emulated MTD device `mtd_emulated_dev0` with 16 sectors, 4 pages
 * per sector and a page size of 64 bytes. The write size is always 1 byte.
 *
 * @note The emulated devices are added to the XFA of MTD device pointers
 *       @ref mtd_dev_xfa with priority 99 to place them at the end of the XFA.
 *
 * @param   n   index of the emulated MTD (results into symbol `mtd_emulated_dev<n>`)
 * @param   sc  sectors of the emulated MTD
 * @param   pps pages per sector of the emulated MTD
 * @param   ps  page size in bytes
 */
#define MTD_EMULATED_DEV(n, sc, pps, ps)                \
    uint8_t _mtd_emulated_memory ## n[sc * pps * ps];   \
                                                        \
    mtd_emulated_t mtd_emulated_dev ## n = {            \
        .base = {                                       \
            .driver = &_mtd_emulated_driver,            \
            .sector_count = sc,                         \
            .pages_per_sector = pps,                    \
            .page_size = ps,                            \
            .write_size = 1,                            \
        },                                              \
        .size = sc * pps * ps,                          \
        .memory = _mtd_emulated_memory ## n,            \
        .init_done = false,                             \
    };                                                  \
                                                        \
    XFA_CONST(mtd_dev_t, mtd_dev_xfa, 99, CONCAT(*mtd_emulated, n)) = (mtd_dev_t *)&mtd_emulated_dev ## n

#if MODULE_VFS_AUTO_MOUNT || DOXYGEN
/**
 * @brief   Macro to define an automatic VFS mount point for an emulated MTD.
 *
 * For example, using
 * ```
 * MTD_EMULATED_DEV_FS(0, 2, fatfs);
 * ```
 * automatically mounts the emulated MTD `mtd_emulated_dev0` with FAT file
 * system under mount point `/mtde0` with unique index 2.
 *
 * @param   n   index of the emulated MTD (symbol `mtd_emulated_dev<n>`, mount point `/mtde0`)
 * @param   m   unique overall index of VFS mount point
 * @param   fs  filesystem type used
 */
#define MTD_EMULATED_DEV_FS(n, m, fs) \
    VFS_AUTO_MOUNT(fs, VFS_MTD(mtd_emulated_dev ## n), "/mtde" # n, m)

#endif /* MODULE_VFS || DOXYGEN */

/**
 * @brief   Device descriptor for a MTD device that is emulated in RAM
 */
typedef struct {
    mtd_dev_t base;     /**< inherit from mtd_dev_t object */
    size_t size;        /**< total size of the MTD device in bytes */
    uint8_t *memory;    /**< RAM that is used for the emulated MTD device */
    bool init_done;     /**< indicates whether initialization is already done */
} mtd_emulated_t;

/**
 * @brief   Emulated MTD device operations table for mtd
 */
extern const mtd_desc_t _mtd_emulated_driver;

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* MTD_EMULATED_H */
