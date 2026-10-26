/*
 * SPDX-FileCopyrightText: 2025 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "esp_ieee802154.h"

#include "esp_ieee802154_hal.h"
#include "iolist.h"
#include "irq.h"
#include "log.h"
#include "net/ieee802154/radio.h"
#include "ztimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* ACK frame timeout in microseconds, should be a multiple of 16 */
#define ESP_IEEE802154_ACK_TIMEOUT_US (3456)

_Static_assert((3456 % 16 == 0), "ACK frame timeout should be a multiple of 16");

/* Although the device driver supports IEEE802154_CAP_IRQ_TX_START,
 * IEEE802154_CAP_IRQ_RX_START and IEEE802154_CAP_IRQ_CCA_DONE, we are not
 * using it for now to avoid unnecessary performance degradation as it is
 * not used by any link layer driver */
#define _USE_RX_START   0
#define _USE_TX_START   0
#define _USE_CCA_DONE   0

#define _USE_SET_RX     1
#define _USE_SET_IDLE   1

/*
 * Timeout in microseconds after which a transmission is aborted if the
 * ESP-IDF driver did not report its completion at all.
 *
 * This is only a safety net against a driver that never completes a
 * transmission, in which case the upper layer would wait forever for the
 * TX_DONE confirmation and the interface would stop sending. It is not meant
 * to treat long waiting times as failure:
 * Interruptions by the coexistence with WiFi or BLE are reported by the
 * ESP-IDF driver itself, but waiting for the radio to be granted, e.g. during
 * a WiFi scan, can take a long time. The timeout therefore has to be much
 * larger than such waiting times and than the 200 ms software timeout that
 * the ESP-IDF driver itself uses while waiting for an ACK frame.
 */
#ifndef ESP_IEEE802154_TX_TIMEOUT_US
#  define ESP_IEEE802154_TX_TIMEOUT_US    (1000U * US_PER_MS)
#endif

static const ieee802154_radio_ops_t esp_ieee802154_driver;
static ieee802154_dev_t *esp_ieee802154_dev;

static uint8_t  _tx_frame[IEEE802154_FRAME_LEN_MAX + 1]; /* additional byte 0 is used for length */
static const uint8_t *_rx_frame;
static const uint8_t *_ack_frame;

static esp_ieee802154_frame_info_t *_rx_frame_info;
static esp_ieee802154_frame_info_t *_ack_frame_info;

static bool _channel_clear;

static esp_ieee802154_tx_error_t _tx_error;     /* error on last transmit */
static bool _cca_send_on = true;

static bool _tx_pending;    /* transmission in progress */
static ztimer_t _tx_timer;  /* timer in case the driver never completes the transmission */

/*
 * `_rx_frame` and `_rx_frame_info` describe the frame of the pending
 * reception, if any. The ESP-IDF driver keeps each received frame in one of
 * its RX buffers until it is released by `esp_ieee802154_receive_handle_done`.
 * Once all RX buffers are in use, the ESP-IDF driver drops all received
 * frames.
 *
 * According to the radio HAL, RX_DONE opens a reception which is closed by
 * exactly one call of `read`, new frame receptions may be blocked while a
 * reception is pending, and the radio doesn't receive while it is not in RX
 * state. Therefore, a frame that is received while a reception is pending or
 * while the radio was not requested to be in RX state (`_rx_on`) is dropped
 * and its RX buffer is released immediately.
 */
static bool _rx_on;         /* radio was requested to be in RX state */

/* release a frame received by the ESP-IDF driver, if any */
static inline void _release_frame(const uint8_t **frame)
{
    if (*frame) {
        esp_ieee802154_receive_handle_done(*frame);
        *frame = NULL;
    }
}

/*
 * End the pending transmission, i.e. clear `_tx_pending` and stop the TX
 * timeout. Returns false if no transmission was pending, e.g. on a late
 * completion of a transmission that was already aborted by the timeout.
 */
static inline bool _tx_end(void)
{
    unsigned state = irq_disable();
    if (!_tx_pending) {
        irq_restore(state);
        return false;
    }
    _tx_pending = false;
    ztimer_remove(ZTIMER_USEC, &_tx_timer);
    irq_restore(state);

    return true;
}

static int _write(ieee802154_dev_t *dev, const iolist_t *psdu)
{
    (void)dev;

    DEBUG("[esp_ieee802154] %s\n", __func__);

    assert(psdu);

    /* copy packet data into the _tx_frame buffer */
    uint8_t *dst = &_tx_frame[1];   /* first byte is frame length */

    for (; psdu; psdu = psdu->iol_next) {
        if (psdu->iol_len) {
            assert(((dst - &_tx_frame[1]) +
                    psdu->iol_len + IEEE802154_FCS_LEN) < ARRAY_SIZE(_tx_frame));
            memcpy(dst, psdu->iol_base, psdu->iol_len);
            dst += psdu->iol_len;
        }
    }

    /* length of the package. */
    _tx_frame[0] = (dst - &_tx_frame[1]) + IEEE802154_FCS_LEN;

    DEBUG("[esp_ieee802154] %s: put %d bytes to _tx_frame\n", __func__, _tx_frame[0]);

    return 0;
}

static int _len(ieee802154_dev_t *dev)
{
    (void)dev;
    DEBUG("[esp_ieee802154] %s\n", __func__);
    return _rx_frame ? (_rx_frame[0] - IEEE802154_FCS_LEN) : 0;
}

static int _read(ieee802154_dev_t *dev, void *buf, size_t size, ieee802154_rx_info_t *info)
{
    (void)dev;

    DEBUG("[esp_ieee802154] %s: buf=%p size=%u info=%p\n",
          __func__, buf, size, info);

    /* `_rx_frame` and `_rx_frame_info` are not changed by the ESP-IDF driver
     * callbacks while a reception is pending */
    if (_rx_frame == NULL) {
        DEBUG("[esp_ieee802154] %s: no frame pending\n", __func__);
        return 0;
    }

    unsigned len = _rx_frame[0] - IEEE802154_FCS_LEN;
    int res = 0;

    if (size < len) {
        DEBUG("[esp_ieee802154] %s: buffer to small (%u < %u)\n",
              __func__, size, len);
        res = -ENOBUFS;
    }
    else if (buf) {
        DEBUG("[esp_ieee802154] %s: read packet of length %d\n", __func__, len);
        if (info) {
            info->rssi = ieee802154_dbm_to_rssi(_rx_frame_info->rssi);
            info->lqi = _rx_frame_info->lqi;
        }
        memcpy(buf, &_rx_frame[1], len);
        res = len;
    }

    /* close the reception */
    _release_frame(&_rx_frame);

    return res;
}

static void _tx_timeout(void *arg)
{
    (void)arg;

    /* the completion of the transmission may be reported concurrently */
    if (!_tx_end()) {
        return;
    }
    _tx_error = ESP_IEEE802154_TX_ERR_ABORT;

    LOG_WARNING("[esp_ieee802154] TX timeout in state %d, transmission aborted\n",
                esp_ieee802154_get_state());

    esp_ieee802154_dev->cb(esp_ieee802154_dev, IEEE802154_RADIO_CONFIRM_TX_DONE);
}

static int _off(ieee802154_dev_t *dev)
{
    (void)dev;
    DEBUG("[esp_ieee802154] %s\n", __func__);
    _rx_on = false;

    /* don't allow multiple disable */
    if (esp_ieee802154_get_state() != ESP_IEEE802154_RADIO_DISABLE) {
        esp_ieee802154_disable();
    }
    /* close a pending reception and release the RX buffer of its frame,
     * the ESP-IDF driver is disabled and doesn't report frames anymore */
    _release_frame(&_rx_frame);
    return 0;
}

static int _request_on(ieee802154_dev_t *dev)
{
    (void)dev;
    DEBUG("[esp_ieee802154] %s\n", __func__);

    /* don't allow multiple enabling to prevent the module clock from
       continuing to run after a single disabling. */
    if ((esp_ieee802154_get_state() == ESP_IEEE802154_RADIO_DISABLE) &&
        (esp_ieee802154_enable() != ESP_OK)) {
        return -EIO;
    }

    /* Enabling the ESP-IDF driver initializes the MAC hardware and resets
     * the whole configuration (PIB) to the ESP-IDF defaults, including the
     * promiscuous mode, which is enabled by default. Therefore, the radio is
     * configured each time it is switched on. */
    esp_ieee802154_reset_pending_table(true);
    esp_ieee802154_reset_pending_table(false);
    esp_ieee802154_set_cca_mode(ESP_IEEE802154_CCA_MODE_ED);
    esp_ieee802154_set_ack_timeout(ESP_IEEE802154_ACK_TIMEOUT_US);
    esp_ieee802154_set_promiscuous(false);

    return 0;
}

static int _confirm_on(ieee802154_dev_t *dev)
{
    (void)dev;
    DEBUG("[esp_ieee802154] %s\n", __func__);
    return 0;
}

static int _request_op(ieee802154_dev_t *dev, ieee802154_hal_op_t op, void *ctx)
{
    (void)dev;
    (void)ctx;

    /* any operation except IEEE802154_HAL_OP_SET_RX ends the RX state */
    _rx_on = false;

    switch (op) {
    case IEEE802154_HAL_OP_TRANSMIT:
        DEBUG("[esp_ieee802154] %s: IEEE802154_HAL_OP_TRANSMIT\n", __func__);
        _tx_error = ESP_IEEE802154_TX_ERR_NONE;
        _ack_frame = NULL;
        _ack_frame_info = NULL;
        _tx_pending = true;
        ztimer_set(ZTIMER_USEC, &_tx_timer, ESP_IEEE802154_TX_TIMEOUT_US);
        if (esp_ieee802154_transmit(_tx_frame, _cca_send_on)) {
            _tx_end();
            return -EIO;
        }
        return 0;
    case IEEE802154_HAL_OP_SET_RX:
        DEBUG("[esp_ieee802154] %s: IEEE802154_HAL_OP_SET_RX\n", __func__);
        /* set before enabling the receiver, frames are dropped otherwise */
        _rx_on = true;
        if (IS_ACTIVE(_USE_SET_RX) && esp_ieee802154_receive()) {
            _rx_on = false;
            return -EIO;
        }
        return 0;
    case IEEE802154_HAL_OP_SET_IDLE:
        /* TODO: ctx = (bool *force) */
        DEBUG("[esp_ieee802154] %s: IEEE802154_HAL_OP_SET_IDLE\n", __func__);
        return (IS_ACTIVE(_USE_SET_IDLE) && esp_ieee802154_sleep()) ? -EIO : 0;
    case IEEE802154_HAL_OP_CCA:
        extern esp_err_t esp_ieee802154_cca(void);
        DEBUG("[esp_ieee802154] %s: IEEE802154_HAL_OP_CCA\n", __func__);
        return esp_ieee802154_cca();
    }

    return -EINVAL;
}

static int _confirm_op(ieee802154_dev_t *dev, ieee802154_hal_op_t op, void *ctx)
{
    (void)dev;

    esp_ieee802154_state_t state = esp_ieee802154_get_state();

    switch (op) {
    case IEEE802154_HAL_OP_TRANSMIT:
        DEBUG("[esp_ieee802154] %s: IEEE802154_HAL_OP_TRANSMIT\n", __func__);

        if (ctx) {
            ieee802154_tx_info_t *info = ctx;

            switch (_tx_error) {
            case ESP_IEEE802154_TX_ERR_NONE:
                info->status = TX_STATUS_SUCCESS;
                if (_ack_frame_info && _ack_frame_info->pending) {
                    info->status = TX_STATUS_FRAME_PENDING;
                }
                break;
            case ESP_IEEE802154_TX_ERR_CCA_BUSY:
                info->status = TX_STATUS_MEDIUM_BUSY;
                break;
            case ESP_IEEE802154_TX_ERR_NO_ACK:
            case ESP_IEEE802154_TX_ERR_INVALID_ACK:
                info->status = TX_STATUS_NO_ACK;
                break;
            default:
                /* ESP_IEEE802154_TX_ERR_ABORT, ESP_IEEE802154_TX_ERR_COEXIST,
                 * ...: the frame was not sent, the upper layer may retry */
                info->status = TX_STATUS_MEDIUM_BUSY;
                break;
            }
        }
        _release_frame(&_ack_frame);
        if (_tx_error == ESP_IEEE802154_TX_ERR_ABORT) {
            /* After an aborted transmission, e.g. by the TX timeout, the
             * operation may still be in progress in the ESP-IDF driver.
             * Stop it so that its completion can't be reported for a later
             * transmission. The radio is left in IDLE state and not set
             * back to RX, the upper layer decides about the next state. */
            esp_ieee802154_sleep();
        }
        return 0;
    case IEEE802154_HAL_OP_SET_RX:
        DEBUG("[esp_ieee802154] %s: IEEE802154_HAL_OP_SET_RX\n", __func__);
        return (!IS_ACTIVE(_USE_SET_RX) ||
                (state == ESP_IEEE802154_RADIO_RECEIVE)) ? 0 : -EAGAIN;
    case IEEE802154_HAL_OP_SET_IDLE:
        DEBUG("[esp_ieee802154] %s: IEEE802154_HAL_OP_SET_IDLE\n", __func__);
        return (!IS_ACTIVE(_USE_SET_IDLE) ||
                ((state == ESP_IEEE802154_RADIO_IDLE) ||
                 (state == ESP_IEEE802154_RADIO_SLEEP))) ? 0 : -EAGAIN;
    case IEEE802154_HAL_OP_CCA:
        DEBUG("[esp_ieee802154] %s: IEEE802154_HAL_OP_CCA\n", __func__);
        assert(ctx);
        *((bool *)ctx) = _channel_clear;
        return 0;
    }

    return -EINVAL;
}

static int _set_cca_threshold(ieee802154_dev_t *dev, int8_t threshold)
{
    (void)dev;
    DEBUG("[esp_ieee802154] %s: threshold %i\n", __func__, threshold);
    return esp_ieee802154_set_cca_threshold(threshold) ? -EIO : 0;
}

static int _set_cca_mode(ieee802154_dev_t *dev, ieee802154_cca_mode_t mode)
{
    (void)dev;

    switch (mode) {
    case IEEE802154_CCA_MODE_ED_THRESHOLD:
        DEBUG("[esp_ieee802154] %s: set IEEE802154_CCA_MODE_ED_THRESHOLD\n", __func__);
        return esp_ieee802154_set_cca_mode(ESP_IEEE802154_CCA_MODE_ED) ? -EIO : 0;
    case IEEE802154_CCA_MODE_CARRIER_SENSING:
        DEBUG("[esp_ieee802154] %s: set IEEE802154_CCA_MODE_CARRIER_SENSING\n", __func__);
        return esp_ieee802154_set_cca_mode(ESP_IEEE802154_CCA_MODE_CARRIER) ? -EIO : 0;
    case IEEE802154_CCA_MODE_ED_THRESH_AND_CS:
        DEBUG("[esp_ieee802154] %s: set IEEE802154_CCA_MODE_ED_THRESH_AND_CS\n", __func__);
        return esp_ieee802154_set_cca_mode(ESP_IEEE802154_CCA_MODE_CARRIER_AND_ED) ? -EIO : 0;
    case IEEE802154_CCA_MODE_ED_THRESH_OR_CS:
        DEBUG("[esp_ieee802154] %s: IEEE802154_CCA_MODE_ED_THRESH_OR_CS\n", __func__);
        return esp_ieee802154_set_cca_mode(ESP_IEEE802154_CCA_MODE_CARRIER_OR_ED) ? -EIO : 0;
    }

    return -EINVAL;
}

static int _config_phy(ieee802154_dev_t *dev, const ieee802154_phy_conf_t *conf)
{
    (void)dev;

    assert(conf);

    DEBUG("[esp_ieee802154] %s\n", __func__);

    if (esp_ieee802154_set_txpower(conf->pow) ||
        esp_ieee802154_set_channel(conf->channel)) {
        return -EIO;
    }

    return 0;
}

static int _set_csma_params(ieee802154_dev_t *dev, const ieee802154_csma_be_t *bd,
                            int8_t retries)
{
    (void)dev;
    DEBUG("[esp_ieee802154] %s %p %d\n", __func__, bd, retries);
    if (retries > 0) {
        return -EINVAL;
    }

    _cca_send_on = retries == 0;
    return 0;
}

static int _set_frame_filter_mode(ieee802154_dev_t *dev, ieee802154_filter_mode_t mode)
{
    (void)dev;

    switch (mode) {
    case IEEE802154_FILTER_ACCEPT:
        DEBUG("[esp_ieee802154] %s: set IEEE802154_FILTER_ACCEPT\n", __func__);
        esp_ieee802154_set_promiscuous(false);
        return 0;
    case IEEE802154_FILTER_PROMISC:
        DEBUG("[esp_ieee802154] %s: set IEEE802154_FILTER_PROMISC\n", __func__);
        esp_ieee802154_set_promiscuous(true);
        return 0;
    case IEEE802154_FILTER_ACK_ONLY:
    case IEEE802154_FILTER_SNIFFER:
        DEBUG("[esp_ieee802154] %s: filter mode %d not supported\n", __func__, (int)mode);
        /* fall through */
    default:
        return -ENOTSUP;
    }
}

static int _config_addr_filter(ieee802154_dev_t *dev, ieee802154_af_cmd_t cmd, const void *value)
{
    (void)dev;

    assert(value);

    const uint8_t *addr = value;

    switch (cmd) {
    case IEEE802154_AF_SHORT_ADDR:
        const network_uint16_t *short_addr = value;
        DEBUG("[esp_ieee802154] %s: set IEEE802154_AF_SHORT_ADDR %02x:%02x\n",
              __func__, addr[0], addr[1]);
        return esp_ieee802154_set_short_address(byteorder_ntohs(*short_addr)) ? -EIO : 0;
    case IEEE802154_AF_EXT_ADDR:
        DEBUG("[esp_ieee802154] %s: set IEEE802154_AF_EXT_ADDR "
              "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n", __func__,
              addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], addr[6], addr[7]);
        const eui64_t *ext_addr = value;
        uint64_t le_addr = byteorder_swapll(ext_addr->uint64.u64);
        return esp_ieee802154_set_extended_address((const uint8_t *)&le_addr) ? -EIO : 0;
    case IEEE802154_AF_PANID:
        DEBUG("[esp_ieee802154] %s: set IEEE802154_AF_PANID %02x:%02x\n",
              __func__, addr[1], addr[0]);
        return esp_ieee802154_set_panid(*((const uint16_t *)value)) ? -EIO : 0;
    case IEEE802154_AF_PAN_COORD:
        DEBUG("[esp_ieee802154] %s: set IEEE802154_AF_PAN_COORD not supported\n", __func__);
        return -ENOTSUP;
    }

    return -EINVAL;
}

static int _config_src_addr_match(ieee802154_dev_t *dev, ieee802154_src_match_t cmd,
                                  const void *value)
{
    (void)dev;

    bool enable = *((bool*)value);

    switch (cmd) {
    case IEEE802154_SRC_MATCH_EN:
        DEBUG("[esp_ieee802154] %s: IEEE802154_SRC_MATCH_EN %d\n", __func__, enable);
        return esp_ieee802154_set_pending_mode(enable
                                                ? ESP_IEEE802154_AUTO_PENDING_ENABLE
                                                : ESP_IEEE802154_AUTO_PENDING_DISABLE) ? -EIO : 0;
    case IEEE802154_SRC_MATCH_SHORT_ADD:
        DEBUG("[esp_ieee802154] %s: set IEEE802154_SRC_MATCH_SHORT_ADD\n", __func__);
        return esp_ieee802154_add_pending_addr(value, true) ? -ENOMEM : 0;
    case IEEE802154_SRC_MATCH_SHORT_CLEAR:
        DEBUG("[esp_ieee802154] %s: set IEEE802154_SRC_MATCH_SHORT_CLEAR\n", __func__);
        return esp_ieee802154_clear_pending_addr(value, true) ? -ENOENT : 0;
    case IEEE802154_SRC_MATCH_EXT_ADD:
        DEBUG("[esp_ieee802154] %s: set IEEE802154_SRC_MATCH_EXT_ADD\n", __func__);
        return esp_ieee802154_add_pending_addr(value, false) ? -ENOMEM : 0;
    case IEEE802154_SRC_MATCH_EXT_CLEAR:
        DEBUG("[esp_ieee802154] %s: set IEEE802154_SRC_MATCH_EXT_CLEAR\n", __func__);
        return esp_ieee802154_clear_pending_addr(value, false) ? -ENOENT : 0;
    }

    return -EINVAL;
}

/* following functions are called back from ESP-IDF driver */

void esp_ieee802154_cca_done(bool channel_busy)
{
    DEBUG("[esp_ieee802154] %s %d\n", __func__, channel_busy);
    _channel_clear = !channel_busy;

    if (IS_ACTIVE(_USE_CCA_DONE)) {
        esp_ieee802154_dev->cb(esp_ieee802154_dev, IEEE802154_RADIO_CONFIRM_CCA);
    }
}

void esp_ieee802154_receive_sfd_done(void)
{
    DEBUG("[esp_ieee802154] %s\n", __func__);
    if (IS_ACTIVE(_USE_RX_START)) {
        esp_ieee802154_dev->cb(esp_ieee802154_dev, IEEE802154_RADIO_INDICATION_RX_START);
    }
}

void esp_ieee802154_receive_done(uint8_t *frame, esp_ieee802154_frame_info_t *frame_info)
{
    assert(frame);
    assert(frame_info);

    DEBUG("[esp_ieee802154] %s: frame=%p frame_info=%p\n",
          __func__, frame, frame_info);

    unsigned state = irq_disable();
    if (!_rx_on || _rx_frame) {
        irq_restore(state);
        /* The radio is not in RX state or a reception is still pending.
         * Drop the frame and release its RX buffer, otherwise the RX buffers
         * of the ESP-IDF driver would get exhausted. */
        DEBUG("[esp_ieee802154] %s: frame dropped\n", __func__);
        esp_ieee802154_receive_handle_done(frame);
        return;
    }
    _rx_frame = frame;
    _rx_frame_info = frame_info;
    irq_restore(state);

    esp_ieee802154_dev->cb(esp_ieee802154_dev, IEEE802154_RADIO_INDICATION_RX_DONE);
}

void esp_ieee802154_transmit_sfd_done(uint8_t *frame)
{
    DEBUG("[esp_ieee802154] %s: frame=%p\n", __func__, frame);
    if (IS_ACTIVE(_USE_TX_START)) {
        esp_ieee802154_dev->cb(esp_ieee802154_dev, IEEE802154_RADIO_INDICATION_TX_START);
    }
}

void esp_ieee802154_transmit_done(const uint8_t *frame, const uint8_t *ack,
                                  esp_ieee802154_frame_info_t *ack_frame_info)
{
    DEBUG("[esp_ieee802154] %s: frame=%p ack_frame=%p ack_frame_info=%p\n",
          __func__, frame, ack, ack_frame_info);

    if (!_tx_end()) {
        /* late completion of an already aborted transmission */
        _release_frame(&ack);
        return;
    }

    _ack_frame = ack;
    _ack_frame_info = ack_frame_info;

    esp_ieee802154_dev->cb(esp_ieee802154_dev, IEEE802154_RADIO_CONFIRM_TX_DONE);
}

void esp_ieee802154_transmit_failed(const uint8_t *frame, esp_ieee802154_tx_error_t error)
{
    DEBUG("[esp_ieee802154] %s: frame=%p error=%u\n", __func__, frame, error);

    if (!_tx_end()) {
        /* late completion of an already aborted transmission */
        return;
    }

    _tx_error = error;

    esp_ieee802154_dev->cb(esp_ieee802154_dev, IEEE802154_RADIO_CONFIRM_TX_DONE);
}

void esp_ieee802154_energy_detect_done(int8_t power)
{
    DEBUG("[esp_ieee802154] %s: power %i\n", __func__, power);
}

void esp_ieee802154_setup(ieee802154_dev_t *dev)
{
    assert(dev);

    DEBUG("[esp_ieee802154] %s: dev=%p\n", __func__, dev);

    dev->driver = &esp_ieee802154_driver;
    esp_ieee802154_dev = dev;

    _tx_timer.callback = _tx_timeout;
    _tx_timer.arg = NULL;
}

int esp_ieee802154_init(void)
{
    DEBUG("[esp_ieee802154] %s\n", __func__);

    /* The radio is enabled and configured when it is switched on by the
     * upper layer, see _request_on. */

    return 0;
}

static const ieee802154_radio_ops_t esp_ieee802154_driver = {
    .caps = IEEE802154_CAP_24_GHZ
          | IEEE802154_CAP_PHY_OQPSK
          | IEEE802154_CAP_AUTO_ACK
          | IEEE802154_CAP_SRC_ADDR_MATCH
#if _USE_CCA_DONE
          | IEEE802154_CAP_IRQ_CCA_DONE
#endif
#if _USE_RX_START
          | IEEE802154_CAP_IRQ_RX_START
#endif
#if _USE_TX_START
          | IEEE802154_CAP_IRQ_TX_START
#endif
          | IEEE802154_CAP_IRQ_TX_DONE
          | IEEE802154_CAP_IRQ_ACK_TIMEOUT,
    .write = _write,
    .read = _read,
    .request_on = _request_on,
    .confirm_on = _confirm_on,
    .len = _len,
    .off = _off,
    .request_op = _request_op,
    .confirm_op = _confirm_op,
    .set_cca_threshold = _set_cca_threshold,
    .set_cca_mode = _set_cca_mode,
    .config_phy = _config_phy,
    .set_csma_params = _set_csma_params,
    .config_addr_filter = _config_addr_filter,
    .config_src_addr_match = _config_src_addr_match,
    .set_frame_filter_mode = _set_frame_filter_mode,
};
