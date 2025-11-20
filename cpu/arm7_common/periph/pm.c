/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_arm7_common
 * @ingroup     drivers_periph_pm
 * @{
 *
 * @file
 * @brief       ARM7 common periph/pm functions
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "arm_cpu.h"

void pm_reboot(void)
{
    while (1) {
        arm_reset();
    }
}
