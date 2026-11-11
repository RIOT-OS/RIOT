/*
 * SPDX-FileCopyrightText: 2026 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     pkg_meshcore
 * @{
 *
 * @file
 * @brief       RIOT implementation of the abstractions of MeshCore
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <errno.h>
#include <string.h>

#include "kernel_defines.h"
#include "log.h"
#include "net/netdev/lora.h"
#include "random.h"
#include "thread.h"
#include "thread_flags.h"
#include "timex.h"
#include "ztimer.h"

#if IS_USED(MODULE_PERIPH_HWRNG)
#include "periph/hwrng.h"
#endif

#include "meshcore_riot.h"

#define ENABLE_DEBUG 0
#include "debug.h"

namespace meshcore_riot {

/* Thread flag set when the radio has a pending interrupt */
#define FLAG_RADIO_ISR      (1U << 0)

static int _set(netdev_t *dev, netopt_t opt, const void *value, size_t len)
{
    int res = dev->driver->set(dev, opt, value, len);

    if (res < 0) {
        DEBUG("[meshcore] can't set option %d: %d\n", opt, res);
    }
    return res;
}

void Radio::_event_cb(netdev_t *dev, netdev_event_t event)
{
    Radio *radio = static_cast<Radio *>(dev->context);

    if (event == NETDEV_EVENT_ISR) {
        thread_flags_set(thread_get(radio->_pid), FLAG_RADIO_ISR);
    }
    else {
        radio->_onEvent(event);
    }
}

void Radio::_onEvent(netdev_event_t event)
{
    switch (event) {
    case NETDEV_EVENT_RX_STARTED:
        _receiving = true;
        _rx_start = ztimer_now(ZTIMER_MSEC);
        break;

    case NETDEV_EVENT_RX_COMPLETE: {
        int len = _dev->driver->recv(_dev, NULL, 0, NULL);
        if ((len > 0) && ((size_t)len <= sizeof(_rx_buf))) {
            netdev_lora_rx_info_t info;
            _dev->driver->recv(_dev, _rx_buf, len, &info);
            _rx_len = len;
            _rssi = info.rssi;
            _snr = info.snr;
            DEBUG("[meshcore] received %d bytes, RSSI %d, SNR %d\n", len, info.rssi, info.snr);
        }
        else {
            /* drop the frame */
            _dev->driver->recv(_dev, NULL, len, NULL);
        }
        _receiving = false;
        _startRx();
        break;
    }

    case NETDEV_EVENT_CRC_ERROR:
    case NETDEV_EVENT_RX_TIMEOUT:
        _receiving = false;
        _startRx();
        break;

    case NETDEV_EVENT_TX_COMPLETE:
        _tx_done = true;
        break;

    default:
        break;
    }
}

void Radio::_startRx()
{
    netopt_state_t state = NETOPT_STATE_RX;

    _set(_dev, NETOPT_STATE, &state, sizeof(state));
}

int Radio::init(const meshcore_radio_params_t &params)
{
    _params = params;
    _pid = thread_getpid();
    _dev->event_callback = _event_cb;
    _dev->context = this;

    int res = _dev->driver->init(_dev);
    if (res < 0) {
        return res;
    }

    netopt_enable_t enable = NETOPT_ENABLE;
    netopt_enable_t disable = NETOPT_DISABLE;
    uint16_t rx_timeout = 0;

    if ((_set(_dev, NETOPT_CHANNEL_FREQUENCY, &params.frequency, sizeof(params.frequency)) < 0) ||
        (_set(_dev, NETOPT_BANDWIDTH, &params.bandwidth, sizeof(params.bandwidth)) < 0) ||
        (_set(_dev, NETOPT_SPREADING_FACTOR, &params.spreading_factor,
              sizeof(params.spreading_factor)) < 0) ||
        (_set(_dev, NETOPT_CODING_RATE, &params.coding_rate, sizeof(params.coding_rate)) < 0) ||
        (_set(_dev, NETOPT_TX_POWER, &params.tx_power, sizeof(params.tx_power)) < 0) ||
        (_set(_dev, NETOPT_SYNCWORD, &params.sync_word, sizeof(params.sync_word)) < 0) ||
        (_set(_dev, NETOPT_PREAMBLE_LENGTH, &params.preamble_len,
              sizeof(params.preamble_len)) < 0) ||
        (_set(_dev, NETOPT_INTEGRITY_CHECK, &enable, sizeof(enable)) < 0) ||
        (_set(_dev, NETOPT_IQ_INVERT, &disable, sizeof(disable)) < 0) ||
        (_set(_dev, NETOPT_FIXED_HEADER, &disable, sizeof(disable)) < 0) ||
        (_set(_dev, NETOPT_RX_SYMBOL_TIMEOUT, &rx_timeout, sizeof(rx_timeout)) < 0)) {
        return -EINVAL;
    }

    /* Not supported by all the drivers: the reception is restarted after
     * each frame */
    _dev->driver->set(_dev, NETOPT_SINGLE_RECEIVE, &disable, sizeof(disable));

    _startRx();

    return 0;
}

void Radio::handleIsr()
{
    _dev->driver->isr(_dev);
}

int Radio::readNoise(uint8_t *dest, size_t len)
{
    int res = 0;

    while (len > 0) {
        uint32_t value;
        res = _dev->driver->get(_dev, NETOPT_RANDOM, &value, sizeof(value));
        if (res < 0) {
            break;
        }
        size_t n = (len < sizeof(value)) ? len : sizeof(value);
        memcpy(dest, &value, n);
        dest += n;
        len -= n;
        res = 0;
    }

    _receiving = false;
    _startRx();

    return res;
}

int Radio::recvRaw(uint8_t *bytes, int sz)
{
    if (_rx_len == 0) {
        return 0;
    }

    int len = (_rx_len < sz) ? _rx_len : sz;
    memcpy(bytes, _rx_buf, len);
    _rx_len = 0;

    return len;
}

uint32_t Radio::getEstAirtimeFor(int len_bytes)
{
    /* MeshCore sends the packets with the LoRa header and the CRC */
    const meshcore_radio_params_t &p = _params;
    uint32_t airtime_us = lora_time_on_air_us(len_bytes, p.spreading_factor, p.bandwidth,
                                              p.coding_rate, p.preamble_len, false, true,
                                              lora_ldro_required(p.spreading_factor,
                                                                 p.bandwidth));

    return (airtime_us + US_PER_MS - 1) / US_PER_MS;
}

float Radio::packetScore(float snr, int packet_len)
{
    /* Same score as MeshCore with RadioLib: approximate minimum SNR of the
     * reception for each spreading factor, from SF7 to SF12 */
    static const float snr_threshold[] = { -7.5, -10, -12.5, -15, -17.5, -20 };
    uint8_t sf = _params.spreading_factor;

    if ((sf < LORA_SF7) || (sf > LORA_SF12) || (snr < snr_threshold[sf - LORA_SF7])) {
        return 0.0f;
    }

    float success_rate = (snr - snr_threshold[sf - LORA_SF7]) / 10.0f;
    float collision_penalty = 1.0f - (packet_len / 256.0f);
    float score = success_rate * collision_penalty;

    if (score < 0.0f) {
        return 0.0f;
    }
    if (score > 1.0f) {
        return 1.0f;
    }
    return score;
}

bool Radio::startSendRaw(const uint8_t *bytes, int len)
{
    if (_tx_busy) {
        return false;
    }

    iolist_t iol;
    iol.iol_next = NULL;
    iol.iol_base = (void *)bytes;
    iol.iol_len = len;

    _tx_busy = true;
    _tx_done = false;
    _receiving = false;

    if (_dev->driver->send(_dev, &iol) < 0) {
        DEBUG("[meshcore] can't send %d bytes\n", len);
        _tx_busy = false;
        _startRx();
        return false;
    }

    return true;
}

bool Radio::isSendComplete()
{
    return _tx_done;
}

void Radio::onSendFinished()
{
    _tx_busy = false;
    _tx_done = false;
    _startRx();
}

bool Radio::isInRecvMode() const
{
    return !_tx_busy;
}

bool Radio::isReceiving()
{
    /* The drivers don't report all the failed receptions, as the header
     * errors: a reception lasts at most the time on air of the longest
     * frame */
    if (_receiving &&
        ((ztimer_now(ZTIMER_MSEC) - _rx_start) > getEstAirtimeFor(MAX_TRANS_UNIT))) {
        _receiving = false;
    }

    return _receiving;
}

float Radio::getLastRSSI() const
{
    return _rssi;
}

float Radio::getLastSNR() const
{
    return _snr;
}

unsigned long MillisClock::getMillis()
{
    return ztimer_now(ZTIMER_MSEC);
}

uint32_t RTCClock::getCurrentTime()
{
    return _base + (ztimer_now(ZTIMER_SEC) - _start);
}

void RTCClock::setCurrentTime(uint32_t time)
{
    _base = time;
    _start = ztimer_now(ZTIMER_SEC);
}

void RNG::random(uint8_t *dest, size_t sz)
{
#if IS_USED(MODULE_PERIPH_HWRNG)
    hwrng_read(dest, sz);
#else
    random_bytes(dest, sz);
#endif
}

void RadioRNG::random(uint8_t *dest, size_t sz)
{
    meshcore_riot::RNG rng;

    rng.random(dest, sz);

    while (sz > 0) {
        uint8_t noise[SEED_SIZE];
        size_t n = (sz < sizeof(noise)) ? sz : sizeof(noise);
        if (_radio.readNoise(noise, n) < 0) {
            LOG_WARNING("[meshcore] the radio can't generate random numbers\n");
            return;
        }
        for (size_t i = 0; i < n; i++) {
            dest[i] ^= noise[i];
        }
        dest += n;
        sz -= n;
    }
}

int Runner::init(const meshcore_radio_params_t &params)
{
    mutex_lock(&_lock);
    int res = _radio.init(params);
    mutex_unlock(&_lock);

    return res;
}

void Runner::run()
{
    mutex_lock(&_lock);
    _mesh.begin();

    while (1) {
        mutex_unlock(&_lock);

        ztimer_t timeout = {};
        ztimer_set_timeout_flag(ZTIMER_MSEC, &timeout, CONFIG_MESHCORE_LOOP_PERIOD_MS);
        thread_flags_t flags = thread_flags_wait_any(FLAG_RADIO_ISR | THREAD_FLAG_TIMEOUT);
        ztimer_remove(ZTIMER_MSEC, &timeout);

        mutex_lock(&_lock);
        if (flags & FLAG_RADIO_ISR) {
            _radio.handleIsr();
        }
        _mesh.loop();
    }
}

} /* namespace meshcore_riot */
