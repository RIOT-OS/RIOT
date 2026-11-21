/*
 * SPDX-FileCopyrightText: 2026 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @brief       zenoh-pico example: publications, subscriptions and queries
 *
 * The zenoh shell command opens a zenoh session over TCP, in peer mode
 * between two nodes (one listens, the other one connects to it) or in client
 * mode with a zenoh router, then publishes, subscribes and queries.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "msg.h"
#include "shell.h"

#include "zenoh-pico.h"

#define MAIN_QUEUE_SIZE     (8)
#define KEY_MAX_LEN         (64U)

static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

static z_owned_session_t _session;
static z_owned_subscriber_t _subscriber;
static z_owned_queryable_t _queryable;
static bool _opened;
static bool _subscribed;
static bool _queryable_declared;
static char _queryable_key[KEY_MAX_LEN];

static void _print_sample(const char *prefix, const z_loaned_sample_t *sample)
{
    z_view_string_t key;
    z_owned_string_t value;

    z_keyexpr_as_view_string(z_sample_keyexpr(sample), &key);
    z_bytes_to_string(z_sample_payload(sample), &value);
    printf("zenoh: %s%.*s: %.*s\n", prefix,
           (int)z_string_len(z_loan(key)), z_string_data(z_loan(key)),
           (int)z_string_len(z_loan(value)), z_string_data(z_loan(value)));
    z_drop(z_move(value));
}

/* the callbacks run in the read task of zenoh-pico */
static void _sample_handler(z_loaned_sample_t *sample, void *arg)
{
    (void)arg;
    _print_sample("", sample);
}

/* the reply is on the key of the queryable: a query with wildcards gets the
 * keys of the queryables it matches */
static void _query_handler(z_loaned_query_t *query, void *arg)
{
    (void)arg;

    z_view_keyexpr_t key;
    z_owned_bytes_t reply;
    z_owned_encoding_t encoding;
    z_query_reply_options_t options;

    z_view_keyexpr_from_str(&key, _queryable_key);
    z_bytes_from_static_str(&reply, RIOT_BOARD);
    z_query_reply_options_default(&options);
    z_encoding_clone(&encoding, z_encoding_text_plain());
    options.encoding = z_move(encoding);
    z_query_reply(query, z_loan(key), z_move(reply), &options);
}

static void _reply_handler(z_loaned_reply_t *reply, void *arg)
{
    (void)arg;

    if (z_reply_is_ok(reply)) {
        _print_sample("reply ", z_reply_ok(reply));
    }
    else {
        puts("zenoh: error reply");
    }
}

static void _reply_end(void *arg)
{
    (void)arg;
    puts("zenoh: end of the replies");
}

static int _open(const char *mode, uint8_t key, const char *locator)
{
    z_owned_config_t config;

    if (_opened) {
        puts("zenoh: a session is open, close it first");
        return 1;
    }
    z_config_default(&config);
    zp_config_insert(z_loan_mut(config), Z_CONFIG_MODE_KEY, mode);
    zp_config_insert(z_loan_mut(config), key, locator);
    if (z_open(&_session, z_move(config), NULL) < 0) {
        puts("zenoh: cannot open the session");
        return 1;
    }
    _opened = true;
    printf("zenoh: %s session %s %s\n", mode,
           (key == Z_CONFIG_LISTEN_KEY) ? "listening on" : "connected to", locator);
    return 0;
}

static void _close(void)
{
    if (_subscribed) {
        z_drop(z_move(_subscriber));
        _subscribed = false;
    }
    if (_queryable_declared) {
        z_drop(z_move(_queryable));
        _queryable_declared = false;
    }
    z_drop(z_move(_session));
    _opened = false;
    puts("zenoh: session closed");
}

/* key expressions, e.g. demo/riot/value, with wildcards for the subscriptions
 * and the queries */
static int _keyexpr(z_view_keyexpr_t *ke, const char *key)
{
    if (z_view_keyexpr_from_str(ke, key) < 0) {
        printf("zenoh: invalid key expression %s\n", key);
        return -1;
    }
    return 0;
}

/* the values are text: the encoding tells it to the other applications, e.g.
 * the REST plugin of zenohd */
static int _pub(const char *key, const char *value)
{
    z_view_keyexpr_t ke;
    z_owned_bytes_t payload;
    z_owned_encoding_t encoding;
    z_put_options_t options;

    if (_keyexpr(&ke, key) < 0) {
        return 1;
    }
    z_bytes_copy_from_str(&payload, value);
    z_put_options_default(&options);
    z_encoding_clone(&encoding, z_encoding_text_plain());
    options.encoding = z_move(encoding);
    if (z_put(z_loan(_session), z_loan(ke), z_move(payload), &options) < 0) {
        puts("zenoh: cannot publish");
        return 1;
    }
    printf("zenoh: published %s on %s\n", value, key);
    return 0;
}

static int _sub(const char *key)
{
    z_view_keyexpr_t ke;
    z_owned_closure_sample_t callback;

    if (_keyexpr(&ke, key) < 0) {
        return 1;
    }
    if (_subscribed) {
        z_drop(z_move(_subscriber));
        _subscribed = false;
    }
    z_closure(&callback, _sample_handler, NULL, NULL);
    if (z_declare_subscriber(z_loan(_session), &_subscriber, z_loan(ke),
                             z_move(callback), NULL) < 0) {
        puts("zenoh: cannot declare the subscriber");
        return 1;
    }
    _subscribed = true;
    printf("zenoh: subscribed to %s\n", key);
    return 0;
}

static int _declare_queryable(const char *key)
{
    z_view_keyexpr_t ke;
    z_owned_closure_query_t callback;

    if (_keyexpr(&ke, key) < 0) {
        return 1;
    }
    if (strlen(key) >= sizeof(_queryable_key)) {
        puts("zenoh: key expression too long");
        return 1;
    }
    if (_queryable_declared) {
        z_drop(z_move(_queryable));
        _queryable_declared = false;
    }
    strcpy(_queryable_key, key);
    z_closure(&callback, _query_handler, NULL, NULL);
    if (z_declare_queryable(z_loan(_session), &_queryable, z_loan(ke),
                            z_move(callback), NULL) < 0) {
        puts("zenoh: cannot declare the queryable");
        return 1;
    }
    _queryable_declared = true;
    printf("zenoh: queryable %s\n", key);
    return 0;
}

static int _get(const char *key)
{
    z_view_keyexpr_t ke;
    z_owned_closure_reply_t callback;

    if (_keyexpr(&ke, key) < 0) {
        return 1;
    }
    z_closure(&callback, _reply_handler, _reply_end, NULL);
    if (z_get(z_loan(_session), z_loan(ke), "", z_move(callback), NULL) < 0) {
        puts("zenoh: cannot query");
        return 1;
    }
    return 0;
}

static int _usage(void)
{
    puts("usage: zenoh listen <locator>     peer session, listening, e.g. tcp/[::]:7447\n"
         "       zenoh connect <locator>    peer session, connected to a listening peer\n"
         "       zenoh client <locator>     client session, connected to a zenoh router\n"
         "       zenoh pub <key> <value>    publish a value\n"
         "       zenoh sub <key>            subscribe, print the samples\n"
         "       zenoh queryable <key>      answer the queries with the board name\n"
         "       zenoh get <key>            query, print the replies\n"
         "       zenoh close                close the session");
    return 1;
}

static int _zenoh_cmd(int argc, char **argv)
{
    if ((argc == 3) && !strcmp(argv[1], "listen")) {
        return _open(Z_CONFIG_MODE_PEER, Z_CONFIG_LISTEN_KEY, argv[2]);
    }
    if ((argc == 3) && !strcmp(argv[1], "connect")) {
        return _open(Z_CONFIG_MODE_PEER, Z_CONFIG_CONNECT_KEY, argv[2]);
    }
    if ((argc == 3) && !strcmp(argv[1], "client")) {
        return _open(Z_CONFIG_MODE_CLIENT, Z_CONFIG_CONNECT_KEY, argv[2]);
    }
    if ((argc < 2) || (argc > 4)) {
        return _usage();
    }
    if (!_opened) {
        puts("zenoh: no session, open one first");
        return 1;
    }
    if ((argc == 2) && !strcmp(argv[1], "close")) {
        _close();
        return 0;
    }
    if ((argc == 4) && !strcmp(argv[1], "pub")) {
        return _pub(argv[2], argv[3]);
    }
    if ((argc == 3) && !strcmp(argv[1], "sub")) {
        return _sub(argv[2]);
    }
    if ((argc == 3) && !strcmp(argv[1], "queryable")) {
        return _declare_queryable(argv[2]);
    }
    if ((argc == 3) && !strcmp(argv[1], "get")) {
        return _get(argv[2]);
    }
    return _usage();
}

SHELL_COMMAND(zenoh, "zenoh sessions, publications, subscriptions and queries", _zenoh_cmd);

int main(void)
{
    /* the shell commands of GNRC (e.g. ping) receive messages in this thread */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    puts("zenoh-pico example");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
