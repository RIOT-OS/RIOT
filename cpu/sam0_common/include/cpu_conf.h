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
#include "vendor/sam0.h"

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
/** @} */

/**
 * @brief   Flash page configuration
 * @{
 */
/* a flashpage in RIOT is mapped to a flash row on the SAM0s */
#define FLASHPAGE_SIZE             (256U)
/* one SAM0 row contains 4 SAM0 pages -> 4x the amount of RIOT flashpages */
#define FLASHPAGE_NUMOF            (FLASH_NB_OF_PAGES / 4)
/* The minimum block size which can be written is 16B. However, the erase
 * block is always FLASHPAGE_SIZE (SAM0 row).
 */
#define FLASHPAGE_RAW_BLOCKSIZE    (16)
/* Writing should be always 4 byte aligned */
#define FLASHPAGE_RAW_ALIGNMENT    (4)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
