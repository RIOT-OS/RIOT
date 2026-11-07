/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test the parsing of RIOT_VERSION into RIOT_VERSION_CODE
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>

#include "riot_version.h"

/* RIOT_VERSION_CODE must be an positive integer constant expression, such that
 * it can be used in preprocessor conditionals. Anything else fails to compile
 * here, except for unknown identifiers, which the preprocessor evaluates to
 * zero. */
#if !defined(RIOT_VERSION_CODE) || (RIOT_VERSION_CODE) <= 0
#  error "RIOT_VERSION_CODE must be a positive integer constant expression"
#endif

int main(void)
{
    const uint64_t code = RIOT_VERSION_CODE;

    printf("RIOT_VERSION: %s\n", RIOT_VERSION);
    printf("RIOT_VERSION_CODE: %u.%u.%u.%u\n",
           (unsigned)((code >> 48) & 0xFFFF),
           (unsigned)((code >> 32) & 0xFFFF),
           (unsigned)((code >> 16) & 0xFFFF),
           (unsigned)(code & 0xFFFF));

    puts("[SUCCESS]");

    return 0;
}
