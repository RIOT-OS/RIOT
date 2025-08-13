/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    sys_checksum_crc8     CRC-8
 * @ingroup     sys_checksum
 * @brief       CRC-8 checksum algorithms
 *
 * @{
 *
 * @file
 * @brief   CRC-8 definitions
 *
 * @author  Gunar Schorcht <gunar@schorcht.net>
 */

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Calculate CRC-8 (MSB first)
 *
 * This CRC8 checksum type is usually more common.
 *
 * @param[in] data  Start of memory area to checksum
 * @param[in] len   Number of bytes in @p buf to calculate checksum for
 * @param[in] poly  The generator polynomial for the checksum
 * @param[in] seed  The seed (starting value) for the checksum
 *
 * @note A final XOR must be realized by the caller if needed.
 *
 * @return  Checksum of the specified memory area.
 */
uint8_t crc8(const uint8_t *data, size_t len, uint8_t poly, uint8_t seed);

/**
 * @brief   Calculate a reflected CRC-8 (LSB first)
 *
 * This CRC8 checksum type is used for example by Onewire.
 *
 * @param[in] data  Start of memory area to checksum
 * @param[in] len   Number of bytes in @p buf to calculate checksum for
 * @param[in] poly  The generator polynomial for the checksum
 * @param[in] seed  The seed (starting value) for the checksum
 *
 * @note A final XOR must be realized by the caller if needed.
 *
 * @return  Checksum of the specified memory area.
 */
uint8_t crc8_lsb(const uint8_t *data, size_t len, uint8_t poly, uint8_t seed);

#ifdef __cplusplus
}
#endif

/** @} */
