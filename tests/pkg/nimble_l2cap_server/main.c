/*
 * SPDX-FileCopyrightText: 2019 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Server to test and benchmark raw L2CAP COC for NimBLE
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>

#include "nimble_riot.h"
#include "host/ble_hs.h"
#include "host/ble_gap.h"
#include "host/util/util.h"

#include "test_utils/expect.h"
#include "net/bluetil/ad.h"

#include "nimble_l2cap_test_conf.h"

/* BLE connection state */
static uint16_t _handle = 0;
static struct ble_l2cap_chan *_coc = NULL;

/* buffer allocation */
static os_membuf_t _coc_mem[OS_MEMPOOL_SIZE(MBUFCNT, MBUFSIZE)];
static struct os_mempool _coc_mempool;
static struct os_mbuf_pool _coc_mbuf_pool;
static uint32_t _rxbuf[APP_MTU / 4];

/* safe AD fields */
static uint8_t _ad_buf[BLE_HS_ADV_MAX_SZ];
static bluetil_ad_t _ad;
static struct ble_gap_adv_params _adv_params = { 0 };

static void _advertise_now(void);

static void _on_data(struct ble_l2cap_event *event)
{
    int res;
    struct os_mbuf *rxd;

    rxd = event->receive.sdu_rx;
    expect(rxd != NULL);
    int rx_len = (int)OS_MBUF_PKTLEN(rxd);
    expect(rx_len <= (int)APP_MTU);

    res = os_mbuf_copydata(rxd, 0, rx_len, _rxbuf);
    expect(res == 0);

    res = ble_l2cap_send(_coc, rxd);
    expect((res == 0) || (res == BLE_HS_ESTALLED));

    /* allocate new mbuf for receiving new data */
    rxd = os_mbuf_get_pkthdr(&_coc_mbuf_pool, 0);
    expect(rxd != NULL);
    res = ble_l2cap_recv_ready(_coc, rxd);
    expect(res == 0);

    printf("# Received: len %5i, seq %5u\n", rx_len, (unsigned)_rxbuf[POS_SEQ]);
}

static int _on_gap_evt(struct ble_gap_event *event, void *arg)
{
    (void)arg;
    printf("# GAP event %i\n", (int)event->type);

    switch (event->type) {
        case BLE_GAP_EVENT_CONNECT:
            _handle = event->connect.conn_handle;
            break;
        case BLE_GAP_EVENT_DISCONNECT:
            _coc = NULL;
            _handle = 0;
            _advertise_now();
            break;
        case BLE_GAP_EVENT_CONN_UPDATE:
        case BLE_GAP_EVENT_CONN_UPDATE_REQ:
        default:
            break;
    }
    return 0;
}

static int _on_l2cap_evt(struct ble_l2cap_event *event, void *arg)
{
    (void)arg;

    switch (event->type) {
        case BLE_L2CAP_EVENT_COC_CONNECTED: {
            _coc = event->connect.chan;
            struct ble_l2cap_chan_info info;
            ble_l2cap_get_chan_info(_coc, &info);
            puts("# L2CAP: CONNECTED");
            printf("# MTUs: our %i, remote %i\n",
                   (int)info.our_l2cap_mtu, (int)info.peer_l2cap_mtu);
            break;
        }
        case BLE_L2CAP_EVENT_COC_DISCONNECTED:
            _coc = NULL;
            puts("# L2CAP: DISCONNECTED");
            break;
        case BLE_L2CAP_EVENT_COC_ACCEPT: {
            struct os_mbuf *sdu_rx = os_mbuf_get_pkthdr(&_coc_mbuf_pool, 0);
            expect(sdu_rx != NULL);
            ble_l2cap_recv_ready(event->accept.chan, sdu_rx);
            break;
        }
        case BLE_L2CAP_EVENT_COC_DATA_RECEIVED:
            _on_data(event);
            break;
        case BLE_L2CAP_EVENT_COC_TX_UNSTALLED:
            /* this event is expected, but we have nothing to do here */
            break;
        default:
            expect(0);
            break;
    }

    return 0;
}

static void _advertise_now(void)
{
    int res = ble_gap_adv_start(nimble_riot_own_addr_type, NULL, BLE_HS_FOREVER,
                                &_adv_params, _on_gap_evt, NULL);
    expect(res == 0);
}

int main(void)
{
    int res;
    (void)res;
    puts("NimBLE L2CAP test server");

    /* initialize buffers and setup the test environment */
    res = os_mempool_init(&_coc_mempool, MBUFCNT, MBUFSIZE, _coc_mem, "appbuf");
    expect(res == 0);
    res = os_mbuf_pool_init(&_coc_mbuf_pool, &_coc_mempool, MBUFSIZE, MBUFCNT);
    expect(res == 0);

    /* create l2cap server */
    res = ble_l2cap_create_server(APP_CID, APP_MTU, _on_l2cap_evt, NULL);
    expect(res == 0);

    /* initialize advertising data and parameters */
    _adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    _adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    bluetil_ad_init_with_flags(&_ad, _ad_buf, sizeof(_ad_buf),
                               BLUETIL_AD_FLAGS_DEFAULT);
    bluetil_ad_add_name(&_ad, APP_NODENAME);
    res = ble_gap_adv_set_data(_ad.buf, (int)_ad.pos);
    expect(res == 0);

    /* start advertising the test server */
    _advertise_now();
    puts("# now advertising");

    /* nothing else to be done in the main thread */
    return 0;
}
