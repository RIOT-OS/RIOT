/*
 * Copyright (C) 2022 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef FAL_CFG_H
#define FAL_CFG_H

#include "board.h"
#include "mtd_default.h"

#ifdef __cplusplus
extern "C" {
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
#define FAL_MTD                                     mtd_default_get_dev(0)
#endif

#if !defined(FAL_PART0_LABEL) || defined(DOXYGEN)
/**
 * @brief   Have at least the label of partition 0 defined
 */
#define FAL_PART0_LABEL                             "part0"
#endif

#if !defined(FAL_PART0_LENGTH) || defined(DOXYGEN)
/**
 * @brief   Have at least the length of partition 0 defined
 */
#define FAL_PART0_LENGTH                            (2 * 4096u)
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
#endif /* FAL_CFG_H */
/** @} */
