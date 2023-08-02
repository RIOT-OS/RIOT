/*
 * Copyright (C) 2021 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_nrf9160
 * @{
 *
 * @file
 * @brief           nRF9160 specific definitions for handling peripherals
 *
 * @author          Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "periph_cpu_common.h"
#include "macros/units.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   System core clock speed, fixed to 64MHz for all NRF9160 CPUs
 */
#define CLOCK_CORECLOCK     MHZ(64)

/**
 * @brief    Peripheral clock speed (fixed to 16MHz for nRF9160 based CPUs)
 */
#define PERIPH_CLOCK        MHZ(16)

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
