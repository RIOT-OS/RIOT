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

#if !defined(CONFIGURATION_FLASHDB_MIN_SECTOR_SIZE_EXP) || defined(DOXYGEN)
/**
 * @brief   Virtual FlashDB sector size exponent of a power of two
 *
 * The default is 2⁸ (256).
 * FlashDB sector size is different from MTD sector size as it is a
 * virtual measure of granularity and not a device property.
 * The virtual sector size must be a multiple of the physical sector size.
 * If you notice that longer key - value records are causing errors, you could
 * tune and increase this parameter.
 */
#define CONFIGURATION_FLASHDB_MIN_SECTOR_SIZE_EXP       8
#endif

#if !defined(CONFIGURATION_FLASHDB_MTD_DEV) || defined(DOXYGEN)
/**
 * @brief   MTD for FlashDB FAL device mode
 */
#define CONFIGURATION_FLASHDB_MTD_DEV               \
    configuration_backend_flashdb_mtd_choose_dev()
#endif

#if !defined(CONFIGURATION_FLASHDB_MTD_PARTITION) || defined(DOXYGEN)
/**
 * @brief   FAL partition to use for configuration data
 */
#define CONFIGURATION_FLASHDB_MTD_PARTITION_LABEL   \
    configuration_backend_flashdb_mtd_choose_partition()
#endif

#if !defined(CONFIGURATION_FLASHDB_VFS_MTD_DEV) || defined(DOXYGEN)
/**
 * @brief   MTD for FlashDB VFS mode
 */
#define CONFIGURATION_FLASHDB_VFS_MTD_DEV           \
    configuration_backend_flashdb_vfs_choose_dev()
#endif

#if !defined(CONFIGURATION_FLASHDB_VFS_MAX_SECTORS) || defined(DOXYGEN)
/**
 * @brief   Maximum number of sectors in the VFS to use for the configuration file
 *          in the FlashDB VFS mode
 */
#define CONFIGURATION_FLASHDB_VFS_MAX_SECTORS       4
#endif

#if !defined(CONFIGURATION_FLASHDB_VFS_FOLDER) || defined(DOXYGEN)
/**
 * @brief   Folder name in the VFS for configuration data in FlashDB VFS mode
 */
#define CONFIGURATION_FLASHDB_VFS_FOLDER            "fdb_kvdb_configuration"
#endif

#if !defined(CONFIGURATION_FLASHDB_VFS_PATH) || defined(DOXYGEN)
/**
 * @brief   Full location path of the folder in the VFS where the configuration
 *          file is stored in FlashDB VFS mode
 */
#define CONFIGURATION_FLASHDB_VFS_PATH              \
    configuration_backend_flashdb_vfs_choose_path()
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
 * The default implementation is to return @ref FAL_PART_LABEL.
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
 * @brief   Get the FlashDB backend
 *
 * @return  FlashDB backend
 */
const conf_backend_t *configuration_backend_flashdb_get(void);

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

#ifdef __cplusplus
}
#endif

#endif /* CONFIGURATION_BACKEND_FLASHDB_H */
/** @} */
