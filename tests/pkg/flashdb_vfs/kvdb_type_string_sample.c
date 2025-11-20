#/*
 * Copyright (c) 2020, Armink, <armink.ztl@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief string KV samples.
 *
 * Key-Value Database string type KV feature samples source file.
 */

#include <flashdb.h>
#include <string.h>

#ifdef FDB_USING_KVDB

#define FDB_LOG_TAG "[sample][kvdb][string]"

extern const char *resstr(bool condition);

void kvdb_type_string_sample(fdb_kvdb_t kvdb)
{
    printf("================= kvdb_type_string_sample =================\n");

    /* CREATE new Key-Value */
    char temp_data1[10] = "36C";

    /* It will create new KV node when "temp" KV not in database. */
    fdb_err_t res = fdb_kv_set(kvdb, "temp", temp_data1);
    printf("create the 'temp' string KV, value is: %s %s\n", temp_data1,
            resstr(res == FDB_NO_ERR));

    /* GET the KV value */
    char *return_value, temp_data2[10] = { 0 };

    /* Get the "temp" KV value.
     * NOTE: The return value saved in fdb_kv_get's buffer. Please copy away as soon as possible.
     */
    return_value = fdb_kv_get(kvdb, "temp");
    /* the return value is NULL when get the value failed */
    if (return_value != NULL) {
        strncpy(temp_data2, return_value, sizeof(temp_data2));
        printf("get the 'temp' value is: %s %s\n", temp_data2,
                resstr(memcmp(temp_data1, temp_data2, sizeof(temp_data1)) == 0));
    } else {
        printf("get the 'temp' value %s\n", resstr(false));
    }

    /* CHANGE the KV value */
    char temp_data3[10] = "38C";

    /* change the "temp" KV's value to "38.1" */
    res = fdb_kv_set(kvdb, "temp", temp_data3);
    printf("set 'temp' value to %s %s\n", temp_data3, resstr(res == FDB_NO_ERR));

    /* DELETE the KV by name */
    res = fdb_kv_del(kvdb, "temp");
    printf("delete the 'temp' finish %s\n", resstr(res == FDB_NO_ERR));

    printf("===========================================================\n");
}

#endif /* FDB_USING_KVDB */
