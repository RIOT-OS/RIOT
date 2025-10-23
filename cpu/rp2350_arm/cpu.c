/*
 * SPDX-FileCopyrightText: 2025 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup         cpu_rp2350_arm
 * @{
 *
 * @file
 * @brief           CPU initialization implementation for the RP2350
 *
 * @author          Tom Hert <git@annsann.eu>
 */

#include "cpu.h"
#include "periph_cpu.h"

void cpu_init(void)
{
    cortexm_init();
    rp2350_init();
}

/** @} */
