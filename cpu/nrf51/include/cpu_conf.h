/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

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

#ifndef CPU_CONF_H
#define CPU_CONF_H

#include "cpu_conf_common.h"
#include "vendor/nrf51.h"
#include "vendor/nrf51_bitfields.h"

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

#if defined(CPU_MODEL_NRF51X22XXAA)
#define FLASHPAGE_NUMOF         (256U)
#elif defined(CPU_MODEL_NRF51X22XXAB)
#define FLASHPAGE_NUMOF         (128U)
#endif
/** @} */

/**
 * @brief   Due to RAM restrictions, we need to limit the default GNRC packet
 *          buffer size on these CPUs
 * @{
 */
#ifndef GNRC_PKTBUF_SIZE
#define GNRC_PKTBUF_SIZE                (2048)
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

#endif /* CPU_CONF_H */
/** @} */
