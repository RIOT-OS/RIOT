/*
 * Copyright (c) 2020, Armink, <armink.ztl@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief TSDB samples.
 *
 * Time series log (like TSDB) feature samples source file.
 *
 * TSL is time series log, the TSDB saved many TSLs.
 */

#include <flashdb.h>
#include <string.h>
#include <inttypes.h>

#ifdef FDB_USING_TSDB

#define FDB_LOG_TAG "[sample][tsdb]"

struct env_status {
    int temp;
    int humi;
};

static bool query_cb(fdb_tsl_t tsl, void *arg);
static bool query_by_time_cb(fdb_tsl_t tsl, void *arg);
static bool set_status_cb(fdb_tsl_t tsl, void *arg);
extern const char *resstr(bool condition);

void tsdb_sample(fdb_tsdb_t tsdb)
{
    struct fdb_blob blob;

    printf("======================= tsdb_sample =======================\n");

    { /* APPEND new TSL (time series log) */
        struct env_status status;

        /* append new log to TSDB */
        status.temp = 36;
        status.humi = 85;
        fdb_err_t res = fdb_tsl_append(tsdb, fdb_blob_make(&blob, &status, sizeof(status)));
        printf("append the new status.temp (%d) and status.humi (%d) %s\n", status.temp,
               status.humi, resstr(res == FDB_NO_ERR));

        status.temp = 38;
        status.humi = 90;
        res = fdb_tsl_append(tsdb, fdb_blob_make(&blob, &status, sizeof(status)));
        printf("append the new status.temp (%d) and status.humi (%d) %s\n", status.temp,
               status.humi, resstr(res == FDB_NO_ERR));
    }

    { /* QUERY the TSDB */
        /* query all TSL in TSDB by iterator */
        fdb_tsl_iter(tsdb, query_cb, tsdb);
    }

    { /* QUERY the TSDB by time */
        /* prepare query time (from 1970-01-01 00:00:00 to 2020-05-05 00:00:00) */
        struct tm tm_from = { .tm_year = 1970 - 1900, .tm_mon = 0, .tm_mday = 1,
                              .tm_hour = 0, .tm_min = 0, .tm_sec = 0 };
        struct tm tm_to = { .tm_year = 2020 - 1900, .tm_mon = 4, .tm_mday = 5,
                            .tm_hour = 0, .tm_min = 0, .tm_sec = 0 };
        time_t from_time = mktime(&tm_from), to_time = mktime(&tm_to);
        unsigned count;
        /* query all TSL in TSDB by time */
        fdb_tsl_iter_by_time(tsdb, from_time, to_time, query_by_time_cb, tsdb);
        /* query all FDB_TSL_WRITE status TSL's count in TSDB by time */
        count = fdb_tsl_query_count(tsdb, from_time, to_time, FDB_TSL_WRITE);
        printf("query count is: %u\n", count);
    }

    { /* SET the TSL status */
        /* Change the TSL status by iterator or time iterator
         * set_status_cb: the change operation will in this callback
         *
         * NOTE: The actions to modify the state must be in order.
         *       like: FDB_TSL_WRITE -> FDB_TSL_USER_STATUS1 -> FDB_TSL_DELETED
         *             -> FDB_TSL_USER_STATUS2
         *       The intermediate states can also be ignored.
         *       such as: FDB_TSL_WRITE -> FDB_TSL_DELETED
         */
        fdb_tsl_iter(tsdb, set_status_cb, tsdb);
    }

    printf("===========================================================\n");
}

static bool query_cb(fdb_tsl_t tsl, void *arg)
{
    struct fdb_blob blob;
    struct env_status status = {0};
    fdb_tsdb_t db = arg;

    size_t len = fdb_blob_read((fdb_db_t)db,
                               fdb_tsl_to_blob(tsl,
                                               fdb_blob_make(&blob, &status, sizeof(status))));
    printf("[query_cb] queried a TSL: time: %"PRId32", temp: %d, humi: %d %s\n", tsl->time,
           status.temp, status.humi, resstr(len == sizeof(status)));

    return false;
}

static bool query_by_time_cb(fdb_tsl_t tsl, void *arg)
{
    struct fdb_blob blob;
    struct env_status status = {0};
    fdb_tsdb_t db = arg;

    size_t len = fdb_blob_read((fdb_db_t)db,
                               fdb_tsl_to_blob(tsl,
                                               fdb_blob_make(&blob, &status, sizeof(status))));
    printf("[query_by_time_cb] queried a TSL: time: %"PRId32", temp: %d, humi: %d %s\n",
           tsl->time, status.temp, status.humi, resstr(len == sizeof(status)));

    return false;
}

static bool set_status_cb(fdb_tsl_t tsl, void *arg)
{
    fdb_tsdb_t db = arg;

    fdb_err_t res = fdb_tsl_set_status(db, tsl, FDB_TSL_USER_STATUS1);
    printf("set the TSL (time %"PRId32") status from %d to %d %s\n", tsl->time, tsl->status,
           FDB_TSL_USER_STATUS1, resstr(res == FDB_NO_ERR));
    return false;
}

#endif /* FDB_USING_TSDB */
