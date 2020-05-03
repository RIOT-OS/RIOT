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

#ifndef PERIPH_F7_PERIPH_CPU_H
#define PERIPH_F7_PERIPH_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Starting address of the CPU ID
 */
#if defined(CPU_LINE_STM32F722xx) || defined(CPU_LINE_STM32F723xx)
#define CPUID_ADDR          (0x1ff07a10)
#else
#define CPUID_ADDR          (0x1ff0f420)
#endif

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_F7_PERIPH_CPU_H */
/** @} */
