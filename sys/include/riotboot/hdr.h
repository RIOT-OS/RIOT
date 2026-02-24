/*
 * SPDX-FileCopyrightText: 2018 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2018 Inria
 * SPDX-FileCopyrightText: 2018 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    sys_riotboot_hdr RIOT header helpers and tools
 * @ingroup     sys
 * @{
 *
 * The header contains
 *
 * - "RIOT" as magic number
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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief  Magic number for riotboot_hdr
 *
 */
#define RIOTBOOT_MAGIC         0x544f4952 /* "RIOT" */

/**
 * @brief Structure to store image header - All members are little endian
 * @{
 */
typedef struct {
    uint32_t magic_number;      /**< Header magic number (always "RIOT")              */
    uint32_t version;           /**< Integer representing the partition version       */
    uint32_t start_addr;        /**< Address after the allocated space for the header */
    uint32_t chksum;            /**< Checksum of riotboot_hdr                         */
} riotboot_hdr_t;
/** @} */

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

#ifdef __cplusplus
}
#endif
