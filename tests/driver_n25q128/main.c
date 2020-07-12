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

#include "xtimer.h"
#include "board.h"
#include "periph/pm.h"
#include "shell.h"
#include "n25q128.h"

/*
 * @brief   The size of the generic buffer.
 */
#define BUF_SIZE     (20U)

/*
 * @brief   The device, used to get access to the n25q128 serial flash memory.
 */
static n25q128_dev_t n25q128;

/*
 * @brief   Generic buffer used for sending and receiving.
 */
static uint8_t buf[BUF_SIZE];

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

    // XXX: Specific configuration for iotlab-m3 nodes.
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

    memset(buf, 0, BUF_SIZE);

    return 0;
}

static int cmd_read_id(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    puts("\nThe first 4 Bytes should look like this:\"0x20 0xba 0x18 0x10\".\n");
    puts("- Manufacturer identification");
    puts("  - 1 Byte; 0x20");
    puts("- Device Information");
    puts("  - Memory Type; 1 Byte; 0xBA");
    puts("  - Memory capacity; 1 Byte; 0x18");
    puts("- UID");
    puts("  - EDID+CFD length; 1 Byte; 0x10");
    puts("  - EDID; 2 Bytes; 'no specific values'");
    puts("  - CFD; 14 Bytes; 'no specific values'\n");

    memset(buf, 0, sizeof(buf));

    n25q128_read_id(&n25q128, buf, BUF_SIZE);

    _print_bytes("Read ID", buf, 20);

    return 0;
}

static int cmd_read_bytes(int argc, char **argv)
{
    int address = 0;

    if (argc < 2) {
        printf("usage: %s <address>\n", argv[0]);
        return 1;
    }

    address = atoi(argv[1]);

    puts("Read bytes from memory.");

    memset(buf, 0, sizeof(buf));

    n25q128_read_data_bytes(&n25q128, address, buf, BUF_SIZE);

    _print_bytes("Read data bytes", buf, BUF_SIZE);

    return 0;
}

static int cmd_page_program(int argc, char **argv)
{
    uint32_t addr = 0;
    size_t len = 0;
    uint8_t values[BUF_SIZE];

    if (argc < 2) {
        printf("usage: %s <address> <value>\n", argv[0]);
        return 1;
    }

    puts("Page program the memory.");

    memset(buf, 0, sizeof(buf));

    addr = atoi(argv[1]);
    len = strlen(argv[2]);
    memcpy(values, argv[2], len);

    n25q128_page_program(&n25q128, addr, values, len);

    return 0;
}

static int cmd_sector_erase(int argc, char **argv)
{
    int addr = 0;

    if (argc < 1) {
        printf("usage: %s <address of the sector>\n", argv[0]);
        return 1;
    }

    addr = atoi(argv[1]);

    puts("Sector erase the sector of the given address.");

    n25q128_sector_erase(&n25q128, addr);

    return 0;
}

static int cmd_bulk_erase(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    puts("Bulk erase the memory. This tooks ~250 seconds.");
    n25q128_bulk_erase(&n25q128);
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

static int cmd_wip_check(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    puts("cmd_wip_check: This may take a few minutes to finish.");

    /* Erase the whole memory. This takes a couple of minutes. */
    n25q128_bulk_erase(&n25q128);

    puts("Waiting for the Write-In-Process (WIP) flag to be cleared: ");

    /* Check every few seconds if the wip bit is set (erase in process)*/
    while(n25q128_write_in_progress(&n25q128) == 1) {
        puts(".");
        xtimer_sleep(1); // 1 second.
    }

    puts("done!");

    return 0;
}

/**
 * @brief   List of shell commands for this test.
 */
static const shell_command_t shell_commands[] = {
    { "reboot", "reboot the device", cmd_reboot },
    { "init", "initialize the device", cmd_init },
    { "id", "Read the ID of the device", cmd_read_id },
    { "read", "Read an amount of bytes from a specific address", cmd_read_bytes },
    { "pp", "Program byte for byte at a specific address", cmd_page_program },
    { "se", "Erase a sector of a given address", cmd_sector_erase},
    { "be", "Erase the whole memory. Tooks ~250 seconds", cmd_bulk_erase },
    { "wip", "Check via bulk erase if WIP bit is set", cmd_wip_check },
    { NULL, NULL, NULL }
};

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    puts("\nApplication: /tests/driver_n25q128");

    cmd_init(0, NULL);

    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
