/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test_utils/benchmark_udp.h"
#include "shell.h"

static const char *bench_server = BENCH_SERVER_DEFAULT;
static uint16_t bench_port = BENCH_PORT_DEFAULT;

int _benchmark_udp_handler(int argc, char **argv)
{
    if (argc < 2) {
        goto usage;
    }

    if (strcmp(argv[1], "start") == 0) {
        if (argc > 2) {
            bench_server = argv[2];
        }
        if (argc > 3) {
            bench_port = atoi(argv[3]);
        }
        return benchmark_udp_start(bench_server, bench_port);
    }
    if (strcmp(argv[1], "config") == 0) {
        if (argc < 3) {
            printf("server: %s\n", bench_server);
            printf("port  : %u\n", bench_port);
        } else {
            bench_server = argv[2];
        }
        if (argc > 3) {
            bench_port = atoi(argv[3]);
        }
    }
    if (strcmp(argv[1], "stop") == 0) {
        if (benchmark_udp_stop()) {
            puts("benchmark process stopped");
        } else {
            puts("no benchmark was running");
        }
        return 0;
    }

usage:
    printf("usage: %s [start|stop|config] <server> <port>\n", argv[0]);
    return -1;
}
/** @} */
