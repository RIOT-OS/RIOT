/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 */

#ifndef __CPU_CONF_H
#define __CPU_CONF_H

#include "samd21.h"

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
 * @brief Buffer size to use by the transceiver
 */
#define TRANSCEIVER_BUFFER_SIZE         (3)

/**
 * @brief CPUID_ID_LEN length of cpuid in bytes
 */
#define CPUID_ID_LEN                    (16) /* 128 bits long, 16 bytes long */

/**
 * @brief Definition of different panic modes
 */
typedef enum {
    NMI_HANDLER,
    HARD_FAULT,
    DUMMY_HANDLER
} panic_t;

#ifdef __cplusplus
}
#endif

#endif /* __CPU_CONF_H */
/** @} */
