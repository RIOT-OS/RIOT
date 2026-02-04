/*
* SPDX-FileCopyrightText: 2020 Armink <armink.ztl@gmail.com>
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

void kvdb_type_string_sample(fdb_kvdb_t kvdb)
{
    printf("================= kvdb_type_string_sample =================\n");

    { /* CREATE new Key-Value */
        char temp_data[10] = "36C";

        /* It will create new KV node when "temp" KV not in database. */
        fdb_kv_set(kvdb, "temp", temp_data);
        printf("create the 'temp' string KV, value is: %s\n", temp_data);
    }

    { /* GET the KV value */
        char *return_value, temp_data[10] = { 0 };

        /* Get the "temp" KV value.
         * NOTE: The return value saved in fdb_kv_get's buffer. Please copy away as soon as possible.
         */
        return_value = fdb_kv_get(kvdb, "temp");
        /* the return value is NULL when get the value failed */
        if (return_value != NULL) {
            strncpy(temp_data, return_value, sizeof(temp_data));
            printf("get the 'temp' value is: %s\n", temp_data);
        }
    }

    { /* CHANGE the KV value */
        char temp_data[10] = "38C";

        /* change the "temp" KV's value to "38.1" */
        fdb_kv_set(kvdb, "temp", temp_data);
        printf("set 'temp' value to %s\n", temp_data);
    }

    { /* DELETE the KV by name */
        fdb_kv_del(kvdb, "temp");
        printf("delete the 'temp' finish\n");
    }

    printf("===========================================================\n");
}

#endif /* FDB_USING_KVDB */
