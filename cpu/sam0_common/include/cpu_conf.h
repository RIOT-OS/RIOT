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

#if defined(CPU_SAMD10)
#include "vendor/samd10/include/samd10.h"
#elif defined(CPU_SAMD20)
#include "vendor/samd20/include/samd20.h"
#elif defined(CPU_SAMD20B)
#include "vendor/samd20/include_b/samd20.h"
#elif defined(CPU_SAMD21A)
#include "vendor/samd21/include_a/samd21.h"
#elif defined(CPU_SAMD21B)
#include "vendor/samd21/include_b/samd21.h"
#elif defined(CPU_SAMD21C)
#include "vendor/samd21/include_c/samd21.h"
#elif defined(CPU_SAMD21D)
#include "vendor/samd21/include_d/samd21.h"
#elif defined(CPU_SAMD51)
#include "vendor/samd51/include/samd51.h"
#elif defined(CPU_SAME51)
#include "vendor/same51/include/same51.h"
#elif defined(CPU_SAME54)
#include "vendor/same54/include/same54.h"
#elif defined(CPU_SAML10)
#include "vendor/saml10/include/sam.h"
#elif defined(CPU_SAML11)
#include "vendor/saml11/include/sam.h"
#elif defined(CPU_SAML21A)
#include "vendor/saml21/include/saml21.h"
#elif defined(CPU_SAML21B)
#include "vendor/saml21/include_b/saml21.h"
#elif defined(CPU_SAMR21)
#include "vendor/samr21/include/samr21.h"
#elif defined(CPU_SAMR30)
#include "vendor/samr30/include/samr30.h"
#elif defined(CPU_SAMR34)
#include "vendor/samr34/include/samr34.h"
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

#ifdef CPU_COMMON_SAML1X
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
#define FLASHPAGE_WRITE_BLOCK_SIZE      (16)
/* Writing should be always 4 byte aligned */
#define FLASHPAGE_WRITE_BLOCK_ALIGNMENT (4)
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

/**
 * @brief   The CPU has Low Power RAM that can be used as Heap
 */
#ifdef CPU_HAS_BACKUP_RAM
#define NUM_HEAPS (2)
#endif

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
