/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2020 Philipp-Alexander Blum <philipp-blum@jakiku.de>
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
 * @author      Philipp-Alexander Blum <philipp-blum@jakiku.de>
 *
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#include "cpu_conf_common.h"

#ifdef CPU_MODEL_NRF52832XXAA
#include "vendor/nrf52.h"
#include "vendor/nrf52_bitfields.h"
#include "vendor/nrf52832_peripherals.h"
#elif defined(CPU_MODEL_NRF52805XXAA)
#include "vendor/nrf52805.h"
#include "vendor/nrf52805_bitfields.h"
#include "vendor/nrf52805_peripherals.h"
#elif defined(CPU_MODEL_NRF52810XXAA)
#include "vendor/nrf52810.h"
#include "vendor/nrf52810_bitfields.h"
#include "vendor/nrf52810_peripherals.h"
#elif defined(CPU_MODEL_NRF52811XXAA)
#include "vendor/nrf52811.h"
#include "vendor/nrf52811_bitfields.h"
#include "vendor/nrf52811_peripherals.h"
#elif defined(CPU_MODEL_NRF52820XXAA)
#include "vendor/nrf52820.h"
#include "vendor/nrf52820_bitfields.h"
#include "vendor/nrf52820_peripherals.h"
#elif defined(CPU_MODEL_NRF52833XXAA)
#include "vendor/nrf52833.h"
#include "vendor/nrf52833_bitfields.h"
#include "vendor/nrf52833_peripherals.h"
#elif defined(CPU_MODEL_NRF52840XXAA)
#include "vendor/nrf52840.h"
#include "vendor/nrf52840_bitfields.h"
#include "vendor/nrf52840_peripherals.h"
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

#if defined(CPU_MODEL_NRF52805XXAA)
#define CPU_IRQ_NUMOF                   (26U)
#elif defined(CPU_MODEL_NRF52810XXAA)
#define CPU_IRQ_NUMOF                   (30U)
#elif defined(CPU_MODEL_NRF52811XXAA)
#define CPU_IRQ_NUMOF                   (30U)
#elif defined(CPU_MODEL_NRF52820XXAA)
#define CPU_IRQ_NUMOF                   (40U)
#elif defined(CPU_MODEL_NRF52832XXAA)
#define CPU_IRQ_NUMOF                   (39U)
#elif defined(CPU_MODEL_NRF52833XXAA)
#define CPU_IRQ_NUMOF                   (48U)
#elif defined(CPU_MODEL_NRF52840XXAA)
#define CPU_IRQ_NUMOF                   (48U)
#endif
/** @} */

/**
 * @brief   Flash page configuration
 * @{
 */
#ifdef BPROT_PRESENT
#define FLASHPAGE_SIZE              BPROT_REGIONS_SIZE
#define FLASHPAGE_NUMOF             BPROT_REGIONS_NUM
#elif defined(CPU_MODEL_NRF52820XXAA)
#define FLASHPAGE_SIZE              (4096U)
#define FLASHPAGE_NUMOF             (64U)
#elif defined(CPU_MODEL_NRF52833XXAA)
#define FLASHPAGE_SIZE              (4096U)
#define FLASHPAGE_NUMOF             (128U)
#elif defined(CPU_MODEL_NRF52840XXAA)
#define FLASHPAGE_SIZE              (4096U)
#define FLASHPAGE_NUMOF             (256U)
#endif

/* The minimum block size which can be written is 4B. However, the erase
 * block is always FLASHPAGE_SIZE.
 */
#define FLASHPAGE_WRITE_BLOCK_SIZE      (4U)
/* Writing should be always 4 bytes aligned */
#define FLASHPAGE_WRITE_BLOCK_ALIGNMENT (4U)
/** @} */

#ifdef CPU_MODEL_NRF52840XXAA
/**
 * @name    OpenWSN timing constants
 *
 * @{
 */
/* Taken from OpenWSN @32.768Hz */
#define PORT_maxTxDataPrepare   (400/PORT_US_PER_TICK)
#define PORT_maxRxAckPrepare    (400/PORT_US_PER_TICK)
#define PORT_maxRxDataPrepare   (400/PORT_US_PER_TICK)
#define PORT_maxTxAckPrepare    (400/PORT_US_PER_TICK)
/* Measured 220us */
#define PORT_delayTx            (300/PORT_US_PER_TICK)
#define PORT_delayRx            (150/PORT_US_PER_TICK)
/** @} */
#endif

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

/** @} */
#endif /* CPU_CONF_H */
