/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           Shared CPU specific configuration for STM32 family
 *
 * @author          Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef CPU_CONF_STM32_COMMON_H
#define CPU_CONF_STM32_COMMON_H

#include "periph_cpu.h"

#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F3) || \
    defined(CPU_FAM_STM32F7) || defined(CPU_FAM_STM32L0) || \
    defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32L4) || \
    defined(CPU_FAM_STM32WB) || defined(CPU_FAM_STM32G4) || \
    defined(CPU_FAM_STM32G0) || defined(CPU_FAM_STM32L5)

/**
 * @brief   Timing register settings
 *
 * @ref i2c_timing_param_t
 */
static const i2c_timing_param_t timing_params[] = {
#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F7) || \
    defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32WB) || \
    defined(CPU_FAM_STM32G4) || defined(CPU_FAM_STM32G0) || \
    defined(CPU_FAM_STM32L5)
    [ I2C_SPEED_NORMAL ]    = {
        .presc  = 0xB,
        .scll   = 0x13,     /* t_SCLL   = 5.0us  */
        .sclh   = 0xF,      /* t_SCLH   = 4.0us  */
        .sdadel = 0x2,      /* t_SDADEL = 500ns  */
        .scldel = 0x4,      /* t_SCLDEL = 1250ns */
    },
    [ I2C_SPEED_FAST ]      = {
        .presc  = 5,
        .scll   = 0x9,      /* t_SCLL   = 1250ns */
        .sclh   = 0x3,      /* t_SCLH   = 500ns  */
        .sdadel = 0x3,      /* t_SDADEL = 375ns  */
        .scldel = 0x3,      /* t_SCLDEL = 500ns  */
    },
    [ I2C_SPEED_FAST_PLUS ] = {
        .presc =  5,
        .scll =   0x3,      /* t_SCLL   = 500ns */
        .sclh =   0x1,      /* t_SCLH   = 250ns */
        .sdadel = 0x0,      /* t_SDADEL = 0ns   */
        .scldel = 0x1,      /* t_SCLDEL = 250ns */
    }
#elif defined(CPU_FAM_STM32F3)
    [ I2C_SPEED_NORMAL ]    = {
        .presc  = 1,
        .scll   = 0x13,     /* t_SCLL   = 5.0us  */
        .sclh   = 0xF,      /* t_SCLH   = 4.0us  */
        .sdadel = 0x2,      /* t_SDADEL = 500ns  */
        .scldel = 0x4,      /* t_SCLDEL = 1250ns */
    },
    [ I2C_SPEED_FAST ]      = {
        .presc  = 0,
        .scll   = 0x9,      /* t_SCLL   = 1250ns */
        .sclh   = 0x3,      /* t_SCLH   = 500ns  */
        .sdadel = 0x1,      /* t_SDADEL = 125ns  */
        .scldel = 0x3,      /* t_SCLDEL = 500ns  */
    },
    [ I2C_SPEED_FAST_PLUS ] = {
        .presc  = 0,
        .scll   = 0x6,      /* t_SCLL   = 875ns */
        .sclh   = 0x3,      /* t_SCLH   = 500ns */
        .sdadel = 0x0,      /* t_SDADEL = 0ns   */
        .scldel = 0x1,      /* t_SCLDEL = 250ns */
    }
#elif defined(CPU_FAM_STM32L0)
    [ I2C_SPEED_NORMAL ]    = {
        .presc  = 1,
        .scll   = 0x56,     /* t_SCLL   = 5.0us  */
        .sclh   = 0x3E,     /* t_SCLH   = 4.0us  */
        .sdadel = 0x1,      /* t_SDADEL = 500ns  */
        .scldel = 0xA,      /* t_SCLDEL = 1250ns */
    },
    [ I2C_SPEED_FAST ]      = {
        .presc  = 0,
        .scll   = 0x2E,     /* t_SCLL   = 1250ns */
        .sclh   = 0x11,     /* t_SCLH   = 500ns  */
        .sdadel = 0x1,      /* t_SDADEL = 125ns  */
        .scldel = 0xB,      /* t_SCLDEL = 500ns  */
    },
    [ I2C_SPEED_FAST_PLUS ] = {
        .presc  = 0,
        .scll   = 0x6,      /* t_SCLL   = 875ns */
        .sclh   = 0x3,      /* t_SCLH   = 500ns */
        .sdadel = 0x0,      /* t_SDADEL = 0ns   */
        .scldel = 0x1,      /* t_SCLDEL = 250ns */
    }
#endif
};

#endif /* CPU_FAM_STM32F0 || CPU_FAM_STM32F3 || CPU_FAM_STM32F7 ||
          CPU_FAM_STM32L0 || CPU_FAM_STM32L4 || CPU_FAM_STM32WB ||
          CPU_FAM_STM32G4 || CPU_FAM_STM32G0 || CPU_FAM_STM32L5 */

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_STM32_COMMON_H */
/** @} */
