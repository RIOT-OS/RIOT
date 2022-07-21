/*
 * Copyright 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Shell command implementation for CAN stack
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "can/conn/raw.h"
#include "can/raw.h"
#include "shell.h"
#include "timex.h"

#define SC_CAN_MAX_FILTERS  10
#define xstr(a) str(a)
#define str(a) #a

static int _can_usage(void);

static int _list(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    for (int i = 0; i < CAN_DLL_NUMOF; i++) {
        const char *name = raw_can_get_name_by_ifnum(i);
        if (name) {
            printf("CAN #%d: %s\n", i, name);
        }
        else {
            break;
        }
    }

    return 0;
}

static int _send(int argc, char **argv)
{
    if (argc < 3 || argc > 11) {
        _can_usage();
        return 1;
    }

    conn_can_raw_t conn;
    struct can_frame frame;
    int ifnum = atoi(argv[1]);
    if (ifnum >= CAN_DLL_NUMOF) {
        puts("Invalid ifnum");
        return 1;
    }
    frame.can_id = strtoul(argv[2], NULL, 16);
    frame.can_dlc = argc - 3;
    for (int i = 0; i < frame.can_dlc; i++) {
        frame.data[i] = strtoul(argv[3 + i], NULL, 16);
    }
    conn_can_raw_create(&conn, NULL, 0, ifnum, 0);
    conn_can_raw_send(&conn, &frame, 0);

    return 0;
}

static int _dump(int argc, char **argv)
{
    if (argc < 4) {
        _can_usage();
        return 0;
    }

    int ret = -1;
    struct can_filter filters[SC_CAN_MAX_FILTERS];
    conn_can_raw_t conn;
    struct can_frame frame;
    int ifnum = atoi(argv[1]);
    int cnt = atoi(argv[2]);
    uint32_t ms = strtoul(argv[3], NULL, 0);
    size_t nb_filters = 1;
    if (ifnum >= CAN_DLL_NUMOF) {
        puts("Invalid ifnum");
        return 1;
    }
    if (cnt) {
        cnt++;
    }

    if (argc > 4) {
        char *p = argv[4];
        char *end;
        int i = 0;
        while (*p != '\0' && i < SC_CAN_MAX_FILTERS) {
            filters[i].can_id = strtoul(p, &end, 16);
            if (*end == ':') {
                p = end + 1;
                filters[i].can_mask = strtoul(p, &end, 16);
            }
            else {
                filters[i].can_mask = 0xffffffff;
            }
            if (*end == ',') {
                p = end + 1;
            }
            else {
                p = end;
            }
            i++;
        }
        nb_filters = i;
    }
    else {
        filters[0].can_id = 0;
        filters[0].can_mask = 0;
    }
    conn_can_raw_create(&conn, filters, nb_filters, ifnum, 0);
    while ((cnt != 1) &&
           ((ret = conn_can_raw_recv(&conn, &frame, ms * US_PER_MS))
           == sizeof(struct can_frame))) {
        printf("%-8s(%d) %8" PRIX32 "  [%x] ",
               raw_can_get_name_by_ifnum(ifnum), ifnum,
               frame.can_id, frame.can_dlc);
        for (int i = 0; i < frame.can_dlc; i++) {
            printf(" %02X", frame.data[i]);
        }
        printf("\n");
        if (cnt) {
            cnt--;
        }
    }
    if (ret < 0) {
        puts("Timeout");
    }
    puts("Closing");
    conn_can_raw_close(&conn);

    return 0;
}

static int _can_usage(void)
{
    puts("usage: can <command> [arguments]");
    puts("commands:");
    puts("\tlist");
    puts("\tsend ifnum id [B1 .. B8]");
    puts("\tdump ifnum nb ms [id1[:mask1][,id2[:mask2], .. id"
         xstr(SC_CAN_MAX_FILTERS) ":[mask" xstr(SC_CAN_MAX_FILTERS) "]]");
    return 0;
}

static int _can_handler(int argc, char **argv)
{
    if (argc < 2) {
        _can_usage();
        return 1;
    }
    else if (strncmp(argv[1], "list", 5) == 0) {
        return _list(argc - 1, argv + 1);
    }
    else if (strncmp(argv[1], "send", 5) == 0) {
        return _send(argc - 1, argv + 1);
    }
    else if (strncmp(argv[1], "dump", 5) == 0) {
        return _dump(argc - 1, argv + 1);
    }
    else {
        printf("unknown command: %s\n", argv[1]);
        return 1;
    }
    return 0;
}

SHELL_COMMAND(can, "CAN commands", _can_handler);
