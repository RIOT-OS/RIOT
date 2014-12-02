/*
 * Copyright (C) 2014 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_cc2538
 * @{
 *
 * @file        cpuid.c
 * @brief       CPU-ID driver implementation
 *
 * The CC2538 provides a 64-bit unique identifier, that is unique for each shipped unit.
 *
 * @author      Ian Martin <ian@locicontrols.com>
 *
 * @}
 */

#include <string.h>

#include "cpu-conf.h"
#include "periph/cpuid.h"

#define Word0 (*(volatile uint32_t *)0x0080A00C)
#define Word1 (*(volatile uint32_t *)0x0080A040)
#define Word2 (*(volatile uint32_t *)0x0080A044)
#define Word3 (*(volatile uint32_t *)0x0080A048)

void cpuid_get(void *id)
{    
    uint32_t source_address[] = {Word0, Word1, Word2, Word3};
    memcpy(id, (void*) source_address, CPUID_ID_LEN);

}
/** @} */