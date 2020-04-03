/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_stm32
 * @{
 *
 * @file
 * @brief       Common configuration for STM32 RTT
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef CFG_RTT_DEFAULT_H
#define CFG_RTT_DEFAULT_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    RTT configuration
 *
 * On the STM32Lx platforms, we always utilize the LPTIM1.
 * @{
 */
#define RTT_FREQUENCY       (1024U)             /* 32768 / 2^n */
#define RTT_MAX_VALUE       (0x0000ffff)        /* 16-bit timer */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CFG_RTT_DEFAULT_H */
/** @} */
