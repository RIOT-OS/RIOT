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

extern const char *resstr(bool condition);

void kvdb_type_blob_sample(fdb_kvdb_t kvdb)
{
    struct fdb_blob blob;

    printf("================== kvdb_type_blob_sample ==================\n");

    /* CREATE new Key-Value */
    int temp_data1 = 36;

    /* It will create new KV node when "temp" KV not in database.
     * fdb_blob_make: It's a blob make function, and it will return the blob when make finish.
     */
    fdb_err_t res = fdb_kv_set_blob(kvdb, "temp",
                                    fdb_blob_make(&blob, &temp_data1, sizeof(temp_data1)));
    printf("create the 'temp' blob KV, value is: %d %s\n", temp_data1,
            resstr(res == FDB_NO_ERR));

    /* GET the KV value */
    int temp_data2 = 0;

    /* get the "temp" KV value */
    size_t len = fdb_kv_get_blob(kvdb, "temp",
                                 fdb_blob_make(&blob, &temp_data2, sizeof(temp_data2)));
    printf("get the 'temp' value is: %d %s\n", temp_data2,
            resstr(len == sizeof(temp_data2) && temp_data1 == temp_data2));

    /* CHANGE the KV value */
    int temp_data = 38;

    /* change the "temp" KV's value to 38 */
    res = fdb_kv_set_blob(kvdb, "temp", fdb_blob_make(&blob, &temp_data, sizeof(temp_data)));
    printf("set 'temp' value to %d %s\n", temp_data, resstr(res == FDB_NO_ERR));

    /* DELETE the KV by name */
    res = fdb_kv_del(kvdb, "temp");
    printf("delete the 'temp' finish %s\n", resstr(res == FDB_NO_ERR));

    printf("===========================================================\n");
}

#endif /* FDB_USING_KVDB */
