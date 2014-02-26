/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup examples
 * @{
 *
 * @file
 * @brief CCN Lite interactive shell example application
 *
 * @author Christian Mehlis <mehlis@inf.fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#include "msg.h"
#include "thread.h"
#include "posix_io.h"
#include "shell.h"
#include "board_uart0.h"
#include "transceiver.h"
#include "rtc.h"
#include "ps.h"
#include "ltc4150.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

#include "ccn_lite/ccnl-riot.h"
#include "ccn_lite/util/ccnl-riot-client.h"

#define RIOT_CCN_APPSERVER (1)
#define RIOT_CCN_TESTS (0)
#define CCNL_DEFAULT_MAX_CACHE_ENTRIES  0   // means: no content caching
#define CCNL_DEFAULT_THRESHOLD_PREFIX   1
#define CCNL_DEFAULT_THRESHOLD_AGGREGATE 2

char relay_stack[KERNEL_CONF_STACKSIZE_PRINTF];

#if RIOT_CCN_APPSERVER
char appserver_stack[KERNEL_CONF_STACKSIZE_PRINTF];
#endif
int relay_pid, appserver_pid;

int shell_max_cache_entries, shell_threshold_prefix, shell_threshold_aggregate;

#define SHELL_MSG_BUFFER_SIZE (64)
msg_t msg_buffer_shell[SHELL_MSG_BUFFER_SIZE];

shell_t shell;

unsigned char big_buf[3 * 1024];
char small_buf[PAYLOAD_SIZE];

#if RIOT_CCN_APPSERVER
static void appserver_thread(void)
{
    ccnl_riot_appserver_start(relay_pid);
}

static void riot_ccn_appserver(int argc, char **argv)
{
    (void) argc; /* the function takes no arguments */
    (void) argv;

    if (appserver_pid) {
        /* already running */
        return;
    }

    appserver_pid = thread_create(appserver_stack, KERNEL_CONF_STACKSIZE_PRINTF, PRIORITY_MAIN - 1, CREATE_STACKTEST, appserver_thread, "appserver");
    DEBUG("ccn-lite appserver on thread_id %d...\n", appserver_pid);
}
#endif

static void riot_ccn_express_interest(int argc, char **argv)
{
    static const char *default_interest = "/ccnx/0.7.1/doc/technical/CanonicalOrder.txt";

    if (argc < 2) {
        strncpy(small_buf, default_interest, 100); // null terminated
    }
    else {
        strncpy(small_buf, argv[1], 100);
    }

    DEBUG("in='%s'\n", small_buf);

    int content_len = ccnl_riot_client_get(relay_pid, small_buf, (char *) big_buf); // small_buf=name to request

    if (content_len == 0) {
        puts("riot_get returned 0 bytes...aborting!");
        return;
    }

    puts("####################################################");
    big_buf[content_len] = '\0';
    printf("data='%s'\n", big_buf);
    puts("####################################################");
    puts("done");
}

static void riot_ccn_register_prefix(int argc, char **argv)
{
    if (argc < 4) {
        puts("enter: prefix </path/to/abc> <type> <faceid>");
        return;
    }

    strncpy(small_buf, argv[1], 100);
    DEBUG("prefix='%s'\n", small_buf);

    char *type = argv[2];
    char *faceid = argv[3]; // 0=trans;1=msg

    int content_len = ccnl_riot_client_publish(relay_pid, small_buf, faceid, type, big_buf);

    DEBUG("shell received: '%s'\n", big_buf);
    DEBUG("received %d bytes.\n", content_len);
    puts("done");
}

static void relay_thread(void)
{
    ccnl_riot_relay_start(shell_max_cache_entries, shell_threshold_prefix, shell_threshold_aggregate);
}

static void riot_ccn_relay_start(int argc, char **argv)
{
    if (relay_pid) {
        /* already running */
        return;
    }

    if (argc < 2) {
        shell_max_cache_entries = CCNL_DEFAULT_MAX_CACHE_ENTRIES;
    }
    else {
        shell_max_cache_entries = atoi(argv[1]);
    }

    if (argc < 3) {
        shell_threshold_prefix = CCNL_DEFAULT_THRESHOLD_PREFIX;
    }
    else {
        shell_threshold_prefix = atoi(argv[2]);
    }

    if (argc < 4) {
        shell_threshold_aggregate = CCNL_DEFAULT_THRESHOLD_AGGREGATE;
    }
    else {
        shell_threshold_aggregate = atoi(argv[3]);
    }

    relay_pid = thread_create(relay_stack, KERNEL_CONF_STACKSIZE_PRINTF, PRIORITY_MAIN - 2, CREATE_STACKTEST, relay_thread, "relay");
    DEBUG("ccn-lite relay on thread_id %d...\n", relay_pid);
}

static void riot_ccn_relay_stop(int argc, char **argv)
{
    (void) argc; /* the function takes no arguments */
    (void) argv;

    msg_t m;
    m.content.value = 0;
    m.type = CCNL_RIOT_HALT;
    msg_send(&m, relay_pid, 1);

    /* mark relay as not running */
    relay_pid = 0;
}

#if RIOT_CCN_TESTS
static void riot_ccn_pit_test(int argc, char **argv)
{
    (void) argc; /* the function takes no arguments */
    (void) argv;

    char name[] = "/riot/test";

    char *prefix[CCNL_MAX_NAME_COMP];
    char *cp = strtok(name, "/");
    int i = 0;

    while (i < (CCNL_MAX_NAME_COMP - 1) && cp) {
        prefix[i++] = cp;
        cp = strtok(NULL, "/");
    }

    //prefix[i] = 0; //segment to request
    prefix[i + 1] = 0;

    msg_t m;
    riot_ccnl_msg_t rmsg;
    char segment_string[16]; //max=999\0
    struct timeval now;

    int segment;

    for (segment = 0; segment < 200; segment++) {
        memset(segment_string, 0, 16);
        snprintf(segment_string, 16, "%d", segment);
        prefix[i] = segment_string;
        unsigned int interest_nonce = genrand_uint32();
        int interest_len = mkInterest(prefix, &interest_nonce, (unsigned char *) small_buf);

        rmsg.payload = &small_buf;
        rmsg.size = interest_len;
        m.content.ptr = (char *) &rmsg;
        m.type = CCNL_RIOT_MSG;

        msg_send(&m, relay_pid, 1);

        if ((segment % 50) == 0) {
            rtc_time(&now);
            printf("done: %d - %ld.%ld\n", segment, now.tv_sec, now.tv_usec);
        }
    }

    printf("done: tried to send %d interests\n", segment);
}

static void riot_ccn_fib_test(int argc, char **argv)
{
    (void) argc; /* the function takes no arguments */
    (void) argv;

    char type[] = "newTRANSface";
    char faceid[] = "42";

    riot_new_face(relay_pid, type, faceid, big_buf);

    struct timeval now;
    int i = -1;

    do {
        i++;
        snprintf(small_buf, sizeof(small_buf), "/riot/test/fib/%d/", i);
        riot_register_prefix(relay_pid, small_buf, faceid, big_buf);

        if (i % 50 == 0) {
            rtc_time(&now);
            printf("done: %d - %ld.%ld\n", i, now.tv_sec, now.tv_usec);
        }
    }
    while (0 == strcmp((const char *) big_buf, "prefixreg cmd worked"));

    DEBUG("%d: '%s'\n", i, big_buf);
    printf("done: %d\n", i - 1);
}
#endif

static void riot_ccn_populate(int argc, char **argv)
{
    (void) argc; /* the function takes no arguments */
    (void) argv;

    msg_t m;
    m.content.value = 0;
    m.type = CCNL_RIOT_POPULATE;
    msg_send(&m, relay_pid, 1);
}

static void riot_ccn_stat(int argc, char **argv)
{
    (void) argc; /* the function takes no arguments */
    (void) argv;

    msg_t m;
    m.content.value = 0;
    m.type = CCNL_RIOT_PRINT_STAT;
    msg_send(&m, relay_pid, 1);
}

static const shell_command_t sc[] = {
    { "ccn", "starts ccn relay", riot_ccn_relay_start },
    { "haltccn", "stops ccn relay", riot_ccn_relay_stop },
    { "interest", "express an interest", riot_ccn_express_interest },
    { "populate", "populate the cache of the relay with data", riot_ccn_populate },
    { "prefix", "registers a prefix to a face", riot_ccn_register_prefix },
    { "stat", "prints out forwarding statistics", riot_ccn_stat },
#if RIOT_CCN_APPSERVER
    { "appserver", "starts an application server to reply to interests", riot_ccn_appserver },
#endif
#if RIOT_CCN_TESTS
    { "pittest", "starts a test for the size and speed of pit operations", riot_ccn_pit_test },
    { "fibtest", "starts a test for the size and speed of fib operations", riot_ccn_fib_test },
#endif
    { NULL, NULL, NULL }
};

void riot_ccn_runner(void)
{
    if (msg_init_queue(msg_buffer_shell, SHELL_MSG_BUFFER_SIZE) != 0) {
        DEBUG("msg init queue failed...abording\n");
        return;
    }

    puts("posix open");
    posix_open(uart0_handler_pid, 0);
    puts("shell init");
    shell_init(&shell, sc, UART0_BUFSIZE, uart0_readc, uart0_putc);
    puts("shell run");
    shell_run(&shell);
}

int main(void)
{
    puts("CCN!");

    puts("starting shell...");
    riot_ccn_runner();

    return 0;
}
