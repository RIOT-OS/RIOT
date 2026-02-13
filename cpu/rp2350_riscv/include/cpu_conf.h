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

/** Default IRQ priority for the CPU,
 * we don't have different levels at the moment */
#define CPU_DEFAULT_IRQ_PRIO 1u
/** The RP2350 has 52 IRQs, see vectors.c for more details */
#define CPU_IRQ_NUMOF 52u
/** This tells rp2350_common that we are using the RISC-V core */
#define RP2350_USE_RISCV 1

#ifdef __cplusplus
}
#endif

/** @} */
