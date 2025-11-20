/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup        cpu_nrf51 Nordic nRF51 MCU
 * @ingroup         cpu
 * @brief           Nordic nRF51 family of CPUs
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "cpu_conf_common.h"
#include "vendor/nrf51.h"
#include "vendor/nrf51_bitfields.h"
#include "vendor/nrf51822_peripherals.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO    (1U)
#define CPU_IRQ_NUMOF           (26U)
#define CPU_FLASH_BASE          (0x00000000)
/** @} */

/**
 * @brief   Flash page configuration
 * @{
 */
#define FLASHPAGE_SIZE          (1024U)

#if defined(CPU_MODEL_NRF51X22XXAA) || defined(CPU_MODEL_NRF51X22XXAC)
#define FLASHPAGE_NUMOF         (256U)
#elif defined(CPU_MODEL_NRF51X22XXAB)
#define FLASHPAGE_NUMOF         (128U)
#endif
/* The minimum block size which can be written is 4B. However, the erase
 * block is always FLASHPAGE_SIZE.
 */
#define FLASHPAGE_WRITE_BLOCK_SIZE      (4U)
/* Writing should be always 4 bytes aligned */
#define FLASHPAGE_WRITE_BLOCK_ALIGNMENT (4U)
/** @} */

/**
 * @brief   Due to RAM restrictions, we need to limit the default GNRC packet
 *          buffer size on these CPUs
 * @{
 */
#ifndef CONFIG_GNRC_PKTBUF_SIZE
#define CONFIG_GNRC_PKTBUF_SIZE                (2048)
#endif
/** @} */

/**
 * @brief   CPU specific PWM configuration
 * @{
 */
#define PWM_GPIOTE_CH           (2U)
#define PWM_PPI_A               (0U)
#define PWM_PPI_B               (1U)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
