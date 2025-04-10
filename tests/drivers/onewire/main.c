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

#include "onewire.h"
#include "soft_onewire.h"
#include "tiny_strerror.h"

static soft_onewire_params_t soft_onewire_params[] = {
    {
#if     defined(BOARD_NATIVE)
        .pin = GPIO_UNDEF,
#elif   defined(BOARD_STM32F429I_DISC1)
        .pin = GPIO_PIN(PORT_C, 0),
#endif
        .pin_imode = GPIO_IN,
#ifdef SOFT_ONEWIRE_HWTIMER
        .timer = TIM_DEV(0),
#endif
    },
};

soft_onewire_t soft_onewire_devs[ARRAY_SIZE(soft_onewire_params)];

static onewire_params_t onewire_params[] = {
    {
        .driver = &soft_onewire_driver,
        .lldev = &soft_onewire_devs[0],
        .lldev_params = &soft_onewire_params[0],
    },
};

#define ONEWIRE_NUMOF ARRAY_SIZE(onewire_params)
onewire_t onewire_buses[ARRAY_SIZE(onewire_params)];

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
    for (unsigned i = 0; i < ONEWIRE_NUMOF; i++) {
        onewire_init(&onewire_buses[i], &onewire_params[i]);
        _enumerate_bus(&onewire_buses[i]);
    }

    return 0;
}
