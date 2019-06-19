/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Client to test and benchmark raw L2CAP COC for NimBLE
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nimble_riot.h"
#include "host/ble_hs.h"
#include "host/ble_gap.h"
#include "host/util/util.h"

#include "assert.h"
#include "shell.h"
#include "net/bluetil/ad.h"

#include "nimble_l2cap_test_conf.h"

#define ENABLE_DEBUG        (1)
#include "debug.h"

#define FLAG_UP         (1u << 0)
#define FLAG_SYNC       (1u << 1)

/* synchronization state */
static thread_t *_main;
static volatile uint32_t _last_rx_seq;

/* BLE connection state */
static uint16_t _handle = 0;
static struct ble_l2cap_chan *_coc = NULL;

/* buffer allocation */
static os_membuf_t _coc_mem[OS_MEMPOOL_SIZE(MBUFCNT, MBUFSIZE)];
static struct os_mempool _coc_mempool;
static struct os_mbuf_pool _coc_mbuf_pool;
static uint32_t _rxbuf[APP_MTU / 4];
static uint32_t _txbuf[APP_MTU / 4];

static void _on_data(struct ble_l2cap_event *event)
{
    int res;
    (void)res;
    struct os_mbuf *rxd = event->receive.sdu_rx;
    assert(rxd != NULL);
    int rx_len = (int)OS_MBUF_PKTLEN(rxd);
    assert(rx_len <= (int)APP_MTU);

    res = os_mbuf_copydata(rxd, 0, rx_len, _rxbuf);
    assert(res == 0);
    _last_rx_seq = _rxbuf[POS_SEQ];
    if (_rxbuf[POS_TYPE] == TYPE_INCTEST) {
        thread_flags_set(_main, FLAG_SYNC);
    }

    /* free buffer */
    res = os_mbuf_free_chain(rxd);
    assert(res == 0);
    rxd = os_mbuf_get_pkthdr(&_coc_mbuf_pool, 0);
    assert(rxd != NULL);
    res = ble_l2cap_recv_ready(_coc, rxd);
    assert(res == 0);
}

static int _on_l2cap_evt(struct ble_l2cap_event *event, void *arg)
{
    (void)arg;

    switch (event->type) {
        case BLE_L2CAP_EVENT_COC_CONNECTED:
            _coc = event->connect.chan;
            puts("# L2CAP: CONNECTED");
            thread_flags_set(_main, FLAG_UP);
            break;
        case BLE_L2CAP_EVENT_COC_DISCONNECTED:
            _coc = NULL;
            puts("# L2CAP: DISCONNECTED");
            break;
        case BLE_L2CAP_EVENT_COC_DATA_RECEIVED:
            _on_data(event);
            break;
        case BLE_L2CAP_EVENT_COC_ACCEPT:
            /* this event should never be triggered for the L2CAP client */
            /* fallthrough */
        default:
            assert(0);
            break;
    }

    return 0;
}

static int _on_gap_evt(struct ble_gap_event *event, void *arg)
{
    (void)arg;
    printf("# GAP event: %i\n", (int)event->type);

    switch (event->type) {
        case BLE_GAP_EVENT_CONNECT: {
            _handle = event->connect.conn_handle;
            struct os_mbuf *sdu_rx = os_mbuf_get_pkthdr(&_coc_mbuf_pool, 0);
            assert(sdu_rx != NULL);
            int res = ble_l2cap_connect(_handle, APP_CID, APP_MTU, sdu_rx,
                                       _on_l2cap_evt, NULL);
            assert(res == 0);
            break;
        }
        case BLE_GAP_EVENT_DISCONNECT:
            _handle = 0;
            puts("# TERMINATED, bye bye");
            break;
        case BLE_GAP_EVENT_CONN_UPDATE:
        case BLE_GAP_EVENT_CONN_UPDATE_REQ:
        default:
            break;
    }

    return 0;
}

static void _filter_and_connect(struct ble_gap_disc_desc *disc)
{
    int res;
    bluetil_ad_t ad;

    bluetil_ad_init(&ad, disc->data,
                    (size_t)disc->length_data, (size_t)disc->length_data);
    res = bluetil_ad_find_and_cmp(&ad, BLE_GAP_AD_NAME,
                                  APP_NODENAME, (sizeof(APP_NODENAME) - 1));
    if (res) {
        res = ble_gap_disc_cancel();
        assert(res == 0);

        printf("# Found Server, connecting now");
        res = ble_gap_connect(nimble_riot_own_addr_type, &disc->addr,
                              BLE_HS_FOREVER, NULL, _on_gap_evt, NULL);
        assert(res == 0);
    }
}

static int _on_scan_evt(struct ble_gap_event *event, void *arg)
{
    (void)arg;

    switch (event->type) {
        case BLE_GAP_EVENT_DISC:
            _filter_and_connect(&event->disc);
            break;
        case BLE_GAP_EVENT_DISC_COMPLETE:
        default:
            break;
    }

    return 0;
}

static void _send(uint32_t type, uint32_t seq, size_t len)
{
    int res = 0;
    (void)res;
    struct os_mbuf *txd;

    assert(_coc);
    assert(len >= 8);
    assert(len <= APP_MTU);
    printf("# Sending: size %5u seq %5u\n", (unsigned)len, (unsigned)seq);

    _txbuf[POS_TYPE] = type;
    _txbuf[POS_SEQ] = seq;
    txd = os_mbuf_get_pkthdr(&_coc_mbuf_pool, 0);
    assert(txd != NULL);
    res = os_mbuf_append(txd, _txbuf, len);
    assert(res == 0);

    do {
        res = ble_l2cap_send(_coc, txd);
    } while (res == BLE_HS_EBUSY);

    if (res != 0) {
        printf("# err: failed to send (%i)\n", res);
        assert(0);
    }
}

static int _cmd_inctest(int argc, char **argv)
{
    size_t step = 10;
    size_t limit = APP_MTU;

    if ((argc == 2) && (strncmp(argv[1], "help", 4) == 0)) {
        printf("usage: %s [step [limit]]\n", argv[0]);
        return 0;
    }
    if (_coc == NULL) {
        puts("err: not connected");
        return 1;
    }

    /* parse params */
    if (argc >= 2) {
        step = (size_t)atoi(argv[1]);
    }
    if (argc >= 3) {
        limit = (size_t)atoi(argv[2]);
        if ((limit < 8) || (limit > APP_MTU)) {
            puts("err: invalid limit payload length given");
            return 1;
        }
    }

    /* send out packets with increasing payload size */
    thread_flags_clear(FLAG_SYNC);
    uint32_t seq = 0;
    uint32_t time = xtimer_now_usec();
    for (size_t i = 8; i < limit; i += step) {
        _send(TYPE_INCTEST, ++seq, i);
        thread_flags_wait_all(FLAG_SYNC);
        if (_last_rx_seq != seq) {
            printf("# err: bad sequence number in response (%u)\n",
                   (unsigned)_last_rx_seq);
            assert(0);
        }
    }
    time = (xtimer_now_usec() - time);
    puts("# TEST COMPLETE");
    printf("-> runtime: %ums\n", (unsigned)(time / 1000));
    return 0;
}

static int _cmd_floodtest(int argc, char **argv)
{
    size_t pktsize = PKTSIZE_DEFAULT;
    unsigned limit = FLOOD_DEFAULT;

    if ((argc == 2) && (strncmp(argv[1], "help", 4) == 0)) {
        printf("usage: %s [payload_size [num_of_packets]]\n", argv[0]);
        return 0;
    }
    if (_coc == NULL) {
        puts("err: not connected");
        return 1;
    }

    if (argc >= 2) {
        pktsize = (size_t)atoi(argv[1]);
        if ((pktsize < 8) || (pktsize > APP_MTU)) {
            puts("err: invalid packet size given");
            return 1;
        }
    }
    if (argc >= 3) {
        limit = (unsigned)atoi(argv[2]);
    }

    /* now lets flood */
    uint32_t seq = 0;
    uint32_t time = xtimer_now_usec();
    unsigned sum = 0;
    for (unsigned i = 0; i < limit; i++) {
        _send(TYPE_FLOODING, ++seq, pktsize);
        sum += pktsize;
    }
    /* we wait for the last packet to be returned to compensate any internal
     * packet buffering, but we also include this extra packet transmission in
     * the throughput calculation */
    sum += pktsize;
    while (_last_rx_seq != seq) {}
    time = (xtimer_now_usec() - time);
    puts("# TEST COMPLETE");
    printf("-> runtime: %ums\n", (unsigned)(time / 1000));
    printf("-> ~ %u bytes/s\n", (unsigned)((sum * 1000) / (time / 1000)));

    return 0;
}

static const shell_command_t _cmds[] = {
    { "inctest", "send stream of payloads of incremental size", _cmd_inctest },
    { "flood", "flood connection with data", _cmd_floodtest },
    { NULL, NULL, NULL }
};

int main(void)
{
    int res;
    (void)res;
    puts("NimBLE L2CAP test application");

    /* save context of the main thread */
    _main = (thread_t *)thread_get(thread_getpid());

    /* initialize buffers and setup the test environment */
    res = os_mempool_init(&_coc_mempool, MBUFCNT, MBUFSIZE, _coc_mem, "appbuf");
    assert(res == 0);
    res = os_mbuf_pool_init(&_coc_mbuf_pool, &_coc_mempool, MBUFSIZE, MBUFCNT);
    assert(res == 0);

    /* start scanning for a suitable test server */
    puts("# Scanning now");
    struct ble_gap_disc_params params = { 0 };
    res = ble_gap_disc(nimble_riot_own_addr_type, BLE_HS_FOREVER,
                       &params, _on_scan_evt, NULL);
    assert(res == 0);

    /* wait until we are connected to the test server */
    thread_flags_wait_all(FLAG_UP);
    puts("# Connection established");
    printf("# MTUs: our %i, remote %i\n",
           ble_l2cap_get_our_mtu(_coc), ble_l2cap_get_peer_mtu(_coc));

    /* start shell */
    puts("# Shell is now available");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(_cmds, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
