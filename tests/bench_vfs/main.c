/*
 * Copyright (C) 2018 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Benchmark application for file systems
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include "board.h"
#include "xtimer.h"
#include "timex.h"
#include "mtd.h"
#include "vfs.h"

/* Define MTD_0 in board.h to use the board mtd if any */
#ifdef MTD_0
#define _dev (MTD_0)
#else
/* Test mock object implementing a simple RAM-based mtd */
#ifndef SECTOR_COUNT
#define SECTOR_COUNT 4
#endif
#ifndef PAGE_PER_SECTOR
#define PAGE_PER_SECTOR 8
#endif
#ifndef PAGE_SIZE
#define PAGE_SIZE 128
#endif

static uint8_t dummy_memory[PAGE_PER_SECTOR * PAGE_SIZE * SECTOR_COUNT];

static int _init(mtd_dev_t *dev)
{
    (void)dev;

    memset(dummy_memory, 0xff, sizeof(dummy_memory));
    return 0;
}

static int _read(mtd_dev_t *dev, void *buff, uint32_t addr, uint32_t size)
{
    (void)dev;

    if (addr + size > sizeof(dummy_memory)) {
        return -EOVERFLOW;
    }
    memcpy(buff, dummy_memory + addr, size);

    return size;
}

static int _write(mtd_dev_t *dev, const void *buff, uint32_t addr, uint32_t size)
{
    (void)dev;

    if (addr + size > sizeof(dummy_memory)) {
        return -EOVERFLOW;
    }
    if (size > PAGE_SIZE) {
        return -EOVERFLOW;
    }
    memcpy(dummy_memory + addr, buff, size);

    return size;
}

static int _erase(mtd_dev_t *dev, uint32_t addr, uint32_t size)
{
    (void)dev;

    if (size % (PAGE_PER_SECTOR * PAGE_SIZE) != 0) {
        return -EOVERFLOW;
    }
    if (addr % (PAGE_PER_SECTOR * PAGE_SIZE) != 0) {
        return -EOVERFLOW;
    }
    if (addr + size > sizeof(dummy_memory)) {
        return -EOVERFLOW;
    }
    memset(dummy_memory + addr, 0xff, size);

    return 0;
}

static int _power(mtd_dev_t *dev, enum mtd_power_state power)
{
    (void)dev;
    (void)power;
    return 0;
}

static const mtd_desc_t driver = {
    .init = _init,
    .read = _read,
    .write = _write,
    .erase = _erase,
    .power = _power,
};

static mtd_dev_t dev = {
    .driver = &driver,
    .sector_count = SECTOR_COUNT,
    .pages_per_sector = PAGE_PER_SECTOR,
    .page_size = PAGE_SIZE,
};

static mtd_dev_t *_dev = (mtd_dev_t*) &dev;
#endif /* MTD_0 */

#if defined(MODULE_SPIFFS)
#include "fs/spiffs_fs.h"
static char fs_name[] = "spiffs";

static struct spiffs_desc spiffs_desc = {
    .lock = MUTEX_INIT,
};

static vfs_mount_t _bench_mount = {
    .fs = &spiffs_file_system,
    .mount_point = "/bench",
    .private_data = &spiffs_desc,
};

static void init_fs(void)
{
#if SPIFFS_HAL_CALLBACK_EXTRA == 1
    spiffs_desc.dev = _dev;
#endif
    mtd_init(_dev);
}
#elif defined(MODULE_LITTLEFS)
#include "fs/littlefs_fs.h"
static char fs_name[] = "littlefs";

static littlefs_desc_t littlefs_desc = {
    .lock = MUTEX_INIT,
};

static vfs_mount_t _bench_mount = {
    .fs = &littlefs_file_system,
    .mount_point = "/bench",
    .private_data = &littlefs_desc,
};

static void init_fs(void)
{
    littlefs_desc.dev = _dev;
    mtd_init(_dev);
}
#else
static char fs_name[] = "raw_mtd";

static void init_fs(void)
{
    mtd_init(_dev);
}

#define BASE_NAME       "/dev/mtd"
#define DO_NOT_MOUNT
#endif

#ifndef FILE_LOOP_SIZE
#define FILE_LOOP_SIZE  (1u)
#endif

#ifndef LOOP_SIZE
#define LOOP_SIZE       (16u)
#endif

#ifndef BASE_NAME
#define BASE_NAME       "/bench/test"
#endif

#ifndef BUF_SIZE
#define BUF_SIZE        (4096u)
#endif

static char buf[BUF_SIZE];

int main(void)
{
    printf("benchmarking file system: %s\n", fs_name);
    init_fs();

    for (size_t i = 0; i < BUF_SIZE; i++) {
        buf[i] = '0' + i % 10;
    }

    xtimer_ticks32_t begin;
    xtimer_ticks32_t end;
    xtimer_ticks32_t diff;

#ifndef DO_NOT_MOUNT
    puts("[BEGIN] Format test...");
    begin = xtimer_now();
    vfs_format(&_bench_mount);
    end = xtimer_now();
    diff = xtimer_diff(end, begin);
    printf("Test time: %" PRIu32 "us\n", xtimer_usec_from_ticks(diff));
    puts("[END] Format test");

    puts("[BEGIN] Mount test...");
    begin = xtimer_now();
    vfs_mount(&_bench_mount);
    end = xtimer_now();
    diff = xtimer_diff(end, begin);
    printf("Test time: %" PRIu32 "us\n", xtimer_usec_from_ticks(diff));
    puts("[END] Mount test");
#endif

    int fd;
    uint32_t total_time = 0;
    int ret;

    puts("[BEGIN] Write test...");
    for (unsigned f = 0; f < FILE_LOOP_SIZE; f++) {
        char name[20];
        sprintf(name, BASE_NAME"%d", f);
        begin = xtimer_now();
        fd = vfs_open(name, O_CREAT | O_RDWR, 0);
        for (unsigned i = 0; i < LOOP_SIZE; i++) {
            ret = vfs_write(fd, buf, sizeof(buf));
            if (ret != (int)sizeof(buf)) {
                printf("[END] error when writing (%d)\n", ret);
                break;
            }
        }
        vfs_close(fd);
        end = xtimer_now();
        diff = xtimer_diff(end, begin);
        printf("File #%d, %u bytes written in: %" PRIu32 "us\n", f,
               LOOP_SIZE * sizeof(buf), xtimer_usec_from_ticks(diff));
        total_time += xtimer_usec_from_ticks(diff);
    }
    printf("Mean time: %" PRIu32 "us\n", total_time / (uint32_t)FILE_LOOP_SIZE);
    printf("Throughput: %f kB/s\n",
           ((float)(FILE_LOOP_SIZE * LOOP_SIZE * sizeof(buf) * 1000.0) / (float)total_time));
    puts("[END] Write test");

    total_time = 0;
    uint32_t read_total = 0;
    puts("[BEGIN] Read test...");
    for (unsigned f = 0; f < FILE_LOOP_SIZE; f++) {
        char name[20];
        sprintf(name, BASE_NAME"%d", f);
        begin = xtimer_now();
        fd = vfs_open(name, O_RDONLY, 0);
        int total = 0;
        int ret;
        while ((ret = vfs_read(fd, buf, sizeof(buf))) > 0) {
            total += ret;
        }
        vfs_close(fd);
        end = xtimer_now();
        diff = xtimer_diff(end, begin);
        printf("File #%d, %d bytes read in: %" PRIu32 "us\n", f,
               total, xtimer_usec_from_ticks(diff));
        total_time += xtimer_usec_from_ticks(diff);
        read_total += total;
    }
    printf("Mean time: %" PRIu32 "us\n", total_time / (uint32_t)FILE_LOOP_SIZE);
    printf("Throughput: %f kB/s\n",
           ((float)(read_total * 1000.0) / (float)total_time));
    puts("[END] Read test");

    total_time = 0;
    puts("[BEGIN] Remove test...");
    for (unsigned f = 0; f < FILE_LOOP_SIZE; f++) {
        char name[20];
        sprintf(name, BASE_NAME"%d", f);
        begin = xtimer_now();
        vfs_unlink(name);
        end = xtimer_now();
        diff = xtimer_diff(end, begin);
        printf("File #%d, test time: %" PRIu32 "us\n", f, xtimer_usec_from_ticks(diff));
        total_time += xtimer_usec_from_ticks(diff);
    }
    printf("Mean time: %" PRIu32 "us\n", total_time / (uint32_t)FILE_LOOP_SIZE);
    puts("[END] Remove test");

#ifndef DO_NOT_MOUNT
    puts("[BEGIN] Unmount test...");
    begin = xtimer_now();
    vfs_umount(&_bench_mount);
    end = xtimer_now();
    diff = xtimer_diff(end, begin);
    printf("Test time: %" PRIu32 "us\n", xtimer_usec_from_ticks(diff));
    puts("[END] Unmount test");
#endif

    return 0;
}
