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

/*
 * @brief   Dual Input Fast Program
 * NOTE: Only available in Dual mode, which is not supported yet.
 */
#define N25Q128_OPCODE_DIFP         (0xA2)

/*
 * @brief   Dual Input Extended Fast Program
 * NOTE: Only available in Dual mode, which is not supported yet.
 */
#define N25Q128_OPCODE_DIEFP        (0xD2)

/*
 * @brief   Quad Input Fast Program
 * NOTE: Only available in Quad mode, which is not supported yet.
 */
#define N25Q128_OPCODE_QIFP         (0x32)

/*
 * @brief   Quad Input Extended Fast Program
 * NOTE: Only available in Quad mode, which is not supported yet.
 */
#define N25Q128_OPCODE_QIEFP        (0x12)

/*
 * @brief   Program OTP (Program of OTP area)
 */
#define N25Q128_OPCODE_POTP         (0x42)

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

/*
 * @brief   Read Lock Register
 */
#define N25Q128_OPCODE_RDLR        (0xE8)

/*
 * @brief   Write to Lock Register
 */
#define N25Q128_OPCODE_WRLR         (0xE5)

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

/*
 * @brief   Write NV Configuration Register
 */
#define N25Q128_OPCODE_WRNVCR       (0xB1)

/*
 * @brief   Read Volatile Configuration Register
 */
#define N25Q128_OPCODE_RDVCR        (0x85)

/*
 * @brief   Write Volatile Configuration Register
 */
#define N25Q128_OPCODE_WRVCR        (0x81)

/*
 * @brief   Read Volatile Enhanced Configuration Register
 */
#define N25Q128_OPCODE_RDVECR       (0x65)

/*
 * @brief   Write Volatile Enhanced Configuration Register
 */
#define N25Q128_OPCODE_WRVECR       (0x61)

/** }@ Extended SPI OPCODE */

/*
 * @brief   Legacy SPI Status Register of the N25Q128 device.
 */
#define N25Q128_STAT_REG_WIP        (0x01)
#define N25Q128_STAT_REG_WEL        (0x02)
#define N25Q128_STAT_REG_BP0        (0x04)
#define N25Q128_STAT_REG_BP1        (0x08)
#define N25Q128_STAT_REG_BP2        (0x10)
#define N25Q128_STAT_REG_TB         (0x20)
#define N25Q128_STAT_REG_BP3        (0x40)
#define N25Q128_STAT_REG_SRWD       (0x80)

/*
 * @brief   Flag Status Register of the N25Q128 device.
 */
/* First bit is 'RESERVED'. */
#define N25Q128_FLAG_REG_PROT       (0x02) /*< Protection; Error. */
#define N25Q128_FLAG_REG_PROG_SUSP  (0x04) /*< Program Suspend; Status. */
#define N25Q128_FLAG_REG_VPP        (0x08) /*< VPP; Error. */
#define N25Q128_FLAG_REG_PROG       (0x10) /*< Program; Error. */
#define N25Q128_FLAG_REG_ERASE      (0x20) /*< Erase; Error. */
#define N25Q128_FLAG_REG_ERASE_SUSP (0x40) /*< Erase Suspend; Status. */
#define N25Q128_FLAG_REG_PE_CONTRL  (0x80) /*< P/E Controller (not WIP); Status. */

#ifdef __cplusplus
}
#endif

#endif /* N25Q128_INTERNAL_H */
/** @} */
