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
#include "tiny_strerror.h"

static soft_onewire_params_t soft_onewire_params[] = {
    {
#ifdef MODULE_ONEWIRE_MULTIDRIVER
        .super = { .driver = &soft_onewire_driver },
#endif
#if     defined(BOARD_NATIVE)
        .pin = GPIO_UNDEF,
#elif   defined(BOARD_STM32F429I_DISC1)
        .pin = GPIO_PIN(PORT_C, 0),
#endif
        .pin_imode = GPIO_IN,
#ifdef MODULE_SOFT_ONEWIRE_HWTIMER
        .timer = TIMER_DEV(1),
#endif
    },
};

#define SOFT_ONEWIRE_NUMOF ARRAY_SIZE(soft_onewire_params)

soft_onewire_t soft_onewire_devs[SOFT_ONEWIRE_NUMOF];

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
    for (unsigned i = 0; i < SOFT_ONEWIRE_NUMOF; i++) {
        soft_onewire_init(&soft_onewire_devs[i], &soft_onewire_params[i]);
        _enumerate_bus(&soft_onewire_devs[i].super);
    }

    return 0;
}
