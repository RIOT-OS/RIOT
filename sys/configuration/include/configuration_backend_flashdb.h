/*
 * Copyright (C) 2023 ML!PA Consulting Gmbh
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup    sys_configuration
 *
 * @{
 *
 * @file
 * @brief   Interface for FlashDB as a configuration backend
 *
 * @author  Fabian Hüßler <fabian.huessler@ml-pa.com>
 */

#ifndef CONFIGURATION_BACKEND_FLASHDB_H
#define CONFIGURATION_BACKEND_FLASHDB_H

#include "board.h"
#include "modules.h"
#include "mtd.h"
#include "vfs_default.h"
#include "configuration.h"
#if IS_USED(MODULE_FLASHDB_KVDB)
#include "fal_cfg.h"
#include <flashdb.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(CONFIGURATION_FLASHDB_MIN_SECTOR_SIZE_DEFAULT_KiB) || defined(DOXYGEN)
/**
 * @brief   Virtual sector size to use for the configuration FlashDB
 */
#define CONFIGURATION_FLASHDB_MIN_SECTOR_SIZE_DEFAULT_KiB   CONFIG_FLASHDB_MIN_SECTOR_SIZE_DEFAULT_KiB
#endif

#if !defined(CONFIGURATION_FLASHDB_VFS_MAX_SECTORS) || defined(DOXYGEN)
/**
 * @brief   Maximum number of sectors in the VFS to use for the configuration file
 *          in the FlashDB VFS mode
 */
#define CONFIGURATION_FLASHDB_VFS_MAX_SECTORS               4
#endif

#if !defined(CONFIGURATION_FLASHDB_VFS_FOLDER) || defined(DOXYGEN)
/**
 * @brief   Folder name in the VFS for configuration data in FlashDB VFS mode
 */
#define CONFIGURATION_FLASHDB_VFS_FOLDER                    "fdb_kvdb_configuration"
#endif

/**
 * @brief   __attribute__((weak)) function to select the FAL device for FlashDB
 *          when the module configuration_backend_flashdb_mtd is used
 *
 * The default implementation is to return @ref FAL_MTD.
 *
 * @return  MTD device to use in the FAL mode of FlashDB
 */
mtd_dev_t *configuration_backend_flashdb_mtd_choose_dev(void);

/**
 * @brief   __attribute__((weak)) function to select the FAL partition for FlashDB
 *          when the module configuration_backend_flashdb_mtd is used
 *
 * The default implementation is to return @ref FAL_PART0_LABEL.
 *
 * @return  FAL partition label to use in the FAL mode of FlashDB
 */
const char *configuration_backend_flashdb_mtd_choose_partition(void);

/**
 * @brief   __attribute__((weak)) function to select the MTD for FlashDB
 *          when the module configuration_backend_flashdb_vfs is used
 *
 * The default implementation is to return @ref MTD_0.
 *
 * @return  MTD device to use in the VFS mode of FlashDB
 */
mtd_dev_t *configuration_backend_flashdb_vfs_choose_dev(void);

/**
 * @brief   __attribute__((weak)) function to select the path for FlashDB
 *          when the module configuration_backend_flashdb_vfs is used
 *
 * The default implementation is to return VFS_DEFAULT_DATA"/"CONFIGURATION_FLASHDB_VFS_FOLDER.
 * @ref CONFIGURATION_FLASHDB_VFS_FOLDER
 *
 * @return  Path to use in the VFS mode of FlashDB
 */
const char *configuration_backend_flashdb_vfs_choose_path(void);

/**
 * @brief   Reset the FlashDB backend, which deletes all configuration data
 *
 * @return  0 on success
 */
int configuration_backend_flashdb_reset(void);

/**
 * @brief   Initialize the FlashDB backend
 *
 * @pre     MTD must have been initialized with @ref mtd_init()
 *
 * @param[in]           mtd             FAL device in FlashDB FAL mode or VFS MTD
 *
 * @return  0 on success
 */
int configuration_backend_flashdb_init(mtd_dev_t *mtd);

/**
 * @brief   FlashDB backend operations
 */
extern const conf_backend_ops_t conf_backend_fdb_ops;

#ifdef __cplusplus
}
#endif

#endif /* CONFIGURATION_BACKEND_FLASHDB_H */
/** @} */
