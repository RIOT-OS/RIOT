/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief Test Utils Result Output test application
 *
 * @author Kevin Weiss <kevin.weiss@haw-hamburg.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>

#include "kernel_defines.h"
#include "test_utils/result_output.h"
#include "shell.h"

#define _BUF_COUNT  4

turo_t ctx;

static int _sc_arg2long(const char *arg, long *val)
{
    errno = 0;
    char *end;
    long res = strtol(arg, &end, 0);

    if ((*end != '\0') || ((res == LONG_MIN || res == LONG_MAX) && errno == ERANGE)) {
        return -1;
    }
    *val = res;
    return 0;
}

/* We need to disable warning since long can mean different things */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
static int _sc_arg2s32(const char *arg, int32_t *val)
{
    long lval;
    int res = _sc_arg2long(arg, &lval);

    if (res == 0) {
        if (lval <= 2147483647 && lval >= -2147483648) {
            *val = (int32_t)lval;
        }
        else {
            res = -1;
        }

    }
    return res;
}
#pragma GCC diagnostic pop

static int _sc_arg2u8(const char *arg, uint8_t *val)
{
    long lval;
    int res = _sc_arg2long(arg, &lval);
    if (res == 0) {
        if (lval <= 255 && lval >= 0) {
            *val = (uint8_t)lval;
        }
        else {
            res = -1;
        }
    }
    return res;
}

static void _netif_list(turo_t *ctx, int32_t netif_num)
{
    uint8_t buf8[] = { 1, 2, 3, 4, 5, 6, 7, 8 };

    turo_dict_open(ctx);
    turo_dict_key(ctx, "netif");
    turo_dict_open(ctx);

    turo_dict_key(ctx, "num");
    turo_s32(ctx, netif_num);

    turo_dict_key(ctx, "HWaddr");
    turo_array_u8(ctx, buf8, ARRAY_SIZE(buf8));

    turo_dict_key(ctx, "inet6 addr");
    turo_dict_open(ctx);
    turo_dict_key(ctx, "addr");
    turo_string(ctx, "fe80::2445:7fff:fe5a:6fd9");
    turo_dict_key(ctx, "scope");
    turo_string(ctx, "link");
    turo_dict_key(ctx, "flags");
    turo_array_open(ctx);
    turo_string(ctx, "VAL");
    turo_array_close(ctx);
    turo_dict_close(ctx);

    turo_dict_key(ctx, "inet6 group");
    turo_array_open(ctx);
    turo_string(ctx, "ff02::2");
    turo_string(ctx, "ff02::1");
    turo_string(ctx, "ff02::1:ff5a:6fd9");
    turo_array_close(ctx);

    turo_dict_key(ctx, "flags");
    turo_array_open(ctx);
    turo_dict_s32(ctx, "L2-PDU", 1500);
    turo_dict_s32(ctx, "MTU", 1500);
    turo_dict_s32(ctx, "HL", 64);
    turo_string(ctx, "RTR");
    turo_string(ctx, "RTR_ADV");
    turo_dict_s32(ctx, "Source address length", 6);
    turo_array_close(ctx);

    turo_dict_close(ctx);
    turo_dict_close(ctx);
}


static int cmd_turo_simple_s32(int argc, char **argv)
{
    int32_t s32 = 0;
    if (argc != 2) {
        turo_simple_exit_status(&ctx, -2);
        return 1;
    }
    if (_sc_arg2s32(argv[1], &s32) != 0) {
        turo_simple_exit_status(&ctx, -3);
        return 1;
    }

    turo_simple_s32(&ctx, s32);
    return 0;
}

static int cmd_turo_simple_array_u8(int argc, char **argv)
{
    uint8_t buf8[_BUF_COUNT];
    if (argc == 1) {
        turo_simple_exit_status(&ctx, -4);
        return 1;
    }
    if (argc > _BUF_COUNT + 1) {
        turo_simple_exit_status(&ctx, -5);
        return 1;
    }

    for (int i = 0; i < argc - 1; i++) {
        if (_sc_arg2u8(argv[i + 1], &buf8[i]) != 0) {
            turo_simple_exit_status(&ctx, -6);
            return 1;
        }
    }

    turo_simple_array_u8(&ctx, buf8, argc - 1);
    return 0;
}

static int cmd_turo_simple_array_s32(int argc, char **argv)
{
    int32_t buf32[_BUF_COUNT];
    if (argc == 1) {
        turo_simple_exit_status(&ctx, -7);
        return 1;
    }
    if (argc > _BUF_COUNT + 1) {
        turo_simple_exit_status(&ctx, -8);
        return 1;
    }

    for (int i = 0; i < argc - 1; i++) {
        if (_sc_arg2s32(argv[i + 1], &buf32[i]) != 0) {
            turo_simple_exit_status(&ctx, -9);
            return 1;
        }
    }

    turo_simple_array_s32(&ctx, buf32, argc - 1);
    return 0;
}

static int cmd_turo_simple_dict_string(int argc, char **argv)
{
    if (argc != 3) {
        turo_simple_exit_status(&ctx, -10);
        return 1;
    }

    turo_simple_dict_string(&ctx, argv[1], argv[2]);
    return 0;
}

static int cmd_turo_simple_dict_s32(int argc, char **argv)
{
    int32_t s32 = 0;
    if (argc != 3) {
        turo_simple_exit_status(&ctx, -11);
        return 1;
    }

    if (_sc_arg2s32(argv[2], &s32) != 0) {
        turo_simple_exit_status(&ctx, -12);
        return 1;
    }

    turo_simple_dict_s32(&ctx, argv[1], s32);
    return 0;
}

static int cmd_turo_simple_exit_status(int argc, char **argv)
{
    int32_t s32 = 0;
    if (argc != 2) {
        turo_simple_exit_status(&ctx, -13);
        return 1;
    }

    if (_sc_arg2s32(argv[1], &s32) != 0) {
        turo_simple_exit_status(&ctx, -14);
        return 1;
    }

    turo_simple_exit_status(&ctx, (int)s32);
    return 0;
}

static int cmd_test_multi_element_dict(int argc, char **argv)
{
    if (argc != 5) {
        turo_simple_exit_status(&ctx, -15);
        return 1;
    }

    turo_container_open(&ctx);
    turo_dict_open(&ctx);
    turo_dict_key(&ctx, argv[1]);
    turo_string(&ctx, argv[2]);
    turo_dict_key(&ctx, argv[3]);
    turo_string(&ctx, argv[4]);
    turo_dict_close(&ctx);
    turo_container_close(&ctx, 0);

    return 0;
}

static int cmd_test_netif(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    turo_container_open(&ctx);
    _netif_list(&ctx, 5);
    _netif_list(&ctx, 6);
    turo_container_close(&ctx, 0);

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "turo_simple_s32", "", cmd_turo_simple_s32 },
    { "turo_simple_array_u8", "", cmd_turo_simple_array_u8 },
    { "turo_simple_array_s32", "", cmd_turo_simple_array_s32 },
    { "turo_simple_dict_string", "", cmd_turo_simple_dict_string },
    { "turo_simple_dict_s32", "", cmd_turo_simple_dict_s32 },
    { "turo_simple_exit_status", "", cmd_turo_simple_exit_status },
    { "test_multi_element_dict", "", cmd_test_multi_element_dict },
    { "test_netif", "", cmd_test_netif },
    { NULL, NULL, NULL }
};

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    puts("Test for the test utilities result output");
    turo_init(&ctx);

    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
