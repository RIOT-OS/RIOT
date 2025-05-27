/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_common_stm32
 * @{
 *
 * @file
 * @brief       Common configuration for STM32 RTT
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

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
#define RTT_CLOCK_FREQUENCY (32768U)                    /* in Hz */

#define RTT_MAX_VALUE       (0x0000ffff)                /* 16-bit timer */
#define RTT_MAX_FREQUENCY   (RTT_CLOCK_FREQUENCY)       /* 32768Hz at @32768Hz */
#define RTT_MIN_FREQUENCY   (RTT_CLOCK_FREQUENCY / 128) /* 256Hz at @32768Hz */

#ifndef RTT_FREQUENCY
#define RTT_FREQUENCY       (RTT_MAX_FREQUENCY)   /* in Hz */
#endif
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
