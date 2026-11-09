/*
 * SPDX-FileCopyrightText: 2026 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Tests for the zenoh-pico package
 *
 * Two zenoh sessions in peer mode communicate over TCP on the IPv6 loopback
 * address: the first one listens, the second one connects to it.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "embUnit.h"
#include "mutex.h"
#include "ztimer.h"

#include "zenoh-pico.h"
#include "zenoh-pico/system/platform.h"

#define LOCATOR         "tcp/[::1]:7447"
#define KEYEXPR_PUB     "demo/riot/pub"
#define KEYEXPR_QUERY   "demo/riot/query"
#define PAYLOAD         "Hello from RIOT"
#define REPLY           "Reply from RIOT"
#define TIMEOUT_MS      (5000U)
/* TIME_WAIT of the connections closed by the node (2 x CONFIG_GNRC_TCP_MSL_MS) */
#define TIME_WAIT_MS    (300U)

static z_owned_session_t _listener;
static z_owned_session_t _connector;

static char _received[64];
static mutex_t _received_lock = MUTEX_INIT_LOCKED;

static void _copy_payload(const z_loaned_bytes_t *payload)
{
    z_owned_string_t str;

    z_bytes_to_string(payload, &str);
    snprintf(_received, sizeof(_received), "%.*s",
             (int)z_string_len(z_loan(str)), z_string_data(z_loan(str)));
    z_drop(z_move(str));
}

static void _data_handler(z_loaned_sample_t *sample, void *ctx)
{
    (void)ctx;
    _copy_payload(z_sample_payload(sample));
    mutex_unlock(&_received_lock);
}

static void _query_handler(z_loaned_query_t *query, void *ctx)
{
    (void)ctx;

    z_owned_bytes_t reply;

    z_bytes_from_static_str(&reply, REPLY);
    z_query_reply(query, z_query_keyexpr(query), z_move(reply), NULL);
}

static void _reply_handler(z_loaned_reply_t *reply, void *ctx)
{
    (void)ctx;
    if (z_reply_is_ok(reply)) {
        _copy_payload(z_sample_payload(z_reply_ok(reply)));
        mutex_unlock(&_received_lock);
    }
}

static int _open(z_owned_session_t *s, uint8_t key, const char *locator)
{
    z_owned_config_t config;

    z_config_default(&config);
    zp_config_insert(z_loan_mut(config), Z_CONFIG_MODE_KEY, Z_CONFIG_MODE_PEER);
    zp_config_insert(z_loan_mut(config), key, locator);
    return z_open(s, z_move(config), NULL);
}

/* elapsed times of more than 2^32 us: 71.6 minutes, the limit of a 32 bit
 * unsigned long, the clock is in milliseconds */
static void test_zenoh_pico_elapsed(void)
{
    z_clock_t epoch = MS_PER_SEC;
    z_clock_t instant = epoch + (1ULL << 32) / US_PER_MS + 5;
    uint64_t ms = instant - epoch;
    uint64_t us = ms * US_PER_MS;

    TEST_ASSERT(zp_clock_elapsed_ms_since(&instant, &epoch) == ms);
    TEST_ASSERT(zp_clock_elapsed_s_since(&instant, &epoch) == ms / MS_PER_SEC);
    TEST_ASSERT(zp_clock_elapsed_us_since(&instant, &epoch)
                == ((us > ULONG_MAX) ? ULONG_MAX : (unsigned long)us));
    /* an instant before the epoch */
    TEST_ASSERT(zp_clock_elapsed_ms_since(&epoch, &instant) == 0);
}

static void test_zenoh_pico_open(void)
{
    TEST_ASSERT_EQUAL_INT(0, _open(&_listener, Z_CONFIG_LISTEN_KEY, LOCATOR));
    TEST_ASSERT_EQUAL_INT(0, _open(&_connector, Z_CONFIG_CONNECT_KEY, LOCATOR));
}

static void test_zenoh_pico_pub_sub(void)
{
    z_owned_closure_sample_t callback;
    z_owned_subscriber_t sub;
    z_owned_publisher_t pub;
    z_view_keyexpr_t ke;

    memset(_received, 0, sizeof(_received));
    z_view_keyexpr_from_str(&ke, KEYEXPR_PUB);
    z_closure(&callback, _data_handler, NULL, NULL);
    TEST_ASSERT_EQUAL_INT(0, z_declare_subscriber(z_loan(_listener), &sub, z_loan(ke),
                                                  z_move(callback), NULL));
    TEST_ASSERT_EQUAL_INT(0, z_declare_publisher(z_loan(_connector), &pub, z_loan(ke), NULL));

    /* publish until the subscriber declaration has reached the publisher */
    int res = -1;

    for (unsigned i = 0; (i < TIMEOUT_MS / 100) && (res != 0); i++) {
        z_owned_bytes_t payload;

        z_bytes_from_static_str(&payload, PAYLOAD);
        z_publisher_put(z_loan(pub), z_move(payload), NULL);
        res = ztimer_mutex_lock_timeout(ZTIMER_MSEC, &_received_lock, 100);
    }
    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_STRING(PAYLOAD, _received);

    z_drop(z_move(pub));
    z_drop(z_move(sub));
}

static void test_zenoh_pico_query(void)
{
    z_owned_closure_query_t callback;
    z_owned_closure_reply_t reply_callback;
    z_owned_queryable_t qable;
    z_view_keyexpr_t ke;

    memset(_received, 0, sizeof(_received));
    z_view_keyexpr_from_str(&ke, KEYEXPR_QUERY);
    z_closure(&callback, _query_handler, NULL, NULL);
    TEST_ASSERT_EQUAL_INT(0, z_declare_queryable(z_loan(_listener), &qable, z_loan(ke),
                                                 z_move(callback), NULL));

    int res = -1;

    for (unsigned i = 0; (i < TIMEOUT_MS / 500) && (res != 0); i++) {
        z_closure(&reply_callback, _reply_handler, NULL, NULL);
        z_get(z_loan(_connector), z_loan(ke), "", z_move(reply_callback), NULL);
        res = ztimer_mutex_lock_timeout(ZTIMER_MSEC, &_received_lock, 500);
    }
    TEST_ASSERT_EQUAL_INT(0, res);
    TEST_ASSERT_EQUAL_STRING(REPLY, _received);

    z_drop(z_move(qable));
}

/* the listening session closes first: GNRC closes the accepted connection
 * with the listening queue, the connection outlives the queue */
static void test_zenoh_pico_close_listener_first(void)
{
    z_drop(z_move(_listener));
    z_drop(z_move(_connector));
}

static void test_zenoh_pico_reopen(void)
{
    ztimer_sleep(ZTIMER_MSEC, TIME_WAIT_MS);
    test_zenoh_pico_open();
}

static void test_zenoh_pico_close(void)
{
    z_drop(z_move(_connector));
    z_drop(z_move(_listener));
}

Test *tests_zenoh_pico(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_zenoh_pico_elapsed),
        new_TestFixture(test_zenoh_pico_open),
        new_TestFixture(test_zenoh_pico_pub_sub),
        new_TestFixture(test_zenoh_pico_query),
        new_TestFixture(test_zenoh_pico_close_listener_first),
        new_TestFixture(test_zenoh_pico_reopen),
        new_TestFixture(test_zenoh_pico_pub_sub),
        new_TestFixture(test_zenoh_pico_close),
    };

    EMB_UNIT_TESTCALLER(zenoh_pico_tests, NULL, NULL, fixtures);
    return (Test *)&zenoh_pico_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_zenoh_pico());
    TESTS_END();

    return 0;
}
