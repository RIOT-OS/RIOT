/*
 * SPDX-FileCopyrightText: 2015-2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_efm32
 * @ingroup     drivers_periph_cpuid
 * @{
 *
 * @file
 * @brief       Low-level CPUID driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "periph/cpuid.h"

#include "em_system.h"

void cpuid_get(void *id)
{
    *(uint64_t *) id = SYSTEM_GetUnique();
}
