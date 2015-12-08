/*
 * Copyright (C) 2015 Sebastian Sontberg <sebastian@sontberg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_xmc1000
 * @{
 *
 * @file
 * @brief       CPUID driver implementation
 *
 * @author      Sebastian Sontberg <sebastian@sontberg.de>
 *
 * @}
 */

#include <string.h>

#include "cpu.h"
#include "periph/cpuid.h"

#define UCID_ADDR (0x10000ff0)

void cpuid_get(void *id)
{
    memcpy(id, (void *)UCID_ADDR, CPUID_ID_LEN);
}
