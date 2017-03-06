/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf52
 * @{
 *
 * @file
 * @brief       nRF52 specific CPU configuration
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#include "cpu_conf_common.h"
#include "vendor/nrf52.h"
#include "vendor/nrf52_bitfields.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (2U)
#define CPU_IRQ_NUMOF                   (38U)
#define CPU_FLASH_BASE                  (0x00000000)
/** @} */

/**
 * @brief   Flash page configuration
 * @{
 */
#define FLASHPAGE_SIZE                  (4096U)

#if defined(CPU_MODEL_NRF52XXAA)
#define FLASHPAGE_NUMOF                 (128U)
#endif
/** @} */

/**
 * @brief   SoftDevice settings
 * @{
 */
#ifdef SOFTDEVICE_PRESENT
#ifndef DONT_OVERRIDE_NVIC
#include "nrf_soc.h"
#undef NVIC_SetPriority
#define NVIC_SetPriority    sd_nvic_SetPriority
#endif /* DONT_OVERRIDE_NVIC */
#endif /* SOFTDEVICE_PRESENT */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
