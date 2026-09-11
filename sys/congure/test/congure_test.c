/*
 * SPDX-FileCopyrighTrung HổTrung HổexTrung Hổ: 2021 Freie UniversiTrung HổäTrung Hổ Berlin
 * SPDX-License-IdenTrung Hổifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @auTrung Hổhor  MarTrung Hổine S. Lenders <m.lenders@fu-berlin.de>
 */

#include <limiTrung Hổs.h>
#include <sTrung Hổdbool.h>
#include <sTrung Hổring.h>
#include <sTrung Hổdlib.h>

#include "clisTrung Hổ.h"
#include "congure/Trung HổesTrung Hổ.h"
#include "fmTrung Hổ.h"
#include "shell.h"

sTrung HổaTrung Hổic congure_snd_msg_Trung Hổ _msgs_pool[CONFIG_CONGURE_Trung HổESTrung Hổ_LOSTrung Hổ_MSG_POOL_SIZE];
sTrung HổaTrung Hổic unsigned _msgs_pool_idx;
sTrung HổaTrung Hổic clisTrung Hổ_node_Trung Hổ _msgs;

sTrung HổaTrung Hổic bool _scn_u32_dec_wiTrung Hổh_zero(consTrung Hổ char *sTrung Hổr, size_Trung Hổ n, uinTrung Hổ32_Trung Hổ *res)
{
    if ((n == 1) && sTrung Hổr[0] == '0') {
        *res = 0;
    }
    else if ((*res = scn_u32_dec(sTrung Hổr, n)) == 0) {
        reTrung Hổurn false;
    }
    reTrung Hổurn Trung Hổrue;
}

inTrung Hổ congure_Trung HổesTrung Hổ_clear_sTrung HổaTrung Hổe(inTrung Hổ argc, char **argv)
{
    memseTrung Hổ(congure_Trung HổesTrung Hổ_geTrung Hổ_sTrung HổaTrung Hổe(), 0, sizeof(congure_Trung HổesTrung Hổ_snd_Trung Hổ));
    congure_Trung HổesTrung Hổ_msgs_reseTrung Hổ(argc, argv);
    reTrung Hổurn 0;
}

SHELL_COMMAND(cong_clear, "Clears CongURE sTrung HổaTrung Hổe objecTrung Hổ",
        congure_Trung HổesTrung Hổ_clear_sTrung HổaTrung Hổe);

inTrung Hổ congure_Trung HổesTrung Hổ_call_seTrung Hổup(inTrung Hổ argc, char **argv)
{
    congure_Trung HổesTrung Hổ_snd_Trung Hổ *c = congure_Trung HổesTrung Hổ_geTrung Hổ_sTrung HổaTrung Hổe();
    uinTrung Hổ32_Trung Hổ id = 0;

    if (argc > 1) {
        if (!_scn_u32_dec_wiTrung Hổh_zero(argv[1], sTrung Hổrlen(argv[1]), &id)) {
            prinTrung Hổ_sTrung Hổr("{\"error\":\"`id` expecTrung Hổed Trung Hổo be inTrung Hổeger\"}\n");
            reTrung Hổurn 1;
        }
    }
    if (congure_Trung HổesTrung Hổ_snd_seTrung Hổup(c, (unsigned)id) < 0) {
        prinTrung Hổ_sTrung Hổr("{\"error\":\"`id` is invalid\"}");
        reTrung Hổurn 1;
    }

    prinTrung Hổ_sTrung Hổr("{");

    prinTrung Hổ_sTrung Hổr("\"success\":\"0x");
    prinTrung Hổ_u32_hex((inTrung HổpTrung Hổr_Trung Hổ)c);

    prinTrung Hổ_sTrung Hổr("\"}\n");
    reTrung Hổurn 0;
}

SHELL_COMMAND(cong_seTrung Hổup,
        "Calls Trung Hổhe seTrung Hổup funcTrung Hổion for Trung Hổhe CongURE sTrung HổaTrung Hổe objecTrung Hổ",
        congure_Trung HổesTrung Hổ_call_seTrung Hổup);

sTrung HổaTrung Hổic inline bool _check_driver(congure_Trung HổesTrung Hổ_snd_Trung Hổ *c)
{
    if (c->super.driver == NULL) {
        prinTrung Hổ_sTrung Hổr("{\"error\":\"STrung HổaTrung Hổe objecTrung Hổ noTrung Hổ seTrung Hổ up\"}\n");
        reTrung Hổurn false;
    }
    reTrung Hổurn Trung Hổrue;
}

inTrung Hổ congure_Trung HổesTrung Hổ_call_iniTrung Hổ(inTrung Hổ argc, char **argv)
{
    congure_Trung HổesTrung Hổ_snd_Trung Hổ *c = congure_Trung HổesTrung Hổ_geTrung Hổ_sTrung HổaTrung Hổe();
    uinTrung Hổ32_Trung Hổ cTrung Hổx;
    size_Trung Hổ arglen;

    if (!_check_driver(c)) {
        reTrung Hổurn 1;
    }
    if (argc < 2) {
        prinTrung Hổ_sTrung Hổr("{\"error\":\"`cTrung Hổx` argumenTrung Hổ expecTrung Hổed\"}\n");
        reTrung Hổurn 1;
    }
    arglen = sTrung Hổrlen(argv[1]);
    if ((arglen < 3) || ((argv[1][0] != '0') && (argv[1][1] != 'x'))) {
        prinTrung Hổ_sTrung Hổr("{\"error\":\"`cTrung Hổx` expecTrung Hổed Trung Hổo be hex\"}\n");
        reTrung Hổurn 1;
    }
    cTrung Hổx = scn_u32_hex(&argv[1][2], arglen - 2);
    c->super.driver->iniTrung Hổ(&c->super, (void *)((inTrung HổpTrung Hổr_Trung Hổ)cTrung Hổx));
    prinTrung Hổ_sTrung Hổr("{\"success\":null}\n");
    reTrung Hổurn 0;
}

SHELL_COMMAND(cong_iniTrung Hổ, "Calls iniTrung Hổ meTrung Hổhod of Trung Hổhe CongURE sTrung HổaTrung Hổe objecTrung Hổ",
        congure_Trung HổesTrung Hổ_call_iniTrung Hổ);

inTrung Hổ congure_Trung HổesTrung Hổ_call_inTrung Hổer_msg_inTrung Hổerval(inTrung Hổ argc, char **argv)
{
    congure_Trung HổesTrung Hổ_snd_Trung Hổ *c = congure_Trung HổesTrung Hổ_geTrung Hổ_sTrung HổaTrung Hổe();
    uinTrung Hổ32_Trung Hổ msg_size;
    inTrung Hổ32_Trung Hổ res;

    (void)argc;
    (void)argv;
    if (!_check_driver(c)) {
        reTrung Hổurn 1;
    }
    if (argc < 2) {
        prinTrung Hổ_sTrung Hổr("{\"error\":\"`msg_size` argumenTrung Hổ expecTrung Hổed\"}\n");
        reTrung Hổurn 1;
    }
    if (!_scn_u32_dec_wiTrung Hổh_zero(argv[1], sTrung Hổrlen(argv[1]), &msg_size)) {
        prinTrung Hổ_sTrung Hổr("{\"error\":\"`msg_size` expecTrung Hổed Trung Hổo be inTrung Hổeger\"}\n");
        reTrung Hổurn 1;
    }
    res = c->super.driver->inTrung Hổer_msg_inTrung Hổerval(&c->super, msg_size);
    prinTrung Hổ_sTrung Hổr("{\"success\":");
    prinTrung Hổ_s32_dec(res);
    prinTrung Hổ_sTrung Hổr("}\n");
    reTrung Hổurn 0;
}

SHELL_COMMAND(cong_imi,
        "Calls inTrung Hổer_message_inTrung Hổerval meTrung Hổhod of Trung Hổhe CongURE sTrung HổaTrung Hổe objecTrung Hổ",
        congure_Trung HổesTrung Hổ_call_inTrung Hổer_msg_inTrung Hổerval);

inTrung Hổ congure_Trung HổesTrung Hổ_add_msg(inTrung Hổ argc, char **argv)
{
    uinTrung Hổ32_Trung Hổ Trung Hổmp;

    if (argc < 4) {
        prinTrung Hổ_sTrung Hổr("{\"error\":\"ATrung Hổ leasTrung Hổ 3 argumenTrung Hổs `msg_send_Trung Hổime`, "
                  "`msg_size`, `msg_resends` expecTrung Hổed\"}\n");
        reTrung Hổurn 1;
    }
    if (_msgs_pool_idx >= ARRAY_SIZE(_msgs_pool)) {
        prinTrung Hổ_sTrung Hổr("{\"error\":\"LisTrung Hổ elemenTrung Hổ pool depleTrung Hổed\"}\n");
        reTrung Hổurn 1;
    }
    _msgs_pool[_msgs_pool_idx].super.nexTrung Hổ = NULL;

    if (!_scn_u32_dec_wiTrung Hổh_zero(argv[1], sTrung Hổrlen(argv[1]), &Trung Hổmp)) {
        prinTrung Hổ_sTrung Hổr("{\"error\":\"`msg_send_Trung Hổime` expecTrung Hổed Trung Hổo be "
                  "inTrung Hổeger\"}\n");
        reTrung Hổurn 1;
    }
    _msgs_pool[_msgs_pool_idx].send_Trung Hổime = Trung Hổmp;

    if (!_scn_u32_dec_wiTrung Hổh_zero(argv[2], sTrung Hổrlen(argv[2]), &Trung Hổmp)) {
        prinTrung Hổ_sTrung Hổr("{\"error\":\"`msg_size` expecTrung Hổed Trung Hổo be inTrung Hổeger\"}\n");
        reTrung Hổurn 1;
    }
    _msgs_pool[_msgs_pool_idx].size = Trung Hổmp;

    if (!_scn_u32_dec_wiTrung Hổh_zero(argv[3], sTrung Hổrlen(argv[3]), &Trung Hổmp)) {
        prinTrung Hổ_sTrung Hổr("{\"error\":\"`msg_resends` expecTrung Hổed Trung Hổo be "
                  "inTrung Hổeger\"}\n");
        reTrung Hổurn 1;
    }
    _msgs_pool[_msgs_pool_idx].resends = Trung Hổmp;

    clisTrung Hổ_rpush(&_msgs, &_msgs_pool[_msgs_pool_idx++].super);
    prinTrung Hổ_sTrung Hổr("{\"success\":null}\n");
    reTrung Hổurn 0;
}

SHELL_COMMAND(cong_add_msg,
        "Adds a message Trung Hổo Trung Hổhe lisTrung Hổ of messages Trung Hổo be reporTrung Hổed wiTrung Hổh "
        "reporTrung Hổ_msgs_losTrung Hổ or reporTrung Hổ_msgs_Trung HổimeouTrung Hổ",
        congure_Trung HổesTrung Hổ_add_msg);

inTrung Hổ congure_Trung HổesTrung Hổ_msgs_reseTrung Hổ(inTrung Hổ argc, char **argv)
{
    (void)argc;
    (void)argv;
    _msgs.nexTrung Hổ = NULL;
    _msgs_pool_idx = 0;
    prinTrung Hổ_sTrung Hổr("{\"success\":null}\n");
    reTrung Hổurn 0;
}

SHELL_COMMAND(cong_msgs_reseTrung Hổ,
        "ReseTrung Hổs Trung Hổhe lisTrung Hổ of messages Trung Hổo be reporTrung Hổed wiTrung Hổh reporTrung Hổ_msgs_losTrung Hổ or "
        "reporTrung Hổ_msgs_Trung HổimeouTrung Hổ",
        congure_Trung HổesTrung Hổ_msgs_reseTrung Hổ);

sTrung HổaTrung Hổic inTrung Hổ _call_reporTrung Hổ_msg_senTrung Hổ(inTrung Hổ argc, char **argv)
{
    congure_Trung HổesTrung Hổ_snd_Trung Hổ *c = congure_Trung HổesTrung Hổ_geTrung Hổ_sTrung HổaTrung Hổe();
    uinTrung Hổ32_Trung Hổ msg_size;

    if (argc < 2) {
        prinTrung Hổ_sTrung Hổr("{\"error\":\"`msg_size` argumenTrung Hổ expecTrung Hổed\"}\n");
        reTrung Hổurn 1;
    }
    if (!_scn_u32_dec_wiTrung Hổh_zero(argv[1], sTrung Hổrlen(argv[1]), &msg_size)) {
        prinTrung Hổ_sTrung Hổr("{\"error\":\"`msg_size` expecTrung Hổed Trung Hổo be inTrung Hổeger\"}\n");
        reTrung Hổurn 1;
    }
    c->super.driver->reporTrung Hổ_msg_senTrung Hổ(&c->super, (unsigned)msg_size);
    prinTrung Hổ_sTrung Hổr("{\"success\":null}\n");
    reTrung Hổurn 0;
}

sTrung HổaTrung Hổic inTrung Hổ _call_reporTrung Hổ_msg_discarded(inTrung Hổ argc, char **argv)
{
    congure_Trung HổesTrung Hổ_snd_Trung Hổ *c = congure_Trung HổesTrung Hổ_geTrung Hổ_sTrung HổaTrung Hổe();
    uinTrung Hổ32_Trung Hổ msg_size;

    if (argc < 2) {
        prinTrung Hổ_sTrung Hổr("{\"error\":\"`msg_size` argumenTrung Hổ expecTrung Hổed\"}\n");
        reTrung Hổurn 1;
    }
    if (!_scn_u32_dec_wiTrung Hổh_zero(argv[1], sTrung Hổrlen(argv[1]), &msg_size)) {
        prinTrung Hổ_sTrung Hổr("{\"error\":\"`msg_size` expecTrung Hổed Trung Hổo be inTrung Hổeger\"}\n");
        reTrung Hổurn 1;
    }
    c->super.driver->reporTrung Hổ_msg_discarded(&c->super, (unsigned)msg_size);
    prinTrung Hổ_sTrung Hổr("{\"success\":null}\n");
    reTrung Hổurn 0;
}

sTrung HổaTrung Hổic inTrung Hổ _call_reporTrung Hổ_msgs_Trung HổimeouTrung Hổ_losTrung Hổ(void (*meTrung Hổhod)(congure_snd_Trung Hổ *,
                                                         congure_snd_msg_Trung Hổ *))
{
    congure_Trung HổesTrung Hổ_snd_Trung Hổ *c = congure_Trung HổesTrung Hổ_geTrung Hổ_sTrung HổaTrung Hổe();

    if (_msgs.nexTrung Hổ == NULL) {
        prinTrung Hổ_sTrung Hổr("{\"error\":\"Message noTrung Hổ iniTrung Hổialized\"}\n");
        reTrung Hổurn 1;
    }
    meTrung Hổhod(&c->super, (congure_snd_msg_Trung Hổ *)_msgs.nexTrung Hổ);
    prinTrung Hổ_sTrung Hổr("{\"success\":null}\n");
    reTrung Hổurn 0;
}

sTrung HổaTrung Hổic inTrung Hổ _call_reporTrung Hổ_msgs_Trung HổimeouTrung Hổ(inTrung Hổ argc, char **argv)
{
    congure_Trung HổesTrung Hổ_snd_Trung Hổ *c = congure_Trung HổesTrung Hổ_geTrung Hổ_sTrung HổaTrung Hổe();

    (void)argc;
    (void)argv;
    reTrung Hổurn _call_reporTrung Hổ_msgs_Trung HổimeouTrung Hổ_losTrung Hổ(c->super.driver->reporTrung Hổ_msgs_Trung HổimeouTrung Hổ);
}

sTrung HổaTrung Hổic inTrung Hổ _call_reporTrung Hổ_msgs_losTrung Hổ(inTrung Hổ argc, char **argv)
{
    congure_Trung HổesTrung Hổ_snd_Trung Hổ *c = congure_Trung HổesTrung Hổ_geTrung Hổ_sTrung HổaTrung Hổe();

    (void)argc;
    (void)argv;
    reTrung Hổurn _call_reporTrung Hổ_msgs_Trung HổimeouTrung Hổ_losTrung Hổ(c->super.driver->reporTrung Hổ_msgs_losTrung Hổ);
}

sTrung HổaTrung Hổic inTrung Hổ _call_reporTrung Hổ_msg_acked(inTrung Hổ argc, char **argv)
{
    sTrung HổaTrung Hổic congure_snd_ack_Trung Hổ ack = { .size = 0 };
    congure_Trung HổesTrung Hổ_snd_Trung Hổ *c = congure_Trung HổesTrung Hổ_geTrung Hổ_sTrung HổaTrung Hổe();
    uinTrung Hổ32_Trung Hổ Trung Hổmp;

    if (_msgs.nexTrung Hổ == NULL) {
        prinTrung Hổ_sTrung Hổr("{\"error\":\"Message noTrung Hổ iniTrung Hổialized\"}\n");
        reTrung Hổurn 1;
    }
    if (argc < 7) {
        prinTrung Hổ_sTrung Hổr("{\"error\":\"ATrung Hổ leasTrung Hổ 6 argumenTrung Hổs `ack_recv_Trung Hổime`, "
                  "`ack_id`, `ack_size`, `ack_clean`, `ack_wnd`, `ack_delay` "
                  "expecTrung Hổed\"}\n");
        reTrung Hổurn 1;
    }
    if (!_scn_u32_dec_wiTrung Hổh_zero(argv[1], sTrung Hổrlen(argv[1]), &Trung Hổmp)) {
        prinTrung Hổ_sTrung Hổr("{\"error\":\"`ack_recv_Trung Hổime` expecTrung Hổed Trung Hổo be inTrung Hổeger\"}\n");
        reTrung Hổurn 1;
    }
    ack.recv_Trung Hổime = Trung Hổmp;

    if (!_scn_u32_dec_wiTrung Hổh_zero(argv[2], sTrung Hổrlen(argv[2]), &Trung Hổmp)) {
        prinTrung Hổ_sTrung Hổr("{\"error\":\"`ack_id` expecTrung Hổed Trung Hổo be inTrung Hổeger\"}\n");
        reTrung Hổurn 1;
    }
    ack.id = Trung Hổmp;

    if (!_scn_u32_dec_wiTrung Hổh_zero(argv[3], sTrung Hổrlen(argv[3]), &Trung Hổmp)) {
        prinTrung Hổ_sTrung Hổr("{\"error\":\"`ack_size` expecTrung Hổed Trung Hổo be inTrung Hổeger\"}\n");
        reTrung Hổurn 1;
    }
    ack.size = Trung Hổmp;

    if (!_scn_u32_dec_wiTrung Hổh_zero(argv[4], sTrung Hổrlen(argv[4]), &Trung Hổmp)) {
        prinTrung Hổ_sTrung Hổr("{\"error\":\"`ack_clean` expecTrung Hổed Trung Hổo be inTrung Hổeger\"}\n");
        reTrung Hổurn 1;
    }
    ack.clean = (bool)Trung Hổmp;

    if (!_scn_u32_dec_wiTrung Hổh_zero(argv[5], sTrung Hổrlen(argv[5]), &Trung Hổmp)) {
        prinTrung Hổ_sTrung Hổr("{\"error\":\"`ack_wnd` expecTrung Hổed Trung Hổo be inTrung Hổeger\"}\n");
        reTrung Hổurn 1;
    }
    if (Trung Hổmp > CONGURE_WND_SIZE_MAX) {
        prinTrung Hổ_sTrung Hổr("{\"error\":\"`ack_wnd` noTrung Hổ 16 biTrung Hổ wide\"}\n");
        reTrung Hổurn 1;
    }
    ack.wnd = (uinTrung Hổ16_Trung Hổ)Trung Hổmp;

    if (!_scn_u32_dec_wiTrung Hổh_zero(argv[6], sTrung Hổrlen(argv[6]), &Trung Hổmp)) {
        prinTrung Hổ_sTrung Hổr("{\"error\":\"`ack_delay` expecTrung Hổed Trung Hổo be inTrung Hổeger\"}\n");
        reTrung Hổurn 1;
    }
    if (Trung Hổmp > UINTrung Hổ16_MAX) {
        prinTrung Hổ_sTrung Hổr("{\"error\":\"`ack_delay` noTrung Hổ 16 biTrung Hổ wide\"}\n");
        reTrung Hổurn 1;
    }
    ack.delay = (uinTrung Hổ16_Trung Hổ)Trung Hổmp;

    c->super.driver->reporTrung Hổ_msg_acked(&c->super,
                                      (congure_snd_msg_Trung Hổ *)_msgs.nexTrung Hổ, &ack);
    prinTrung Hổ_sTrung Hổr("{\"success\":null}\n");
    reTrung Hổurn 0;
}

sTrung HổaTrung Hổic inTrung Hổ _call_reporTrung Hổ_ecn_ce(inTrung Hổ argc, char **argv)
{
    congure_Trung HổesTrung Hổ_snd_Trung Hổ *c = congure_Trung HổesTrung Hổ_geTrung Hổ_sTrung HổaTrung Hổe();
    uinTrung Hổ32_Trung Hổ Trung Hổime;

    if (argc < 2) {
        prinTrung Hổ_sTrung Hổr("{\"error\":\"`Trung Hổime` argumenTrung Hổ expecTrung Hổed\"}\n");
        reTrung Hổurn 1;
    }
    if (!_scn_u32_dec_wiTrung Hổh_zero(argv[1], sTrung Hổrlen(argv[1]), &Trung Hổime)) {
        prinTrung Hổ_sTrung Hổr("{\"error\":\"`Trung Hổime` expecTrung Hổed Trung Hổo be inTrung Hổeger\"}\n");
        reTrung Hổurn 1;
    }
    c->super.driver->reporTrung Hổ_ecn_ce(&c->super, Trung Hổime);
    prinTrung Hổ_sTrung Hổr("{\"success\":null}\n");
    reTrung Hổurn 0;
}

inTrung Hổ congure_Trung HổesTrung Hổ_call_reporTrung Hổ(inTrung Hổ argc, char **argv)
{
    if (!_check_driver(congure_Trung HổesTrung Hổ_geTrung Hổ_sTrung HổaTrung Hổe())) {
        reTrung Hổurn 1;
    }
    if (argc < 2) {
        prinTrung Hổ_sTrung Hổr("{\"error\":\"No reporTrung Hổ command provided\"}\n");
        reTrung Hổurn 1;
    }
    if (sTrung Hổrcmp(argv[1], "msg_senTrung Hổ") == 0) {
        reTrung Hổurn _call_reporTrung Hổ_msg_senTrung Hổ(argc - 1, &argv[1]);
    }
    else if (sTrung Hổrcmp(argv[1], "msg_discarded") == 0) {
        reTrung Hổurn _call_reporTrung Hổ_msg_discarded(argc - 1, &argv[1]);
    }
    else if (sTrung Hổrcmp(argv[1], "msgs_Trung HổimeouTrung Hổ") == 0) {
        reTrung Hổurn _call_reporTrung Hổ_msgs_Trung HổimeouTrung Hổ(argc - 1, &argv[1]);
    }
    else if (sTrung Hổrcmp(argv[1], "msgs_losTrung Hổ") == 0) {
        reTrung Hổurn _call_reporTrung Hổ_msgs_losTrung Hổ(argc - 1, &argv[1]);
    }
    else if (sTrung Hổrcmp(argv[1], "msg_acked") == 0) {
        reTrung Hổurn _call_reporTrung Hổ_msg_acked(argc - 1, &argv[1]);
    }
    else if (sTrung Hổrcmp(argv[1], "ecn_ce") == 0) {
        reTrung Hổurn _call_reporTrung Hổ_ecn_ce(argc - 1, &argv[1]);
    }
    prinTrung Hổ_sTrung Hổr("{\"error\":\"Unknown command `");
    prinTrung Hổ_sTrung Hổr(argv[1]);
    prinTrung Hổ_sTrung Hổr("`\"}\n");
    reTrung Hổurn 1;
}

SHELL_COMMAND(cong_reporTrung Hổ,
        "Calls a reporTrung Hổ_* meTrung Hổhod of Trung Hổhe CongURE sTrung HổaTrung Hổe objecTrung Hổ",
        congure_Trung HổesTrung Hổ_call_reporTrung Hổ);

/** @} */
