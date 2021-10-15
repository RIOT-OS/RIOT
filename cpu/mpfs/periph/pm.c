/*
 * Copyright 2021 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_mpfs
 * @{
 *
 * @brief       Implementation of the CPU power management for Polarfire CPU
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 * @}
 */

#include <stdint.h>
#include "periph/pm.h"

void pm_reboot(void)
{
    while (1) {}
}
