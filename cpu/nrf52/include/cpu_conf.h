/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup        cpu_nrf52 Nordic nRF52 MCU
 * @ingroup         cpu
 * @brief           Nordic nRF52 family of CPUs
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

#ifdef CPU_MODEL_NRF52832XXAA
#include "vendor/nrf52.h"
#include "vendor/nrf52_bitfields.h"
#elif defined(CPU_MODEL_NRF52840XXAA)
#include "vendor/nrf52840.h"
#include "vendor/nrf52840_bitfields.h"
#else
#error "The CPU_MODEL of your board is currently not supported"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (2U)
#define CPU_FLASH_BASE                  (0x00000000)
#ifdef CPU_MODEL_NRF52832XXAA
#define CPU_IRQ_NUMOF                   (38U)
#elif CPU_MODEL_NRF52840XXAA
#define CPU_IRQ_NUMOF                   (46U)
#endif
/** @} */

/**
 * @brief   Flash page configuration
 * @{
 */
#define FLASHPAGE_SIZE                  (4096U)

#if defined(CPU_MODEL_NRF52832XXAA)
#define FLASHPAGE_NUMOF                 (128U)
#elif defined(CPU_MODEL_NRF52840XXAA)
#define FLASHPAGE_NUMOF                 (256U)
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

/**
 * @brief   Put the CPU in the low-power 'wait for event' state
 */
static inline void nrf52_sleep(void)
{
    __SEV();
    __WFE();
    __asm("nop");
}

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
