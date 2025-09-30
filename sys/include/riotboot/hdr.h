/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *               2018 Inria
 *               2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    sys_riotboot_hdr RIOT header helpers and tools
 * @ingroup     sys
 * @{
 *
 * The header contains
 *
 * - the magic number
 * - the application version
 * - the address where the RIOT firmware is found
 * - the checksum of the three previous fields
 *
 * @file
 * @brief       RIOT "partition" header and tools
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Magic number for riotboot_hdr version 1
 */
#define RIOTBOOT_MAGIC_V1       0x544f4952 /* "RIOT" */

/**
 * @brief  Magic number for riotboot_hdr version 2
 */
#define RIOTBOOT_MAGIC_V2       0x54304952 /* "RI0T" */

#ifndef DOXYGEN
#if defined(MODULE_RIOTBOOT_HDR_V2)
#  define RIOTBOOT_MAGIC        RIOTBOOT_MAGIC_V2
#else
#  define RIOTBOOT_MAGIC        RIOTBOOT_MAGIC_V1
#endif
#endif

/**
 * @brief   Bit shift position of tentative boot counter
 */
#define RIOTBOOT_HDR_IMAGE_BOOT_COUNT_SHIFT     0u
/**
 * @brief   4-bit mask of tentative boot counter
 */
#define RIOTBOOT_HDR_IMAGE_BOOT_COUNT_MASK      (0x0000000f << RIOTBOOT_HDR_IMAGE_BOOT_COUNT_SHIFT)
/**
 * @brief   Bit shift position of image boot state
 */
#define RIOTBOOT_HDR_IMAGE_STATE_SHIFT          4u
/**
 * @brief   4-bit mask of image boot state
 */
#define RIOTBOOT_HDR_IMAGE_STATE_MASK           (0x0000000f << RIOTBOOT_HDR_IMAGE_STATE_SHIFT)
/**
 * @brief   Maximum number of attempts to boot a tentative image before reverting to the previous one
 */
#ifndef CONFIG_RIOTBOOT_MAX_ATTEMPTS
#  define CONFIG_RIOTBOOT_MAX_ATTEMPTS          3u
#endif

static_assert(CONFIG_RIOTBOOT_MAX_ATTEMPTS > 0,
              "CONFIG_RIOTBOOT_MAX_ATTEMPTS must be at least 1");

static_assert(CONFIG_RIOTBOOT_MAX_ATTEMPTS <= 4,
              "CONFIG_RIOTBOOT_MAX_ATTEMPTS must be <= 4");

/**
 * @brief   Image state enumeration
 */
typedef enum riotboot_hdr_img_state {
    RIOTBOOT_HDR_IMG_STATE_INSTALLED   = 0x0f,      /**< Image is installed (1111) */
    RIOTBOOT_HDR_IMG_STATE_DEACTIVATED = 0x0e,      /**< Image is deactivated (1110) */
    RIOTBOOT_HDR_IMG_STATE_ACTIVATED   = 0x0c,      /**< Image is activated (1100) */
    RIOTBOOT_HDR_IMG_STATE_CONFIRMED   = 0x08,      /**< Image is confirmed (1000) */
    RIOTBOOT_HDR_IMG_STATE_DISMISSED   = 0x00,      /**< Image is dismissed (0000) */
} riotboot_hdr_img_state_t;

/**
 * @brief Structure to store image header - All members are little endian
 */
struct riotboot_hdr_v1 {
    uint32_t magic_number;      /**< Header magic number                              */
    uint32_t version;           /**< Integer representing the partition version       */
    uint32_t start_addr;        /**< Address after the allocated space for the header */
    uint32_t chksum;            /**< Checksum of riotboot_hdr                         */
};

/**
 * @brief Structure to store image header v2 - All members are little endian
 */
struct riotboot_hdr_v2 {
    uint32_t magic_number;              /**< Header magic number                                */
    uint32_t version;                   /**< Integer representing the partition version         */
    uint32_t start_addr;                /**< Address after the allocated space for the header   */
    uint32_t chksum;                    /**< Checksum of riotboot_hdr                           */
    /**
     * @brief   General purpose flags for image state and boot count
     *
     * The flags are not covered by the checksum, because checksum recomputation
     * would require flash page erase of the currently running program.
     */
    uint32_t flags;
};

/**
 * @brief   Union to store riotboot headers in different versions
 */
typedef union riotboot_hdr {
    struct riotboot_hdr_v1 v1;  /**< Version 1 header */
    struct riotboot_hdr_v2 v2;  /**< Version 2 header */
} riotboot_hdr_t;

/**
 * @brief   Getter for riotboot magic number
 *
 * @param[in]   riotboot_hdr  ptr to image header
 *
 * @return Magic number
 */
uint32_t riotboot_hdr_get_magic_number(const riotboot_hdr_t *riotboot_hdr);

/**
 * @brief   Getter for riotboot version
 *
 * @param[in]   riotboot_hdr  ptr to image header
 *
 * @return Version number
 */
uint32_t riotboot_hdr_get_version(const riotboot_hdr_t *riotboot_hdr);

/**
 * @brief   Getter for riotboot start address
 *
 * @param[in]   riotboot_hdr  ptr to image header
 *
 * @return Start address
 */
uint32_t riotboot_hdr_get_start_addr(const riotboot_hdr_t *riotboot_hdr);

/**
 * @brief   Getter for riotboot flags
 *
 * @param[in]   riotboot_hdr  ptr to image header
 *
 * @return Flags
 */
uint32_t riotboot_hdr_get_flags(const riotboot_hdr_t *riotboot_hdr);

/**
 * @brief  Getter for the header checksum
 *
 * @param[in] riotboot_hdr  ptr to image header
 *
 * @return  Checksum of the given riotboot_hdr
 */
uint32_t riotboot_hdr_get_checksum(const riotboot_hdr_t *riotboot_hdr);

/**
 * @brief  Print formatted riotboot_hdr_t to STDIO
 *
 * @param[in] riotboot_hdr  ptr to image header
 *
 */
void riotboot_hdr_print(const riotboot_hdr_t *riotboot_hdr);

/**
 * @brief  Validate image header
 *
 * @param[in] riotboot_hdr  ptr to image header
 *
 * @returns 0 if OK
 * @returns -1 if not OK
 */
int riotboot_hdr_validate(const riotboot_hdr_t *riotboot_hdr);

/**
 * @brief  Calculate header checksum
 *
 * @param[in] riotboot_hdr  ptr to image header
 *
 * @returns the checksum of the given riotboot_hdr
 */
uint32_t riotboot_hdr_checksum(const riotboot_hdr_t *riotboot_hdr);

/**
 * @brief   Check if the header is version 2
 *
 * @param[in]   riotboot_hdr    ptr to image header
 *
 * @return true if version is 2, false otherwise
 */
bool riotboot_hdr_is_v2(const riotboot_hdr_t *riotboot_hdr);

/**
 * @brief   Get current image boot count
 *
 * @param[in]   riotboot_hdr  ptr to image header
 *
 * @return boot counter
 */
unsigned riotboot_hdr_get_boot_count(const riotboot_hdr_t *riotboot_hdr);

/**
 * @brief   Set image boot count
 *
 * @param[in]   riotboot_hdr  ptr to image header
 * @param[in]   boot_count    new boot count
 */
void riotboot_hdr_set_boot_count(riotboot_hdr_t *riotboot_hdr,
                                 unsigned boot_count);

/**
 * @brief   Set image state
 *
 * @param[in]   riotboot_hdr  ptr to image header
 * @param[in]   state         new image state
 * @return 0 on success, -1 on failure
 */
int riotboot_hdr_set_img_state(riotboot_hdr_t *riotboot_hdr,
                               riotboot_hdr_img_state_t state);

/**
 * @brief   Get image state
 *
 * @param[in]   riotboot_hdr  ptr to image header
 * @return image state
 */
int riotboot_hdr_get_img_state(const riotboot_hdr_t *riotboot_hdr);

#ifdef __cplusplus
}
#endif
