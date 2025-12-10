/*
 * SPDX-FileCopyrightText: 2023 ML!PA Consulting Gmbh
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup   tests
 * @{
 *
 * @file
 * @brief       Short test for the FlashDB FAL configuration initialization
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 *
 * @}
 */

#include <stdio.h>

#include "container.h"
#include "fal.h"
#include "macros/math.h"
#include "mutex.h"
#include "periph/rtc.h"

#include <flashdb.h>

/**
 * @brief   FlashDB Magic Word
 *
 *          The value here is not relevant but it must be defined.
 */
#define FAL_PART_MAGIC_WORD         0x45503130

/**
 * @brief   Number of FAL partitions
 */
#define FAL_PART_TABLE_NUMOF        ARRAY_SIZE(((const struct fal_partition[])FAL_PART_TABLE))

#if !defined(FAL_TSDB_MAX) || defined(DOXYGEN)
/**
 * @brief   Maximum length of a TSDB entry
 */
#define FAL_TSDB_MAX                128
#endif

#if !defined(FAL_PART_TABLE_KVDB) || defined(DOXYGEN)
/**
 * @brief   Indices of partitions in @ref FAL_PART_TABLE that can be initialized as a Key-Value-DB
 */
#define FAL_PART_TABLE_KVDB     \
{                               \
    0,                          \
    1,                          \
}
#endif

#if !defined(FAL_PART_TABLE_TSDB) || defined(DOXYGEN)
/**
 * @brief   Indices of partitions in @ref FAL_PART_TABLE that can be initialized as a Time-Series-DB
 */
#define FAL_PART_TABLE_TSDB     \
{                               \
    2,                          \
    3,                          \
}
#endif

extern void fdb_mtd_init(mtd_dev_t *mtd);

static mutex_t _locker = MUTEX_INIT;
static const unsigned _fdb_part_kvdb[] = FAL_PART_TABLE_KVDB;
static const unsigned _fdb_part_tsdb[] = FAL_PART_TABLE_TSDB;
static struct fdb_kvdb _kvdb[ARRAY_SIZE(_fdb_part_kvdb)];
static struct fdb_tsdb _tsdb[ARRAY_SIZE(_fdb_part_tsdb)];

/**
 * @brief Select MTD device to use for FlashDB
 */
#if !defined(FDB_MTD)
#define FDB_MTD FAL_MTD
#endif

static void _lock(fdb_db_t db)
{
    mutex_lock(db->user_data);
}

static void _unlock(fdb_db_t db)
{
    mutex_unlock(db->user_data);
}

static fdb_time_t _get_time(void)
{
    struct tm now;
    rtc_get_time(&now);
    return mktime(&now);
}

int main(void)
{
    if (FDB_MTD == NULL) {
        puts("No MTD device configured for test, abort.");
        return -1;
    }

    int init_failed;
    fdb_mtd_init(FDB_MTD);
    size_t size = FDB_MTD->pages_per_sector * FDB_MTD->page_size;
    /* scale hardware sector size to minimum required virtual sector size */
    size = DIV_ROUND_UP((CONFIG_FLASHDB_MIN_SECTOR_SIZE_DEFAULT_KiB * KiB(1)), size) * size;
    printf("Informational: Make sure the following requirements are fulfilled for a successful initialization:\n");
    printf("The virtual sector size is a multiple of the physical sector size: %lu %% %lu == 0\n",
           (unsigned long)size, (unsigned long)FDB_MTD->pages_per_sector * FDB_MTD->page_size);
    printf("The maximum partition size is a multiple of the virtual sector size: %lu %% %lu == 0\n",
           (unsigned long)FAL_PART0_LENGTH, (unsigned long)size);

    for (unsigned i = 0; i < ARRAY_SIZE(_fdb_part_kvdb); i++) {
        unsigned part = _fdb_part_kvdb[i];
        if (part >= FAL_PART_TABLE_NUMOF) {
            continue;
        }
        fdb_kvdb_control(&_kvdb[i], FDB_KVDB_CTRL_SET_SEC_SIZE, &size);
        fdb_kvdb_control(&_kvdb[i], FDB_KVDB_CTRL_SET_LOCK, (void *)(uintptr_t)_lock);
        fdb_kvdb_control(&_kvdb[i], FDB_KVDB_CTRL_SET_UNLOCK, (void *)(uintptr_t)_unlock);
        const char *spart = ((const struct fal_partition[])FAL_PART_TABLE)[part].name;
        printf("Initializing FlashDB KVDB partition %s\n", spart);
        if ((init_failed = fdb_kvdb_init(&_kvdb[i], "kvdb", spart, NULL, &_locker)) != FDB_NO_ERR) {
            printf("Failed to initialize FlashDB KVDB partition %s (%d)\n", spart, init_failed);
            return 1;
        }
    }
    for (unsigned i = 0; i < ARRAY_SIZE(_fdb_part_tsdb); i++) {
        unsigned part = _fdb_part_tsdb[i];
        if (part >= FAL_PART_TABLE_NUMOF) {
            continue;
        }
        fdb_tsdb_control(&_tsdb[i], FDB_TSDB_CTRL_SET_LOCK, (void *)(uintptr_t)_lock);
        fdb_tsdb_control(&_tsdb[i], FDB_TSDB_CTRL_SET_UNLOCK, (void *)(uintptr_t)_unlock);
        fdb_tsdb_control(&_tsdb[i], FDB_TSDB_CTRL_SET_SEC_SIZE, &size);
        const char *spart = ((const struct fal_partition[])FAL_PART_TABLE)[part].name;
        printf("Initializing FlashDB TSDB partition %s\n", spart);
        if ((init_failed = fdb_tsdb_init(&_tsdb[i], "tsdb", spart, _get_time, FAL_TSDB_MAX, &_locker)) != FDB_NO_ERR) {
            printf("Failed to initialize FlashDB TSDB partition %s (%d)\n", spart, init_failed);
            return 1;
        }
    }
    puts("SUCCESS");
    return 0;
}
