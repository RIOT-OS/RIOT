/*
 * Copyright (C) 2024 BISSELL Homecare, Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       Configure STM32C0 clock
 *
 * @author      Jason Parker <Jason.Parker@bissell.com>
 */


#include "cfg_clock_common_fx_gx_mp1_c0.h"
#include "kernel_defines.h"
#include "macros/units.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    C0 clock settings
 *
 * @{
 */
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && (CONFIG_CLOCK_HSE < MHZ(4) || CONFIG_CLOCK_HSE > MHZ(48))
#error "HSE clock frequency must be between 4MHz and 48MHz"
#endif

#ifdef CPU_FAM_STM32C0
#ifndef CONFIG_CLOCK_HSISYS_DIV
#define CONFIG_CLOCK_HSISYS_DIV         (1)
#endif
#endif

#if IS_ACTIVE(CONFIG_USE_CLOCK_HSI)
#define CLOCK_CORECLOCK                 (CONFIG_CLOCK_HSI / CONFIG_CLOCK_HSISYS_DIV)

#elif IS_ACTIVE(CONFIG_USE_CLOCK_HSE)
#if !IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
#error "The board doesn't provide an HSE oscillator"
#endif
#define CLOCK_CORECLOCK                 (CONFIG_CLOCK_HSE)

#endif

#define CLOCK_AHB                       CLOCK_CORECLOCK  /* max: 64MHz (G0), 170MHZ (G4) */

#ifndef CONFIG_CLOCK_APB1_DIV
#define CONFIG_CLOCK_APB1_DIV           (1)
#endif
#define CLOCK_APB1                      (CLOCK_CORECLOCK / CONFIG_CLOCK_APB1_DIV)  \
                                        /* max: 64MHz (G0), 170MHZ (G4) */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
