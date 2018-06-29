/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_stm32f7
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "cpu.h"
#include "periph_cpu_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Starting address of the CPU ID
 */
#ifdef CPU_LINE_STM32F722xx
#define CPUID_ADDR          (0x1ff07a10)
#else
#define CPUID_ADDR          (0x1ff0f420)
#endif

/**
 * @brief   Available ports
 */
enum {
    PORT_A = 0,             /**< port A */
    PORT_B = 1,             /**< port B */
    PORT_C = 2,             /**< port C */
    PORT_D = 3,             /**< port D */
    PORT_E = 4,             /**< port E */
    PORT_F = 5,             /**< port F */
    PORT_G = 6,             /**< port G */
    PORT_H = 7,             /**< port H */
    PORT_I = 8,             /**< port I */
    PORT_J = 9,             /**< port J */
    PORT_K = 10             /**< port K */
};

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
