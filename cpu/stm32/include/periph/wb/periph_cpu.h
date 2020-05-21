/*
 * Copyright (C) 2019 Inria
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
 * @brief           STM32WB CPU specific definitions for internal peripheral handling
 *
 * @author          Francisco Molina <francois-xavier.molina@inria.fr>
 *
 */

#ifndef PERIPH_WB_PERIPH_CPU_H
#define PERIPH_WB_PERIPH_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Starting address of the CPU ID
 */
#define CPUID_ADDR          (0x1fff7580)

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_WB_PERIPH_CPU_H */
/** @} */
