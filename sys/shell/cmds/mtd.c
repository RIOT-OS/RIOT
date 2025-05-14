/*
 * SPDX-FileCopyrightText: 2025 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Command to low-level access Memory Technology Devices (MTD)
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 *
 * @}
 */

#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"
#include <ctype.h>
#include "errno.h"
#include "fmt.h"
#include "macros/units.h"
#include "mtd.h"
#include "od.h"
#include "shell.h"

static uint64_t _get_size(mtd_dev_t *dev)
{
    return (uint64_t)dev->sector_count
         * dev->pages_per_sector
         * dev->page_size;
}

static int _print_read_usage(const char *progname)
{
    printf("usage: %s <addr> <len>\n", progname);
    return -1;
}

static int cmd_read(mtd_dev_t *dev, int argc, char **argv)
{
    uint32_t addr, len;

    assert(strcmp(*argv, "read") == 0);
    if (argc < 3) {
        return _print_read_usage(argv[0]);
    }

    addr = atoi(argv[1]);
    len  = atoi(argv[2]);

    void *buffer = malloc(len);
    if (buffer == NULL) {
        puts("out of memory");
        return -1;
    }

    /* don't print random data if read fails */
    memset(buffer, 0x3F, len);

    int res = mtd_read(dev, buffer, addr, len);

    if (res) {
        printf("error: %i\n", res);
    }
    else {
        od_hex_dump_ext(buffer, len, 0, addr);
    }

    free(buffer);

    return res;
}

static int _print_read_page_usage(const char *progname)
{
    printf("usage: %s <page> <offset> <len>\n", progname);
    return -1;
}

static int cmd_read_page(mtd_dev_t *dev, int argc, char **argv)
{
    uint32_t page, offset, len;

    assert(strcmp(*argv, "read_page") == 0);
    if (argc < 4) {
        return _print_read_page_usage(argv[0]);
    }

    page   = atoi(argv[1]);
    offset = atoi(argv[2]);
    len    = atoi(argv[3]);

    void *buffer = malloc(len);
    if (buffer == NULL) {
        puts("out of memory");
        return -1;
    }

    int res = mtd_read_page(dev, buffer, page, offset, len);

    if (res) {
        printf("error: %i\n", res);
    }
    else {
        od_hex_dump_ext(buffer, len, 0, page * dev->page_size + offset);
    }

    free(buffer);

    return res;
}

static int _print_write_usage(const char *progname)
{
    printf("usage: %s [-b] <addr> <data>\n", progname);
    return -1;
}

ssize_t _fmt_binary(char *buf)
{
    size_t len = 0;
    if (strlen(buf) % 2) {
        return -1;
    }
    for (size_t i = 0; i < len; i++) {
        if (!isxdigit((int)(buf[i]))) {
            return -1;
        }
    }
    return fmt_hex_bytes((uint8_t *)buf, buf);
}

static int cmd_write(mtd_dev_t *dev, int argc, char **argv)
{
    uint32_t addr, len;
    void *data;

    assert(strcmp(*argv, "write") == 0);
    if (argc < 3) {
        return _print_write_usage(argv[0]);
    }

    bool binary = argc > 3 && (strcmp(argv[1], "-b") == 0);
    if (binary) {
        addr = atoi(argv[2]);
        data = argv[3];
        ssize_t blen  = _fmt_binary(data);
        if (blen < 0) {
            printf("error: data must be hexadecimal: %s\n", (char *)data);
            return -1;
        }
        len = blen;
    }
    else {
        addr = atoi(argv[1]);
        data = argv[2];
        len  = strlen(data);
    }

    int res = mtd_write(dev, data, addr, len);

    if (res) {
        printf("error: %i\n", res);
    }

    return res;
}

static int _print_write_page_raw_usage(const char *progname)
{
    printf("usage: %s [-b] <page> <offset> <data>\n", progname);
    return -1;
}

static int cmd_write_page_raw(mtd_dev_t *dev, int argc, char **argv)
{
    uint32_t page, offset, len;
    void *data;

    assert(strcmp(*argv, "write_page_raw") == 0);
    if (argc < 4) {
        return _print_write_page_raw_usage(argv[0]);
    }

    bool binary = argc > 4 && (strcmp(argv[1], "-b") == 0);
    if (binary) {
        page   = atoi(argv[2]);
        offset = atoi(argv[3]);
        data = argv[4];
        ssize_t blen  = _fmt_binary(data);
        if (blen < 0) {
            printf("error: data must be hexadecimal: %s\n", (char *)data);
            return -1;
        }
        len = blen;
    }
    else {
        page   = atoi(argv[1]);
        offset = atoi(argv[2]);
        data = argv[3];
        len    = strlen(data);
    }

    int res = mtd_write_page_raw(dev, data, page, offset, len);

    if (res) {
        printf("error: %i\n", res);
    }

    return res;
}

static int _print_write_page_usage(const char *progname)
{
    printf("usage: %s [-b] <page> <offset> <data>\n", progname);
    return -1;
}

static int cmd_write_page(mtd_dev_t *dev, int argc, char **argv)
{
#if IS_USED(MODULE_MTD_WRITE_PAGE)
    uint32_t page, offset, len;
    void *data;

    assert(strcmp(*argv, "write_page") == 0);
    if (argc < 4) {
        return _print_write_page_usage(argv[0]);
    }

    bool binary = argc > 4 && (strcmp(argv[1], "-b") == 0);
    if (binary) {
        page   = atoi(argv[2]);
        offset = atoi(argv[3]);
        data = argv[4];
        ssize_t blen  = _fmt_binary(data);
        if (blen < 0) {
            printf("error: data must be hexadecimal: %s\n", (char *)data);
            return -1;
        }
        len = blen;
    }
    else {
        page   = atoi(argv[1]);
        offset = atoi(argv[2]);
        data = argv[3];
        len    = strlen(data);
    }

    int res = mtd_write_page(dev, data, page, offset, len);

    if (res) {
        printf("error: %i\n", res);
    }

    return res;
#else
    (void)dev;
    (void)argc;
    (void)argv;
    printf("error: write_page not supported, missing module mtd_write_page\n");
    return -ENOTSUP;
#endif
}

static int _print_erase_usage(const char *progname)
{
    printf("usage: %s <addr> <len>\n", progname);
    return -1;
}

static int cmd_erase(mtd_dev_t *dev, int argc, char **argv)
{
    uint32_t addr;
    uint32_t len;

    assert(strcmp(*argv, "erase") == 0);
    if (argc < 3) {
        return _print_erase_usage(argv[0]);
    }

    addr = atoi(argv[1]);
    len  = atoi(argv[2]);

    int res = mtd_erase(dev, addr, len);

    if (res) {
        printf("error: %i\n", res);
    }

    return res;
}

static int _print_erase_sector_usage(const char *progname)
{
    printf("usage: %s <sector> [count]\n", progname);
    return -1;
}

static int cmd_erase_sector(mtd_dev_t *dev, int argc, char **argv)
{
    uint32_t sector, count = 1;

    assert(strcmp(*argv, "erase_sector") == 0);
    if (argc < 2) {
        return _print_erase_sector_usage(argv[0]);
    }

    sector = atoi(argv[1]);
    if (argc > 2) {
        count = atoi(argv[2]);
    }

    int res = mtd_erase_sector(dev, sector, count);

    if (res) {
        printf("error: %i\n", res);
    }

    return res;
}

static void _print_size(uint64_t size)
{
    unsigned long len;
    const char *unit;

    if (size == 0) {
        len = 0;
        unit = "byte";
    }
    else if ((size & (GiB(1) - 1)) == 0) {
        len = size / GiB(1);
        unit = "GiB";
    }
    else if ((size & (MiB(1) - 1)) == 0) {
        len = size / MiB(1);
        unit = "MiB";
    }
    else if ((size & (KiB(1) - 1)) == 0) {
        len = size / KiB(1);
        unit = "kiB";
    }
    else {
        len = size;
        unit = "byte";
    }

    printf("%lu %s", len, unit);
}

static void _print_info(mtd_dev_t *dev)
{
    assert(dev);
    printf("sectors: %"PRIu32"\n", dev->sector_count);
    printf("pages per sector: %"PRIu32"\n", dev->pages_per_sector);
    printf("page size: %"PRIu32"\n", dev->page_size);
    printf("total: ");
    _print_size(_get_size(dev));
    puts("");
}

static int cmd_info(mtd_dev_t *dev, int argc, char **argv)
{
    (void)argc;
    (void)argv;
    assert(strcmp(*argv, "info") == 0);

    if (dev) {
        _print_info(dev);
    }
    else {
        printf("mtd devices: %u\n", (unsigned)MTD_NUMOF);
        for (unsigned i = 0; i < MTD_NUMOF; ++i) {
            printf(" -=[ MTD_%d ]=-\n", i);
            _print_info(mtd_dev_get(i));
        }
    }

    return 0;
}

static int _print_power_usage(const char *progname)
{
    printf("usage: %s <on|off>\n", progname);
    return -1;
}

static int cmd_power(mtd_dev_t *dev, int argc, char **argv)
{
    enum mtd_power_state state;

    assert(strcmp(*argv, "power") == 0);
    if (argc < 2) {
        return _print_power_usage(argv[0]);
    }

    if (strcmp(argv[1], "off") == 0) {
        state = MTD_POWER_DOWN;
    }
    else if (strcmp(argv[1], "on") == 0) {
        state = MTD_POWER_UP;
    }
    else {
        return _print_power_usage(argv[0]);
    }

    mtd_power(dev, state);

    return 0;
}

static int _cmd_mtd(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s [dev] <command> [args]\n", argv[0]);
        printf("commands:\n"
               "  read:             Read a region of memory on the MTD\n"
               "  read_page:        Read a region of memory on the MTD (pagewise addressing)\n"
               "  write:            Write a region of memory on the MTD\n"
               "  write_page_raw:   Write a region of memory on the MTD (pagewise addressing)\n"
               "  write_page:       Write a region of memory on the MTD (pagewise addressing, read-modify-write)\n"
               "  erase:            Erase a region of memory on the MTD\n"
               "  erase_sector:     Erase a sector of memory on the MTD\n"
               "  info:             Print properties of the MTD device\n"
               "  power:            Turn the MTD device on/off\n"
            );
        return -1;
    }

    if (argc == 2) {
        if (!strcmp(argv[1], "info")) {
            argc -= 1;
            argv += 1;
            return cmd_info(NULL, argc, argv);
        }
        printf("unknown command: %s\n", argv[1]);
        return -1;
    }

    unsigned idx = atoi(argv[1]);
    if (idx > MTD_NUMOF) {
        printf("%s: invalid device: %s\n", argv[0], argv[1]);
        return -1;
    }
    argc -= 2;
    argv += 2;

    mtd_dev_t *dev =  mtd_dev_get(idx);
    if (dev == NULL) {
        return -1;
    }

    if (!strcmp(argv[0], "read")) {
        return cmd_read(dev, argc, argv);
    }
    else if (!strcmp(argv[0], "read_page")) {
        return cmd_read_page(dev, argc, argv);
    }
    else if (!strcmp(argv[0], "write")) {
        return cmd_write(dev, argc, argv);
    }
    else if (!strcmp(argv[0], "write_page_raw")) {
        return cmd_write_page_raw(dev, argc, argv);
    }
    else if (!strcmp(argv[0], "write_page")) {
        return cmd_write_page(dev, argc, argv);
    }
    else if (!strcmp(argv[0], "erase")) {
        return cmd_erase(dev, argc, argv);
    }
    else if (!strcmp(argv[0], "erase_sector")) {
        return cmd_erase_sector(dev, argc, argv);
    }
    else if (!strcmp(argv[0], "info")) {
        return cmd_info(dev, argc, argv);
    }
    else if (!strcmp(argv[0], "power")) {
        return cmd_power(dev, argc, argv);
    }
    printf("unknown command: %s\n", argv[0]);
    return -1;
}

SHELL_COMMAND(mtd, "Read and write raw data to an MTD", _cmd_mtd);
