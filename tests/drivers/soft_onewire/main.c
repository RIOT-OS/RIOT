/*
 * Copyright (C) 2025 Joshua DeWeese
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
 * @brief       Test application for the 1-Wire (onewire) bus driver
 *
 * @author      Joshua DeWeese <josh.deweese@gmail.com>
 * @}
 */

#include <stdio.h>

#include "onewire.h"
#include "soft_onewire.h"
#include "soft_onewire_params.h"
#include "tiny_strerror.h"

static void _enumerate_bus(onewire_t *bus)
{
    onewire_rom_t id;
    int res = ONEWIRE_SEARCH_FIRST;

    onewire_aquire(bus);

    do {
        res = onewire_search(bus, &id, res);
        if (res < 0) {
            printf("failure to enumerate device: %s\n", tiny_strerror(-res));
            break;
        }

        printf("found device: ");
        onewire_rom_print(&id);
        puts("");

    } while (res > 0);

    onewire_release(bus);
}

int main(void)
{
    soft_onewire_t buses[SOFT_ONEWIRE_NUMOF];

    for (unsigned i = 0; i < SOFT_ONEWIRE_NUMOF; i++) {
        soft_onewire_init(&buses[i], &soft_onewire_params[i]);
        printf("searching for 1-wire devices on bus %i...\n", i);
        _enumerate_bus(&buses[i].super);
    }

    return 0;
}
