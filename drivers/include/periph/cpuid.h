/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    drivers_periph_cpuid CPUID
 * @ingroup     drivers_periph
 * @brief       Low-level CPU ID peripheral driver
 *
 * Provides access the CPU's serial number
 *
 * @note This is *distinct* from what is called CPUID in ARM CPUs (which only
 * covers variation down to silicon revisions, not down to individual instances
 * of a CPU).
 *
 * # (Low-) Power Implications
 *
 * The implementation **should** make sure, that calling cpuid_get() does not
 * introduce any long-term power usage penalties. If e.g. some peripheral has to
 * be powered on for the CPU ID to be read, the implementation **should** take
 * care to disable the peripheral again after the read is finished.
 *
 * @{
 * @file
 * @brief       Low-level CPUID peripheral driver interface definitions
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

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
 * @param[out] id   The serial number of the CPU of length CPUID_LEN (must be
 *                  defined in the CPU's cpu_conf.h)
 */
void cpuid_get(void *id);
#endif /* CPUID_LEN */

#ifdef __cplusplus
}
#endif

/** @} */
