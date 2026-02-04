/*
 * SPDX-FileCopyrightText: 2019 Benjamin Valentin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the AT24MAC driver
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <stdio.h>
#include "at24mac.h"

static int test_get_eui48(void)
{
    if (at24mac_get_type(0) != AT24MAC4XX) {
        return 0;
    }

    eui48_t e48;
    if (at24mac_get_eui48(0, &e48) != 0) {
        puts("[FAILED]");
        return 1;
    }

    printf("EUI-48:");
    for (unsigned i = 0; i < sizeof(e48.uint8); ++i) {
        printf(" %02x", e48.uint8[i]);
    }
    puts("");

    return 0;
}

static int test_get_eui64(void)
{
    if (at24mac_get_type(0) != AT24MAC6XX) {
        return 0;
    }

    eui64_t e64;
    if (at24mac_get_eui64(0, &e64) != 0) {
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

static int test_get_id128(void)
{
    uint8_t id[AT24MAC_ID_LEN];
    if (at24mac_get_id128(0, &id) != 0) {
        puts("[FAILED]");
        return 1;
    }

    printf("ID-128:");
    for (unsigned i = 0; i < sizeof(id); ++i) {
        printf(" %02x", id[i]);
    }
    puts("");

    return 0;
}

int main(void)
{
    if (test_get_eui48()) {
        return -1;
    }

    if (test_get_eui64()) {
        return -1;
    }

    if (test_get_id128()) {
        return -1;
    }

    puts("[SUCCESS]");

    return 0;
}
