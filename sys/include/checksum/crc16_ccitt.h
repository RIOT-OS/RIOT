/*
 * Copyright 2016 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_checksum_crc16_ccitt CRC16-CCITT
 * @ingroup     sys_checksum
 *
 * @brief       CRC16-CCITT checksum algorithm
 * @details     This implementation of CRC16 is based on the CCITT
 *              specification using 0x1D0F as a start value.
 *              Using a different start value is possible by calling
 *              crc16_ccitt_update() with the desired start value
 *              instead of crc16_ccitt_calc().
 *
 * @{
 * @file
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 */

#ifndef CRC16_CCITT_H
#define CRC16_CCITT_H

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief           Update CRC16-CCITT
 *
 * @param[in]  crc  A start value for the CRC calculation, usually the
 *                  return value of a previous call to
 *                  crc16_ccitt_calc() or crc16_ccitt_update()
 * @param[in]  buf  Start of the memory area to checksum
 * @param[in]  len  Number of bytes to checksum
 *
 * @return          Checksum of the specified memory area based on the
 *                  given start value
 */
uint16_t crc16_ccitt_update(uint16_t crc, const unsigned char *buf, size_t len);

/**
 * @brief           Calculate CRC16-CCITT
 *
 * @param[in]  buf  Start of the memory area to checksum
 * @param[in]  len  Number of bytes to checksum
 *
 * @return          Checksum of the specified memory area
 */
uint16_t crc16_ccitt_calc(const unsigned char *buf, size_t len);


#ifdef __cplusplus
}
#endif

#endif /* CRC16_CCITT_H */

/** @} */
