/*
 * Copyright (C) 2019 Ludwig Maximilian Universität
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc3200
 * @{
 *
 * @file
 * @brief       CPUID driver implementation
 *
 * @author      Wladislaw Meixner <wladislaw.meixner@campus.lmu.de>
 *
 * @}
 */

#include <string.h>

#include "cpu.h"
#include "periph/cpuid.h"

void cpuid_get(void *id)
{
    memcpy(id, CPUID_ADDR, CPUID_LEN);
}
