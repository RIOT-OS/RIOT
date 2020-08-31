/*
 * Copyright (C) 2016-2020 Freie Universität Berlin
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
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "fmt.h"
#include "shell.h"
#include "xtimer.h"

#include "onewire.h"

#define DEVICE_LIMIT        (10U)

static int _noinit = 1;
static onewire_t _owi = { GPIO_UNDEF, GPIO_OD_PU, GPIO_IN_PU };
static onewire_rom_t _devlist[DEVICE_LIMIT];

static int _cmd_init(int argc, char **argv)
{
    int portsel;
    int pinsel;
    onewire_params_t params;

    if (argc < 3) {
        printf("usage: %s <port num> <pin num> [mode]\n", argv[0]);
        puts("\tmode: 0: open drain with pull-up (default)");
        puts("\t      1: open-drain without pull resistor");
        puts("\t      2: push pull output (external pull-up needed)");
        return 1;
    }

    portsel = atoi(argv[1]);
    pinsel = atoi(argv[2]);
    params.pin = GPIO_PIN(portsel, pinsel);
    params.pin_mode = GPIO_OD_PU;
    if (argc > 3) {
        switch (atoi(argv[3])) {
            case 0:
                break;
            case 1:
                params.pin_mode = GPIO_OD;
                break;
            case 2:
                params.pin_mode = GPIO_OUT;
                break;
            default:
                puts("unable to parse mode");
                return 1;
        }
    }

    printf("Initializing 1-Wire bus at GPIO_PIN(%i, %i)\n", portsel, pinsel);

    /* initialize the one wire bus */
    if (onewire_init(&_owi, &params) == ONEWIRE_OK) {
        _noinit = 0;
        puts("1-Wire bus successfully initialized");
    }
    else {
        puts("Error: pin initialization failed: pin mode not supported\n");
        _noinit = 1;
    }

    return _noinit;
}

static int _cmd_discover(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (_noinit) {
        puts("Error: 1-Wire bus is not initialized");
        return 1;
    }

    /* reset device list and detect all connected devices */
    int ld = ONEWIRE_SEARCH_FIRST;
    unsigned cnt = 0;
    onewire_rom_t tmp;

    memset(_devlist, 0, ARRAY_SIZE(_devlist));
    do {
        ld = onewire_search(&_owi, &tmp, ld);
        if (ld >= 0) {
            memcpy(&_devlist[cnt++], &tmp, sizeof(onewire_rom_t));
        }
    } while ((ld > 0) && (cnt < DEVICE_LIMIT));

    printf("Discovery finished, found %u devices:\n", cnt);
    puts("Dev # - ROM code");
    for (unsigned i = 0; i < cnt; i++) {
        char rom_str[ONEWIRE_ROM_STR_LEN];
        onewire_rom_to_str(rom_str, &_devlist[i]);
        printf("[%2i]    %s\n", i, rom_str);
    }

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "init", "initialize OneWire interface on given pin", _cmd_init },
    { "discover", "list all connected devices", _cmd_discover },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("1-Wire bus driver test application\n");

    /* run the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
