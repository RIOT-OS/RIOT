/*
 * Copyright (C) 2023 ML!PA Consulting Gmbh
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_configuration
 * @{
 *
 * @file
 * @brief       Implementation of the FlashDB configuration backend
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>

#include "configuration.h"
#include "configuration_backend_flashdb.h"
#include "macros/math.h"
#include "macros/units.h"
#include "mutex.h"
#include "mtd_default.h"
#include "vfs_default.h"
#include "fal_cfg.h"

static mutex_t _kvdb_locker = MUTEX_INIT;
static struct fdb_kvdb _kvdb = { 0 };

__attribute__((weak))
mtd_dev_t *configuration_backend_flashdb_mtd_choose_dev(void)
{
    return FAL_MTD;
}

__attribute__((weak))
const char *configuration_backend_flashdb_mtd_choose_partition(void)
{
    return FAL_PART0_LABEL;
}

__attribute__((weak))
mtd_dev_t *configuration_backend_flashdb_vfs_choose_dev(void)
{
    return mtd_default_get_dev(0);
}

__attribute__((weak))
const char *configuration_backend_flashdb_vfs_choose_path(void)
{
    return VFS_DEFAULT_DATA"/"CONFIGURATION_FLASHDB_VFS_FOLDER;
}

static void _lock(fdb_db_t db)
{
    mutex_lock(db->user_data);
}

static void _unlock(fdb_db_t db)
{
    mutex_unlock(db->user_data);
}

static int _be_fdb_init(mtd_dev_t *mtd)
{
    assert(mtd);
    uint32_t size;
#if IS_USED(MODULE_CONFIGURATION_BACKEND_FLASHDB_VFS)
    bool file_mode = true;
    fdb_kvdb_control(&_kvdb, FDB_KVDB_CTRL_SET_FILE_MODE, &file_mode);
    int fail;
    if ((fail = vfs_mkdir(configuration_backend_flashdb_vfs_choose_path(), 0777)) < 0) {
        if (fail != -EEXIST) {
            /* probably not mounted (try with vfs_auto_format) */
            return fail;
        }
    }
#endif
    /* The MTD must be initialized! */
    size = mtd->pages_per_sector * mtd->page_size;
    size = DIV_ROUND_UP((CONFIGURATION_FLASHDB_MIN_SECTOR_SIZE_DEFAULT_KiB * KiB(1)), size) * size;
    /* sector size must be set before max size */
    fdb_kvdb_control(&_kvdb, FDB_KVDB_CTRL_SET_SEC_SIZE, &size);
#if IS_USED(MODULE_CONFIGURATION_BACKEND_FLASHDB_VFS)
    size *= CONFIGURATION_FLASHDB_VFS_MAX_SECTORS;
    fdb_kvdb_control(&_kvdb, FDB_KVDB_CTRL_SET_MAX_SIZE, &size);
#endif
    fdb_kvdb_control(&_kvdb, FDB_KVDB_CTRL_SET_LOCK, (void *)(uintptr_t)_lock);
    fdb_kvdb_control(&_kvdb, FDB_KVDB_CTRL_SET_UNLOCK, (void *)(uintptr_t)_unlock);
    if (fdb_kvdb_init(&_kvdb,
                      "kvdb_configuration",
#if IS_USED(MODULE_CONFIGURATION_BACKEND_FLASHDB_VFS)
                      configuration_backend_flashdb_vfs_choose_path(),
#elif IS_USED(MODULE_CONFIGURATION_BACKEND_FLASHDB_MTD)
                      configuration_backend_flashdb_mtd_choose_partition(),
#endif
                      NULL,
                      &_kvdb_locker) != FDB_NO_ERR) {
        return -EINVAL;
    }
    return 0;
}

static int _be_fdb_reset(void) {
    fdb_err_t result;
    if ((result = fdb_kv_set_default(&_kvdb)) != FDB_NO_ERR) {
        return -EIO;
    }
    return 0;
}

static int _be_fdb_load(const struct conf_backend *be,
                        conf_key_buf_t *key, void *val, size_t *size,
                        size_t offset, conf_backend_flags_t *flg)
{
    (void)be;
    (void)flg;

    if (offset) {
        /* reading partial records is not supported */
        return -ENOTSUP;
    }

    struct fdb_blob blob;
    size_t sz;
    if ((sz = fdb_kv_get_blob(&_kvdb, configuration_key_buf(key),
                              fdb_blob_make(&blob, val, *size))) <= 0) {
        return -EIO;
    }
    if (!blob.saved.len) {
        return -ENODATA;
    }
    assert(*size >= sz);
    *size -= sz;
    return 0;

}

static int _be_fdb_store(const struct conf_backend *be,
                         conf_key_buf_t *key, const void *val, size_t *size,
                         size_t offset, conf_backend_flags_t *flg)
{
    (void)be;
    (void)flg;

    if (offset) {
        /* writing partial records is not supported */
        return -ENOTSUP;
    }

    struct fdb_blob blob;
    fdb_err_t err;
    if ((err = fdb_kv_set_blob(&_kvdb, configuration_key_buf(key),
                               fdb_blob_make(&blob, val, *size))) != FDB_NO_ERR) {
        return -EIO;
    }
    if (!blob.saved.len) {
        return -ENODATA;
    }
    return 0;
}

static int _be_fdb_delete(const struct conf_backend *be, conf_key_buf_t *key)
{
    (void)be;
    fdb_err_t err;
    if ((err = fdb_kv_del(&_kvdb, configuration_key_buf(key))) != FDB_NO_ERR &&
                                                           err != FDB_KV_NAME_ERR) {
        return -EIO;
    }
    return 0;
}

const conf_backend_ops_t conf_backend_fdb_ops = {
    .be_load = _be_fdb_load,
    .be_store = _be_fdb_store,
    .be_delete = _be_fdb_delete,
};

int configuration_backend_flashdb_reset(void)
{
    return _be_fdb_reset();
}

int configuration_backend_flashdb_init(mtd_dev_t *mtd)
{
    return _be_fdb_init(mtd);
}

void auto_init_configuration_backend_flashdb(void)
{
#if IS_USED(MODULE_CONFIGURATION_BACKEND_FLASHDB_MTD)
    extern void fdb_mtd_init(mtd_dev_t *mtd);
    fdb_mtd_init(configuration_backend_flashdb_mtd_choose_dev());
    int fail = configuration_backend_flashdb_init(configuration_backend_flashdb_mtd_choose_dev());
#else
    int fail = configuration_backend_flashdb_init(configuration_backend_flashdb_vfs_choose_dev());
#endif
    assert(!fail); (void)fail;
#if (IS_USED(MODULE_CONFIGURATION_BACKEND_RESET_FLASHDB))
    fail = configuration_backend_flashdb_reset();
    assert(!fail);
#endif
}

#ifndef AUTO_INIT_PRIO_MOD_CONFIGURATION_BACKEND_FLASHDB
#define AUTO_INIT_PRIO_MOD_CONFIGURATION_BACKEND_FLASHDB    1010
#endif

AUTO_INIT_CONFIGURATION(auto_init_configuration_backend_flashdb,
                        AUTO_INIT_PRIO_MOD_CONFIGURATION_BACKEND_FLASHDB);
