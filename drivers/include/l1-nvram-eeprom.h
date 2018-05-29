/*
 * Copyright (C) 2016 Unwired Devices <info@unwds.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     nvram
 * @{
 *
 * @file		l1-nvram-eeprom.h
 *
 * @brief       STM32L1 Data EEPROM driver definitions
 *
 * Tested on:
 * - STM32L151CCU6
 *
 * @author      EP <ep@unwds.com>
 */
#ifndef L1_NVRAM_EEPROM_H_
#define L1_NVRAM_EEPROM_H_

#include "nvram.h"

/**
 * @brief Initializes an nvram_t structure
 *
 * @param[out]	dev		Pointer to NVRAM device structure
 *
 * @return	0 on success
 * @return	<0 on error
 */
int nvram_l1_eeprom_init(nvram_t *dev);

/** @defgroup FLASH_Flags
 * @{
 */
#define FLASH_FLAG_BSY                 FLASH_SR_BSY         /*!< FLASH Busy flag */
#define FLASH_FLAG_EOP                 FLASH_SR_EOP         /*!< FLASH End of Programming flag */
#define FLASH_FLAG_ENDHV               FLASH_SR_ENHV        /*!< FLASH End of High Voltage flag */
#define FLASH_FLAG_READY               FLASH_SR_READY       /*!< FLASH Ready flag after low power mode */
#define FLASH_FLAG_WRPERR              FLASH_SR_WRPERR      /*!< FLASH Write protected error flag */
#define FLASH_FLAG_PGAERR              FLASH_SR_PGAERR      /*!< FLASH Programming Alignment error flag */
#define FLASH_FLAG_SIZERR              FLASH_SR_SIZERR      /*!< FLASH Size error flag  */
#define FLASH_FLAG_OPTVERR             FLASH_SR_OPTVERR     /*!< FLASH Option Validity error flag  */
#define FLASH_FLAG_OPTVERRUSR          FLASH_SR_OPTVERRUSR  /*!< FLASH Option User Validity error flag  */
#define FLASH_FLAG_RDERR               FLASH_SR_RDERR       /*!< FLASH Read protected error flag
                                                                 (available only in STM32L1XX_MDP devices)  */

#define IS_FLASH_CLEAR_FLAG(FLAG) ((((FLAG) &(uint32_t)0xFFFFC0FD) == 0x00000000) && ((FLAG) != 0x00000000))

#define IS_FLASH_GET_FLAG(FLAG)  (((FLAG) == FLASH_FLAG_BSY) || ((FLAG) == FLASH_FLAG_EOP) || \
                                  ((FLAG) == FLASH_FLAG_ENDHV) || ((FLAG) == FLASH_FLAG_READY) || \
                                  ((FLAG) ==  FLASH_FLAG_WRPERR) || ((FLAG) == FLASH_FLAG_PGAERR) || \
                                  ((FLAG) ==  FLASH_FLAG_SIZERR) || ((FLAG) == FLASH_FLAG_OPTVERR) || \
                                  ((FLAG) ==  FLASH_FLAG_OPTVERRUSR) || ((FLAG) ==  FLASH_FLAG_RDERR))
/**
 * @}
 */

/**
 * @brief STM32 Data EEPROM unlock keys
 */
#define FLASH_PEKEY1               ((uint32_t)0x89ABCDEF)
#define FLASH_PEKEY2               ((uint32_t)0x02030405)

/**
 * @brief Checks that given address is within the FLASH area
 */
#define IS_FLASH_DATA_ADDRESS(ADDRESS) (((ADDRESS) >= 0x08080000) && ((ADDRESS) <= 0x08083FFF))

/**
 * @brief Timeout of FLASH programming
 */
#define FLASH_ER_PRG_TIMEOUT         ((uint32_t)0x8000)

/**
 * @brief Data EEPROM base address
 */
#define EEPROM_BASE ((uint32_t) 0x08080000)

#endif /* L1_NVRAM_EEPROM_H_ */
