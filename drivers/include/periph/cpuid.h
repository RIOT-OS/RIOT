/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_cpuid CPUID
 * @ingroup     drivers_periph
 * @brief       Low-level CPU ID peripheral driver
 *
 * Provides access the CPU's serial number
 *
 * @{
 * @file
 * @brief       Low-level CPUID peripheral driver interface definitions
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#ifndef PERIPH_CPUID_H_
#define PERIPH_CPUID_H_

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def CPUID_LEN
 *
 * @brief   The length in byte of the CPU's serial number.
 *
 * @note    Must be defined in the CPU's @ref cpu_conf.h
 */
#if CPUID_LEN

/**
 * @brief   Gets the serial number of the CPU.
 *
 * @param[out] id   The serial number of the CPU of length CPU_ID_LEN (must be
 *                  defined in the CPU's cpu_conf.h)
 */
void cpuid_get(void *id);
#endif /* CPUID_LEN */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPUID_H_ */
/**
 * @}
 */
