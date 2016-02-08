/*
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_kinetis_common_cpuid
 *
 * @{
 *
 * @file
 * @brief       Low-level CPUID driver implementation
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 * @}
 */

#include <string.h>

#include "cpu.h"
#include "periph/cpuid.h"

void cpuid_get(void *id)
{
    memcpy(id, (void *)&(SIM_UIDH), CPUID_LEN);
}
