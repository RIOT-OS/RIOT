/*
 * SPDX-FileCopyrightText: 2025 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_rp2350_arm
 * @{
 * @file
 * @brief           CPU configuration for the RP2350
 *
 * @author          Tom Hert <git@annsann.eu>
 */

#include "RP2350.h"
#include "core_cm33.h"
#include "cpu_conf_common.h"

#define CPU_DEFAULT_IRQ_PRIO 1u
#define CPU_IRQ_NUMOF 52u
/** This tells rp2350_common that we are using the ARM core */
#define RP2350_USE_ARM 1

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

/** @} */
