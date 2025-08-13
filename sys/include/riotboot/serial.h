/*
 * Copyright (C) 2020 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    sys_riotboot_serial Serial Bootloader Protocol
 * @ingroup     sys
 * @{
 *
 * @file
 * @brief       riotboot as a serial bootloader
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief riotboot commands
 *        Commands typically have the format [type|length|value]
 *        where type and length are one byte and value is $length bytes.
 *
 *        Commands are signed with a CRC-8 checksum that is calculated
 *        over the entire record. The Checksum is not part of length.
 * @{
 */
/**
 * start application.
 * parameter:
 *  - '\n': launch default application
 *  - '0' : launch slot 0
 *  - '1' : launch slot 1
 */
#define RIOTBOOT_CMD_BOOT       'b'

/**
 * erase page
 * parameter:
 *  - length of page number (8 bit - should be 4)
 *  - page number (32 bit little endian)
 */
#define RIOTBOOT_CMD_ERASE      'e'

/**
 * write data
 * parameter:
 *  - length of data + address (8 bit)
 *  - destination address (32 bit little endian)
 *  - data (up to (RX_BUF_LEN-6) bytes)
 */
#define RIOTBOOT_CMD_WRITE      'w'

/**
 * get page of address
 * parameter:
 *  - length of address (8 bit - should be 4)
 *  - address (32 bit little endian)
 *
 * returns status code followed by 32 bit little endian
 * value that represents the page in which the address lies.
 */
#define RIOTBOOT_CMD_GET_PAGE   'P'
/** @} */

/**
 * @brief riotboot response codes to commands
 * @{
 */
/** operation successful */
#define RIOTBOOT_STAT_OK        '.'
/** CRC error - try again */
#define RIOTBOOT_STAT_BAD_CRC   '?'
/** illegal parameter */
#define RIOTBOOT_STAT_ILLEGAL   '!'
/** @} */

/**
 * @brief riotboot serial loader synchronisation
 * @{
 */
/* sent to stop auto-boot */
#define RIOTBOOT_ENTER_LOADER   'B'
/* sent to probe if auto-boot is paused and riotboot is active */
#define RIOTBOOT_PROBE          '?'

/* continuously sent by riotboot before booting */
#define RIOTBOOT_STAT_WAITING   'b'
/* indicates riotboot is ready to accept commands */
#define RIOTBOOT_STAT_READY     '>'
/** @} */

/**
 * @brief   CRC-8 Polynom used for riotboot chunks
 */
#ifndef RIOTBOOT_CRC8_POLY
#define RIOTBOOT_CRC8_POLY      (0x31)
#endif

/**
 * @brief  Start interactive serial bootloader
 * @return slot to boot, -1 if default slot should be started
 */
int riotboot_serial_loader(void);

#ifdef __cplusplus
}
#endif
