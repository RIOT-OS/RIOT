/*
 * Shell commands for l2_ping module
 *
 * Copyright (C) 2014, INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 *
 * @ingroup shell_commands
 * @{
 * @file
 * @brief   provides shell commands use link layer ping functionality
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "l2_ping.h"
#include "transceiver.h"
#include "timex.h"
#include "vtimer.h"

void _l2_ping_req_handler(int argc, char **argv)
{
    size_t payload_strlen;
    uint16_t count = 5;
    timex_t start, end, period;

    if (transceiver_pid == KERNEL_PID_UNDEF) {
        puts("Transceiver not initialized");
        return;
    }
    if (argc < 2) {
        printf("Usage:\t%s <ADDR> [COUNT] [MSG]\n", argv[0]);
        return;
    }

    char l2_payload[L2_PING_PAYLOAD_SIZE];
    if (argc > 3) {
        payload_strlen = strlen(argv[3]);
        if (payload_strlen > L2_PING_PAYLOAD_SIZE) {
            printf("[l2_ping] Your input is too long and will be truncated to \"%.*s\".\n", L2_PING_PAYLOAD_SIZE, argv[3]);
            payload_strlen = L2_PING_PAYLOAD_SIZE;
        }
        memset(l2_payload, 0, L2_PING_PAYLOAD_SIZE);
        strncpy(l2_payload, argv[3], payload_strlen);
    }
    else {
        payload_strlen = 0;
    }

    if (argc > 2) {
        count = atoi(argv[2]);
    }

    printf("[l2_ping] Send %" PRIu8 " ping requests to %" PRIu16 " with interval %" PRIu32 ".%" PRIu32 "s and payload %s\n",
            count, atoi(argv[1]),
            timex_from_uint64(L2_PING_DEFAULT_INTERVAL).seconds,
            timex_from_uint64(L2_PING_DEFAULT_INTERVAL).microseconds,
            (argc > 3) ? l2_payload : "NULL");
    vtimer_now(&start);
    l2_ping((radio_address_t) atoi(argv[1]), count, L2_PING_DEFAULT_INTERVAL,
            l2_payload, payload_strlen, 0);
    vtimer_now(&end);
    period = timex_sub(end, start);

    printf("  --- ping statistics for host %" PRIu16 " ---\n", l2_ping_stats.dst);
    printf("  %" PRIu16 " packets transmitted, %" PRIu16 " received, %" PRIu16 "%% packet loss, time %" PRIu32 ".%06" PRIu32 "s\n",
           l2_ping_stats.ping_count,
           l2_ping_stats.pong_count,
           100 - ((l2_ping_stats.pong_count * 100) / l2_ping_stats.ping_count),
           period.seconds,
           period.microseconds);
    printf("  rtt min/avg/max = %" PRIu32 ".%06" PRIu32 "/%" PRIu32 ".%06" PRIu32 "/%" PRIu32 ".%06" PRIu32 " s\n",
            l2_ping_stats.min_rtt.seconds, l2_ping_stats.min_rtt.microseconds,
            l2_ping_stats.avg_rtt.seconds, l2_ping_stats.avg_rtt.microseconds,
            l2_ping_stats.max_rtt.seconds, l2_ping_stats.max_rtt.microseconds);
}

void _l2_ping_probe_handler(int argc, char **argv)
{
    size_t payload_strlen;
    uint16_t count = 5;
    timex_t start, end, period;

    if (transceiver_pid == KERNEL_PID_UNDEF) {
        puts("Transceiver not initialized");
        return;
    }
    if (argc < 2) {
        printf("Usage:\t%s <ADDR> [COUNT] [MSG]\n", argv[0]);
        return;
    }

    char l2_payload[L2_PING_PAYLOAD_SIZE];
    if (argc > 3) {
        payload_strlen = strlen(argv[3]);
        if (payload_strlen > L2_PING_PAYLOAD_SIZE) {
            printf("[l2_ping] Your input is too long and will be truncated to \"%.*s\".\n", L2_PING_PAYLOAD_SIZE, argv[3]);
            payload_strlen = L2_PING_PAYLOAD_SIZE;
        }
        memset(l2_payload, 0, L2_PING_PAYLOAD_SIZE);
        strncpy(l2_payload, argv[3], payload_strlen);
    }
    else {
        payload_strlen = 0;
    }

    if (argc > 2) {
        count = atoi(argv[2]);
    }

    printf("[l2_ping] Send %" PRIu16 " probes to %" PRIu16 " with interval %u  and payload %s\n",
            count, atoi(argv[1]), L2_PING_DEFAULT_INTERVAL, (argc > 3) ? l2_payload : "NULL");
    vtimer_now(&start);
    l2_ping((radio_address_t) atoi(argv[1]), count, L2_PING_DEFAULT_INTERVAL,
            l2_payload, payload_strlen, 1);
    vtimer_now(&end);
    period = timex_sub(end, start);

    printf("  --- ping statistics for host %" PRIu16 " ---\n", l2_ping_stats.dst);
    printf("  %" PRIu16 " packets transmitted in %" PRIu32 ".%06" PRIu32 "s\n", l2_ping_stats.ping_count,
            period.seconds, period.microseconds);
}

void _l2_ping_get_probe_handler(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    l2_probe_stat_entry_t *stats;
    uint16_t count;

    l2_probe_stats(&stats, &count);

    printf("[l2_ping] Getting link layer probe statistics:\n");
    for (uint16_t i = 0; i < count; i++) {
        printf("...received %" PRIu16 " probes from node %" PRIu16 ".\n", stats[i].count, stats[i].src);
    }
}
