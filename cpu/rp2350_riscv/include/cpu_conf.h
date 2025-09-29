/*
 * SPDX-FileCopyrightText: 2025 Tom Hert git@annsann.eu>
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_rp2350_riscv
 * @{
 * @file
 * @brief           CPU configuration for the RP2350
 *
 * @author          Tom Hert <git@annsann.eu>
 */

#include "cpu_conf_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CPU_DEFAULT_IRQ_PRIO 1u
#define CPU_IRQ_NUMOF 52u

#ifdef __cplusplus
}
#endif

/** @} */
