/*
 * SPDX-FileCopyrightText: 2020 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for IEEE 802.15.4 Radio HAL
 *
 * @author      José I. Alamos <jose.alamos@haw-hamburg.de>
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <strings.h>

#include "test_common.h"
#include "errno.h"
#include "event/thread.h"
#include "luid.h"
#include "od.h"

#include "net/l2util.h"
#include "net/ieee802154.h"
#include "net/ieee802154/radio.h"

#include "shell.h"

#include "test_utils/expect.h"
#include "xtimer.h"

#define SYMBOL_TIME (16U) /**< 16 us */
#define ACK_TIMEOUT_TIME (40 * SYMBOL_TIME)
#define TX_RX_TURNAROUND (12 * SYMBOL_TIME)

/* the CC2538 takes 193 us to put the transceiver in RX_ON, which officially
 * violates the official TX<->RX turnaround time (192 us for O-QPSK).
 * However, the radio is able to pick up the preamble of a frame even if the
 * first symbol is lost. We add a tolerance of half a symbol to the
 * TX_RX_TURNAROUND in order to be sure the TX<->RX measurement test doesn't
 * fail
 */
#define MAX_TX_RX_TURNAROUND (TX_RX_TURNAROUND + (SYMBOL_TIME >> 1))

#define IEEE802154_LONG_ADDRESS_LEN_STR_MAX \
    (sizeof("00:00:00:00:00:00:00:00"))

static uint8_t buffer[127];
static xtimer_t timer_ack;
static mutex_t lock;

static eui64_t ext_addr;
static network_uint16_t short_addr;
static uint8_t seq;

static ieee802154_dev_t _radio;

static void _print_packet(size_t size, uint8_t lqi, int16_t rssi)
{
    if (buffer[0] & IEEE802154_FCF_TYPE_ACK && ((seq-1) == buffer[2])) {
        printf("Received valid ACK with sqn %i\n", buffer[2]);
    }
    else {
        puts("Frame received:");
        od_hex_dump(buffer, size, 0);
    }
    printf("LQI: %i, RSSI: %i\n", (int) lqi, (int) rssi);
    puts("");
}

static int print_addr(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    char addr_str[IEEE802154_LONG_ADDRESS_LEN_STR_MAX];
    printf("%s\n", l2util_addr_to_str(
               ext_addr.uint8, IEEE802154_LONG_ADDRESS_LEN, addr_str));
    return 0;
}

static void _ack_timeout(event_t *event)
{
    (void) event;
    ieee802154_dev_t *dev = &_radio;

    ieee802154_radio_set_frame_filter_mode(dev, IEEE802154_FILTER_ACCEPT);
}

static event_t _ack_timeout_ev = {
    .handler = _ack_timeout,
};

void _timer_ack_handler(void *arg)
{
    (void) arg;
    event_post(EVENT_PRIO_HIGHEST, &_ack_timeout_ev);
}

static xtimer_t timer_ack = {
    .callback = _timer_ack_handler,
};

void _crc_error_handler(event_t *event)
{
    (void) event;
    puts("Frame with invalid CRC received");
    ieee802154_dev_t* dev = &_radio;
    /* Force transition to IDLE before calling the read function */
    ieee802154_radio_set_idle(dev, true);

    /* We are not interested in the content of the frame */
    ieee802154_radio_read(dev, NULL, 0, NULL);

    ieee802154_radio_set_rx(dev);
}

static event_t _crc_error_event = {
    .handler = _crc_error_handler,
};

void _rx_done_handler(event_t *event)
{
    (void) event;
    ieee802154_rx_info_t info;
    ieee802154_dev_t* dev = &_radio;

    /* Force transition to IDLE before calling the read function */
    ieee802154_radio_set_idle(dev, true);

    /* Read packet from internal framebuffer
     *
     * NOTE: It's possible to call `ieee802154_radio_len` to retrieve the packet
     * length. Since the buffer is fixed in this test, we don't use it
     */
    int size = ieee802154_radio_read(&_radio, buffer, 127, &info);
    if (size > 0) {
        /* Print packet while we wait for the state transition */
        _print_packet(size, info.lqi, info.rssi);
    }

    ieee802154_radio_set_rx(dev);
}

static event_t _rx_done_event = {
    .handler = _rx_done_handler,
};

/* Event Notification callback */
static void _hal_radio_cb(ieee802154_dev_t *dev, ieee802154_trx_ev_t status)
{
    (void) dev;
    switch (status) {
        case IEEE802154_RADIO_CONFIRM_TX_DONE:
        case IEEE802154_RADIO_CONFIRM_CCA:
            mutex_unlock(&lock);
            break;
        case IEEE802154_RADIO_INDICATION_RX_DONE:
            event_post(EVENT_PRIO_HIGHEST, &_rx_done_event);
            break;
        case IEEE802154_RADIO_INDICATION_CRC_ERROR:
            event_post(EVENT_PRIO_HIGHEST, &_crc_error_event);
            break;
        default:
           break;
    }
}

static void _tx_finish_handler(event_t *event)
{
    ieee802154_tx_info_t tx_info;

    (void) event;
    /* The TX_DONE event indicates it's safe to call the confirm counterpart */
    expect(ieee802154_radio_confirm_transmit(&_radio, &tx_info) >= 0);

    ieee802154_radio_set_rx(&_radio);
    if (!ieee802154_radio_has_irq_ack_timeout(&_radio) &&
        !ieee802154_radio_has_frame_retrans(&_radio)) {
        /* This is just to show how the MAC layer would handle ACKs... */
        xtimer_set(&timer_ack, ACK_TIMEOUT_TIME);
    }

    switch (tx_info.status) {
        case TX_STATUS_SUCCESS:
            puts("Transmission succeeded");
            break;
        case TX_STATUS_FRAME_PENDING:
            puts("Transmission succeeded and there's pending data");
            break;
        case TX_STATUS_MEDIUM_BUSY:
            puts("Medium busy");
            break;
        case TX_STATUS_NO_ACK:
            puts("No ACK");
            break;
    }

    puts("");

    if (ieee802154_radio_has_frame_retrans_info(&_radio)) {
        printf("Retransmission attempts: %i\n", tx_info.retrans);
    }

    puts("");
}

static event_t _tx_finish_ev = {
    .handler = _tx_finish_handler,
};

static void _send(iolist_t *pkt)
{
    /* Request a state change to IDLE */
    if (ieee802154_radio_request_set_idle(&_radio, false) < 0) {
        puts("Couldn't send frame");
        return;
    }

    /* Write the packet to the radio while the radio is transitioning to IDLE */
    ieee802154_radio_write(&_radio, pkt);

    /* Block until the radio confirms the state change */
    while (ieee802154_radio_confirm_set_idle(&_radio) == -EAGAIN) {}

    /* Set the frame filter to receive ACKs */
    ieee802154_radio_set_frame_filter_mode(&_radio, IEEE802154_FILTER_ACK_ONLY);

    /* Trigger the transmit and wait for the mutex unlock (TX_DONE event).
     * Spin if the radio is busy before transmission (this indicates the
     * transmission is requested before the end of the IFS).
     * This won't be necessary anymore when the upper layers take care
     * of the IFS.
     */
    while (ieee802154_radio_request_transmit(&_radio) == -EBUSY) {}
    mutex_lock(&lock);

    event_post(EVENT_PRIO_HIGHEST, &_tx_finish_ev);
}

struct _reg_container {
    int count;
};

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
        case IEEE802154_DEV_TYPE_KW2XRF:
            printf("kw2xrf");
            break;
        case IEEE802154_DEV_TYPE_MRF24J40:
            printf("mrf24j40");
            break;
        case IEEE802154_DEV_TYPE_ESP_IEEE802154:
            printf("esp_ieee802154");
            break;
    }

    puts(".");
    if (reg->count > 0) {
        puts("For the moment this test only supports one radio");
        return NULL;
    }

    reg->count++;
    puts("Success");
    return &_radio;
}

static int _init(void)
{
    int res;
    struct _reg_container reg = {0};

    ieee802154_hal_test_init_devs(_reg_callback, &reg);

    /* Set the Event Notification */
    ((ieee802154_dev_t*) &_radio)->cb = _hal_radio_cb;

    /* Note that addresses are not kept in the radio. This assumes MAC layers
     * already have a copy of the address */
    luid_get_eui64(&ext_addr);
    luid_get_short(&short_addr);

    /* Since the device was already initialized, turn on the radio.
     * The transceiver state will be "TRX_OFF" */
    res = ieee802154_radio_request_on(&_radio);
    expect(res >= 0);
    while (ieee802154_radio_confirm_on(&_radio) == -EAGAIN) {}

    ieee802154_radio_set_frame_filter_mode(&_radio, IEEE802154_FILTER_ACCEPT);

    uint16_t panid = CONFIG_IEEE802154_DEFAULT_PANID;

    /* Set all IEEE addresses */
    res = ieee802154_radio_config_addr_filter(&_radio,
                                        IEEE802154_AF_SHORT_ADDR, &short_addr);
    expect(res >= 0);
    res = ieee802154_radio_config_addr_filter(&_radio,
                                        IEEE802154_AF_EXT_ADDR, &ext_addr);
    expect(res >= 0);
    res = ieee802154_radio_config_addr_filter(&_radio,
                                        IEEE802154_AF_PANID, &panid);
    expect(res >= 0);

    /* Set PHY configuration */
    ieee802154_phy_conf_t conf = { .channel=CONFIG_IEEE802154_DEFAULT_CHANNEL,
                                   .page=CONFIG_IEEE802154_DEFAULT_SUBGHZ_PAGE,
                                   .pow=CONFIG_IEEE802154_DEFAULT_TXPOWER};

    res = ieee802154_radio_config_phy(&_radio, &conf);
    expect(res >= 0);

    /* ieee802154_radio_set_cca_mode*/
    res = ieee802154_radio_set_cca_mode(&_radio, IEEE802154_CCA_MODE_ED_THRESHOLD);
    expect(res >= 0);
    res = ieee802154_radio_set_cca_threshold(&_radio, CONFIG_IEEE802154_CCA_THRESH_DEFAULT);
    expect(res >= 0);

    /* Set the transceiver state to RX_ON in order to receive packets */
    ieee802154_radio_set_rx(&_radio);
    return 0;
}

uint8_t payload[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. "
                    "Etiam ornare lacinia mi elementum interdum ligula.";

static int send(uint8_t *dst, size_t dst_len,
                size_t len, bool ack_req)
{
    uint8_t flags;
    uint8_t mhr[IEEE802154_MAX_HDR_LEN];
    int mhr_len;

    le_uint16_t src_pan, dst_pan;
    iolist_t iol_data = {
        .iol_base = payload,
        .iol_len = len,
        .iol_next = NULL,
    };

    flags = IEEE802154_FCF_TYPE_DATA | (ack_req ? IEEE802154_FCF_ACK_REQ : 0);

    src_pan = byteorder_btols(byteorder_htons(CONFIG_IEEE802154_DEFAULT_PANID));
    dst_pan = byteorder_btols(byteorder_htons(CONFIG_IEEE802154_DEFAULT_PANID));
    uint8_t src_len = IEEE802154_LONG_ADDRESS_LEN;
    void *src = &ext_addr;

    /* fill MAC header, seq should be set by device */
    if ((mhr_len = ieee802154_set_frame_hdr(mhr, src, src_len,
                                        dst, dst_len,
                                        src_pan, dst_pan,
                                        flags, seq++)) < 0) {
        puts("txtsnd: Error preperaring frame");
        return 1;
    }

    iolist_t iol_hdr = {
        .iol_next = &iol_data,
        .iol_base = mhr,
        .iol_len = mhr_len,
    };

    _send(&iol_hdr);
    return 0;
}

int _cca(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    int res;
    if (ieee802154_radio_has_irq_cca_done(&_radio)) {
        if (ieee802154_radio_request_cca(&_radio) < 0) {
            puts("Couldn't perform CCA");
            return -ENODEV;
        }
        mutex_lock(&lock);
        res = ieee802154_radio_confirm_cca(&_radio);
        expect(res >= 0);
    }
    else {
        res = ieee802154_radio_cca(&_radio);
    }

    if (res > 0) {
        puts("CLEAR");
    }
    else {
        puts("BUSY");
    }

    return 0;
}

int _test_states(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    uint32_t usecs;
    int res;
    xtimer_ticks32_t a;

    /* Force transition to IDLE */
    res = ieee802154_radio_set_idle(&_radio, true);
    expect(res == 0);

    printf("Testing TX<->RX transition time: ");
    a = xtimer_now();
    res = ieee802154_radio_set_rx(&_radio);
    expect(res == 0);
    usecs = xtimer_usec_from_ticks(xtimer_diff(xtimer_now(), a));
    printf("%" PRIu32 " us (%s)\n", usecs, usecs > MAX_TX_RX_TURNAROUND
                                           ? "FAIL"
                                           : "PASS");

    printf("Testing RX<->TX transition time: ");
    a = xtimer_now();
    res = ieee802154_radio_set_idle(&_radio, true);
    expect(res == 0);
    usecs = xtimer_usec_from_ticks(xtimer_diff(xtimer_now(), a));
    printf("%" PRIu32 " us (%s)\n", usecs, usecs > MAX_TX_RX_TURNAROUND
                                           ? "FAIL"
                                           : "PASS");

    ieee802154_radio_set_rx(&_radio);

    return 0;
}

int txtsnd(int argc, char **argv)
{
    uint8_t addr[IEEE802154_LONG_ADDRESS_LEN];
    size_t len;
    size_t res;
    bool ack_req = false;

    if (argc != 3 && !(argc == 4 && strcmp(argv[3], "ackreq") == 0)) {
        puts("Usage: txtsnd <long_addr> <len> [ackreq]");
        return 1;
    }

    res = l2util_addr_from_str(argv[ 1], addr);
    if (res == 0) {
        puts("Usage: txtsnd <ext_addr> <len>");
        return 1;
    }
    len = atoi(argv[2]);

    if (argc == 4) {
        ack_req = true;
    }
    return send(addr, res, len, ack_req);
}

static int promisc(int argc, char **argv)
{
    ieee802154_filter_mode_t conf;
    if (argc < 2) {
        printf("Usage: %s <on|off>", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "on") == 0) {
        conf = IEEE802154_FILTER_PROMISC;
        puts("Enabled promiscuos mode");
    }
    else {
        conf = IEEE802154_FILTER_ACCEPT;
        puts("Disabled promiscuos mode");
    }

    return ieee802154_radio_set_frame_filter_mode(&_radio, conf);
}

int config_phy(int argc, char **argv)
{
    int res = -EINVAL;
    if (argc < 4) {
        puts("Usage: config_phy <phy_mode> <channel> <tx_pow>");
        return 1;
    }

    ieee802154_phy_mode_t phy_mode;
    if (strcasecmp(argv[1], "BPSK")) {
        phy_mode = IEEE802154_PHY_BPSK;
    }
    else if (strcasecmp(argv[1], "ASK")) {
        phy_mode = IEEE802154_PHY_ASK;
    }
    else if (strcasecmp(argv[1], "O-QPSK")) {
        phy_mode = IEEE802154_PHY_OQPSK;
    }
    else if (strcasecmp(argv[1], "MR-O-QPSK")) {
        phy_mode = IEEE802154_PHY_MR_OQPSK;
    }
    else if (strcasecmp(argv[1], "MR-OFDM")) {
        phy_mode = IEEE802154_PHY_MR_OFDM;
    }
    else if (strcasecmp(argv[1], "MR-FSK")) {
        phy_mode = IEEE802154_PHY_MR_FSK;
    }
    else {
        puts("Wrong PHY mode specified. Must be one of:");
        puts("  * BPSK");
        puts("  * ASK");
        puts("  * O-QPSK");
        puts("  * MR-O-QPSK");
        puts("  * MR-OFDM");
        puts("  * MR-FSK");
        return 1;
    }

    uint8_t channel = atoi(argv[2]);
    int8_t tx_pow = atoi(argv[3]);

    if (channel < 11 || channel > 26) {
        puts("Wrong channel configuration (11 <= channel <= 26).");
        return 1;
    }

    ieee802154_dev_t *dev = &_radio;
    ieee802154_radio_set_idle(dev, true);
    ieee802154_phy_conf_t conf = {.phy_mode=phy_mode, .channel=channel, .page=0, .pow=tx_pow};
    if (ieee802154_radio_config_phy(dev, &conf) < 0) {
        puts("Channel or TX power settings not supported");
    }
    else {
        puts("Success!");
        res = 0;
    }

    ieee802154_radio_set_rx(dev);

    return res;
}

int txmode_cmd(int argc, char **argv)
{
    ieee802154_dev_t *dev = &_radio;
    int res = -EINVAL;

    if (argc < 2) {
        printf("Usage: %s <csma_ca|cca|direct>\n", argv[0]);
        return 1;
    }

    ieee802154_csma_be_t csma_conf = {.min = 3, .max = 5};

    int retries;
    if (strcmp(argv[1], "direct") == 0) {
        retries = -1;
    }
    else if (strcmp(argv[1], "cca") == 0) {
        retries = 0;
    }
    else {
        retries = 4;
    }

    if (ieee802154_radio_set_csma_params(dev, &csma_conf, retries) < 0) {
        puts("Not supported");
    }
    else {
        puts("Ok");
        res = 0;
    }
    return res;
}

static int _config_cca_cmd(int argc, char **argv)
{
    ieee802154_dev_t *dev = &_radio;
    int res = -EINVAL;

    ieee802154_cca_mode_t mode;

    if (argc < 2) {
        printf("Usage: %s <ed|cs|or|and> [ED threshold]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "ed") == 0) {
        mode = IEEE802154_CCA_MODE_ED_THRESHOLD;
    }
    else if (strcmp(argv[1], "cs") == 0) {
        mode = IEEE802154_CCA_MODE_CARRIER_SENSING;
    }
    else if (strcmp(argv[1], "or") == 0) {
        mode = IEEE802154_CCA_MODE_ED_THRESH_OR_CS;
    }
    else {
        mode = IEEE802154_CCA_MODE_ED_THRESH_AND_CS;
    }

    if (ieee802154_radio_set_cca_mode(dev, mode) == -ENOTSUP) {
        puts("CCA mode not supported.");
        return 1;
    }
    puts("CCA mode set.");

    if (argc > 2) {
        int8_t thresh = atoi(argv[2]);
        if (ieee802154_radio_set_cca_threshold(dev, thresh) < 0) {
            puts("Error setting the threshold");
        }
        else {
            printf("Set threshold to %i\n", thresh);
            res = 0;
        }
    }
    return res;
}

static int _caps_cmd(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    bool has_frame_retrans = false;
    bool has_auto_csma = false;
    bool has_24_ghz = false;
    bool has_sub_ghz = false;
    bool has_irq_tx_done = false;
    bool has_irq_rx_start = false;
    bool has_irq_ack_timeout = false;
    bool has_irq_cca_done = false;
    bool has_frame_retrans_info = false;
    bool has_phy_bpsk = false;
    bool has_phy_ask = false;
    bool has_phy_oqpsk = false;
    bool has_phy_mr_oqpsk = false;
    bool has_phy_mr_ofdm = false;
    bool has_phy_mr_fsk = false;

    if (ieee802154_radio_has_frame_retrans(&_radio)) {
       has_frame_retrans = true;
    }

    if (ieee802154_radio_has_auto_csma(&_radio)) {
        has_auto_csma = true;
    }

    if (ieee802154_radio_has_24_ghz(&_radio)) {
        has_24_ghz = true;
    }

    if (ieee802154_radio_has_sub_ghz(&_radio)) {
        has_sub_ghz = true;
    }

    if (ieee802154_radio_has_irq_tx_done(&_radio)) {
        has_irq_tx_done = true;
    }

    if (ieee802154_radio_has_irq_rx_start(&_radio)) {
        has_irq_rx_start = true;
    }

    if (ieee802154_radio_has_irq_ack_timeout(&_radio)) {
        has_irq_ack_timeout = true;
    }

    if (ieee802154_radio_has_irq_cca_done(&_radio)) {
        has_irq_cca_done = true;
    }

    if (ieee802154_radio_has_frame_retrans_info(&_radio)) {
        has_frame_retrans_info = true;
    }

    if (ieee802154_radio_has_phy_bpsk(&_radio)) {
        has_phy_bpsk = true;
    }

    if (ieee802154_radio_has_phy_ask(&_radio)) {
        has_phy_ask = true;
    }

    if (ieee802154_radio_has_phy_oqpsk(&_radio)) {
        has_phy_oqpsk = true;
    }

    if (ieee802154_radio_has_phy_mr_oqpsk(&_radio)) {
        has_phy_mr_oqpsk = true;
    }

    if (ieee802154_radio_has_phy_mr_ofdm(&_radio)) {
        has_phy_mr_ofdm = true;
    }

    if (ieee802154_radio_has_phy_mr_fsk(&_radio)) {
        has_phy_mr_fsk = true;
    }

    puts("               CAPS              ");
    puts("=================================");
    printf("- Frame Retransmissions:        %s\n", has_frame_retrans ? "y" : "n");
    printf("    * Info retries:             %s\n", has_frame_retrans_info ? "y" : "n");
    printf("- Auto CSMA-CA:                 %s\n", has_auto_csma ? "y" : "n");
    printf("- 2.4 GHz band:                 %s\n", has_24_ghz ? "y" : "n");
    printf("- SubGHz band:                  %s\n", has_sub_ghz ? "y" : "n");
    printf("- TX DONE indication:           %s\n", has_irq_tx_done ? "y" : "n");
    printf("    * ACK Timeout indication:   %s\n", has_irq_ack_timeout ? "y" : "n");
    printf("- RX_START indication:          %s\n", has_irq_rx_start ? "y" : "n");
    printf("- CCA Done indication:          %s\n", has_irq_cca_done ? "y" : "n");
    printf("- PHY modes:                      \n");
    printf("-   * BPSK:                     %s\n", has_phy_bpsk ? "y" : "n");
    printf("-   * ASK:                      %s\n", has_phy_ask ? "y" : "n");
    printf("-   * O-QPSK:                   %s\n", has_phy_oqpsk ? "y" : "n");
    printf("-   * MR-O-QPSK:                %s\n", has_phy_mr_oqpsk ? "y" : "n");
    printf("-   * MR-OFDM:                  %s\n", has_phy_mr_ofdm ? "y" : "n");
    printf("-   * MR-FSK:                   %s\n", has_phy_mr_fsk? "y" : "n");

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "config_phy", "Set channel and TX power", config_phy},
    { "print_addr", "Print IEEE802.15.4 addresses", print_addr},
    { "txtsnd", "Send IEEE 802.15.4 packet", txtsnd },
    { "test_states", "Test state changes", _test_states },
    { "cca", "Perform CCA", _cca },
    { "config_cca", "Config CCA parameters", _config_cca_cmd },
    { "promisc", "Set promiscuos mode", promisc },
    { "tx_mode", "Enable CSMA-CA, CCA or direct transmission", txmode_cmd },
    { "caps", "Get a list of caps supported by the device", _caps_cmd },
    { NULL, NULL, NULL }
};

int main(void)
{
    mutex_init(&lock);
    mutex_lock(&lock);
    _init();

    /* start the shell */
    puts("Initialization successful - starting the shell now");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
