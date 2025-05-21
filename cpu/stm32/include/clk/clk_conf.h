/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           Main header for STM32 clock configuration
 *
 * @author          Alexandre Abadie <alexandre.abadie@inria.fr>
*/

#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F1) || \
    defined(CPU_FAM_STM32F3)
#include "f0f1f3/cfg_clock_default.h"
#elif defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) || \
      defined(CPU_FAM_STM32F7)
#include "f2f4f7/cfg_clock_default.h"
#elif defined(CPU_FAM_STM32G0) || defined(CPU_FAM_STM32G4)
#include "g0g4/cfg_clock_default.h"
#elif defined(CPU_FAM_STM32C0)
#include "c0/cfg_clock_default.h"
#elif defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
#include "l0l1/cfg_clock_default.h"
#elif defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32L5) || \
      defined(CPU_FAM_STM32WB) || defined(CPU_FAM_STM32WL)
#include "l4l5wx/cfg_clock_default.h"
#elif defined(CPU_FAM_STM32U5)
#include "u5/cfg_clock_default.h"
#elif defined(CPU_FAM_STM32MP1)
#include "mp1/cfg_clock_default.h"
#else
#error "No clock configuration available"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

/** @} */
