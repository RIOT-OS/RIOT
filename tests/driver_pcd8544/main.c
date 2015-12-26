/*
 * Copyright (C) 2015 Freie Universität Berlin
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
 * @brief       Test application for the PCD8544 display driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#ifndef TEST_PCD8544_SPI
#error "TEST_PCD8544_SPI not defined"
#endif
#ifndef TEST_PCD8544_CS
#error "TEST_PCD8544_CS not defined"
#endif
#ifndef TEST_PCD8544_RESET
#error "TEST_PCD8544_RESET not defined"
#endif
#ifndef TEST_PCD8544_MODE
#error "TEST_PCD8544_MODE not defined"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "shell.h"
#include "pcd8544.h"

static pcd8544_t dev;

static int _contrast(int argc, char **argv)
{
    uint8_t val;

    if (argc < 2) {
        printf("usage: %s VAL [0-127]\n", argv[0]);
        return 1;
    }
    val = (uint8_t)atoi(argv[1]);
    pcd8544_set_contrast(&dev, val);
    return 0;
}

static int _temp(int argc, char **argv)
{
    uint8_t val;

    if (argc < 2) {
        printf("usage: %s VAL [0-3]\n", argv[0]);
        return 1;
    }
    val = (uint8_t)atoi(argv[1]);
    pcd8544_set_tempcoef(&dev, val);
    return 0;
}

static int _bias(int argc, char **argv)
{
    uint8_t val;

    if (argc < 2) {
        printf("usage: %s VAL [0-7]\n", argv[0]);
        return 1;
    }
    val = (uint8_t)atoi(argv[1]);
    pcd8544_set_bias(&dev, val);
    return 0;
}

static int _on(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    pcd8544_poweron(&dev);
    return 0;
}

static int _off(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    pcd8544_poweroff(&dev);
    return 0;
}

static int _clear(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    pcd8544_clear(&dev);
    return 0;
}

static int _invert(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    pcd8544_invert(&dev);
    return 0;
}

static int _riot(int argc, char **argv) {
    (void)argc;
    (void)argv;

    pcd8544_riot(&dev);
    return 0;
}

static int _write(int argc, char **argv)
{
    uint8_t x, y;

    if (argc < 4) {
        printf("usage: %s X Y STRING\n", argv[0]);
        return -1;
    }

    x = (uint8_t)atoi(argv[1]);
    y = (uint8_t)atoi(argv[2]);

    pcd8544_write_s(&dev, x, y, argv[3]);
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "contrast", "set contrast", _contrast },
    { "temp", "set temperature coefficient", _temp },
    { "bias", "set BIAS value", _bias },
    { "on", "turn display on", _on },
    { "off", "turn display off", _off },
    { "clear", "clear memory", _clear },
    { "invert", "invert display", _invert },
    { "riot", "display RIOT logo", _riot },
    { "write", "write string to display", _write},
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("PCD8544 LCD display test application\n");
    printf("Initializing PCD8544 LCD at SPI_%i... ", TEST_PCD8544_SPI);
    if (pcd8544_init(&dev, TEST_PCD8544_SPI, TEST_PCD8544_CS,
                     TEST_PCD8544_RESET, TEST_PCD8544_MODE) != 0) {
        puts("Failed to initialize PCD8544 display");
        return 1;
    }

    /* run shell */
    puts("All OK, running shell now");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
