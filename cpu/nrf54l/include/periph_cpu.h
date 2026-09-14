/*
 * SPDX-FileCopyrightText: 2026 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_nrf54l
 * @{
 *
 * @file
 * @brief           nRF54L specific definitions for handling peripherals
 *
 * @author          Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "periph_cpu_common.h"
#include "macros/units.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    Peripheral clock speed of the slow peripheral domains (TIMER2x,
 *           UARTE2x, ...)
 */
#define PERIPH_CLOCK        MHZ(16)

#ifdef __cplusplus
}
#endif

/** @} */
