/*
 * SPDX-FileCopyrightText: 2020 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Application for testing MTD implementations
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "macros/utils.h"
#include "mtd.h"
#include "shell.h"
#include "test_utils/expect.h"

static mtd_dev_t *_get_dev(int argc, char **argv)
{
    if (argc < 2) {
        printf("%s: please specify the MTD device\n", argv[0]);
        return NULL;
    }

    unsigned idx = atoi(argv[1]);

    if (idx > MTD_NUMOF) {
        printf("%s: invalid device: %s\n", argv[0], argv[1]);
        return NULL;
    }

    return mtd_dev_get(idx);
}

static bool mem_is_all_set(const uint8_t *buf, uint8_t c, size_t n)
{
    for (const uint8_t *end = buf + n; buf != end; ++buf) {
        if (*buf != c) {
            return false;
        }
    }

    return true;
}

static int cmd_test(int argc, char **argv)
{
    mtd_dev_t *dev = _get_dev(argc, argv);
    uint32_t sector;

    if (argc < 2 || dev == NULL) {
        printf("usage: %s <dev>\n", argv[0]);
        return -1;
    }

    if (dev->sector_count < 2) {
        return -1;
    }

    if (argc > 2) {
        sector = atoi(argv[2]);
    } else {
        sector = dev->sector_count - 2;
    }

    uint8_t *buffer = malloc(dev->page_size);

    if (buffer == NULL) {
        puts("out of memory");
        return -1;
    }

    uint32_t page_0 = dev->pages_per_sector * sector;
    uint32_t page_1 = dev->pages_per_sector * (sector + 1);
    uint32_t page_size = dev->page_size;

    puts("[START]");

    /* write dummy data to sectors */
    memset(buffer, 0x23, dev->page_size);
    expect(mtd_write_page_raw(dev, buffer, page_0, 0, page_size) == 0);
    expect(mtd_write_page_raw(dev, buffer, page_1, 0, page_size) == 0);

    /* erase two sectors and check if they have been erased */
    expect(mtd_erase_sector(dev, sector, 2) == 0);
    expect(mtd_read_page(dev, buffer, page_0, 0, page_size) == 0);
    expect(mem_is_all_set(buffer, 0xFF, page_size) || mem_is_all_set(buffer, 0x00, page_size));
    expect(mtd_read_page(dev, buffer, page_1, 0, page_size) == 0);
    expect(mem_is_all_set(buffer, 0xFF, page_size) || mem_is_all_set(buffer, 0x00, page_size));

    /* write test data & read it back */
    const char test_str[] = "0123456789";
    uint32_t offset = 5;

    expect(mtd_write_page_raw(dev, test_str, page_0, offset, sizeof(test_str)) == 0);
    expect(mtd_read_page(dev, buffer, page_0, offset, sizeof(test_str)) == 0);
    expect(memcmp(test_str, buffer, sizeof(test_str)) == 0);

    /* write across page boundary */
    offset = page_size - sizeof(test_str) / 2;
    expect(mtd_write_page_raw(dev, test_str, page_0, offset, sizeof(test_str)) == 0);
    expect(mtd_read_page(dev, buffer, page_0, offset, sizeof(test_str)) == 0);
    expect(memcmp(test_str, buffer, sizeof(test_str)) == 0);

    /* write across sector boundary */
    offset = page_size - sizeof(test_str) / 2
           + (dev->pages_per_sector - 1) * page_size;
    expect(mtd_write_page_raw(dev, test_str, page_0, offset, sizeof(test_str)) == 0);
    expect(mtd_read_page(dev, buffer, page_0, offset, sizeof(test_str)) == 0);
    expect(memcmp(test_str, buffer, sizeof(test_str)) == 0);

    /* overwrite first test string, rely on MTD for read-modify-write */
    const char test_str_2[] = "Hello World!";
    offset = 5;
    expect(mtd_write_page(dev, test_str_2, page_0, offset, sizeof(test_str_2)) == 0);
    expect(mtd_read_page(dev, buffer, page_0, offset, sizeof(test_str_2)) == 0);
    expect(memcmp(test_str_2, buffer, sizeof(test_str_2)) == 0);

    /* test write_page across sectors */
    offset = dev->pages_per_sector * dev->page_size - 2;
    expect(mtd_write_page(dev, test_str, page_0, offset, sizeof(test_str)) == 0);
    expect(mtd_read_page(dev, buffer, page_0, offset, sizeof(test_str)) == 0);
    expect(memcmp(test_str, buffer, sizeof(test_str)) == 0);

    puts("[SUCCESS]");

    free(buffer);

    return 0;
}

SHELL_COMMAND(test, "Erase & write test data to the last two sectors", cmd_test);

static int cmd_test_erase(int argc, char **argv)
{
    mtd_dev_t *dev = _get_dev(argc, argv);

    /* limit the erased region, erasing e.g. a whole SD card would take forever */
    uint32_t limit_mib = (argc > 2) ? strtoul(argv[2], NULL, 10) : 16;

    if (dev == NULL || limit_mib == 0) {
        printf("usage: %s <dev> [size in MiB, default 16]\n", argv[0]);
        return -1;
    }

    uint32_t sector_size = dev->pages_per_sector * dev->page_size;
    uint32_t sector_count = (uint64_t)limit_mib * 1024 * 1024 / sector_size;
    sector_count = MIN(sector_count, dev->sector_count);

    uint8_t byte = 0x23;

    puts("[START]");

    /* write one byte to every sector so that the erase can't skip any */
    for (uint32_t sector = 0; sector < sector_count; sector++) {
        expect(mtd_write_page_raw(dev, &byte, sector * dev->pages_per_sector, 0, 1) == 0);
    }

    /* erase the whole region at once and check that the data is gone */
    expect(mtd_erase_sector(dev, 0, sector_count) == 0);
    for (uint32_t sector = 0; sector < sector_count; sector++) {
        expect(mtd_read_page(dev, &byte, sector * dev->pages_per_sector, 0, 1) == 0);
        expect(byte == 0xFF || byte == 0x00);
    }

    puts("[SUCCESS]");

    return 0;
}

SHELL_COMMAND(test_erase, "Write to large region & erase it at once", cmd_test_erase);

int main(void)
{
    puts("Manual MTD test");

    if (MTD_NUMOF == 0) {
        puts("no MTD device present on the board.");
    }

    for (unsigned i = 0; i < MTD_NUMOF; ++i) {
        printf("init MTD_%d… ", i);

        mtd_dev_t *dev = mtd_dev_get(i);
        int res = mtd_init(dev);
        if (res) {
            printf("error: %d\n", res);
            continue;
        }

        mtd_power(dev, MTD_POWER_UP);
    }

    return 0;
}
