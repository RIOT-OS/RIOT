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

#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "clist.h"
#include "fmt.h"

#include "congure/test.h"

static congure_snd_msg_t _msgs_pool[CONFIG_CONGURE_TEST_LOST_MSG_POOL_SIZE];
static unsigned _msgs_pool_idx;
static clist_node_t _msgs;

static bool _scn_u32_dec_with_zero(const char *str, size_t n, uint32_t *res)
{
    if ((n == 1) && str[0] == '0') {
        *res = 0;
    }
    else if ((*res = scn_u32_dec(str, n)) == 0) {
        return false;
    }
    return true;
}

int congure_test_clear_state(int argc, char **argv)
{
    memset(congure_test_get_state(), 0, sizeof(congure_test_snd_t));
    congure_test_msgs_reset(argc, argv);
    return 0;
}

int congure_test_call_setup(int argc, char **argv)
{
    congure_test_snd_t *c = congure_test_get_state();
    uint32_t id = 0;

    if (argc > 1) {
        if (!_scn_u32_dec_with_zero(argv[1], strlen(argv[1]), &id)) {
            print_str("{\"error\":\"`id` expected to be integer\"}\n");
            return 1;
        }
    }
    if (congure_test_snd_setup(c, (unsigned)id) < 0) {
        print_str("{\"error\":\"`id` is invalid\"}");
        return 1;
    }

    print_str("{");

    print_str("\"success\":\"0x");
    print_u32_hex((intptr_t)c);

    print_str("\"}\n");
    return 0;
}

static inline bool _check_driver(congure_test_snd_t *c)
{
    if (c->super.driver == NULL) {
        print_str("{\"error\":\"State object not set up\"}\n");
        return false;
    }
    return true;
}

int congure_test_call_init(int argc, char **argv)
{
    congure_test_snd_t *c = congure_test_get_state();
    uint32_t ctx;
    size_t arglen;

    if (!_check_driver(c)) {
        return 1;
    }
    if (argc < 2) {
        print_str("{\"error\":\"`ctx` argument expected\"}\n");
        return 1;
    }
    arglen = strlen(argv[1]);
    if ((arglen < 3) || ((argv[1][0] != '0') && (argv[1][1] != 'x'))) {
        print_str("{\"error\":\"`ctx` expected to be hex\"}\n");
        return 1;
    }
    ctx = scn_u32_hex(&argv[1][2], arglen - 2);
    c->super.driver->init(&c->super, (void *)((intptr_t)ctx));
    print_str("{\"success\":null}\n");
    return 0;
}

int congure_test_call_inter_msg_interval(int argc, char **argv)
{
    congure_test_snd_t *c = congure_test_get_state();
    uint32_t msg_size;
    int32_t res;

    (void)argc;
    (void)argv;
    if (!_check_driver(c)) {
        return 1;
    }
    if (argc < 2) {
        print_str("{\"error\":\"`msg_size` argument expected\"}\n");
        return 1;
    }
    if (!_scn_u32_dec_with_zero(argv[1], strlen(argv[1]), &msg_size)) {
        print_str("{\"error\":\"`msg_size` expected to be integer\"}\n");
        return 1;
    }
    res = c->super.driver->inter_msg_interval(&c->super, msg_size);
    print_str("{\"success\":");
    print_s32_dec(res);
    print_str("}\n");
    return 0;
}

int congure_test_add_msg(int argc, char **argv)
{
    uint32_t tmp;

    if (argc < 4) {
        print_str("{\"error\":\"At least 3 arguments `msg_send_time`, "
                  "`msg_size`, `msg_resends` expected\"}\n");
        return 1;
    }
    if (_msgs_pool_idx >= ARRAY_SIZE(_msgs_pool)) {
        print_str("{\"error\":\"List element pool depleted\"}\n");
        return 1;
    }
    _msgs_pool[_msgs_pool_idx].super.next = NULL;

    if (!_scn_u32_dec_with_zero(argv[1], strlen(argv[1]), &tmp)) {
        print_str("{\"error\":\"`msg_send_time` expected to be "
                  "integer\"}\n");
        return 1;
    }
    _msgs_pool[_msgs_pool_idx].send_time = tmp;

    if (!_scn_u32_dec_with_zero(argv[2], strlen(argv[2]), &tmp)) {
        print_str("{\"error\":\"`msg_size` expected to be integer\"}\n");
        return 1;
    }
    _msgs_pool[_msgs_pool_idx].size = tmp;

    if (!_scn_u32_dec_with_zero(argv[3], strlen(argv[3]), &tmp)) {
        print_str("{\"error\":\"`msg_resends` expected to be "
                  "integer\"}\n");
        return 1;
    }
    _msgs_pool[_msgs_pool_idx].resends = tmp;

    clist_rpush(&_msgs, &_msgs_pool[_msgs_pool_idx++].super);
    print_str("{\"success\":null}\n");
    return 0;
}

int congure_test_msgs_reset(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    _msgs.next = NULL;
    _msgs_pool_idx = 0;
    print_str("{\"success\":null}\n");
    return 0;
}

static int _call_report_msg_sent(int argc, char **argv)
{
    congure_test_snd_t *c = congure_test_get_state();
    uint32_t msg_size;

    if (argc < 2) {
        print_str("{\"error\":\"`msg_size` argument expected\"}\n");
        return 1;
    }
    if (!_scn_u32_dec_with_zero(argv[1], strlen(argv[1]), &msg_size)) {
        print_str("{\"error\":\"`msg_size` expected to be integer\"}\n");
        return 1;
    }
    c->super.driver->report_msg_sent(&c->super, (unsigned)msg_size);
    print_str("{\"success\":null}\n");
    return 0;
}

static int _call_report_msg_discarded(int argc, char **argv)
{
    congure_test_snd_t *c = congure_test_get_state();
    uint32_t msg_size;

    if (argc < 2) {
        print_str("{\"error\":\"`msg_size` argument expected\"}\n");
        return 1;
    }
    if (!_scn_u32_dec_with_zero(argv[1], strlen(argv[1]), &msg_size)) {
        print_str("{\"error\":\"`msg_size` expected to be integer\"}\n");
        return 1;
    }
    c->super.driver->report_msg_discarded(&c->super, (unsigned)msg_size);
    print_str("{\"success\":null}\n");
    return 0;
}

static int _call_report_msgs_timeout_lost(void (*method)(congure_snd_t *,
                                                         congure_snd_msg_t *))
{
    congure_test_snd_t *c = congure_test_get_state();

    if (_msgs.next == NULL) {
        print_str("{\"error\":\"Message not initialized\"}\n");
        return 1;
    }
    method(&c->super, (congure_snd_msg_t *)_msgs.next);
    print_str("{\"success\":null}\n");
    return 0;
}

static int _call_report_msgs_timeout(int argc, char **argv)
{
    congure_test_snd_t *c = congure_test_get_state();

    (void)argc;
    (void)argv;
    return _call_report_msgs_timeout_lost(c->super.driver->report_msgs_timeout);
}

static int _call_report_msgs_lost(int argc, char **argv)
{
    congure_test_snd_t *c = congure_test_get_state();

    (void)argc;
    (void)argv;
    return _call_report_msgs_timeout_lost(c->super.driver->report_msgs_lost);
}

static int _call_report_msg_acked(int argc, char **argv)
{
    static congure_snd_ack_t ack = { .size = 0 };
    congure_test_snd_t *c = congure_test_get_state();
    uint32_t tmp;

    if (_msgs.next == NULL) {
        print_str("{\"error\":\"Message not initialized\"}\n");
        return 1;
    }
    if (argc < 7) {
        print_str("{\"error\":\"At least 6 arguments `ack_recv_time`, "
                  "`ack_id`, `ack_size`, `ack_clean`, `ack_wnd`, `ack_delay` "
                  "expected\"}\n");
        return 1;
    }
    if (!_scn_u32_dec_with_zero(argv[1], strlen(argv[1]), &tmp)) {
        print_str("{\"error\":\"`ack_recv_time` expected to be integer\"}\n");
        return 1;
    }
    ack.recv_time = tmp;

    if (!_scn_u32_dec_with_zero(argv[2], strlen(argv[2]), &tmp)) {
        print_str("{\"error\":\"`ack_id` expected to be integer\"}\n");
        return 1;
    }
    ack.id = tmp;

    if (!_scn_u32_dec_with_zero(argv[3], strlen(argv[3]), &tmp)) {
        print_str("{\"error\":\"`ack_size` expected to be integer\"}\n");
        return 1;
    }
    ack.size = tmp;

    if (!_scn_u32_dec_with_zero(argv[4], strlen(argv[4]), &tmp)) {
        print_str("{\"error\":\"`ack_clean` expected to be integer\"}\n");
        return 1;
    }
    ack.clean = (bool)tmp;

    if (!_scn_u32_dec_with_zero(argv[5], strlen(argv[5]), &tmp)) {
        print_str("{\"error\":\"`ack_wnd` expected to be integer\"}\n");
        return 1;
    }
    if (tmp > CONGURE_WND_SIZE_MAX) {
        print_str("{\"error\":\"`ack_wnd` not 16 bit wide\"}\n");
        return 1;
    }
    ack.wnd = (uint16_t)tmp;

    if (!_scn_u32_dec_with_zero(argv[6], strlen(argv[6]), &tmp)) {
        print_str("{\"error\":\"`ack_delay` expected to be integer\"}\n");
        return 1;
    }
    if (tmp > UINT16_MAX) {
        print_str("{\"error\":\"`ack_delay` not 16 bit wide\"}\n");
        return 1;
    }
    ack.delay = (uint16_t)tmp;

    c->super.driver->report_msg_acked(&c->super,
                                      (congure_snd_msg_t *)_msgs.next, &ack);
    print_str("{\"success\":null}\n");
    return 0;
}

static int _call_report_ecn_ce(int argc, char **argv)
{
    congure_test_snd_t *c = congure_test_get_state();
    uint32_t time;

    if (argc < 2) {
        print_str("{\"error\":\"`time` argument expected\"}\n");
        return 1;
    }
    if (!_scn_u32_dec_with_zero(argv[1], strlen(argv[1]), &time)) {
        print_str("{\"error\":\"`time` expected to be integer\"}\n");
        return 1;
    }
    c->super.driver->report_ecn_ce(&c->super, time);
    print_str("{\"success\":null}\n");
    return 0;
}

int congure_test_call_report(int argc, char **argv)
{
    if (!_check_driver(congure_test_get_state())) {
        return 1;
    }
    if (argc < 2) {
        print_str("{\"error\":\"No report command provided\"}\n");
        return 1;
    }
    if (strcmp(argv[1], "msg_sent") == 0) {
        return _call_report_msg_sent(argc - 1, &argv[1]);
    }
    else if (strcmp(argv[1], "msg_discarded") == 0) {
        return _call_report_msg_discarded(argc - 1, &argv[1]);
    }
    else if (strcmp(argv[1], "msgs_timeout") == 0) {
        return _call_report_msgs_timeout(argc - 1, &argv[1]);
    }
    else if (strcmp(argv[1], "msgs_lost") == 0) {
        return _call_report_msgs_lost(argc - 1, &argv[1]);
    }
    else if (strcmp(argv[1], "msg_acked") == 0) {
        return _call_report_msg_acked(argc - 1, &argv[1]);
    }
    else if (strcmp(argv[1], "ecn_ce") == 0) {
        return _call_report_ecn_ce(argc - 1, &argv[1]);
    }
    print_str("{\"error\":\"Unknown command `");
    print_str(argv[1]);
    print_str("`\"}\n");
    return 1;
}

/** @} */
