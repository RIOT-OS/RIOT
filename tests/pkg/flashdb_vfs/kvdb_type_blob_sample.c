/*
 * Copyright (c) 2020, Armink, <armink.ztl@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief blob KV samples.
 *
 * Key-Value Database blob type KV feature samples
 */

#include <flashdb.h>

#ifdef FDB_USING_KVDB

#define FDB_LOG_TAG "[sample][kvdb][blob]"

void kvdb_type_blob_sample(fdb_kvdb_t kvdb)
{
    struct fdb_blob blob;

    printf("================== kvdb_type_blob_sample ==================\n");

    { /* CREATE new Key-Value */
        int temp_data = 36;

        /* It will create new KV node when "temp" KV not in database.
         * fdb_blob_make: It's a blob make function, and it will return the blob when make finish.
         */
        fdb_kv_set_blob(kvdb, "temp", fdb_blob_make(&blob, &temp_data, sizeof(temp_data)));
        printf("create the 'temp' blob KV, value is: %d\n", temp_data);
    }

    { /* GET the KV value */
        int temp_data = 0;

        /* get the "temp" KV value */
        fdb_kv_get_blob(kvdb, "temp", fdb_blob_make(&blob, &temp_data, sizeof(temp_data)));
        /* the blob.saved.len is more than 0 when get the value successful */
        if (blob.saved.len > 0) {
            printf("get the 'temp' value is: %d\n", temp_data);
        }
    }

    { /* CHANGE the KV value */
        int temp_data = 38;

        /* change the "temp" KV's value to 38 */
        fdb_kv_set_blob(kvdb, "temp", fdb_blob_make(&blob, &temp_data, sizeof(temp_data)));
        printf("set 'temp' value to %d\n", temp_data);
    }

    { /* DELETE the KV by name */
        fdb_kv_del(kvdb, "temp");
        printf("delete the 'temp' finish\n");
    }

    printf("===========================================================\n");
}

#endif /* FDB_USING_KVDB */
