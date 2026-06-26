/*
 * SPDX-FileCopyrightText: 2026 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_nrf54l
 * @{
 *
 * @file
 * @brief       nRF54L specific CPU configuration
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifdef CPU_MODEL_NRF54L15XXAA
/* keep the include order: types must come first, the peripheral instances
 * (global) second and the core specific definitions (application) last */
#include "vendor/nrf54l15_types.h"
#include "vendor/nrf54l15_global.h"
#include "vendor/nrf54l15_application.h"
#include "vendor/nrf54l15_application_peripherals.h"
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
#define CPU_DEFAULT_IRQ_PRIO            (1U)          /**< Default ARM IRQ priority */
#define CPU_FLASH_BASE                  (0x00000000)  /**< RRAM base address */
#define CPU_IRQ_NUMOF                   (270U)        /**< nRF54L specific IRQ count */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
