/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           Main header for STM32F2/F4/F7 clock configuration
 *
 * @author          Alexandre Abadie <alexandre.abadie@inria.fr>
*/

#ifndef CLK_F2F4F7_CFG_CLOCK_DEFAULT_H
#define CLK_F2F4F7_CFG_CLOCK_DEFAULT_H

#if defined(CPU_FAM_STM32F2)
#include "f2f4f7/cfg_clock_default_120.h"
#elif defined(CPU_FAM_STM32F4)
#if defined(CPU_LINE_STM32F401xC) || defined(CPU_LINE_STM32F401xE)
#include "f2f4f7/cfg_clock_default_84.h"
#elif defined(CPU_LINE_STM32F410Cx) || defined(CPU_LINE_STM32F410Rx) || \
      defined(CPU_LINE_STM32F410Tx) || defined(CPU_LINE_STM32F411xE) || \
      defined(CPU_LINE_STM32F412Cx) || defined(CPU_LINE_STM32F412Rx) || \
      defined(CPU_LINE_STM32F412Vx) || defined(CPU_LINE_STM32F412Zx) || \
      defined(CPU_LINE_STM32F413xx) || defined(CPU_LINE_STM32F423xx)
#include "f2f4f7/cfg_clock_default_100.h"
#elif defined(CPU_LINE_STM32F405xx) || defined(CPU_LINE_STM32F407xx) || \
      defined(CPU_LINE_STM32F415xx) || defined(CPU_LINE_STM32F417xx) || \
      defined(CPU_LINE_STM32F427xx) || defined(CPU_LINE_STM32F437xx) || \
      defined(CPU_LINE_STM32F429xx) || defined(CPU_LINE_STM32F439xx) || \
      defined(CPU_LINE_STM32F446xx) || defined(CPU_LINE_STM32F469xx) || \
      defined(CPU_LINE_STM32F479xx)
#include "f2f4f7/cfg_clock_default_180.h"
#else
#error "No clock configuration available for this F4 line"
#endif
#elif defined(CPU_FAM_STM32F7)
#include "f2f4f7/cfg_clock_default_216.h"
#else
#error "No clock configuration available for this family"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* CLK_F2F4F7_CFG_CLOCK_DEFAULT_H */
/** @} */
