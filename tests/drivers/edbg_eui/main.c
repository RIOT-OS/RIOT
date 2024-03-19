/*
 * Copyright (C) 2020 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the EDBG EUI driver
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <stdio.h>
#include "edbg_eui.h"

static int test_get_eui64(void)
{
    eui64_t e64;
    if (edbg_get_eui64(&e64) != 0) {
        puts("[FAILED]");
        return 1;
    }

    printf("EUI-64:");
    for (unsigned i = 0; i < sizeof(e64.uint8); ++i) {
        printf(" %02x", e64.uint8[i]);
    }
    puts("");

    return 0;
}

int main(void)
{
    if (test_get_eui64()) {
        return -1;
    }

    puts("[SUCCESS]");

    return 0;
}
