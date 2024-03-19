/*
 * Copyright (C) 2023 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup        cpu_nrf53 Nordic nRF53 MCU
 * @ingroup         cpu
 * @brief           Nordic nRF53 family of CPUs
 * @{
 *
 * @file
 * @brief       nRF53 specific CPU configuration
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#ifdef CPU_MODEL_NRF5340_APP
#include "vendor/nrf5340_application.h"
#include "vendor/nrf5340_application_bitfields.h"
#include "vendor/nrf5340_application_peripherals.h"
#elif defined(CPU_MODEL_NRF5340_NET)
#include "vendor/nrf5340_network.h"
#include "vendor/nrf5340_network_bitfields.h"
#include "vendor/nrf5340_network_peripherals.h"
#else
#error "The CPU_MODEL of your board is currently not supported"
#endif

#include "cpu_conf_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)    /**< Default ARM IRQ priority */

#ifdef CPU_MODEL_NRF5340_APP
#define CPU_FLASH_BASE                  (0x00000000) /**< ROM Base Address */
#define CPU_IRQ_NUMOF                   (69U)   /**< nRF53 specific IRQ count */
#endif /* def CPU_MODEL_NRF5340_APP */
/** @} */

/**
 * @brief   Flash page configuration
 * @{
 */
#define FLASHPAGE_SIZE              (4096U)  /**< Size of a page in bytes */
#define FLASHPAGE_NUMOF             (256U)   /**< Total number of flash pages */

/* The minimum block size which can be written is 4B. However, the erase
 * block is always FLASHPAGE_SIZE.
 */
#define FLASHPAGE_WRITE_BLOCK_SIZE      (4U)    /**< Minimum block size */
/* Writing should be always 4 bytes aligned */
#define FLASHPAGE_WRITE_BLOCK_ALIGNMENT (4U)    /**< Mandatory alignment */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
