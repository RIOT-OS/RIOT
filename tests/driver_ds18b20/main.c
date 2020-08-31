/*
 * Copyright (C) 2016 Freie Universität Berlin
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
 * @brief       xtimer_msg test application
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

#include "ds18b20.h"

#define DEVICE_LIMIT        (10U)
#define INIT_FAIL           (0xff)

static onewire_t owi = { GPIO_UNDEF, GPIO_OD_PU, GPIO_IN_PU };
static ds18b20_params_t _dslist[DEVICE_LIMIT];
static ds18b20_t _devlist[DEVICE_LIMIT];
static unsigned _cnt = 0;


static int _cmd_init(int argc, char **argv)
{
    int portsel, pinsel, modesel, res;
    onewire_params_t params;
    uint8_t dsres = DS18B20_RES_12BIT;

    if (argc < 3) {
        printf("usage: %s <port num> <pin num> [mode] [res]\n", argv[0]);
        puts("\tmode: 0: open drain with pull-up (default)");
        puts("\t      1: open-drain without pull resistor");
        puts("\t      2: push pull output (external transistor needed)");
        puts("\t res: 9, 10, 11, or 12-bit resolution, default is 12-bit");
        return 1;
    }

    portsel = atoi(argv[1]);
    pinsel = atoi(argv[2]);
    params.pin = GPIO_PIN(portsel, pinsel);
    params.pin_mode = GPIO_OD_PU;

    if (argc >= 4) {
        modesel = atoi(argv[3]);
        switch (modesel) {
            case 0: break;
            case 1: params.pin_mode = GPIO_OD;      break;
            case 2: params.pin_mode = GPIO_OUT;     break;
            default:
                puts("err: unable to parse 1-Wire pin mode");
                return 1;
        }
    }

    if (argc >= 5) {
        res = atoi(argv[4]);
        if (res < 9 || res > 12) {
            puts("err: resolution must be 9, 10, 11, or 12");
            return 1;
        }
        dsres = res - 9;
    }

    /* initialize the one wire bus */
    printf("initializing 1-Wire bus on GPIO_PIN(%i, %i)...\n", portsel, pinsel);
    if (onewire_init(&owi, &params) != ONEWIRE_OK) {
        puts("err: open-drain pin mode not applicable\n");
        owi.pin = GPIO_UNDEF;
        return 1;
    }
    puts("-> 1-Wire Bus initialized\n");

    /* reset device list */
    memset(_dslist, 0xff, sizeof(_dslist));

    /* search for devices */
    puts("discovering DS18B20 devices...");
    _cnt = 0;
    onewire_rom_t tmp;
    res = ONEWIRE_SEARCH_FIRST;
    do {
        res = onewire_search(&owi, &tmp, res);
        if ((res >= 0) && (ds18b20_id(&tmp) == DS18B20_OK)) {
            memcpy(&_dslist[_cnt].rom, &tmp, sizeof(onewire_rom_t));
            _dslist[_cnt].res = dsres;
            ++_cnt;
        }
    } while ((res > 0) && (_cnt < DEVICE_LIMIT));

    printf("discovered %u devices:\n", _cnt);
    for (unsigned i = 0; i < _cnt; i++) {
        char rom_str[ONEWIRE_ROM_STR_LEN];
        onewire_rom_to_str(rom_str, &_dslist[i].rom);
        printf("[%2u] ROM code: %s\n", i, rom_str);
    }

    /* initialize all discovered devices */
    puts("\ninitializing DS18B20 sensors...");
    for (unsigned i = 0; i < _cnt; i++) {
        char rom_str[ONEWIRE_ROM_STR_LEN];
        onewire_rom_to_str(rom_str, &_dslist[i].rom);

        res = ds18b20_init(&_devlist[i], &owi, &_dslist[i]);
        if (res == DS18B20_OK) {
            printf("[%2u] %s: OK\n", i, rom_str);
        }
        else {
            _dslist[i].res = INIT_FAIL;  /* mark as invalid */
            printf("[%2u] %s: err - initialization failed\n", i, rom_str);
        }
    }

    return 0;
}

static void _print_res(unsigned num, int state, uint16_t temp)
{
    char rom_str[ONEWIRE_ROM_STR_LEN];
    onewire_rom_to_str(rom_str, &_dslist[num].rom);

    if (state == DS18B20_OK) {
        char temp_str[20];
        size_t pos = fmt_s16_dfp(temp_str, temp, -2);
        temp_str[pos] = '\0';
        printf("[%2u] %s: %s°C\n", num, rom_str, temp_str);
    }
    else {
        printf("[%2u] %s: err - unable to acquire temperature\n", num, rom_str);
    }
}

static int _cmd_read(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (_cnt == 0) {
        puts("err: no device detected");
        return 1;
    }

    /* trigger a new sample on all devices connected to the bus */
    puts("triggering a new sample for all devices on the bus...");
    ds18b20_trigger_all(&_devlist[0]);

    /* wait for the conversion to finish */
    puts("wait for conversion time to pass...");
    xtimer_usleep(ds18b20_get_conv_time_us(&_devlist[0]));

    /* read temperature data from each device */
    puts("reading results...");
    for (unsigned i = 0; i < _cnt; i++) {
        if (_dslist[i].res != INIT_FAIL) {
            int16_t temp;
            int res = ds18b20_read(&_devlist[i], &temp);
            _print_res(i, res, temp);
        }
    }

    return 0;
}

static int _cmd_read_seq(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (_cnt == 0) {
        puts("err: no device detected");
        return 1;
    }

    for (unsigned i = 0; i < _cnt; i++) {
        if (_dslist[i].res != INIT_FAIL) {
            int16_t temp;
            int res = ds18b20_get_temp(&_devlist[i], &temp);
            _print_res(i, res, temp);
        }
    }

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "init", "initialize 1-Wire driver with given pin", _cmd_init },
    { "read", "read temperature data from all sensors", _cmd_read },
    { "read_seq", "read all sensors sequential", _cmd_read_seq },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("DS18B20 driver test application\n");

    /* run the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
