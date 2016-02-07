/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  cpu_saml21
 * @{
 *
 * @file
 * @brief       Low-level CPUID driver implementation
 *
 * @author      Troels Hoffmeyer <troels.d.hoffmeyer@gmail.com>
 *
 * @}
 */

#include <stdint.h>
#include <string.h>

#include "periph/cpuid.h"

#define SAML21_CPUID_WORD0 (*(volatile uint32_t *)0x0080A00C)
#define SAML21_CPUID_WORD1 (*(volatile uint32_t *)0x0080A040)
#define SAML21_CPUID_WORD2 (*(volatile uint32_t *)0x0080A044)
#define SAML21_CPUID_WORD3 (*(volatile uint32_t *)0x0080A048)


void cpuid_get(void *id)
{
    uint32_t source_address[] = { SAML21_CPUID_WORD0,
                                  SAML21_CPUID_WORD1,
                                  SAML21_CPUID_WORD2,
                                  SAML21_CPUID_WORD3};
    memcpy(id, (void*) source_address, CPUID_LEN);
}
