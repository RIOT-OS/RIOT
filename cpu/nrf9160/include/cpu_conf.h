/*
 * Copyright (C) 2021 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef CPU_CONF_H
#define CPU_CONF_H

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

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
