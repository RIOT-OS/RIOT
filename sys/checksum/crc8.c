/*
 * SPDX-FileCopyrightText: 2018 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_checksum_crc8
 * @brief       CRC-8 checksum algorithms
 *
 * @{
 *
 * @file
 * @brief   CRC-8 implementation
 *
 * @author  Gunar Schorcht <gunar@schorcht.net>
 */

#include <stdbool.h>
#include "checksum/crc8.h"

uint8_t crc8(const uint8_t *data, size_t len, uint8_t g_polynom, uint8_t crc)
{
    /* iterate over all bytes */
    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];

        for (unsigned j = 0; j < 8; j++) {
            bool xor = crc & 0x80;
            crc = crc << 1;
            crc = xor ? crc ^ g_polynom : crc;
        }
    }

    return crc;
}

uint8_t crc8_lsb(const uint8_t *data, size_t len, uint8_t g_polynom, uint8_t crc)
{
    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];

        for (unsigned j = 0; j < 8; j++) {
            bool xor = crc & 0x01;
            crc = crc >> 1;
            crc = xor ? crc ^ g_polynom : crc;
        }
    }

    return crc;
}

/** @} */
