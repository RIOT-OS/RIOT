/*
 * Copyright 2016, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_mips32r2_common
 * @ingroup     drivers_periph_pm
 * @{
 *
 * @file
 * @brief        common periph/pm functions
 *
 * @author       Neil Jones <neil.jones@imgtec.com>
 *
 * @}
 */

#include <mips/m32c0.h>
#include "periph/pm.h"

void pm_set_lowest(void)
{
    /* Don't wait if interrupts are not enabled - we would never return!*/
    if (mips32_get_c0(C0_STATUS) & SR_IE) {
        __asm volatile ("wait");
    }
}
