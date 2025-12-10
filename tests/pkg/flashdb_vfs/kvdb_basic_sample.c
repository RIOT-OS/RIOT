/*
 * SPDX-FileCopyrightText: 2020 Armink <armink.ztl@gmail.com>
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief basic KV samples.
 *
 * basic Key-Value Database KV feature samples
 * get and show current boot counts
 */

#include <flashdb.h>

#ifdef FDB_USING_KVDB

#define FDB_LOG_TAG "[sample][kvdb][basic]"

void kvdb_basic_sample(fdb_kvdb_t kvdb)
{
    struct fdb_blob blob;
    int boot_count = 0;

    printf("==================== kvdb_basic_sample ====================\n");

    { /* GET the KV value */
        /* get the "boot_count" KV value */
        fdb_kv_get_blob(kvdb, "boot_count", fdb_blob_make(&blob, &boot_count, sizeof(boot_count)));
        /* the blob.saved.len is more than 0 when get the value successful */
        if (blob.saved.len > 0) {
            printf("get the 'boot_count' value is %d\n", boot_count);
        } else {
            printf("get the 'boot_count' failed\n");
        }
    }

    { /* CHANGE the KV value */
        /* increase the boot count */
        boot_count ++;
        /* change the "boot_count" KV's value */
        fdb_kv_set_blob(kvdb, "boot_count", fdb_blob_make(&blob, &boot_count, sizeof(boot_count)));
        printf("set the 'boot_count' value to %d\n", boot_count);
    }

    printf("===========================================================\n");
}

#endif /* FDB_USING_KVDB */
