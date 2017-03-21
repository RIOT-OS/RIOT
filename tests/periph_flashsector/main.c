/*
 * Copyright (C) 2017 HAW Hamburg
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
 * @brief       Manual test application for sectored Flash peripheral driver
 *
 * @author      Jannik Beyerstedt <jannik.beyerstedt@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "shell.h"
#include "periph/flashsector.h"

#define LINE_LEN            (16)
#define BUFFER_SIZE         (2048U)

/**
 * @brief   Allocate space for 2KB of flash data in RAM
 */
static uint8_t sector_mem[BUFFER_SIZE];


static int getsector(const char *str)
{
    int sector = atoi(str);
    if ((sector >= FLASHSECTOR_NUMOF) || (sector < 0)) {
        printf("error: sector %i is invalid\n", sector);
        return -1;
    }
    return sector;
}

static int getsize(const char *str)
{
    int size = atoi(str);
    if ((size >= BUFFER_SIZE) || (size < 0)) {
        printf("error: size %i is too big for local buffer\n", size);
        return -1;
    }
    return size;
}

static void dumpchar(uint8_t mem)
{
    if (mem >= ' ' && mem <= '~') {
        printf("  %c  ", mem);
    }
    else {
        printf("  ?? ");
    }
}

static void memdump(void *addr, size_t len)
{
    unsigned pos = 0;
    uint8_t *mem = (uint8_t *)addr;

    while (pos < (unsigned)len) {
        for (unsigned i = 0; i < LINE_LEN; i++) {
            printf("0x%02x ", mem[pos + i]);
        }
        puts("");
        for (unsigned i = 0; i < LINE_LEN; i++) {
            dumpchar(mem[pos++]);
        }
        puts("");
    }
}

static void dump_local(void)
{
    puts("Local sector buffer:");
    memdump(sector_mem, BUFFER_SIZE);
}

static int cmd_info(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("Flash start addr:\t0x%08x\n", (int)CPU_FLASH_BASE);
    printf("Number of sectors:\t%i\n", (int)FLASHSECTOR_NUMOF);
    printf("Internal buffer size:\t0x%08x\n", (int)BUFFER_SIZE);

    return 0;
}

static int cmd_dump(int argc, char **argv)
{
    int sector, size;
    void *addr;

    if (argc < 3) {
        printf("usage: %s <sector> <byte>\n", argv[0]);
        return 1;
    }

    sector = getsector(argv[1]);
    if (sector < 0) {
        return 1;
    }

    size = getsize(argv[2]);
    if (size < 0) {
        return 1;
    }
    addr = flashsector_addr(sector);

    printf("Flash sector %i at address %p, first %i byte\n", sector, addr, size);
    memdump(addr, size);

    return 0;
}

static int cmd_dump_local(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    dump_local();

    return 0;
}

static int cmd_read(int argc, char **argv)
{
    int sector, size;

    if (argc < 3) {
        printf("usage: %s <sector> <size>\n", argv[0]);
        return 1;
    }

    sector = getsector(argv[1]);
    if (sector < 0) {
        return 1;
    }

    size = getsize(argv[2]);
    if (size < 0) {
        return 1;
    }

    flashsector_read(sector, sector_mem, size);
    printf("Read first %i bytes of flash sector %i into local sector buffer\n", size, sector);
    dump_local();

    return 0;
}

static int cmd_write(int argc, char **argv)
{
    int sector;

    if (argc < 2) {
        printf("usage: %s <sector>\n", argv[0]);
        return 1;
    }

    sector = getsector(argv[1]);
    if (sector < 0) {
        return 1;
    }

    if (flashsector_write_and_verify(sector, sector_mem, BUFFER_SIZE) != FLASHSECTOR_OK) {
        printf("error: verification for sector %i failed\n", sector);
        return 1;
    }

    printf("wrote local buffer to flash sector %i at addr %p\n",
           sector, flashsector_addr(sector));
    return 0;
}

static int cmd_erase(int argc, char **argv)
{
    int sector;

    if (argc < 2) {
        printf("usage: %s <sector>\n", argv[0]);
        return 1;
    }

    sector = getsector(argv[1]);
    if (sector < 0) {
        return 1;
    }
    flashsector_write(sector, NULL, 0);

    printf("successfully erased sector %i (addr %p)\n",
           sector, flashsector_addr(sector));
    return 0;
}

static int cmd_edit(int argc, char **argv)
{
    int offset;
    size_t data_len;

    if (argc < 3) {
        printf("usage: %s <offset> <data>\n", argv[0]);
        return 1;
    }

    offset = atoi(argv[1]);
    if (offset >= BUFFER_SIZE) {
        printf("error: given offset is out of bounce\n");
        return -1;
    }
    data_len = strlen(argv[2]);
    if ((data_len + offset) > BUFFER_SIZE) {
        data_len = BUFFER_SIZE - offset;
    }

    memcpy(&sector_mem[offset], argv[2], data_len);
    dump_local();

    return 0;
}

static int cmd_test(int argc, char **argv)
{
    int sector;
    char fill = 'a';

    if (argc < 2) {
        printf("usage: %s <sector>\n", argv[0]);
        return 1;
    }

    sector = getsector(argv[1]);
    if (sector < 0) {
        return 1;
    }

    for (int i = 0; i < sizeof(sector_mem); i++) {
        sector_mem[i] = (uint8_t)fill++;
        if (fill > 'z') {
            fill = 'a';
        }
    }

    if (flashsector_write_and_verify(sector, sector_mem, BUFFER_SIZE) != FLASHSECTOR_OK) {
        printf("error verifying the content of sector %i\n", sector);
        return 1;
    }

    printf("wrote local sector to flash sector %i at addr %p\n",
           sector, flashsector_addr(sector));
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "info", "Show information about sectors", cmd_info },
    { "dump", "Dump the selected sector to STDOUT", cmd_dump },
    { "dump_local", "Dump the local sector buffer to STDOUT", cmd_dump_local },
    { "read", "Read and output the given sector", cmd_read },
    { "write", "Write (ASCII) data to the given sector", cmd_write },
    { "erase", "Erase the given sector", cmd_erase },
    { "edit", "Write bytes to the local sector", cmd_edit },
    { "test", "Write and verify test pattern", cmd_test },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("ROM flash read write test\n");
    puts("Please refer to the README.md for further information\n");

    cmd_info(0, NULL);

    /* run the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
