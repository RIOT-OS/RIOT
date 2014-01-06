/**
 * RIOT CCN SHELL
 *
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * \{
 * \file riot_ccn.c
 * \author Christian Mehlis <mehlis@inf.fu-berlin.de>
 * \}
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

static void riot_ccn_appserver(char *str)
{
    (void) str; /* unused */

    if (appserver_pid) {
        /* already running */
        return;
    }

    appserver_pid = thread_create(appserver_stack, KERNEL_CONF_STACKSIZE_PRINTF, PRIORITY_MAIN - 1, CREATE_STACKTEST, appserver_thread, "appserver");
    DEBUG("ccn-lite appserver on thread_id %d...\n", appserver_pid);
}
#endif

static void riot_ccn_express_interest(char *str)
{
    char *given_interest = strtok(str, " "); /* str=interest, skip that */
    given_interest = strtok(NULL, " ");
    static const char *default_interest = "/ccnx/0.7.1/doc/technical/CanonicalOrder.txt";

    if (!given_interest) {
        strncpy(small_buf, default_interest, 100); // null terminated
    }
    else {
        strncpy(small_buf, given_interest, 100);
    }

    DEBUG("in='%s'\n", small_buf);

    ltc4150_start();
    int content_len = ccnl_riot_client_get(relay_pid, small_buf, (char *) big_buf); // small_buf=name to request
    ltc4150_stop();
    printf("mAh=%f\n", ltc4150_get_total_mAh());

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

static void riot_ccn_register_prefix(char *str)
{
    char *given_prefix = strtok(str, " ");
    given_prefix = strtok(NULL, " ");
    static const char *default_prefix = "/ccnx/0.7.1/doc/technical";

    if (!given_prefix) {
        strncpy(small_buf, default_prefix, 100);
    }
    else {
        strncpy(small_buf, given_prefix, 100);
    }

    DEBUG("prefix='%s'\n", small_buf);

    char *type = strtok(NULL, " ");

    if (!type) {
        puts("enter: prefix /path/to/abc <type> faceid");
        return;
    }

    char *faceid = strtok(NULL, " ");//"2"; // 0=trans;1=msg

    if (!faceid) {
        puts("enter: prefix /path/to/abc <type> faceid");
        return;
    }

    int content_len = ccnl_riot_client_publish(relay_pid, small_buf, faceid, type, big_buf);

    DEBUG("shell received: '%s'\n", big_buf);
    DEBUG("received %d bytes.\n", content_len);
    puts("done");
}

static void relay_thread(void)
{
    ccnl_riot_relay_start(shell_max_cache_entries, shell_threshold_prefix, shell_threshold_aggregate);
}

static void riot_ccn_relay_start(char *str)
{
    if (relay_pid) {
        /* already running */
        return;
    }

    char *toc_str = strtok(str, " ");

    toc_str = strtok(NULL, " ");
    if (!toc_str) {
        shell_max_cache_entries = CCNL_DEFAULT_MAX_CACHE_ENTRIES;
    } else {
        shell_max_cache_entries = atoi(toc_str);
    }

    toc_str = strtok(NULL, " ");
    if (!toc_str) {
        shell_threshold_prefix = CCNL_DEFAULT_THRESHOLD_PREFIX;
    } else {
        shell_threshold_prefix = atoi(toc_str);
    }

    toc_str = strtok(NULL, " ");
    if (!toc_str) {
        shell_threshold_aggregate = CCNL_DEFAULT_THRESHOLD_AGGREGATE;
    } else {
        shell_threshold_aggregate = atoi(toc_str);
    }

    relay_pid = thread_create(relay_stack, KERNEL_CONF_STACKSIZE_PRINTF, PRIORITY_MAIN - 2, CREATE_STACKTEST, relay_thread, "relay");
    DEBUG("ccn-lite relay on thread_id %d...\n", relay_pid);
}

static void riot_ccn_relay_stop(char *str)
{
    (void) str; /* unused */

    msg_t m;
    m.content.value = 0;
    m.type = CCNL_RIOT_HALT;
    msg_send(&m, relay_pid, 1);

    /* mark relay as not running */
    relay_pid = 0;
}

#if RIOT_CCN_TESTS
static void riot_ccn_pit_test(char *str)
{
    (void) str; /* unused */

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
        int interest_len = mkInterest(prefix, NULL, (unsigned char *) small_buf);

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

static void riot_ccn_fib_test(char *str)
{
    (void) str; /* unused */

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

static void riot_ccn_populate(char *str)
{
    (void) str; /* unused */

    msg_t m;
    m.content.value = 0;
    m.type = CCNL_RIOT_POPULATE;
    msg_send(&m, relay_pid, 1);
}

static void riot_ccn_stat(char *str)
{
    (void) str; /* unused */

    msg_t m;
    m.content.value = 0;
    m.type = CCNL_RIOT_PRINT_STAT;
    msg_send(&m, relay_pid, 1);
}

static void riot_ccn_columb(char *str)
{
    (void) str; /* unused */

    ltc4150_start();
}

static void riot_ccn_columb_stop(char *str)
{
    (void) str; /* unused */

    ltc4150_stop();
    printf("mAh=%lf\n", ltc4150_get_total_mAh());
}

static const shell_command_t sc[] = {
    { "ccn", "starts ccn relay", riot_ccn_relay_start },
    { "haltccn", "stops ccn relay", riot_ccn_relay_stop },
    { "interest", "express an interest", riot_ccn_express_interest },
    { "populate", "populate the cache of the relay with data", riot_ccn_populate },
    { "prefix", "registers a prefix to a face", riot_ccn_register_prefix },
    { "stat", "prints out forwarding statistics", riot_ccn_stat },
    { "columb", "starts the columb counter", riot_ccn_columb },
    { "columbstop", "stops the columb counter", riot_ccn_columb_stop },
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
