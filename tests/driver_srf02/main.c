/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences
 *               2015 Freie Universität Berlin
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
 * @brief       Test application for the SRF02 ultrasonic range sensor
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "shell.h"
#include "xtimer.h"
#include "srf02.h"

#ifndef TEST_SRF02_I2C
#error "TEST_SRF02_I2C not defined"
#endif
#ifndef TEST_MODE
#error "TEST_MODE not defined"
#endif

#define SAMPLE_PERIOD       (100 * 1000U)

static srf02_t dev;

static void sample(void)
{
    uint16_t distance = srf02_get_distance(&dev, TEST_MODE);
    printf("distance = %3i cm\n", distance);
}

static int cmd_init(int argc, char **argv)
{
    int res;

    if (argc < 2) {
        printf("usage: %s <addr (decimal)>\n", argv[0]);
        return 1;
    }

    uint8_t addr = (uint8_t)atoi(argv[1]);

    printf("Initializing SRF02 sensor at I2C_DEV(%i), address is %i\n... ",
           TEST_SRF02_I2C, (int)addr);
    res = srf02_init(&dev, TEST_SRF02_I2C, addr);
    if (res < 0) {
        puts("[Failed]");
        return 1;
    }
    else {
        puts("[Ok]\n");
    }
    return 0;
}

static int cmd_sample(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    uint32_t wakeup = xtimer_now();

    while(1) {
        sample();
        xtimer_usleep_until(&wakeup, SAMPLE_PERIOD);
    }

    return 0;
}

static int cmd_shoot(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    sample();
    return 0;
}

static int cmd_set_addr(int argc, char **argv)
{
    uint8_t new_addr;

    if (argc < 2) {
        printf("usage: %s <new_addr (decimal)>\n", argv[0]);
        return 1;
    }

    new_addr = (uint8_t)atoi(argv[1]);
    srf02_set_addr(&dev, new_addr);
    printf("Set address to %i\n", (int)new_addr);
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "init", "initialize a device", cmd_init },
    { "sample", "start sampling", cmd_sample },
    { "shoot", "get a single sample", cmd_shoot },
    { "addr", "reprogram the devices address", cmd_set_addr },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("\nSRF02 Ultrasonic Range Sensor Test\n");
    puts("Use the following flow to test your device/setup. First you need to\n"
         "initialize your device (e.g. 'init 224'). Next you can sample your \n"
         "device continuously ('sample'), or get one value ('shoot').\n\n"
         "This application let's you also reprogram your device's I2C"
         "address:\n"
         " 1. initialize your device -> e.g. 'init 224'\n"
         " 2. specify the new address -> e.g.'addr 228'\n"
         "The device will be programmed with the new address and it is\n"
         "re-initialized with the new address, so you can use it right away\n");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
}
