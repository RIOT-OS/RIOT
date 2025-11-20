/*
 * SPDX-FileCopyrightText: 2021 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
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

static uint8_t nwkskey[] =
{ 0x44, 0x02, 0x42, 0x41, 0xed, 0x4c, 0xe9, 0xa6, 0x8c, 0x6a, 0x8b, 0xc0, 0x55, 0x23, 0x3f, 0xd3 };
static uint8_t lorawan_packet_no_mic[] =
{ 0x40, 0xF1, 0x7D, 0xBE, 0x49, 0x00, 0x02, 0x00, 0x01, 0x95, 0x43, 0x78, 0x76 };
static uint8_t lorawan_packet_wrong[] =
{ 0x40, 0xF1, 0x7D, 0x00, 0x49, 0x00, 0x02, 0xAA, 0x01, 0x95, 0x43, 0x78, 0x76 };
static uint8_t mic[] = { 0x2B, 0x11, 0xFF, 0x0D };
static le_uint32_t dev_addr = {
    .u8 =  { 0xF1, 0x7D, 0xBE, 0x49 }
};

static uint8_t fopts[] = { 0x02, 0x41, 0x5 };
static uint8_t wrong_fopts[] = { 0x00, 0x41, 0x5 };

static uint16_t fcnt = 2;

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
    mac.ctx.fnwksintkey = nwkskey;
    mac.mcps.fcnt = fcnt;

    gnrc_lorawan_calculate_mic_uplink(&pkt, 0x00, &mac, &calc_mic);

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
    mac.ctx.fnwksintkey = nwkskey;
    mac.mcps.fcnt = fcnt;

    gnrc_lorawan_calculate_mic_uplink(&pkt, 0x00, &mac, &calc_mic);

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

    size_t size = gnrc_lorawan_build_hdr(MTYPE_UNCNF_UPLINK, &dev_addr, 2, 0, 0, &lw_buf);

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
