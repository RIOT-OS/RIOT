/*
 * Copyright (C) 2019 HAW Hamburg
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
 * @brief       Test for the N25Q128 serial flash memory
 *
 * @author      René Herthel <rene-herthel@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "n25q128.h"
#include "board.h"
#include "periph/pm.h"
#include "shell.h"

/* The device, this test is working on. Multiple devices possible. */
static n25q128_dev_t n25q128;

static inline void _print_bytes(char * title, uint8_t * data, size_t len)
{
    printf("\n%4s; len = %d\n\n", title, len);
    for (size_t i = 0; i < len; i++) {
        printf("  %2i ", (int)i);
    }
    printf("\n");
    printf("\n ");
    for (size_t i = 0; i < len; i++) {
        printf(" 0x%02x", (int)data[i]);
    }
    printf("\n ");
    for (size_t i = 0; i < len; i++) {
        if (data[i] < ' ' || data[i] > '~') {
            printf("  ?? ");
        }
        else {
            printf("   %c ", (char)data[i]);
        }
    }
    printf("\n\n");
}

static int cmd_init(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    int status = 0;

    puts("cmd_init: Initialize the N25Q128 flash memory.\n");

    /* XXX: This is only for iot-lab-m3 boards at the moment! See board.h */
    n25q128.conf.bus = EXTFLASH_SPI;
    n25q128.conf.mode = SPI_MODE_0;
    n25q128.conf.clk = SPI_CLK_100KHZ;
    n25q128.conf.cs = EXTFLASH_CS;
    n25q128.conf.write = EXTFLASH_WRITE;
    n25q128.conf.hold = EXTFLASH_HOLD;

    status = n25q128_init(&n25q128);

    if (status == 0) {
        printf("cmd_init -> Success!\n");
    }
    else if (status < 0) {
        printf("cmd_init -> Failed!\n");
    }

    return 0;
}

static int cmd_read_id(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    /* The id is 20 bytes in size. See Datasheet -> Page 81. */
    uint8_t rdid[20] = {0};

    n25q128_read_id(&n25q128, rdid, 20);

    /* First 4 bytes should be: 0x20, 0xBA, 0x18, 0x10.
     * Datasheet: N25Q_128_3_Volt_with_boot_sector.pdf -> Page 81. */
    _print_bytes("Read ID", rdid, 20);

    return 0;
}

static int cmd_read_bytes(int argc, char **argv)
{
    int len = 0;
    int address = 0;
    uint8_t buf[16] = {0};

    if (argc < 2) {
        printf("usage: %s <number-of-bytes> <address>\n", argv[0]);
        return 1;
    }

    /* Parse the number of bytes. */
    len = atoi(argv[1]);

    // XXX: What is the maximum here?
    if (len < 0 || len > 16) {
        puts("error: number of bytes is out of range from buffer\n");
    }

    /* Parse the address. */
    // XXX: Check for correct address range.
    // NOTE: When the highest address is reached, the address counter rolls over
    //      to 000000h, allowing the read sequence to be continued indefinitely.
    address = atoi(argv[2]);

    n25q128_read_data_bytes(&n25q128, address, buf, len);

    _print_bytes("Read data bytes", buf, len);

    return 0;
}

static int cmd_page_program(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    return 0;
}

static int cmd_sector_erase(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    return 0;
}

static int cmd_bulk_erase(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    return 0;
}

static int cmd_suspend(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    return 0;
}

static int cmd_resume(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    return 0;
}

static int cmd_reboot(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    puts("CPU will reboot.");
    pm_reboot();
    return 0;
}

/**
 * @brief   List of shell commands for this test.
 */
static const shell_command_t shell_commands[] = {
    { "reboot", "reboots your device", cmd_reboot },
    { "init", "initialize the device", cmd_init },
    { "id", "Read the id of the device", cmd_read_id },
    { "read", "Read an amount of bytes", cmd_read_bytes },
    { "pp", "Page program the memory", cmd_page_program },
    { "se", "Sector erase", cmd_sector_erase},
    { "be", "bulk erase (the whole memory)", cmd_bulk_erase },
    { "suspend", "suspend the program/erase program", cmd_suspend },
    { "resume", "resume the program/erase program", cmd_resume },
    { NULL, NULL, NULL }
};

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    puts("\nApplication: /tests/driver_n25q128");
    puts("This driver only supports the extended SPI protocol at the moment.");
    puts("Make sure to execute the 'init' command to initialize the memory.\n");

    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
