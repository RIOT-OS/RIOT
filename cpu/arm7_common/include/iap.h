/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef IAP_H_
#define IAP_H_

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

/* IAP-Commands  */
#define PREPARE_SECTOR_FOR_WRITE_OPERATION  (50)
#define COPY_RAM_TO_FLASH                   (51)
#define ERASE_SECTOR                        (52)
#define BLANK_CHECK_SECTOR                  (53)
#define READ_PART_ID                        (54)
#define READ_BOOT_CODE_VERSION              (55)
#define COMPARE                             (56)

/* IAP status codes */
#define CMD_SUCCESS                                 (0)
#define INVALID_COMMAND                             (1)
#define SRC_ADDR_ERROR                              (2)
#define DST_ADDR_ERROR                              (3)
#define SRC_ADDR_NOT_MAPPED                         (4)
#define DST_ADDR_NOT_MAPPED                         (5)
#define COUNT_ERROR                                 (6)
#define INVALID_SECTOR                              (7)
#define SECTOR_NOT_BLANK                            (8)
#define SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION     (9)
#define COMPARE_ERROR                               (10)
#define BUSY                                        (11)

#define INVALID_ADDRESS     (0xFF)

/* IAP start location on flash */
#define IAP_LOCATION    (0x7FFFFFF1)

/* PLL */
#define PLLCON_PLLE     (0x01)      ///< PLL Enable
#define PLLCON_PLLD     (0x00)      ///< PLL Disable
#define PLLCON_PLLC     (0x03)      ///< PLL Connect
#define PLLSTAT_PLOCK   (0x0400)    //</ PLL Lock Status

/*
 * @brief:  Converts 'addr' to sector number
 * @note:   Sector table (Users Manual P. 610)
 *
 * @param addr      Flash address
 *
 * @return  Sector number. 0xFF on error
 */
uint8_t iap_get_sector(uint32_t addr);

#ifdef __cplusplus
}
#endif


#endif /*IAP_H_*/
