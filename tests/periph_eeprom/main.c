/*
 * Copyright (C) 2018 Inria
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
 * @brief       Manual test application for the EEPROM peripheral drivers
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "shell.h"

#include "periph/eeprom.h"
#include "xtimer.h"

#ifndef BUFFER_SIZE
#define BUFFER_SIZE     (42U)
#endif

#ifndef TEST_BUFFER_SIZE
#define TEST_BUFFER_SIZE    (256U)
#endif

static char buffer[BUFFER_SIZE + 1];

static int cmd_info(int argc, char **argv)
{
    (void)argc;
    (void)argv;

#ifdef EEPROM_START_ADDR
    printf("EEPROM start addr:\t0x%08x\n", (int)EEPROM_START_ADDR);
#endif
    printf("EEPROM size:\t\t%i\n", (int)EEPROM_SIZE);

    return 0;
}

static int cmd_read(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <pos> <count>\n", argv[0]);
        return 1;
    }

    uint32_t pos = atoi(argv[1]);
    uint8_t count = atoi(argv[2]);

    if (!count) {
        puts("Count should be greater than 0");
        return 1;
    }

    if (count > BUFFER_SIZE) {
        puts("Count exceeds buffer size");
        return 1;
    }

    if (pos + count >= EEPROM_SIZE) {
        puts("Failed: cannot read out of eeprom bounds");
        return 1;
    }

    size_t ret = eeprom_read(pos, (uint8_t *)buffer, count);
    buffer[count] = '\0';

    printf("Data read from EEPROM (%d bytes): %s\n", (int)ret, buffer);

    return 0;
}

static int cmd_write(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: %s <pos> <data>\n", argv[0]);
        return 1;
    }

    uint32_t pos = atoi(argv[1]);

    if (pos + strlen(argv[2]) >= EEPROM_SIZE) {
        puts("Failed: cannot write out of eeprom bounds");
        return 1;
    }

    size_t ret = eeprom_write(pos, (uint8_t *)argv[2], strlen(argv[2]));
    printf("%d bytes written to EEPROM\n", (int)ret);

    return 0;
}

static int cmd_test(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    uint8_t bytes[TEST_BUFFER_SIZE] = { 0 };
    eeprom_write(0, bytes, TEST_BUFFER_SIZE);
    uint32_t start, stop;

    for (uint32_t i = 0; i < TEST_BUFFER_SIZE; i ++) {
        bytes[i] = i;
    }

    printf("Writing %u bytes to clean EEPROM: ", TEST_BUFFER_SIZE);
    start = xtimer_now_usec();
    eeprom_write(0, bytes, TEST_BUFFER_SIZE);
    stop = xtimer_now_usec();
    printf("%" PRIu32 " usec\n", stop - start);

    for (uint32_t i = 0; i < TEST_BUFFER_SIZE; i ++) {
        bytes[i] = TEST_BUFFER_SIZE - i;
    }

    printf("Writing %u bytes to dirty EEPROM: ", TEST_BUFFER_SIZE);
    start = xtimer_now_usec();
    eeprom_write(0, bytes, TEST_BUFFER_SIZE);
    stop = xtimer_now_usec();
    printf("%" PRIu32 " usec\n", stop - start);

    printf("Reading %u bytes from EEPROM: ", TEST_BUFFER_SIZE);
    start = xtimer_now_usec();
    eeprom_read(0, bytes, TEST_BUFFER_SIZE);
    stop = xtimer_now_usec();
    printf("%" PRIu32 " usec\n", stop - start);

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "info", "Print information about eeprom", cmd_info },
    { "read", "Read bytes from eeprom", cmd_read },
    { "write", "Write bytes to eeprom", cmd_write},
    { "test", "Test write and read speed", cmd_test},
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("EEPROM read write test\n");
    puts("Please refer to the README.md for more details\n");

    cmd_info(0, NULL);

    /* run the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
