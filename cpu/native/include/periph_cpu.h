/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         native_cpu
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 */

#ifndef PERIPH_CPU_H_
#define PERIPH_CPU_H_

#include "periph/dev_enums.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Length of the CPU_ID in octets
 */
#ifndef CPUID_LEN
#define CPUID_LEN           (4U)
#endif

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H_ */
/** @} */
