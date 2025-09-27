/*
 * SPDX-FileCopyrightText: 2021 Mesotic SAS
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup        cpu_nrf9160 Nordic nRF9160 MCU
 * @ingroup         cpu
 * @brief           Nordic nRF9160 family of CPUs
 * @{
 *
 * @file
 * @brief       nRF9160 specific CPU configuration
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 */

#include "vendor/nrf9160.h"
#include "vendor/nrf9160_bitfields.h"
#include "vendor/nrf9160_peripherals.h"

#include "cpu_conf_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)    /**< Default ARM IRQ priority */
#define CPU_FLASH_BASE                  (0x00000000) /**< ROM Base Address */

#define CPU_IRQ_NUMOF                   (65U)   /**< nRF9160 specific IRQ count */
/** @} */

/**
 * @name   Flash page configuration
 * @{
 */
#define FLASHPAGE_SIZE              (4096U) /**< Size of a page in bytes */
#define FLASHPAGE_NUMOF             (256U)  /**< Total number of flash pages */

/* The minimum block size which can be written is 4B. However, the erase
 * block is always FLASHPAGE_SIZE.
 */
#define FLASHPAGE_WRITE_BLOCK_SIZE      (4U) /**< Minimum block size */
/* Writing should be always 4 bytes aligned */
#define FLASHPAGE_WRITE_BLOCK_ALIGNMENT (4U) /**< Mandatory alignment */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
