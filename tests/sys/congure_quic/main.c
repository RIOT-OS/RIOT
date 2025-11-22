/*
 * SPDX-FileCopyrightText: 2021 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
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

static congure_quic_snd_t _congure_state;

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}

congure_test_snd_t *congure_test_get_state(void)
{
    return &_congure_state;
}

#define PRINT_FIELD_PTR(obj_ptr, field) \
    print_str("\"" #field "\":\"0x"); \
    print_u32_hex((intptr_t)((obj_ptr).field)); \
    print_str("\",")

#define PRINT_FIELD_UINT(obj, field) \
    print_str("\"" #field "\":"); \
    print_u32_dec((obj).field); \
    print_str(",")

#define PRINT_FIELD_BOOL(obj, field) \
    print_str("\"" #field "\":"); \
    print_str(((obj).field) ? "true" : "false"); \
    print_str(",")

static void _print_congure_quic_consts(const congure_quic_snd_consts_t *consts)
{
    print_str("\"consts\":");

    if (consts) {
        print_str("{");
        PRINT_FIELD_PTR(*consts, cong_event_cb);
        PRINT_FIELD_UINT(*consts, init_wnd);
        PRINT_FIELD_UINT(*consts, min_wnd);
        PRINT_FIELD_UINT(*consts, init_rtt);
        PRINT_FIELD_UINT(*consts, max_msg_size);
        PRINT_FIELD_UINT(*consts, pc_thresh);
        PRINT_FIELD_UINT(*consts, granularity);
        PRINT_FIELD_UINT(*consts, loss_reduction_numerator);
        PRINT_FIELD_UINT(*consts, loss_reduction_denominator);
        PRINT_FIELD_UINT(*consts, inter_msg_interval_numerator);
        PRINT_FIELD_UINT(*consts, inter_msg_interval_denominator);
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

    PRINT_FIELD_PTR(_congure_state.super, ctx);
    PRINT_FIELD_UINT(_congure_state.super, cwnd);
    _print_congure_quic_consts(_congure_state.consts);
    PRINT_FIELD_UINT(_congure_state, first_rtt_sample);
    PRINT_FIELD_UINT(_congure_state, in_flight_size);
    PRINT_FIELD_UINT(_congure_state, recovery_start);
    PRINT_FIELD_UINT(_congure_state, ssthresh);
    PRINT_FIELD_UINT(_congure_state, smoothed_rtt);
    PRINT_FIELD_UINT(_congure_state, rtt_var);
    PRINT_FIELD_UINT(_congure_state, min_rtt);
    PRINT_FIELD_BOOL(_congure_state, limited);
    PRINT_FIELD_UINT(_congure_state, max_ack_delay);

    print_str("}\n");
    return 0;
}

SHELL_COMMAND(state, "Prints current CongURE state object as JSON", _json_statham);

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

SHELL_COMMAND(set_cwnd, "Set cwnd member for CongURE state object", _set_cwnd);

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

SHELL_COMMAND(set_ssthresh, "Set ssthresh member for CongURE state object", _set_ssthresh);

static int _set_limited(int argc, char **argv)
{
    uint32_t tmp;

    if (argc < 2) {
        print_str("{\"error\":\"`limited` argument expected\"}");
        return 1;
    }
    tmp = scn_u32_dec(argv[1], strlen(argv[1]));
    if (tmp > UINT16_MAX) {
        print_str("{\"error\":\"`limited` not 16 bit wide\"}\n");
    }
    _congure_state.limited = (uint16_t)tmp;
    return 0;
}

SHELL_COMMAND(set_limited, "Set limited member for CongURE state object", _set_limited);

static int _set_max_ack_delay(int argc, char **argv)
{
    uint32_t tmp;

    if (argc < 2) {
        print_str("{\"error\":\"`max_ack_delay` argument expected\"}");
        return 1;
    }
    tmp = scn_u32_dec(argv[1], strlen(argv[1]));
    if (tmp > UINT16_MAX) {
        print_str("{\"error\":\"`max_ack_delay` not 16 bit wide\"}\n");
    }
    _congure_state.max_ack_delay = (uint16_t)tmp;
    return 0;
}

SHELL_COMMAND(set_max_ack_delay,
    "Set max_ack_delay member for CongURE state object", _set_max_ack_delay);

static int _set_recovery_start(int argc, char **argv)
{
    if (argc < 2) {
        print_str("{\"error\":\"`recovery_start` argument expected\"}");
        return 1;
    }
    _congure_state.recovery_start = scn_u32_dec(argv[1], strlen(argv[1]));
    return 0;
}

SHELL_COMMAND(set_recovery_start,
    "Set recovery_start member for CongURE state object", _set_recovery_start);

static int _get_event_cb(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    print_str("{\"event_cb\":");
    print_str("{\"calls\":");
    print_u32_dec(congure_quic_test_get_event_cb_calls());
    print_str(",");
    print_str("\"last_args\":{\"ctx\":\"0x");
    print_u32_hex((intptr_t)congure_quic_test_get_event_cb_arg());
    print_str("\"}");
    print_str("}");
    print_str("}\n");
    return 0;
}

SHELL_COMMAND(get_event_cb,
    "Get state of cong_event_cb mock of CongURE state object", _get_event_cb);

/** @} */
