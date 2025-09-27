/*
 * SPDX-FileCopyrightText: 2021 Mesotic SAS
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_nrf9160
 * @{
 *
 * @file
 * @brief           nRF9160 specific definitions for handling peripherals
 *
 * @author          Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

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

/** @} */
