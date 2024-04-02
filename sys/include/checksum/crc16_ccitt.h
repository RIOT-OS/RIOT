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
 * @brief       Variations of CRC16-CCITT checksum algorithms
 * @details     This provides implementations for multiple variations
 *              of CRC16 checks.
 *
 *              There is a more generalized version in @ref sys_checksum_ucrc16,
 *              that does not optionally utilize a look-up table as these implementations
 *              can do (and is thus also far more memory efficient). Its caveat
 *              however is that it is slower by about factor 8 than these versions when
 *              enabling the look-up table.
 *
 *              @note enable the `crc16_fast` module for a look-up table based
 *              implementation that trades code size for speed.
 *
 * @{
 * @file
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * @author      Bennet Blischke <bennet.blischke@haw-hamburg.de>
 */

#ifndef CHECKSUM_CRC16_CCITT_H
#define CHECKSUM_CRC16_CCITT_H

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief           Update CRC16-CCITT-KERMIT
 *
 * @param[in]  crc  A start value for the CRC calculation, usually the
 *                  return value of a previous call to
 *                  crc16_ccitt_kermit_calc() or crc16_ccitt_kermit_update()
 * @param[in]  buf  Start of the memory area to checksum
 * @param[in]  len  Number of bytes to checksum
 *
 * @return          Checksum of the specified memory area based on the
 *                  given start value
 */
uint16_t crc16_ccitt_kermit_update(uint16_t crc, const unsigned char *buf, size_t len);

/**
 * @brief           Calculate CRC16-CCITT-KERMIT
 *
 * Parameter | Value
 * --------: | :----
 * Polynom   | `0x1021`
 * Init      | `0x0000`
 * Refin     | `true`
 * Refout    | `true`
 * Xorout    | `0x0000`
 * Check     | `0x2189`
 *
 * @param[in]  buf  Start of the memory area to checksum
 * @param[in]  len  Number of bytes to checksum
 *
 * @return          Checksum of the specified memory area
 */
uint16_t crc16_ccitt_kermit_calc(const unsigned char *buf, size_t len);

/**
 * @brief           Update CRC16-CCITT-MCRF4XX
 *
 * @param[in]  crc  A start value for the CRC calculation, usually the
 *                  return value of a previous call to
 *                  crc16_ccitt_mcrf4xx_calc() or crc16_ccitt_mcrf4xx_update()
 * @param[in]  buf  Start of the memory area to checksum
 * @param[in]  len  Number of bytes to checksum
 *
 * @return          Checksum of the specified memory area based on the
 *                  given start value
 */
static inline uint16_t crc16_ccitt_mcrf4xx_update(uint16_t crc, const unsigned char *buf,
                                                  size_t len)
{
    /* Since CCITT-KERMIT and CCITT-MCRF4XX only differ in the starting
     * seed, we wrap around crc16_ccitt_kermit_update() for updating */
    return crc16_ccitt_kermit_update(crc, buf, len);
}

/**
 * @brief           Calculate CRC16-CCITT-MCRF4XX
 *
 * Parameter | Value
 * --------: | :----
 * Polynom   | `0x1021`
 * Init      | `0xffff`
 * Refin     | `true`
 * Refout    | `true`
 * Xorout    | `0x0000`
 * Check     | `0x6f91`
 *
 * @param[in]  buf  Start of the memory area to checksum
 * @param[in]  len  Number of bytes to checksum
 *
 * @return          Checksum of the specified memory area
 */
uint16_t crc16_ccitt_mcrf4xx_calc(const unsigned char *buf, size_t len);

/**
 * @brief           Update CRC16-CCITT-FALSE
 *
 * @param[in]  crc  A start value for the CRC calculation, usually the
 *                  return value of a previous call to
 *                  crc16_ccitt_false_calc() or crc16_ccitt_false_update()
 * @param[in]  buf  Start of the memory area to checksum
 * @param[in]  len  Number of bytes to checksum
 *
 * @return          Checksum of the specified memory area based on the
 *                  given start value
 */
uint16_t crc16_ccitt_false_update(uint16_t crc, const unsigned char *buf, size_t len);

/**
 * @brief           Calculate CRC16-CCITT-FALSE
 *
 * Parameter | Value
 * --------: | :----
 * Polynom   | `0x1021`
 * Init      | `0xffff`
 * Refin     | `false`
 * Refout    | `false`
 * Xorout    | `0x0000`
 * Check     | `0x29b1`
 *
 * @param[in]  buf  Start of the memory area to checksum
 * @param[in]  len  Number of bytes to checksum
 *
 * @return          Checksum of the specified memory area
 */
uint16_t crc16_ccitt_false_calc(const unsigned char *buf, size_t len);

/**
 * @brief           Update CRC16-CCITT-AUG
 *
 * @param[in]  crc  A start value for the CRC calculation, usually the
 *                  return value of a previous call to
 *                  crc16_ccitt_aug_calc() or crc16_ccitt_aug_update()
 * @param[in]  buf  Start of the memory area to checksum
 * @param[in]  len  Number of bytes to checksum
 *
 * @return          Checksum of the specified memory area based on the
 *                  given start value
 */
static inline uint16_t crc16_ccitt_aug_update(uint16_t crc, const unsigned char *buf, size_t len)
{
    /* Since CCITT-AUG and CCITT-FALSE only differ in the starting
     * seed, we wrap around crc16_ccitt_false_update() for updating */
    return crc16_ccitt_false_update(crc, buf, len);
}

/**
 * @brief           Calculate CRC16-CCITT-AUG
 *
 * Parameter | Value
 * --------: | :----
 * Polynom   | `0x1021`
 * Init      | `0x1d0f`
 * Refin     | `false`
 * Refout    | `false`
 * Xorout    | `0x0000`
 * Check     | `0xe5cc`
 *
 * @param[in]  buf  Start of the memory area to checksum
 * @param[in]  len  Number of bytes to checksum
 *
 * @return          Checksum of the specified memory area
 */
uint16_t crc16_ccitt_aug_calc(const unsigned char *buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* CHECKSUM_CRC16_CCITT_H */

/** @} */
