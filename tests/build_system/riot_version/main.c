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

/* the version code must be usable in preprocessor conditionals */
#if RIOT_VERSION_CODE <= RIOT_VERSION_NUM(0, 0, 0, 0)
#  error "RIOT_VERSION_CODE is invalid"
#endif

int main(void)
{
    const uint64_t code = RIOT_VERSION_CODE;

    /* printf() with uint64_t is not supported by all boards, so print in
     * parts */
    printf("RIOT_VERSION: %s\n", RIOT_VERSION);
    printf("RIOT_VERSION_CODE: %u.%u.%u.%u\n",
           (unsigned)((code >> 48) & 0xFFFF),
           (unsigned)((code >> 32) & 0xFFFF),
           (unsigned)((code >> 16) & 0xFFFF),
           (unsigned)(code & 0xFFFF));

    puts("[SUCCESS]");

    return 0;
}
