/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
    for (size_t i=0; i < len; i++)
    {
        crc ^= data[i];

        for (int i = 0; i < 8; i++)
        {
            bool xor = crc & 0x80;
            crc = crc << 1;
            crc = xor ? crc ^ g_polynom : crc;
        }
    }

    return crc;
}

/** @} */
