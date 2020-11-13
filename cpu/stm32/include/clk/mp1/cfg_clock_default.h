/*
 * Copyright (C) 2020 Savoir-faire Linux
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
 * @brief           Main header for STM32MP1 clock configuration
 *
 * @author          Gilles DOFFE <gilles.doffe@savoirfairelinux.com>
*/

#ifndef CLK_MP1_CFG_CLOCK_DEFAULT_H
#define CLK_MP1_CFG_CLOCK_DEFAULT_H

#if defined(CPU_LINE_STM32MP157Cxx)
#include "mp1/cfg_clock_default_208.h"
#else
#error "No clock configuration available for this family"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* CLK_MP1_CFG_CLOCK_DEFAULT_H */
/** @} */
