/*
 * Copyright (C) 2022 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef CHECKSUM_CRC32_H
#define CHECKSUM_CRC32_H

/**
 * @defgroup    sys_checksum_crc32 CRC32
 * @ingroup     sys_checksum
 *
 * @brief       CRC32 checksum algorithm implementation according to IEEE standards
 * @{
 *
 * @file
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   CRC-32 checksum
 *
 * Uses the `0xedb88320` polynomial
 *
 * @note enable the `crc32_fast` module for a look-up table based implementation
 *       that trades code size for speed.
 *
 * @param[in] buf   The data to checksum
 * @param[in] size  Length of the data in bytes
 *
 * @return 32 bit sized hash in the interval [0..2^32]
 */
uint32_t crc32(const void *buf, size_t size);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* CHECKSUM_CRC32_H */
