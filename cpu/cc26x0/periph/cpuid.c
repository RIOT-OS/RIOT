/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc26x0
 * @{
 *
 * @file
 * @brief       cpuid implementation for the CC26x0
 *
 * @author      Leon M. George <leon@georgemail.eu>
 *
 * @}
 */

#include <string.h>

#include "periph/cpuid.h"

void cpuid_get(void *id)
{
    memcpy(id, (void *) &FCFG->MAC_BLE_0, CPUID_LEN);
}
