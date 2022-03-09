/*
 * Copyright (C) 2015 Freie Universität Berlin
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
 * @brief       Test application for AT86RF2xx network device driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "sys/uio.h"
#include "luid.h"

#include "net/netdev.h"
#include "shell.h"
#include "shell_commands.h"
#include "net/ieee802154/submac.h"
#include "net/ieee802154.h"
#include "net/netdev/ieee802154_submac.h"
#include "net/l2util.h"
#include "test_utils/expect.h"
#include "ztimer.h"

#include "common.h"

#define MAX_LINE    (80)

ieee802154_submac_t submac;                                             /**< IEEE 802.15.4 SubMAC descriptor */
mutex_t lock;                                                           /**< lock used to synchronize SubMAC operation */
ztimer_t ack_timer;                                                     /**< required for the ACK timer */
eui64_t long_addr;                                                      /**< SubMAC extended address */
network_uint16_t short_addr;                                            /**< SubMAC short address */

static void _ev_tx_done_handler(event_t *event);                        /**< TX Done event handler */
static void _ev_rx_done_handler(event_t *event);                        /**< RX Done event handler */
static void _ev_crc_error_handler(event_t *event);                      /**< CRC Error event handler */
static void _ev_bh_request_handler(event_t *event);                     /**< BH Request event handler */
static void _ev_ack_timeout_handler(event_t *event);                    /**< ACK Timeout event handler */
static void _ev_set_rx_handler(event_t *event);                         /**< Set RX event handler */

static event_t ev_tx_done = { .handler = _ev_tx_done_handler };         /**< TX Done descriptor */
static event_t ev_rx_done = { .handler = _ev_rx_done_handler };         /**< RX Done descriptor */
static event_t ev_crc_error = { .handler = _ev_crc_error_handler };     /**< CRC Error descriptor */
static event_t ev_bh_request = { .handler = _ev_bh_request_handler };   /**< BH Request descriptor */
static event_t ev_ack_timeout = { .handler = _ev_ack_timeout_handler }; /**< ACK TO descriptor */
static event_t ev_set_rx = { .handler = _ev_set_rx_handler };           /**< Set RX descriptor */

uint8_t buffer[IEEE802154_FRAME_LEN_MAX];                               /* buffer to store IEEE 802.15.4 frames */
uint8_t seq;                                                            /* sequence number of IEEE 802.15.4 frame */

struct _reg_container {
    int count;  /* device index */
};

static void submac_rx_done(ieee802154_submac_t *submac);
static void submac_tx_done(ieee802154_submac_t *submac, int status,
                           ieee802154_tx_info_t *info);

static const ieee802154_submac_cb_t _cb = {
    .rx_done = submac_rx_done,
    .tx_done = submac_tx_done,
};

static const uint8_t payload[] =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Etiam ornare" \
    "lacinia mi elementum interdum ligula.";

static int print_addr(int argc, char **argv);
static int txtsnd(int argc, char **argv);
static const shell_command_t shell_commands[] = {
    { "print_addr", "Print IEEE802.15.4 addresses", print_addr },
    { "txtsnd", "Send IEEE 802.15.4 packet", txtsnd },
    { NULL, NULL, NULL }
};

/****** SubMAC South Bond API Implementation ******/

static void _ev_tx_done_handler(event_t *event)
{
    (void)event;
    mutex_lock(&lock);
    ieee802154_submac_tx_done_cb(&submac);
    mutex_unlock(&lock);
}

static void _ev_rx_done_handler(event_t *event)
{
    (void)event;
    mutex_lock(&lock);
    ieee802154_submac_rx_done_cb(&submac);
    mutex_unlock(&lock);
}

static void _ev_crc_error_handler(event_t *event)
{
    (void)event;
    mutex_lock(&lock);
    ieee802154_submac_crc_error_cb(&submac);
    mutex_unlock(&lock);
}

static void _ev_bh_request_handler(event_t *event)
{
    (void)event;
    mutex_lock(&lock);
    ieee802154_submac_bh_process(&submac);
    mutex_unlock(&lock);
}

static void _ev_ack_timeout_handler(event_t *event)
{
    (void)event;
    mutex_lock(&lock);
    ieee802154_submac_ack_timeout_fired(&submac);
    mutex_unlock(&lock);
}

void ieee802154_submac_ack_timer_set(ieee802154_submac_t *submac, uint16_t us)
{
    (void)submac;
    ztimer_set(ZTIMER_USEC, &ack_timer, us);
}

void ieee802154_submac_ack_timer_cancel(ieee802154_submac_t *submac)
{
    (void)submac;
    ztimer_remove(ZTIMER_USEC, &ack_timer);
    /* Avoid race conditions between RX_DONE and ACK_TIMEOUT */
    if (ev_ack_timeout.list_node.next) {
        event_cancel(EVENT_PRIO_HIGHEST, &ev_ack_timeout);
    }
}

static void _ack_timeout(void *arg)
{
    (void)arg;
    event_post(EVENT_PRIO_HIGHEST, &ev_ack_timeout);
}

static void _hal_radio_cb(ieee802154_dev_t *dev, ieee802154_trx_ev_t status)
{
    (void)dev;
    switch (status) {
    case IEEE802154_RADIO_CONFIRM_TX_DONE:
        event_post(EVENT_PRIO_HIGHEST, &ev_tx_done);
        break;
    case IEEE802154_RADIO_INDICATION_RX_DONE:
        event_post(EVENT_PRIO_HIGHEST, &ev_rx_done);
        break;
    case IEEE802154_RADIO_INDICATION_CRC_ERROR:
        event_post(EVENT_PRIO_HIGHEST, &ev_crc_error);
        break;
    default:
        break;
    }
}

void ieee802154_submac_bh_request(ieee802154_submac_t *submac)
{
    (void)submac;
    event_post(EVENT_PRIO_HIGHEST, &ev_bh_request);
}

static ieee802154_dev_t *_reg_callback(ieee802154_dev_type_t type, void *opaque)
{
    struct _reg_container *reg = opaque;

    printf("Trying to register ");
    switch (type) {
    case IEEE802154_DEV_TYPE_CC2538_RF:
        printf("cc2538_rf");
        break;
    case IEEE802154_DEV_TYPE_NRF802154:
        printf("nrf52840");
        break;
    case IEEE802154_DEV_TYPE_SOCKET_ZEP:
        printf("socket_zep");
        break;
    }

    puts(".");
    if (reg->count > 0) {
        puts("For the moment this test only supports one radio");
        return NULL;
    }

    puts("Success");
    return &submac.dev;
}

/****** Helpers ******/

void _print_addr(uint8_t *addr, size_t addr_len)
{
    for (size_t i = 0; i < addr_len; i++) {
        if (i != 0) {
            printf(":");
        }
        printf("%02x", (unsigned)addr[i]);
    }
}

/****** Upper layer implementation ******/
#define IEEE802154_LONG_ADDRESS_LEN_STR_MAX \
    (sizeof("00:00:00:00:00:00:00:00"))

static int print_addr(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    char addr_str[IEEE802154_LONG_ADDRESS_LEN_STR_MAX];
    printf("%s\n", l2util_addr_to_str(
               long_addr.uint8, IEEE802154_LONG_ADDRESS_LEN, addr_str));
    return 0;
}

static void _ev_set_rx_handler(event_t *event)
{
    (void)event;
    mutex_lock(&lock);
    ieee802154_set_rx(&submac);
    mutex_unlock(&lock);
}

static void submac_tx_done(ieee802154_submac_t *submac, int status,
                           ieee802154_tx_info_t *info)
{
    (void)info;
    (void)submac;
    switch (status) {
    case TX_STATUS_SUCCESS:
        puts("Tx complete");
        break;
    case TX_STATUS_FRAME_PENDING:
        puts("Tx complete with pending data");
        break;
    case TX_STATUS_MEDIUM_BUSY:
        puts("Medium Busy");
        break;
    case TX_STATUS_NO_ACK:
        puts("No ACK");
        break;
    default:
        break;
    }

    /* Schedule the state change. Calling this function directly in the callback
     * will return error */
    event_post(EVENT_PRIO_HIGHEST, &ev_set_rx);
}

static void submac_rx_done(ieee802154_submac_t *submac)
{
    uint8_t src[IEEE802154_LONG_ADDRESS_LEN], dst[IEEE802154_LONG_ADDRESS_LEN];
    int data_len;
    size_t mhr_len, src_len, dst_len;
    le_uint16_t src_pan, dst_pan;
    ieee802154_rx_info_t rx_info;

    putchar('\n');
    data_len = ieee802154_read_frame(submac, buffer, sizeof(buffer), &rx_info);
    if (data_len < 0) {
        puts("Couldn't read frame");
        return;
    }
    mhr_len = ieee802154_get_frame_hdr_len(buffer);
    if (mhr_len == 0) {
        puts("Unexpected MHR for incoming packet");
        return;
    }
    dst_len = ieee802154_get_dst(buffer, dst, &dst_pan);
    src_len = ieee802154_get_src(buffer, src, &src_pan);
    switch (buffer[0] & IEEE802154_FCF_TYPE_MASK) {
    case IEEE802154_FCF_TYPE_BEACON:
        puts("BEACON");
        break;
    case IEEE802154_FCF_TYPE_DATA:
        puts("DATA");
        break;
    case IEEE802154_FCF_TYPE_ACK:
        puts("ACK");
        break;
    case IEEE802154_FCF_TYPE_MACCMD:
        puts("MACCMD");
        break;
    default:
        puts("UNKNOWN");
        break;
    }
    printf("Dest. PAN: 0x%04x, Dest. addr.: ",
           byteorder_ltohs(dst_pan));
    _print_addr(dst, dst_len);
    printf("\nSrc. PAN: 0x%04x, Src. addr.: ",
           byteorder_ltohs(src_pan));
    _print_addr(src, src_len);
    printf("\nSecurity: ");
    if (buffer[0] & IEEE802154_FCF_SECURITY_EN) {
        printf("1, ");
    }
    else {
        printf("0, ");
    }
    printf("Frame pend.: ");
    if (buffer[0] & IEEE802154_FCF_FRAME_PEND) {
        printf("1, ");
    }
    else {
        printf("0, ");
    }
    printf("ACK req.: ");
    if (buffer[0] & IEEE802154_FCF_ACK_REQ) {
        printf("1, ");
    }
    else {
        printf("0, ");
    }
    printf("PAN comp.: ");
    if (buffer[0] & IEEE802154_FCF_PAN_COMP) {
        puts("1");
    }
    else {
        puts("0");
    }
    printf("Version: ");
    printf("%u, ", (unsigned)((buffer[1] & IEEE802154_FCF_VERS_MASK) >> 4));
    printf("Seq.: %u\n", (unsigned)ieee802154_get_seq(buffer));
    od_hex_dump(buffer + mhr_len, data_len - mhr_len, 0);
    printf("txt: ");
    for (int i = mhr_len; i < data_len; i++) {
        if ((buffer[i] > 0x1F) && (buffer[i] < 0x80)) {
            putchar((char)buffer[i]);
        }
        else {
            putchar('?');
        }
        if (((((i - mhr_len) + 1) % (MAX_LINE - sizeof("txt: "))) == 1) &&
            (i - mhr_len) != 0) {
            printf("\n     ");
        }
    }
    printf("\n");
    printf("RSSI: %i, LQI: %u\n\n", rx_info.rssi, rx_info.lqi);

    /* Schedule the state change. Calling this function directly in the callback
     * will return error */
    event_post(EVENT_PRIO_HIGHEST, &ev_set_rx);
}

static int send(uint8_t *dst, size_t dst_len,
                size_t len)
{
    uint8_t flags;
    uint8_t mhr[IEEE802154_MAX_HDR_LEN];
    int mhr_len;

    le_uint16_t src_pan, dst_pan;
    iolist_t iol_data = {
        .iol_base = (void *)payload,
        .iol_len = len,
        .iol_next = NULL,
    };

    flags = IEEE802154_FCF_TYPE_DATA | IEEE802154_FCF_ACK_REQ;
    src_pan = byteorder_btols(byteorder_htons(CONFIG_IEEE802154_DEFAULT_PANID));
    dst_pan = byteorder_btols(byteorder_htons(CONFIG_IEEE802154_DEFAULT_PANID));

    uint8_t src_len = IEEE802154_LONG_ADDRESS_LEN;
    void *src = &submac.ext_addr;

    /* fill MAC header, seq should be set by device */
    if ((mhr_len = ieee802154_set_frame_hdr(mhr, src, src_len,
                                            dst, dst_len,
                                            src_pan, dst_pan,
                                            flags, seq++)) < 0) {
        puts("txtsnd: Error preperaring frame");
        return 1;
    }

    iolist_t pkt;
    pkt.iol_next = &iol_data;
    pkt.iol_base = mhr;
    pkt.iol_len = mhr_len;

    mutex_lock(&lock);
    int res = ieee802154_send(&submac, &pkt);
    mutex_unlock(&lock);
    if (res < 0) {
        puts("Error: Frame couldn't be sent");
    }
    return 0;
}

static int txtsnd(int argc, char **argv)
{
    uint8_t addr[IEEE802154_LONG_ADDRESS_LEN];
    size_t len;
    size_t res;

    if (argc != 3) {
        puts("Usage: txtsnd <long_addr> <len>");
        return 1;
    }

    res = l2util_addr_from_str(argv[1], addr);
    if (res == 0) {
        puts("Usage: txtsnd <long_addr> <len>");
        return 1;
    }
    len = atoi(argv[2]);
    return send(addr, res, len);
}

static int _init(void)
{
    mutex_init(&lock);

    submac.cb = &_cb;

    /* Set the Event Notification */
    submac.dev.cb = _hal_radio_cb;

    ack_timer.callback = _ack_timeout;
    ack_timer.arg = NULL;

    luid_base(&long_addr, sizeof(long_addr));
    eui64_set_local(&long_addr);
    eui64_clear_group(&long_addr);
    eui_short_from_eui64(&long_addr, &short_addr);

    struct _reg_container reg = { 0 };
    ieee802154_hal_test_init_devs(_reg_callback, &reg);

    int res = ieee802154_submac_init(&submac, &short_addr, &long_addr);

    if (res < 0) {
        return res;
    }

    return 0;
}

int main(void)
{
    _init();

    /* start the shell */
    puts("Initialization successful - starting the shell now");

    char line_buf[SHELL_DEFAULT_BUFSIZE];

    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
