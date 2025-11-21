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

#include <stdlib.h>

#include "clist.h"
#include "congure/test.h"
#include "fmt.h"
#include "shell.h"

#include "congure_impl.h"

static congure_test_snd_t _congure_state;

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

static int _print_congure_snd_msg(clist_node_t *node, void *arg)
{
    congure_snd_msg_t *msg = (congure_snd_msg_t *)node;

    (void)arg;
    print_str("{");

    print_str("\"send_time\":");
    print_u64_dec(msg->send_time);
    print_str(",");

    print_str("\"size\":");
    print_u32_dec(msg->size);
    print_str(",");

    print_str("\"resends\":");
    print_u32_dec(msg->resends);

    print_str("},");

    return 0;
}

static int _print_congure_snd_ack(congure_snd_ack_t *ack)
{
    print_str("{");

    print_str("\"recv_time\":");
    print_u64_dec(ack->recv_time);
    print_str(",");

    print_str("\"id\":");
    print_u32_dec(ack->id);
    print_str(",");

    print_str("\"size\":");
    print_u32_dec(ack->size);
    print_str(",");

    print_str("\"clean\":");
    print_str(ack->clean ? "true" : "false");
    print_str(",");

    print_str("\"wnd\":");
    print_u32_dec(ack->wnd);
    print_str(",");

    print_str("\"delay\":");
    print_u32_dec(ack->delay);
    print_str(",");

    print_str("},");

    return 0;
}

static void _print_init_state(void)
{
    print_str("\"init\":{");

    print_str("\"calls\":");
    print_u32_dec(_congure_state.init_calls);
    print_str(",");

    print_str("\"last_args\":{");

    print_str("\"c\":\"0x");
    print_u32_hex((intptr_t)_congure_state.init_args.c);
    print_str("\",");

    print_str("\"ctx\":\"0x");
    print_u32_hex((intptr_t)_congure_state.init_args.ctx);
    print_str("\"");

    print_str("}");

    print_str("},");
}

static void _print_inter_msg_interval_state(void)
{
    print_str("\"inter_msg_interval\":{");

    print_str("\"calls\":");
    print_u32_dec(_congure_state.inter_msg_interval_calls);
    print_str(",");

    print_str("\"last_args\":{");

    print_str("\"c\":\"0x");
    print_u32_hex((intptr_t)_congure_state.inter_msg_interval_args.c);
    print_str("\",");

    print_str("\"msg_size\":");
    print_u32_dec(_congure_state.inter_msg_interval_args.msg_size);
    print_str("");

    print_str("}");

    print_str("},");
}

static void _print_report_msg_sent_state(void)
{
    print_str("\"report_msg_sent\":{");

    print_str("\"calls\":");
    print_u32_dec(_congure_state.report_msg_sent_calls);
    print_str(",");

    print_str("\"last_args\":{");

    print_str("\"c\":\"0x");
    print_u32_hex((intptr_t)_congure_state.report_msg_sent_args.c);
    print_str("\",");

    print_str("\"msg_size\":");
    print_u32_dec(_congure_state.report_msg_sent_args.msg_size);
    print_str("");

    print_str("}");

    print_str("},");
}

static void _print_report_msg_discarded_state(void)
{
    print_str("\"report_msg_discarded\":{");

    print_str("\"calls\":");
    print_u32_dec(_congure_state.report_msg_discarded_calls);
    print_str(",");

    print_str("\"last_args\":{");

    print_str("\"c\":\"0x");
    print_u32_hex((intptr_t)_congure_state.report_msg_discarded_args.c);
    print_str("\",");

    print_str("\"msg_size\":");
    print_u32_dec(_congure_state.report_msg_discarded_args.msg_size);
    print_str("");

    print_str("}");

    print_str("},");
}

static void _print_report_msgs_timeout_state(void)
{
    print_str("\"report_msgs_timeout\":{");

    print_str("\"calls\":");
    print_u32_dec(_congure_state.report_msgs_timeout_calls);
    print_str(",");

    print_str("\"last_args\":{");

    print_str("\"c\":\"0x");
    print_u32_hex((intptr_t)_congure_state.report_msgs_timeout_args.c);
    print_str("\",");

    print_str("\"msgs\":[");
    clist_foreach((clist_node_t *)&_congure_state.report_msgs_timeout_args.msgs,
                  _print_congure_snd_msg, NULL);
    print_str("]");

    print_str("}");

    print_str("},");
}

static void _print_report_msgs_lost_state(void)
{
    print_str("\"report_msgs_lost\":{");

    print_str("\"calls\":");
    print_u32_dec(_congure_state.report_msgs_lost_calls);
    print_str(",");

    print_str("\"last_args\":{");

    print_str("\"c\":\"0x");
    print_u32_hex((intptr_t)_congure_state.report_msgs_lost_args.c);
    print_str("\",");

    print_str("\"msgs\":[");
    clist_foreach((clist_node_t *)&_congure_state.report_msgs_lost_args.msgs,
                  _print_congure_snd_msg, NULL);
    print_str("]");

    print_str("}");

    print_str("},");
}

static void _print_report_msg_acked_state(void)
{
    clist_node_t msgs = {
        .next = &_congure_state.report_msg_acked_args.msg->super,
    };
    print_str("\"report_msg_acked\":{");

    print_str("\"calls\":");
    print_u32_dec(_congure_state.report_msg_acked_calls);
    print_str(",");

    print_str("\"last_args\":{");

    print_str("\"c\":\"0x");
    print_u32_hex((intptr_t)_congure_state.report_msg_acked_args.c);
    print_str("\",");

    /* Check sanity of `congure_test` internal message list: `cong_msg_add`
     * should have been only called at most once by the test script */
    assert(clist_count(&msgs) < 2);
    print_str("\"msg\":");
    if (_congure_state.report_msg_acked_args.msg) {
        _print_congure_snd_msg(msgs.next, NULL);
    }
    else {
        print_str("null,");
    }

    print_str("\"ack\":");
    if (_congure_state.report_msg_acked_args.ack) {
        _print_congure_snd_ack(_congure_state.report_msg_acked_args.ack);
    }
    else {
        print_str("null");
    }

    print_str("}");

    print_str("},");
}

static void _print_report_ecn_ce_state(void)
{
    print_str("\"report_ecn_ce\":{");

    print_str("\"calls\":");
    print_u32_dec(_congure_state.report_ecn_ce_calls);
    print_str(",");

    print_str("\"last_args\":{");

    print_str("\"c\":\"0x");
    print_u32_hex((intptr_t)_congure_state.report_ecn_ce_args.c);
    print_str("\",");

    print_str("\"time\":");
    print_u64_dec((intptr_t)_congure_state.report_ecn_ce_args.time);
    print_str(",");

    print_str("}");

    print_str("},");
}

static int _json_statham(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    print_str("{");
    print_str("\"driver\":\"0x");
    print_u32_hex((intptr_t)_congure_state.super.driver);
    print_str("\",");

    print_str("\"ctx\":\"0x");
    print_u32_hex((intptr_t)_congure_state.super.ctx);
    print_str("\",");

    print_str("\"cwnd\":");
    print_u32_dec(_congure_state.super.cwnd);
    print_str(",");

    _print_init_state();
    _print_inter_msg_interval_state();
    _print_report_msg_sent_state();
    _print_report_msg_discarded_state();
    _print_report_msgs_timeout_state();
    _print_report_msgs_lost_state();
    _print_report_msg_acked_state();
    _print_report_ecn_ce_state();

    print_str("}\n");
    return 0;
}

SHELL_COMMAND(state, "Prints current CongURE state object as JSON", _json_statham);

/** @} */
