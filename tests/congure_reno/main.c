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

#define SHELL_BUFSIZE           512U

static char _line_buf[SHELL_BUFSIZE];

static int _json_statham(int argc, char **argv);
static int _set_mss(int argc, char **argv);
static int _set_cwnd(int argc, char **argv);
static int _set_ssthresh(int argc, char **argv);
static int _get_fr_calls(int argc, char **argv);
static int _set_same_wnd_adv_res(int argc, char **argv);

static congure_reno_snd_t _congure_state;
static const shell_command_t shell_commands[] = {
    { "state", "Prints current CongURE state object as JSON", _json_statham },
    { "set_cwnd", "Set cwnd member for CongURE state object", _set_cwnd },
    { "set_mss", "Set new MSS for CongURE state object", _set_mss },
    { "set_ssthresh", "Set ssthresh member for CongURE state object",
      _set_ssthresh },
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
    shell_run(shell_commands, _line_buf, SHELL_BUFSIZE);
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

static void _print_congure_reno_consts(const congure_reno_snd_consts_t *consts)
{
    print_str("\"consts\":");

    if (consts) {
        print_str("{");
        PRINT_FIELD_PTR(consts, fr);
        PRINT_FIELD_PTR(consts, same_wnd_adv);
        PRINT_FIELD_PTR(consts, ss_cwnd_inc);
        PRINT_FIELD_PTR(consts, ca_cwnd_inc);
        PRINT_FIELD_PTR(consts, fr_cwnd_dec);
        PRINT_FIELD_UINT(*consts, init_mss);
        PRINT_FIELD_UINT(*consts, cwnd_upper);
        PRINT_FIELD_UINT(*consts, cwnd_lower);
        PRINT_FIELD_UINT(*consts, init_ssthresh);
        PRINT_FIELD_UINT(*consts, frthresh);
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
    _print_congure_reno_consts(_congure_state.consts);
    PRINT_FIELD_UINT(_congure_state, mss);
    PRINT_FIELD_UINT(_congure_state, last_ack);
    PRINT_FIELD_UINT(_congure_state, ssthresh);
    PRINT_FIELD_UINT(_congure_state, in_flight_size);
    PRINT_FIELD_UINT(_congure_state, dup_acks);

    print_str("}\n");
    return 0;
}

static int _set_mss(int argc, char **argv)
{
    uint32_t tmp;

    if (argc < 2) {
        print_str("{\"error\":\"`mss` argument expected\"}");
        return 1;
    }
    tmp = scn_u32_dec(argv[1], strlen(argv[1]));
    if (tmp > CONGURE_WND_SIZE_MAX) {
        print_str("{\"error\":\"`mss` not 16 bit wide\"}\n");
    }
    congure_reno_set_mss(&_congure_state, (congure_wnd_size_t)tmp);
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
        print_str("{\"error\":\"`cwnd` not 16 bit wide\"}\n");
    }
    _congure_state.super.cwnd = (congure_wnd_size_t)tmp;
    return 0;
}

static int _set_ssthresh(int argc, char **argv)
{
    uint32_t tmp;

    if (argc < 2) {
        print_str("{\"error\":\"`ssthresh` argument expected\"}");
        return 1;
    }
    tmp = scn_u32_dec(argv[1], strlen(argv[1]));
    if (tmp > CONGURE_WND_SIZE_MAX) {
        print_str("{\"error\":\"`ssthresh` not 16 bit wide\"}\n");
    }
    _congure_state.ssthresh = (congure_wnd_size_t)tmp;
    return 0;
}

static int _get_fr_calls(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    print_str("{\"fr_calls\":");
    print_u32_dec(congure_reno_test_get_fr_calls());
    print_str("}\n");
    return 0;
}

static int _set_same_wnd_adv_res(int argc, char **argv)
{
    if (argc < 2) {
        print_str("{\"error\":\"`value` argument expected\"}");
        return 1;
    }
    congure_reno_test_set_same_wnd_adv_res(
        (bool)scn_u32_dec(argv[1], strlen(argv[1]))
    );
    return 0;
}

/** @} */
