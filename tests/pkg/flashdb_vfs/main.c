/*
 * Copyright (c) 2021, Armink, <armink.ztl@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <flashdb.h>

#include "fal_cfg.h"
#include "board.h"
#include "mutex.h"
#ifdef MODULE_VFS
#include "vfs_default.h"
#else
#include "mtd.h"
#endif

/**
 * @brief Select MTD device to use for FlashDB
 */
#if !defined(FDB_MTD)
#define FDB_MTD FAL_MTD
#endif

#define FDB_LOG_TAG "[main]"

#if IS_USED(MODULE_FLASHDB_MTD)
#define FDB_DIR     ""
#else
#define FDB_DIR     VFS_DEFAULT_DATA
#endif

#define KVDB_DIR    (FDB_DIR "/fdb_kvdb1")
#define TSDB_DIR    (FDB_DIR "/fdb_tsdb1")

static mutex_t kv_locker, ts_locker;
static uint32_t boot_count = 0;
static time_t boot_time[10] = {0, 1, 2, 3};
/* default KV nodes */
static struct fdb_default_kv_node default_kv_table[] = {
        {"username", "armink", 0}, /* string KV */
        {"password", "123456", 0}, /* string KV */
        {"boot_count", &boot_count, sizeof(boot_count)}, /* int type KV */
        {"boot_time", &boot_time, sizeof(boot_time)},    /* int array type KV */
};
/* KVDB object */
static struct fdb_kvdb kvdb = { 0 };
/* TSDB object */
struct fdb_tsdb tsdb = { 0 };
/* counts for simulated timestamp */
static int counts = 0;

extern void kvdb_basic_sample(fdb_kvdb_t kvdb);
extern void kvdb_type_string_sample(fdb_kvdb_t kvdb);
extern void kvdb_type_blob_sample(fdb_kvdb_t kvdb);
extern void tsdb_sample(fdb_tsdb_t tsdb);

extern void fdb_mtd_init(mtd_dev_t *mtd);

static void lock(fdb_db_t db)
{
    mutex_lock(db->user_data);
}

static void unlock(fdb_db_t db)
{
    mutex_unlock(db->user_data);
}

static fdb_time_t get_time(void)
{
    return time(NULL);
}

/* converts a test condition to a human readable string */
const char *resstr(bool condition)
{
    return condition ? "[OK]" : "[FAILED]";
}

int main(void)
{
    fdb_err_t result;
    uint32_t sec_size = 4096, db_size = sec_size * 4;


    if (IS_USED(MODULE_FLASHDB_MTD)) {
#if defined(FDB_MTD)
    fdb_mtd_init(FDB_MTD);
    sec_size = FDB_MTD->pages_per_sector * FDB_MTD->page_size;
#else
    puts("error: no MTD device configured for FlashDB");
    return 0;
#endif
    }

#ifdef FDB_USING_KVDB
    { /* KVDB Sample */
        struct fdb_default_kv default_kv;

#if IS_USED(MODULE_FLASHDB_VFS)
        /* enable file mode */
        bool file_mode = true;
        fdb_kvdb_control(&kvdb, FDB_KVDB_CTRL_SET_FILE_MODE, &file_mode);
        /* create database directory */
        int res = vfs_mkdir(KVDB_DIR, 0777);
        printf("mkdir '%s' %s\n", KVDB_DIR, resstr(res == 0 || res == -EEXIST));
        if (!(res == 0 || res == -EEXIST)) {
            return -1;
        }
#endif
        default_kv.kvs = default_kv_table;
        default_kv.num = sizeof(default_kv_table) / sizeof(default_kv_table[0]);
        /* set the lock and unlock function if you want */
        mutex_init(&kv_locker);
        fdb_kvdb_control(&kvdb, FDB_KVDB_CTRL_SET_LOCK, (void *)(uintptr_t)lock);
        fdb_kvdb_control(&kvdb, FDB_KVDB_CTRL_SET_UNLOCK, (void *)(uintptr_t)unlock);
        /* set the sector and database max size */
        fdb_kvdb_control(&kvdb, FDB_KVDB_CTRL_SET_SEC_SIZE, &sec_size);
        fdb_kvdb_control(&kvdb, FDB_KVDB_CTRL_SET_MAX_SIZE, &db_size);
        /* Key-Value database initialization
         *
         *       &kvdb: database object
         *       "env": database name
         * "fdb_kvdb1": The flash partition name base on FAL. Please make sure it's in FAL partition table.
         *              Please change to YOUR partition name.
         * &default_kv: The default KV nodes. It will auto add to KVDB when first initialize successfully.
         *  &kv_locker: The locker object.
         */
        result = fdb_kvdb_init(&kvdb, "env", KVDB_DIR, &default_kv, &kv_locker);
        printf("fdb_kvdb_init %s\n", resstr(result == FDB_NO_ERR));

        if (result != FDB_NO_ERR) {
            return -1;
        }

        /* run basic KV samples */
        kvdb_basic_sample(&kvdb);
        /* run string KV samples */
        kvdb_type_string_sample(&kvdb);
        /* run blob KV samples */
        kvdb_type_blob_sample(&kvdb);
    }
#endif /* FDB_USING_KVDB */

#ifdef FDB_USING_TSDB
    { /* TSDB Sample */
        /* set the lock and unlock function if you want */
        mutex_init(&ts_locker);
        fdb_tsdb_control(&tsdb, FDB_TSDB_CTRL_SET_LOCK, (void *)(uintptr_t)lock);
        fdb_tsdb_control(&tsdb, FDB_TSDB_CTRL_SET_UNLOCK, (void *)(uintptr_t)unlock);
        /* set the sector and database max size */
        fdb_tsdb_control(&tsdb, FDB_TSDB_CTRL_SET_SEC_SIZE, &sec_size);
        fdb_tsdb_control(&tsdb, FDB_TSDB_CTRL_SET_MAX_SIZE, &db_size);
#if IS_USED(MODULE_FLASHDB_VFS)
        /* enable file mode */
        bool file_mode = true;
        fdb_tsdb_control(&tsdb, FDB_TSDB_CTRL_SET_FILE_MODE, &file_mode);
        /* create database directory */
        int res = vfs_mkdir(TSDB_DIR, 0777);
        printf("mkdir '%s' %s\n", TSDB_DIR, resstr(res == 0 || res == -EEXIST));
        if (!(res == 0 || res == -EEXIST)) {
            return -1;
        }
#endif
        /* Time series database initialization
         *
         *       &tsdb: database object
         *       "log": database name
         * "fdb_tsdb1": The flash partition name base on FAL. Please make sure it's in FAL partition table.
         *              Please change to YOUR partition name.
         *    get_time: The get current timestamp function.
         *         128: maximum length of each log
         *   ts_locker: The locker object.
         */
        result = fdb_tsdb_init(&tsdb, "log", TSDB_DIR, get_time, 128, &ts_locker);
        /* read last saved time for simulated timestamp */
        fdb_tsdb_control(&tsdb, FDB_TSDB_CTRL_GET_LAST_TIME, &counts);

        if (result != FDB_NO_ERR) {
            return -1;
        }

        /* run TSDB sample */
        tsdb_sample(&tsdb);
    }
#endif /* FDB_USING_TSDB */

    return 0;
}
