/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine S. Lenders <m.lenders@fu-berlin.de>
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "clist.h"
#include "congure/test.h"
#include "fmt.h"
#include "shell.h"

#include "congure_impl.h"

static int _json_statham(int argc, char **argv);
static int _set_cwnd(int argc, char **argv);
static int _get_fr_calls(int argc, char **argv);
static int _set_same_wnd_adv_res(int argc, char **argv);

static congure_abe_snd_t _congure_state;
static const shell_command_t shell_commands[] = {
    { "state", "Prints current CongURE state object as JSON", _json_statham },
    { "set_cwnd", "Set cwnd member for CongURE state object", _set_cwnd },
    { "get_ff_calls",
      "Get the number of calls to fast_retransmit callback of CongURE state "
      "object", _get_fr_calls },
    { "set_same_wnd_adv",
      "Set the result for the same_window_advertised callback of CongURE state "
      "object", _set_same_wnd_adv_res },
    { NULL, NULL, NULL }
};

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}

congure_test_snd_t *congure_test_get_state(void)
{
    return &_congure_state;
}

#define PRINT_FIELD_PTR(obj_ptr, field) \
    print_str("\"" #field "\":\"0x"); \
    print_u32_hex((intptr_t)((obj_ptr)->field)); \
    print_str("\",")

#define PRINT_FIELD_UINT(obj, field) \
    print_str("\"" #field "\":"); \
    print_u32_dec((obj).field); \
    print_str(",")

static void _print_congure_abe_consts(const congure_abe_snd_consts_t *consts)
{
    print_str("\"consts\":");

    if (consts) {
        print_str("{");
        PRINT_FIELD_PTR(&consts->reno, fr);
        PRINT_FIELD_PTR(&consts->reno, same_wnd_adv);
        PRINT_FIELD_PTR(&consts->reno, ss_cwnd_inc);
        PRINT_FIELD_PTR(&consts->reno, ca_cwnd_inc);
        PRINT_FIELD_PTR(&consts->reno, fr_cwnd_dec);
        PRINT_FIELD_UINT(consts->reno, init_mss);
        PRINT_FIELD_UINT(consts->reno, cwnd_upper);
        PRINT_FIELD_UINT(consts->reno, cwnd_lower);
        PRINT_FIELD_UINT(consts->reno, init_ssthresh);
        PRINT_FIELD_UINT(consts->reno, frthresh);
        PRINT_FIELD_UINT(*consts, abe_multiplier_numerator);
        PRINT_FIELD_UINT(*consts, abe_multiplier_denominator);
        print_str("},");
    }
    else {
        print_str("null,");
    }
}

static int _json_statham(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    print_str("{");

    PRINT_FIELD_UINT(_congure_state.super, cwnd);
    _print_congure_abe_consts(
        (congure_abe_snd_consts_t *)_congure_state.consts
    );
    PRINT_FIELD_UINT(_congure_state, mss);
    PRINT_FIELD_UINT(_congure_state, last_ack);
    PRINT_FIELD_UINT(_congure_state, ssthresh);
    PRINT_FIELD_UINT(_congure_state, in_flight_size);
    PRINT_FIELD_UINT(_congure_state, dup_acks);

    print_str("}\n");
    return 0;
}

static int _set_cwnd(int argc, char **argv)
{
    uint32_t tmp;

    if (argc < 2) {
        print_str("{\"error\":\"`cwnd` argument expected\"}");
        return 1;
    }
    tmp = scn_u32_dec(argv[1], strlen(argv[1]));
    if (tmp > CONGURE_WND_SIZE_MAX) {
        print_str("{\"error\":\"`ssthresh` not 16 bit wide\"}\n");
    }
    _congure_state.super.cwnd = (congure_wnd_size_t)tmp;
    return 0;
}

static int _get_fr_calls(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    print_str("{\"fr_calls\":");
    print_u32_dec(congure_abe_test_get_fr_calls());
    print_str("}\n");
    return 0;
}

static int _set_same_wnd_adv_res(int argc, char **argv)
{
    if (argc < 2) {
        print_str("{\"error\":\"`value` argument expected\"}");
        return 1;
    }
    congure_abe_test_set_same_wnd_adv_res(
        (bool)scn_u32_dec(argv[1], strlen(argv[1]))
    );
    return 0;
}

/** @} */
