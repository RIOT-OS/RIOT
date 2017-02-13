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
 * @brief       Manual test application for UART peripheral drivers
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "shell.h"
#include "periph/flashpage.h"

#define LINE_LEN            (16)

/**
 * @brief   Allocate space for 1 flash page in RAM
 */
static uint8_t page_mem[FLASHPAGE_SIZE];

static int getpage(const char *str)
{
    int page = atoi(str);
    if ((page >= FLASHPAGE_NUMOF) || (page < 0)) {
        printf("error: page %i is invalid\n", page);
        return -1;
    }
    return page;
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
    puts("Local page buffer:");
    memdump(page_mem, FLASHPAGE_SIZE);
}

static int cmd_info(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("Flash start addr:\t0x%08x\n", (int)CPU_FLASH_BASE);
    printf("Page size:\t\t%i\n", (int)FLASHPAGE_SIZE);
    printf("Number of pages:\t%i\n", (int)FLASHPAGE_NUMOF);

    return 0;
}

static int cmd_dump(int argc, char **argv)
{
    int page;
    void *addr;

    if (argc < 2) {
        printf("usage: %s <page>\n", argv[0]);
        return 1;
    }

    page = getpage(argv[1]);
    if (page < 0) {
        return 1;
    }
    addr = flashpage_addr(page);

    printf("Flash page %i at address %p\n", page, addr);
    memdump(addr, FLASHPAGE_SIZE);

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
    int page;

    if (argc < 2) {
        printf("usage: %s <page>\n", argv[0]);
        return 1;
    }

    page = getpage(argv[1]);
    if (page < 0) {
        return 1;
    }

    flashpage_read(page, page_mem);
    printf("Read flash page %i into local page buffer\n", page);
    dump_local();

    return 0;
}

static int cmd_write(int argc, char **argv)
{
    int page;

    if (argc < 2) {
        printf("usage: %s <page>\n", argv[0]);
        return 1;
    }

    page = getpage(argv[1]);
    if (page < 0) {
        return 1;
    }

    if (flashpage_write_and_verify(page, page_mem) != FLASHPAGE_OK) {
        printf("error: verification for page %i failed\n", page);
        return 1;
    }

    printf("wrote local page to flash page %i at addr %p\n",
           page, flashpage_addr(page));
    return 0;
}

static int cmd_erase(int argc, char **argv)
{
    int page;

    if (argc < 2) {
        printf("usage: %s <page>\n", argv[0]);
        return 1;
    }

    page = getpage(argv[1]);
    if (page < 0) {
        return 1;
    }
    flashpage_write(page, NULL);

    printf("successfully erased page %i (addr %p)\n",
           page, flashpage_addr(page));
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
    if (offset >= FLASHPAGE_SIZE) {
        printf("error: given offset is out of bounce\n");
        return -1;
    }
    data_len = strlen(argv[2]);
    if ((data_len + offset) > FLASHPAGE_SIZE) {
        data_len = FLASHPAGE_SIZE - offset;
    }

    memcpy(&page_mem[offset], argv[2], data_len);
    dump_local();

    return 0;
}

static int cmd_test(int argc, char **argv)
{
    int page;
    char fill = 'a';

    if (argc < 2) {
        printf("usage: %s <page>\n", argv[0]);
        return 1;
    }

    page = getpage(argv[1]);
    if (page < 0) {
        return 1;
    }

    for (int i = 0; i < sizeof(page_mem); i++) {
        page_mem[i] = (uint8_t)fill++;
        if (fill > 'z') {
            fill = 'a';
        }
    }

    if (flashpage_write_and_verify(page, page_mem) != FLASHPAGE_OK) {
        printf("error verifying the content of page %i: ", page);
        return 1;
    }

    printf("wrote local page to flash page %i at addr %p\n",
           page, flashpage_addr(page));
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "info", "Show information about pages", cmd_info },
    { "dump", "Dump the selected page to STDOUT", cmd_dump },
    { "dump_local", "Dump the local page buffer to STDOUT", cmd_dump_local },
    { "read", "Read and output the given page", cmd_read },
    { "write", "Write (ASCII) data to the given page", cmd_write },
    { "erase", "Erase the given page", cmd_erase },
    { "edit", "Write bytes to the local page", cmd_edit },
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
