/*
 * Copyright (C) 2020 HAW Hamburg
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
 * @brief       Test application for IEEE 802.15.4 Radio HAL
 *
 * @author      José I. Alamos <jose.alamos@haw-hamburg.de>
 *
 * @}
 */

#include <assert.h>
#include <stdio.h>
#include <strings.h>

#include "common.h"
#include "errno.h"
#include "event/thread.h"
#include "luid.h"

#include "net/ieee802154.h"
#include "net/ieee802154/radio.h"

#include "shell.h"
#include "shell_commands.h"

#include "xtimer.h"

#define SYMBOL_TIME (16U) /**< 16 us */

#define ACK_TIMEOUT_TIME (40 * SYMBOL_TIME)

#define RADIO_DEFAULT_ID (0U)

static inline void _set_trx_state(int state, bool verbose);

static uint8_t buffer[127];
static xtimer_t timer_ack;
static mutex_t lock;

static const char *str_states[3]= {"TRX_OFF", "RX", "TX"};
static ieee802154_rx_mode_t current_rx_mode;
static eui64_t ext_addr;
static network_uint16_t short_addr;
static uint8_t seq;

static void _print_packet(size_t size, uint8_t lqi, int16_t rssi)
{
    if (buffer[0] & IEEE802154_FCF_TYPE_ACK && ((seq-1) == buffer[2])) {
        printf("Received valid ACK with sqn %i\n", buffer[2]);
    }
    else {
        puts("Packet received:");
        for (unsigned i=0;i<size;i++) {
            printf("%02x ", buffer[i]);
        }
    }
    puts("");
    printf("LQI: %i, RSSI: %i\n", (int) lqi, (int) rssi);
    puts("");
}

static int print_addr(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    uint8_t *_p = (uint8_t*) &ext_addr;
    for(int i=0;i<8;i++) {
        printf("%02x", *_p++);
    }
    printf("\n");
    return 0;
}

static void _ack_timeout(event_t *event)
{
    (void) event;
    ieee802154_dev_t *dev = ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID);

    ieee802154_radio_set_rx_mode(dev, current_rx_mode);
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
    puts("Packet with invalid CRC received");
    ieee802154_dev_t* dev = ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID);
    /* switch back to RX_ON state */
    ieee802154_radio_request_set_trx_state(dev, IEEE802154_TRX_STATE_RX_ON);
    while (ieee802154_radio_confirm_set_trx_state(dev) == -EAGAIN) {}
}

static event_t _crc_error_event = {
    .handler = _crc_error_handler,
};


void _rx_done_handler(event_t *event)
{
    (void) event;
    ieee802154_rx_info_t info;

    /* Read packet from internal framebuffer
     *
     * NOTE: It's possible to call `ieee802154_radio_len` to retrieve the packet
     * length. Since the buffer is fixed in this test, we don't use it
     */
    int size = ieee802154_radio_read(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID), buffer, 127, &info);
    if (size > 0) {
        /* Print packet while we wait for the state transition */
        _print_packet(size, info.lqi, info.rssi);
    }

    /* Go out of the HAL's FB Lock state after frame reception and trigger a
     * state change */
    _set_trx_state(IEEE802154_TRX_STATE_RX_ON, false);
}

static event_t _rx_done_event = {
    .handler = _rx_done_handler,
};

/* Event Notification callback */
static void _hal_radio_cb(ieee802154_dev_t *dev, ieee802154_trx_ev_t status)
{
    (void) dev;
    switch(status) {
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
    assert(ieee802154_radio_confirm_transmit(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID), &tx_info) >= 0);

    if (!ieee802154_radio_has_irq_ack_timeout(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID)) && !ieee802154_radio_has_frame_retrans(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID))) {
        /* This is just to show how the MAC layer would handle ACKs... */
        _set_trx_state(IEEE802154_TRX_STATE_RX_ON, false);
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

    if (ieee802154_radio_has_frame_retrans_info(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID))) {
        printf("Retransmission attempts: %i\n", tx_info.retrans);
    }

    puts("");
}

static event_t _tx_finish_ev = {
    .handler = _tx_finish_handler,
};

static void _send(iolist_t *pkt)
{
    /* Request a state change to RX_ON */
    ieee802154_radio_request_set_trx_state(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID), IEEE802154_TRX_STATE_TX_ON);

    /* Write the packet to the radio */
    ieee802154_radio_write(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID), pkt);

    /* Block until the radio confirms the state change */
    while(ieee802154_radio_confirm_set_trx_state(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID)) == -EAGAIN);

    /* Trigger the transmit and wait for the mutex unlock (TX_DONE event) */
    ieee802154_radio_request_transmit(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID));
    ieee802154_radio_set_rx_mode(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID), IEEE802154_RX_WAIT_FOR_ACK);
    mutex_lock(&lock);

    event_post(EVENT_PRIO_HIGHEST, &_tx_finish_ev);
}

static int _init(void)
{
    ieee802154_hal_test_init_devs();

    /* Set the Event Notification */
    ((ieee802154_dev_t*) ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID))->cb = _hal_radio_cb;

    /* Note that addresses are not kept in the radio. This assumes MAC layers
     * already have a copy of the address */
    luid_get_eui64(&ext_addr);
    luid_get_short(&short_addr);

    /* Since the device was already initialized, turn on the radio.
     * The transceiver state will be "TRX_OFF" */
    ieee802154_radio_request_on(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID));
    while(ieee802154_radio_confirm_on(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID)) == -EAGAIN) {}

    current_rx_mode = IEEE802154_RX_AACK_ENABLED;
    ieee802154_radio_set_rx_mode(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID), current_rx_mode);

    uint16_t panid = CONFIG_IEEE802154_DEFAULT_PANID;
    /* Set all IEEE addresses */
    ieee802154_radio_config_addr_filter(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID),
                                        IEEE802154_AF_SHORT_ADDR, &short_addr);
    ieee802154_radio_config_addr_filter(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID),
                                        IEEE802154_AF_EXT_ADDR, &ext_addr);
    ieee802154_radio_config_addr_filter(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID),
                                        IEEE802154_AF_PANID, &panid);

    /* Set PHY configuration */
    ieee802154_phy_conf_t conf = {.channel=CONFIG_IEEE802154_DEFAULT_CHANNEL, .page=CONFIG_IEEE802154_DEFAULT_SUBGHZ_PAGE, .pow=CONFIG_IEEE802154_DEFAULT_TXPOWER};

    ieee802154_radio_config_phy(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID), &conf);

    /* ieee802154_radio_set_cca_mode*/
    ieee802154_radio_set_cca_mode(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID), IEEE802154_CCA_MODE_ED_THRESHOLD);
    ieee802154_radio_set_cca_threshold(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID), CONFIG_IEEE802154_CCA_THRESH_DEFAULT);

    /* Set the transceiver state to RX_ON in order to receive packets */
    _set_trx_state(IEEE802154_TRX_STATE_RX_ON, false);

    return 0;
}

uint8_t payload[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Etiam ornare lacinia mi elementum interdum ligula.";

static int send(uint8_t *dst, size_t dst_len,
                size_t len)
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

    flags = IEEE802154_FCF_TYPE_DATA | IEEE802154_FCF_ACK_REQ;

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

static inline int _dehex(char c, int default_)
{
    if ('0' <= c && c <= '9') {
        return c - '0';
    }
    else if ('A' <= c && c <= 'F') {
        return c - 'A' + 10;
    }
    else if ('a' <= c && c <= 'f') {
        return c - 'a' + 10;
    }
    else {
        return default_;
    }
}

static size_t _parse_addr(uint8_t *out, size_t out_len, const char *in)
{
    const char *end_str = in;
    uint8_t *out_end = out;
    size_t count = 0;
    int assert_cell = 1;

    if (!in || !*in) {
        return 0;
    }
    while (end_str[1]) {
        ++end_str;
    }

    while (end_str >= in) {
        int a = 0, b = _dehex(*end_str--, -1);
        if (b < 0) {
            if (assert_cell) {
                return 0;
            }
            else {
                assert_cell = 1;
                continue;
            }
        }
        assert_cell = 0;

        if (end_str >= in) {
            a = _dehex(*end_str--, 0);
        }

        if (++count > out_len) {
            return 0;
        }
        *out_end++ = (a << 4) | b;
    }
    if (assert_cell) {
        return 0;
    }
    /* out is reversed */

    while (out < --out_end) {
        uint8_t tmp = *out_end;
        *out_end = *out;
        *out++ = tmp;
    }

    return count;
}

int _cca(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    ieee802154_radio_request_cca(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID));
    mutex_lock(&lock);
    int res = ieee802154_radio_confirm_cca(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID));
    assert(res >= 0);

    if (res > 0) {
        puts("CLEAR");
    }
    else {
        puts("BUSY");
    }

    return 0;
}

static inline void _set_trx_state(int state, bool verbose)
{
    xtimer_ticks32_t a;
    int res = ieee802154_radio_request_set_trx_state(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID), state);
    if (verbose) {
        a = xtimer_now();
        if(res != 0) {
            printf("%i != 0 \n", res);
            assert(false);
        }
    }

    while ((res = ieee802154_radio_confirm_set_trx_state(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID))) == -EAGAIN) {}
    if (verbose) {
        if (res != 0) {
            printf("%i != 0 \n", res);
            assert(false);
        }
        uint32_t secs = xtimer_usec_from_ticks(xtimer_diff(xtimer_now(), a));
        printf("\tTransition took %" PRIu32 " usecs\n", secs);
    }
}

int _test_states(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    for (int i=0; i<3;i++) {
        printf("Setting state to: %s\n", str_states[i]);
        _set_trx_state(i, true);
        for (int j=0; j<3; j++) {
            if (i == j) {
                continue;
            }
            printf("%s-> %s\n", str_states[i], str_states[j]);
            _set_trx_state(j, true);
            printf("%s-> %s\n", str_states[j], str_states[i]);
            _set_trx_state(i, true);
        }
        puts("");
    }

    _set_trx_state(IEEE802154_TRX_STATE_RX_ON, true);

    return 0;
}

int txtsnd(int argc, char **argv)
{
    uint8_t addr[IEEE802154_LONG_ADDRESS_LEN];
    size_t len;
    size_t res;

    if (argc != 3) {
        puts("Usage: txtsnd <long_addr> <len>");
        return 1;
    }

    res = _parse_addr(addr, sizeof(addr), argv[1]);
    if (res == 0) {
        puts("Usage: txtsnd <long_addr> <len>");
        return 1;
    }
    len = atoi(argv[2]);
    return send(addr, res, len);
}

static int rx_mode_cmd(int argc, char **argv)
{
    ieee802154_rx_mode_t conf;
    if (argc < 2) {
        printf("Usage: %s <on|off|pend|promisc>", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "pend") == 0) {
        conf = IEEE802154_RX_AACK_FRAME_PENDING;
        puts("ACK enabled with Frame Pending");
    }
    else if (strcmp(argv[1], "off") == 0) {
        conf = IEEE802154_RX_AACK_DISABLED;
        puts("ACK disabled");
    }
    else if (strcmp(argv[1], "promisc") == 0) {
        conf = IEEE802154_RX_PROMISC;
        puts("Promiscuous mode enabled");
    }
    else {
        conf = IEEE802154_RX_AACK_ENABLED;
        puts("ACK enabled");
    }

    current_rx_mode = conf;
    ieee802154_radio_set_rx_mode(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID), conf);
    return 0;
}

int config_phy(int argc, char **argv)
{
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
    _set_trx_state(IEEE802154_TRX_STATE_TRX_OFF, false);
    ieee802154_dev_t *dev = ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID);
    ieee802154_phy_conf_t conf = {.phy_mode=phy_mode, .channel=channel, .page=0, .pow=tx_pow};
    if (ieee802154_radio_config_phy(dev, &conf) < 0) {
        puts("Channel or TX power settings not supported");
    }
    else {
        puts("Success!");
    }

    _set_trx_state(IEEE802154_TRX_STATE_RX_ON, false);

    return 0;
}

int txmode_cmd(int argc, char **argv)
{
    ieee802154_dev_t *dev = ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID);

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
    }
    return 0;
}

static int _config_cca_cmd(int argc, char **argv)
{
    ieee802154_dev_t *dev = ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID);
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
            return 1;
        }
        else {
            printf("Set threshold to %i\n", thresh);
        }
    }
    return 0;
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

    if (ieee802154_radio_has_frame_retrans(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID))) {
       has_frame_retrans = true;
    }

    if (ieee802154_radio_has_auto_csma(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID))) {
        has_auto_csma = true;
    }

    if (ieee802154_radio_has_24_ghz(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID))) {
        has_24_ghz = true;
    }

    if (ieee802154_radio_has_sub_ghz(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID))) {
        has_sub_ghz = true;
    }

    if (ieee802154_radio_has_irq_tx_done(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID))) {
        has_irq_tx_done = true;
    }

    if (ieee802154_radio_has_irq_rx_start(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID))) {
        has_irq_rx_start = true;
    }

    if (ieee802154_radio_has_irq_ack_timeout(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID))) {
        has_irq_ack_timeout = true;
    }

    if (ieee802154_radio_has_irq_cca_done(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID))) {
        has_irq_cca_done = true;
    }

    if (ieee802154_radio_has_frame_retrans_info(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID))) {
        has_frame_retrans_info = true;
    }

    if (ieee802154_radio_has_phy_bpsk(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID))) {
        has_phy_bpsk = true;
    }

    if (ieee802154_radio_has_phy_ask(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID))) {
        has_phy_ask = true;
    }

    if (ieee802154_radio_has_phy_oqpsk(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID))) {
        has_phy_oqpsk = true;
    }

    if (ieee802154_radio_has_phy_mr_oqpsk(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID))) {
        has_phy_mr_oqpsk = true;
    }

    if (ieee802154_radio_has_phy_mr_ofdm(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID))) {
        has_phy_mr_ofdm = true;
    }

    if (ieee802154_radio_has_phy_mr_fsk(ieee802154_hal_test_get_dev(RADIO_DEFAULT_ID))) {
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
    { "rx_mode", "Enable/Disable AACK or set Frame Pending bit or set promiscuos mode", rx_mode_cmd },
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
