/*
 * Copyright (C) 2022 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     pkg_flashdb
 * @{
 *
 * @file
 * @brief       Flash Abstraction Layer partition configuration
 *
 * FlashDB can only use a single MTD device, but allows for multiple
 * named partitions on the MTD device.
 *
 * This file pre-defines up to 4 partitions, if you need more edit
 * this file or provide your own `FAL_PART_TABLE`.
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include "board.h"
#include "macros/units.h"
#include "mtd_default.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(CONFIG_FLASHDB_MIN_SECTOR_SIZE_DEFAULT_KiB) || defined(DOXYGEN)
/**
 * @brief   Minimal virtual sector size in KiB for FlashDB
 *
 * This is just a reasonable default for an automatic partition configuration, hence "DEFAULT".
 * The "MIN" stands for a required minimum to guarantee an expected size of key value pairs.
 * The actually acceptable sector size must be a multiple of the physical sector size though.
 * Thus, it can be larger than the minimum required size.
 *
 * The default is 4 (4096).
 * FlashDB sector size is different from MTD sector size as it is a
 * virtual measure of granularity and not a device property.
 * The virtual sector size must be a multiple of the physical sector size.
 *
 * From the documentation of FLashDB:
 * The internal storage structure of FlashDB is composed of N sectors, and each formatting takes
 * sector as the smallest unit. A sector is usually N times the size of the Flash block.
 * For example, the block size of Nor Flash is generally 4096.
 *
 * By default, KVDB will use 1 times the block size as the sector size, that is, 4096.
 * At this time, the KVDB cannot store a KV longer than 4096. If you want to save, for example,
 * a KV with a length of 10K, you can use the control function to set the sector size to 12K or larger.
 *
 */
#define CONFIG_FLASHDB_MIN_SECTOR_SIZE_DEFAULT_KiB  4
#endif

/**
 * @brief   Partition table is defined at compile time (not read from flash)
 */
#define FAL_PART_HAS_TABLE_CFG

/**
 * @brief FAL <-> MTD adapter
 */
extern struct fal_flash_dev mtd_flash0;

/**
 * @brief   flash device table
 */
#define FAL_FLASH_DEV_TABLE                                          \
{                                                                    \
    &mtd_flash0,                                                     \
}

#if !defined(FAL_MTD) || defined(DOXYGEN)
/**
 * @brief   Default MTD to use for flashdb
 */
#define FAL_MTD                                     mtd_dev_get(0)
#endif

#if !defined(FAL_PART0_LABEL) || defined(DOXYGEN)
/**
 * @brief   Have at least the label of partition 0 defined
 */
#define FAL_PART0_LABEL                             "part0"
#endif

#if !defined(FAL_PART0_LENGTH) || defined(DOXYGEN)
/**
 * @brief   Have at least the length of partition 0 defined, which must be at least two sectors
 *          and a multiple of the virtual sector size.
 *
 * The virtual sector size is however bound to the physical sector size of @ref FAL_MTD.
 * So make sure that @ref CONFIG_FLASHDB_MIN_SECTOR_SIZE_DEFAULT_KiB times 1024 is a multiple of the MTD sector size.
 * For example if the MTD sector size is 4KiB, then @ref CONFIG_FLASHDB_MIN_SECTOR_SIZE_DEFAULT_KiB must be a multiple of 4.
 * If the MTD sector size is 1KiB, then you have all options for @ref CONFIG_FLASHDB_MIN_SECTOR_SIZE_DEFAULT_KiB.
 */
#define FAL_PART0_LENGTH                            (2 * (CONFIG_FLASHDB_MIN_SECTOR_SIZE_DEFAULT_KiB * KiB(1)))
#endif

/**
 * @brief Partition 0
 */
#if defined(FAL_PART0_LABEL) || defined(DOXYGEN)
#if !defined(FAL_PART0_OFFSET) || defined(DOXYGEN)
/**
 * @brief Offset of partition 0
 */
#define FAL_PART0_OFFSET    0
#endif
/**
 * @brief   Partition 0 compound definition
 */
#define FAL_ROW_PART0   { FAL_PART_MAGIC_WORD, FAL_PART0_LABEL, "fal_mtd", \
                          FAL_PART0_OFFSET, FAL_PART0_LENGTH, 0 },
#else
#define FAL_ROW_PART0
#endif

/**
 * @brief Partition 1
 */
#if defined(FAL_PART1_LABEL) || defined(DOXYGEN)
#if !defined(FAL_PART1_OFFSET)  || defined(DOXYGEN)
/**
 * @brief Offset of partition 1
 */
#define FAL_PART1_OFFSET    (FAL_PART0_OFFSET + FAL_PART0_LENGTH)
#endif
/**
 * @brief   Partition 1 compound definition
 */
#define FAL_ROW_PART1   { FAL_PART_MAGIC_WORD, FAL_PART1_LABEL, "fal_mtd", \
                          FAL_PART1_OFFSET, FAL_PART1_LENGTH, 0 },
#else
#define FAL_ROW_PART1
#endif

/**
 * @brief Partition 2
 */
#if defined(FAL_PART2_LABEL) || defined(DOXYGEN)
#if !defined(FAL_PART2_OFFSET) || defined(DOXYGEN)
/**
 * @brief Offset of partition 2
 */
#define FAL_PART2_OFFSET    (FAL_PART1_OFFSET + FAL_PART1_LENGTH)
#endif
/**
 * @brief   Partition 2 compound definition
 */
#define FAL_ROW_PART2   { FAL_PART_MAGIC_WORD, FAL_PART2_LABEL, "fal_mtd", \
                          FAL_PART2_OFFSET, FAL_PART2_LENGTH, 0 },
#else
#define FAL_ROW_PART2
#endif

/**
 * @brief Partition 3
 */
#if defined(FAL_PART3_LABEL) || defined(DOXYGEN)
#if !defined(FAL_PART3_OFFSET) || defined(DOXYGEN)
/**
 * @brief Offset of partition 3
 */
#define FAL_PART3_OFFSET    (FAL_PART2_OFFSET + FAL_PART2_LENGTH)
#endif
/**
 * @brief   Partition 3 compound definition
 */
#define FAL_ROW_PART3   { FAL_PART_MAGIC_WORD, FAL_PART3_LABEL, "fal_mtd", \
                          FAL_PART3_OFFSET, FAL_PART3_LENGTH, 0 },
#else
#define FAL_ROW_PART3
#endif

/**
 * @brief   Partition Table
 */
#define FAL_PART_TABLE  \
{                       \
    FAL_ROW_PART0       \
    FAL_ROW_PART1       \
    FAL_ROW_PART2       \
    FAL_ROW_PART3       \
}

#ifdef __cplusplus
}
#endif
/** @} */
