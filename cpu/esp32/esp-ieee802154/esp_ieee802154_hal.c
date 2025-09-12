/*
 * SPDX-FileCopyrightText: 2025 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "esp_ieee802154.h"

#include "esp_ieee802154_hal.h"
#include "iolist.h"
#include "log.h"
#include "net/ieee802154/radio.h"

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

static const ieee802154_radio_ops_t esp_ieee802154_driver;
static ieee802154_dev_t *esp_ieee802154_dev;

static uint8_t  _tx_frame[IEEE802154_FRAME_LEN_MAX + 1]; /* additional byte 0 is used for length */
static uint8_t *_rx_frame;
static const uint8_t *_ack_frame;

static esp_ieee802154_frame_info_t *_rx_frame_info;
static esp_ieee802154_frame_info_t *_ack_frame_info;

static bool _channel_clear;

static esp_ieee802154_tx_error_t _tx_error;     /* error on last transmit */

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
    return _rx_frame[0] - IEEE802154_FCS_LEN;
}

static int _read(ieee802154_dev_t *dev, void *buf, size_t size, ieee802154_rx_info_t *info)
{
    (void)dev;

    DEBUG("[esp_ieee802154] %s: buf=%p size=%u info=%p\n",
          __func__, buf, size, info);

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
            info->rssi = ieee802154_dbm_to_rssi(_rx_frame[len]);
            info->lqi = _rx_frame[len+1];
        }
        memcpy(buf, &_rx_frame[1], len);
        res = len;
    }

    esp_ieee802154_receive_handle_done(_rx_frame);

    return res;
}

static int _off(ieee802154_dev_t *dev)
{
    (void)dev;
    DEBUG("[esp_ieee802154] %s\n", __func__);
    esp_ieee802154_disable();
    return 0;
}

static int _request_on(ieee802154_dev_t *dev)
{
    (void)dev;
    DEBUG("[esp_ieee802154] %s\n", __func__);
    esp_ieee802154_enable();
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

    switch (op) {
    case IEEE802154_HAL_OP_TRANSMIT:
        DEBUG("[esp_ieee802154] %s: IEEE802154_HAL_OP_TRANSMIT\n", __func__);
        _tx_error = ESP_IEEE802154_TX_ERR_NONE;
        _ack_frame = NULL;
        _ack_frame_info = NULL;
        return esp_ieee802154_transmit(_tx_frame, true) ? -EIO : 0;
    case IEEE802154_HAL_OP_SET_RX:
        DEBUG("[esp_ieee802154] %s: IEEE802154_HAL_OP_SET_RX\n", __func__);
        return (IS_ACTIVE(_USE_SET_RX) && esp_ieee802154_receive()) ? -EIO : 0;
    case IEEE802154_HAL_OP_SET_IDLE:
        /* TODO: ctx = (bool *force) */
        DEBUG("[esp_ieee802154] %s: IEEE802154_HAL_OP_SET_IDLE\n", __func__);
        return (IS_ACTIVE(_USE_SET_IDLE) && esp_ieee802154_sleep()) ? -EIO : 0;
        return 0;
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
                info->status = TX_STATUS_NO_ACK;
                break;
            default:
                break;
            }
        }
        esp_ieee802154_receive_handle_done(_ack_frame);
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

static int _set_frame_retrans(ieee802154_dev_t *dev, uint8_t retrans)
{
    (void)dev;
    DEBUG("[esp_ieee802154] %s\n", __func__);
    return -ENOTSUP;
}

static int _set_csma_params(ieee802154_dev_t *dev, const ieee802154_csma_be_t *bd,
                            int8_t retries)
{
    (void)dev;
    DEBUG("[esp_ieee802154] %s\n", __func__);
    return -ENOTSUP;
}

static int _set_frame_filter_mode(ieee802154_dev_t *dev, ieee802154_filter_mode_t mode)
{
    (void)dev;
    DEBUG("[esp_ieee802154] %s\n", __func__);
    return -ENOTSUP;
}

static int _config_addr_filter(ieee802154_dev_t *dev, ieee802154_af_cmd_t cmd, const void *value)
{
    (void)dev;

    assert(value);

    const uint8_t *addr = value;

    switch (cmd) {
    case IEEE802154_AF_SHORT_ADDR:
        DEBUG("[esp_ieee802154] %s: set IEEE802154_AF_SHORT_ADDR %02x:%02x\n",
              __func__, addr[0], addr[1]);
        return esp_ieee802154_set_short_address(*((const uint16_t *)value)) ? -EIO : 0;
    case IEEE802154_AF_EXT_ADDR:
        DEBUG("[esp_ieee802154] %s: set IEEE802154_AF_EXT_ADDR "
              "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n", __func__,
              addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], addr[6], addr[7]);
        return esp_ieee802154_set_extended_address(addr) ? -EIO : 0;
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

    _rx_frame = frame;
    _rx_frame_info = frame_info;

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

    _ack_frame = ack;
    _ack_frame_info = ack_frame_info;

    esp_ieee802154_dev->cb(esp_ieee802154_dev, IEEE802154_RADIO_CONFIRM_TX_DONE);
}

void esp_ieee802154_transmit_failed(const uint8_t *frame, esp_ieee802154_tx_error_t error)
{
    DEBUG("[esp_ieee802154] %s: frame=%p error=%u\n", __func__, frame, error);

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
}

int esp_ieee802154_init(void)
{
    DEBUG("[esp_ieee802154] %s\n", __func__);

    esp_ieee802154_enable();
    esp_ieee802154_reset_pending_table(true);
    esp_ieee802154_reset_pending_table(false);
    esp_ieee802154_set_rx_when_idle(true);
    esp_ieee802154_receive();
    esp_ieee802154_set_cca_mode(ESP_IEEE802154_CCA_MODE_ED);
    esp_ieee802154_set_ack_timeout(ESP_IEEE802154_ACK_TIMEOUT_US);
    esp_ieee802154_set_promiscuous(false);

    return 0;
}

static const ieee802154_radio_ops_t esp_ieee802154_driver = {
    .caps = IEEE802154_CAP_24_GHZ
          | IEEE802154_CAP_PHY_OQPSK
          | IEEE802154_CAP_AUTO_CSMA
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
    .set_frame_retrans = _set_frame_retrans,
};
