/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers
 * @{
 *
 * @file
 * @brief       Internal defines for the  N25Q128 serial flash memory
 *
 * @author      René Herthel <rene-herthel@haw-hamburg.de>
 *
 * @}
 */

#ifndef N25Q128_INTERNAL_H
#define N25Q128_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * @brief   Extended SPI Operation Codes.
 * @{
 */

/*
 * @brief   Read Identification
 */
#define N25Q128_OPCODE_RDID         (0x9E)
#define N25Q128_OPCODE_RDID_MAX     (20)

/*
 * @brief   Read Data Bytes
 */
#define N25Q128_OPCODE_READ         (0x03)

/*
 * @brief   Read Data Bytes at Higher Speed
 */
#define N25Q128_OPCODE_FAST_READ    (0x0B)

/*
 * @brief   Dual Output Fast Read
 * NOTE: Only available in Dual mode, which is not supported yet.
 */
#define N25Q128_OPCODE_DOFR         (0x3B)

/*
 * @brief   Dual Input/Output Fast Read
 * NOTE: Only available in Dual mode, which is not supported yet.
 */
#define N25Q128_OPCODE_DIOFR        (0xBB)

/*
 * @brief   Quad Output Fast Read
 * NOTE: Only available in Quad mode, which is not supported yet.
 */
#define N25Q128_OPCODE_QOFR         (0x6B)

/*
 * @brief   Quad Input/Output Fast Read
 * NOTE: Only available in Quad mode, which is not supported yet.
 */
#define N25Q128_OPCODE_QIOFR        (0xEB)

/*
 * @brief   Read OTP (Read of OTP area)
 */
#define N25Q128_OPCODE_ROTP         (0x4B)
#define N25Q128_OPCODE_ROTP_MAX     (65)

/*
 * @brief   Write Enable
 */
#define N25Q128_OPCODE_WREN         (0x06)

/*
 * @brief   Write Disable
 */
#define N25Q128_OPCODE_WRDI         (0x04)

/*
 * @brief   Page Program
 */
#define N25Q128_OPCODE_PP           (0x02)
#define N25Q128_OPCODE_PP_MAX       (256)

/*
 * @brief   Dual Input Fast Program
 * NOTE: Only available in Dual mode, which is not supported yet.
 */
#define N25Q128_OPCODE_DIFP         (0xA2)
#define N25Q128_OPCODE_DIFP_MAX     (256)

/*
 * @brief   Dual Input Extended Fast Program
 * NOTE: Only available in Dual mode, which is not supported yet.
 */
#define N25Q128_OPCODE_DIEFP        (0xD2)
#define N25Q128_OPCODE_DIEFP_MAX    (256)

/*
 * @brief   Quad Input Fast Program
 * NOTE: Only available in Quad mode, which is not supported yet.
 */
#define N25Q128_OPCODE_QIFP         (0x32)
#define N25Q128_OPCODE_QIFP_MIN     (1)
#define N25Q128_OPCODE_QIFP_MAX     (256)

/*
 * @brief   Quad Input Extended Fast Program
 * NOTE: Only available in Quad mode, which is not supported yet.
 */
#define N25Q128_OPCODE_QIEFP        (0x12)
#define N25Q128_OPCODE_QIEFP_MAX    (256)

/*
 * @brief   Program OTP (Program of OTP area)
 */
#define N25Q128_OPCODE_POTP         (0x42)
#define N25Q128_OPCODE_POTP_MAX     (65)

/*
 * @brief   SubSector Erase
 */
#define N25Q128_OPCODE_SSE          (0x20)

/*
 * @brief   Sector Erase
 */
#define N25Q128_OPCODE_SE           (0xD8)

/*
 * @brief   Bulk Erase
 */
#define N25Q128_OPCODE_BE           (0xC7)

/*
 * @brief   Program/Erase Resume
 */
#define N25Q128_OPCODE_PER          (0x7A)

/*
 * @brief   Program/Erase Suspend
 */
#define N25Q128_OPCODE_PES          (0x75)

/*
 * @brief   Read Status Register
 */
#define N25Q128_OPCODE_RDSR         (0x05)

/*
 * @brief   Write Status Register
 */
#define N25Q128_OPCODE_WRSR         (0x01)
#define N25Q128_OPCODE_WRSR_MAX     (1)

/*
 * @brief   Read Lock Register
 */
#define N25Q128_OPCODE_RDLR        (0xE8)

/*
 * @brief   Write to Lock Register
 */
#define N25Q128_OPCODE_WRLR         (0xE5)
#define N25Q128_OPCODE_WRLR_MAX     (1)

/*
 * @brief   Read Flag Status Register
 */
#define N25Q128_OPCODE_RFSR         (0x70)

/*
 * @brief   Clear Flag Status Register
 */
#define N25Q128_OPCODE_CLFSR        (0x50)

/*
 * @brief   Read NV Configuration Register 1
 */
#define N25Q128_OPCODE_RDNVCR       (0xB5)
#define N25Q128_OPCODE_RDNVCR_MAX   (2)

/*
 * @brief   Write NV Configuration Register
 */
#define N25Q128_OPCODE_WRNVCR       (0xB1)
#define N25Q128_OPCODE_WRNVCR_MAX   (2)

/*
 * @brief   Read Volatile Configuration Register
 */
#define N25Q128_OPCODE_RDVCR        (0x85)

/*
 * @brief   Write Volatile Configuration Register
 */
#define N25Q128_OPCODE_WRVCR        (0x81)
#define N25Q128_OPCODE_WRVCR_MAX    (1)

/*
 * @brief   Read Volatile Enhanced Configuration Register
 */
#define N25Q128_OPCODE_RDVECR       (0x65)

/*
 * @brief   Write Volatile Enhanced Configuration Register
 */
#define N25Q128_OPCODE_WRVECR       (0x61)
#define N25Q128_OPCODE_WRVECR_MAX   (1)

/** }@ Extended SPI OPCODE */

#ifdef __cplusplus
}
#endif

#endif /* N25Q128_INTERNAL_H */
/** @} */
