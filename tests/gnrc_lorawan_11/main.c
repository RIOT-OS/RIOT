/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @author      Jose I. Alamos <jose.alamos@haw-hamburg.de>
 * @file
 */
#include <stdio.h>
#include <string.h>
#include "embUnit.h"

#include "net/gnrc/lorawan.h"
#include "net/lorawan/hdr.h"

static uint8_t fnwksintkey[] =
{ 0x59, 0x42, 0x2d, 0x58, 0x68, 0x5f, 0x7d, 0x6e, 0x9f, 0xb6, 0x91, 0x9a, 0x7b, 0x3f, 0x44, 0x8a };
static uint8_t snwksintkey[] =
{ 0xc0, 0x9f, 0x9e, 0x9a, 0x13, 0x91, 0xae, 0xcc, 0x54, 0xdb, 0x49, 0x0e, 0x11, 0x26, 0x1f, 0x21 };

static uint8_t lorawan_packet_no_mic[] =
{ 0x40, 0x30, 0xe2, 0xde, 0x00, 0x00, 0x00, 0x00, 0x01, 0x3b, 0xc5, 0xf0, 0xa0, 0x69, 0x49, 0x66,
  0xee, 0x00, 0xc1, 0xaa, 0x0d, 0xee, 0x20 };
static uint8_t lorawan_packet_wrong[] =
{ 0x40, 0x30, 0xe2, 0xde, 0x00, 0x00, 0x00, 0xaa, 0x01, 0x3b, 0xc5, 0xf0, 0xa0, 0x69, 0x49, 0x66,
  0xee, 0x00, 0xc1, 0xaa, 0x0d, 0xee, 0x20 };

static uint8_t mic[] = { 0x12, 0xcd, 0x3c, 0x8a };
static le_uint32_t dev_addr = {
    .u8 = { 0x30, 0xe2, 0xde, 0x00 }
};
static uint8_t fopts[] = { 0x02, 0x41, 0x5 };
static uint8_t wrong_fopts[] = { 0x00, 0x41, 0x5 };
static uint16_t fcnt = 0x00;

static uint16_t conf_fcnt = 0x00;
static uint8_t tx_dr = 0x00;
static uint8_t tx_ch = 0x04;

static void (*mlme_confirm_cb)(gnrc_lorawan_t *mac, mlme_confirm_t *confirm);
static bool mlme_confirm_exec;

/* Callback function required by GNRC LoRaWAN */
void gnrc_lorawan_mlme_confirm(gnrc_lorawan_t *mac, mlme_confirm_t *confirm)
{
    TEST_ASSERT(mlme_confirm_cb);
    mlme_confirm_cb(mac, confirm);
    mlme_confirm_exec = true;
}

static void _cb__gnrc_lorawan_fopts__perform(gnrc_lorawan_t *mac, mlme_confirm_t *confirm)
{
    (void)mac;
    TEST_ASSERT(confirm->type == MLME_LINK_CHECK);
    TEST_ASSERT(confirm->status == GNRC_LORAWAN_REQ_STATUS_SUCCESS);
    TEST_ASSERT(confirm->link_req.margin == 0x41);
    TEST_ASSERT(confirm->link_req.num_gateways == 5);
}

void set_up(void)
{
    mlme_confirm_exec = false;
    mlme_confirm_cb = NULL;
}

static void test_gnrc_lorawan__validate_mic(void)
{
    iolist_t pkt = { .iol_base = lorawan_packet_no_mic,
                     .iol_len = sizeof(lorawan_packet_no_mic),
                     .iol_next = NULL };

    /* Uplink packet */
    le_uint32_t calc_mic;

    gnrc_lorawan_t mac = { 0 };

    memcpy(&mac.dev_addr, &dev_addr, sizeof(dev_addr));
    mac.ctx.fnwksintkey = fnwksintkey;
    mac.ctx.snwksintkey = snwksintkey;
    mac.last_dr = tx_dr;
    mac.last_chan_idx = tx_ch;
    mac.mcps.fcnt = fcnt;
    mac.optneg = true;

    gnrc_lorawan_calculate_mic_uplink(&pkt, conf_fcnt, &mac, &calc_mic);

    TEST_ASSERT(memcmp(&calc_mic, mic, sizeof(le_uint32_t)) == 0);
}

static void test_gnrc_lorawan__wrong_mic(void)
{
    iolist_t pkt = { .iol_base = lorawan_packet_wrong,
                     .iol_len = sizeof(lorawan_packet_wrong),
                     .iol_next = NULL };

    /* Uplink packet */
    le_uint32_t calc_mic;

    gnrc_lorawan_t mac = { 0 };

    memcpy(&mac.dev_addr, &dev_addr, sizeof(dev_addr));
    mac.ctx.fnwksintkey = fnwksintkey;
    mac.ctx.snwksintkey = snwksintkey;
    mac.last_dr = tx_dr;
    mac.last_chan_idx = tx_ch;
    mac.mcps.fcnt = fcnt;

    gnrc_lorawan_calculate_mic_uplink(&pkt, conf_fcnt, &mac, &calc_mic);

    TEST_ASSERT(memcmp(&calc_mic, mic, sizeof(le_uint32_t)) != 0);
}

static void test_gnrc_lorawan__build_hdr(void)
{
    uint8_t buf[sizeof(lorawan_hdr_t)];

    lorawan_buffer_t lw_buf = {
        .data = buf,
        .size = sizeof(buf),
        .index = 0
    };

    size_t size = gnrc_lorawan_build_hdr(MTYPE_UNCNF_UPLINK, &dev_addr, fcnt, 0, 0, &lw_buf);

    TEST_ASSERT(size == sizeof(lorawan_hdr_t));
    TEST_ASSERT(memcmp(lw_buf.data, lorawan_packet_no_mic, sizeof(lorawan_hdr_t)) == 0);
}

static void test_gnrc_lorawan_fopts__mlme_link_check_req(void)
{
    uint8_t buf[1];
    lorawan_buffer_t lw_buf = {
        .data = buf,
        .size = sizeof(buf),
        .index = 0
    };

    gnrc_lorawan_t mac = { 0 };
    uint8_t size;

    size = gnrc_lorawan_build_options(&mac, NULL);

    TEST_ASSERT(size == 0);
    TEST_ASSERT(lw_buf.index == 0);

    mac.mlme.pending_mlme_opts = GNRC_LORAWAN_MLME_OPTS_LINK_CHECK_REQ;
    size = gnrc_lorawan_build_options(&mac, NULL);

    TEST_ASSERT(size == 1);
    TEST_ASSERT(lw_buf.index == 0);

    size = gnrc_lorawan_build_options(&mac, &lw_buf);
    TEST_ASSERT(size == 1);
    TEST_ASSERT(lw_buf.index == 1);
    TEST_ASSERT(*buf == 0x02);
}

static void test_gnrc_lorawan_fopts__perform(void)
{
    gnrc_lorawan_t mac;

    mac.mlme.pending_mlme_opts = GNRC_LORAWAN_MLME_OPTS_LINK_CHECK_REQ;

    mlme_confirm_exec = false;
    mlme_confirm_cb = _cb__gnrc_lorawan_fopts__perform;
    gnrc_lorawan_process_fopts(&mac, fopts, sizeof(fopts));

    TEST_ASSERT(!(mac.mlme.pending_mlme_opts & GNRC_LORAWAN_MLME_OPTS_LINK_CHECK_REQ));
    TEST_ASSERT(mlme_confirm_exec);
}

static void test_gnrc_lorawan_fopts__perform_wrong(void)
{
    gnrc_lorawan_t mac;

    mac.mlme.pending_mlme_opts = GNRC_LORAWAN_MLME_OPTS_LINK_CHECK_REQ;

    gnrc_lorawan_process_fopts(&mac, wrong_fopts, sizeof(wrong_fopts));

    TEST_ASSERT(mac.mlme.pending_mlme_opts & GNRC_LORAWAN_MLME_OPTS_LINK_CHECK_REQ);
}

Test *tests_gnrc_lorawan_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_gnrc_lorawan__validate_mic),
        new_TestFixture(test_gnrc_lorawan__wrong_mic),
        new_TestFixture(test_gnrc_lorawan__build_hdr),
        new_TestFixture(test_gnrc_lorawan_fopts__mlme_link_check_req),
        new_TestFixture(test_gnrc_lorawan_fopts__perform),
        new_TestFixture(test_gnrc_lorawan_fopts__perform_wrong),
    };

    EMB_UNIT_TESTCALLER(gnrc_lorawan_tests, set_up, NULL, fixtures);

    return (Test *)&gnrc_lorawan_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_gnrc_lorawan_tests());
    TESTS_END();

    return 0;
}
/** @} */
