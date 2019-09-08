/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_sam0_common
 * @brief           Implementation specific CPU configuration options
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#include "cpu_conf_common.h"
#if defined(CPU_SAML1X)
#include "vendor/sam23.h"
#elif defined(CPU_SAMD5X)
#include "vendor/samd5x.h"
#else
#include "vendor/sam0.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#define CPU_IRQ_NUMOF                   PERIPH_COUNT_IRQn
#define CPU_FLASH_BASE                  FLASH_ADDR

#ifdef CPU_SAML1X
#define CPU_FLASH_RWWEE_BASE            DATAFLASH_ADDR
#else
#define CPU_FLASH_RWWEE_BASE            NVMCTRL_RWW_EEPROM_ADDR
#endif
/** @} */

/**
 * @brief   Flash page configuration
 * @{
 */
/* a flashpage in RIOT is mapped to a flash row on the SAM0s */
#if defined(NVMCTRL_ROW_SIZE)
#define FLASHPAGE_SIZE             (NVMCTRL_ROW_SIZE)
#elif defined(NVMCTRL_BLOCK_SIZE)
#define FLASHPAGE_SIZE             (NVMCTRL_BLOCK_SIZE)
#elif defined(NVMCTRL_PAGE_SIZE)
/* saml1x: The NVM is organized into rows, where each row contains four pages,
as shown in the NVM Row Organization figure. */
#define FLASHPAGE_SIZE             (4 * NVMCTRL_PAGE_SIZE)
#else
#error "Unsupported Device"
#endif

/* one SAM0 row contains 4 SAM0 pages, so 4 SAM0 pages contain
 * the amount of a RIOT flashpage
 */
#define FLASHPAGE_PAGES_PER_ROW    (FLASHPAGE_SIZE/FLASH_PAGE_SIZE)
/* number of RIOT flashpages on device */
#define FLASHPAGE_NUMOF            (FLASH_NB_OF_PAGES / FLASHPAGE_PAGES_PER_ROW)
/* The minimum block size which can be written is 16B. However, the erase
 * block is always FLASHPAGE_SIZE (SAM0 row).
 */
#define FLASHPAGE_RAW_BLOCKSIZE    (16)
/* Writing should be always 4 byte aligned */
#define FLASHPAGE_RAW_ALIGNMENT    (4)
/* Add RWWEE memory if supported by revision of the chip
 * On some chips it is called RWW EEPROM while on some DATAFLASH, try to
 * catch all without relying on the CPU model but on the named defines
 */
#ifdef NVMCTRL_RWW_EEPROM_SIZE
#define FLASHPAGE_RWWEE_NUMOF      (NVMCTRL_RWWEE_PAGES / FLASHPAGE_PAGES_PER_ROW)
#endif
#ifdef DATAFLASH_SIZE
#define FLASHPAGE_RWWEE_NUMOF      (DATAFLASH_NB_OF_PAGES / FLASHPAGE_PAGES_PER_ROW)
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
