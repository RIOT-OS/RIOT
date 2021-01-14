/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_nrf52_802154
 * @{
 *
 * @file
 * @brief       Implementation of the IEEE 802.15.4 for nRF52 radios
 *
 * @author      José I. Alamos <jose.alamos@haw-hamburg.de>
 * @}
 */

#include <assert.h>
#include <string.h>
#include <errno.h>

#include "cpu.h"
#include "luid.h"

#include "net/ieee802154.h"
#include "periph/timer.h"
#include "nrf802154.h"
#include "net/ieee802154/radio.h"

#define ENABLE_DEBUG        0
#include "debug.h"

#define ED_RSSISCALE        (4U)    /**< RSSI scale for internal HW value */
#define ED_RSSIOFFS         (-92)   /**< RSSI offset for internal HW value */

/* Set timer period to 16 us (IEEE 802.15.4 symbol time) */
#define TIMER_FREQ          (62500UL)

#define TX_POWER_MIN        (-40)                               /* in dBm */
#define TX_POWER_MAX        ((int)RADIO_TXPOWER_TXPOWER_Max)    /* in dBm */

/**
 * @brief Default nrf802154 radio shortcuts
 *
 * With this configuration the radio goes on the RXREADY event to RXSTART
 * and on TXREADY to TXSTART, without requiring to trigger the task manually.
 */
#define DEFAULT_SHORTS      (RADIO_SHORTS_RXREADY_START_Msk | \
                             RADIO_SHORTS_TXREADY_START_Msk);

/**
 * @brief nrf52840 shortcuts for CCA on send
 *
 * With this configuration the radio automatically triggers a CCA request on
 * RXREADY event. If the CCA succeeds, the radio will automatically send the
 * frame. Otherwise it will simply go to DISABLE.
 */
#define CCA_SHORTS          (RADIO_SHORTS_RXREADY_CCASTART_Msk | \
                             RADIO_SHORTS_CCAIDLE_STOP_Msk | \
                             RADIO_SHORTS_CCAIDLE_TXEN_Msk | \
                             RADIO_SHORTS_CCABUSY_DISABLE_Msk | \
                             RADIO_SHORTS_TXREADY_START_Msk);

#define MAC_TIMER_CHAN_ACK  (0U)    /**< MAC timer channel for transmitting an ACK frame */
#define MAC_TIMER_CHAN_IFS  (1U)    /**< MAC timer channel for handling IFS logic */

static uint8_t rxbuf[IEEE802154_FRAME_LEN_MAX + 3]; /* len PHR + PSDU + LQI */
static uint8_t txbuf[IEEE802154_FRAME_LEN_MAX + 3]; /* len PHR + PSDU + LQI */
static uint8_t ack[IEEE802154_ACK_FRAME_LEN];

typedef enum {
    STATE_IDLE,
    STATE_TX,
    STATE_ACK,
    STATE_RX,
    STATE_CCA_CLEAR,
    STATE_CCA_BUSY,
} nrf802154_state_t;

static volatile uint8_t _state;

static uint8_t nrf802154_short_addr[IEEE802154_SHORT_ADDRESS_LEN];
static uint8_t nrf802154_long_addr[IEEE802154_LONG_ADDRESS_LEN];
static uint16_t nrf802154_pan_id;

static struct {
    bool ifs        : 1;    /**< if true, the device is currently inside the IFS period */
    bool cca_send   : 1;    /**< whether the next transmission uses CCA or not */
    bool ack_filter : 1;    /**< whether the ACK filter is activated or not */
    bool promisc    : 1;    /**< whether the device is in promiscuous mode or not */
} cfg = {
    .cca_send   = true,
    .ack_filter = true,
};

static const ieee802154_radio_ops_t nrf802154_ops;

ieee802154_dev_t nrf802154_hal_dev = {
    .driver = &nrf802154_ops,
};

static bool _l2filter(uint8_t *mhr)
{
    uint8_t dst_addr[IEEE802154_LONG_ADDRESS_LEN];
    le_uint16_t dst_pan;
    uint8_t pan_bcast[] = IEEE802154_PANID_BCAST;

    int addr_len = ieee802154_get_dst(mhr, dst_addr, &dst_pan);

    /* filter PAN ID */
    /* Will only work on little endian platform (all?) */

    if ((memcmp(pan_bcast, dst_pan.u8, 2) != 0) &&
        (memcmp(&nrf802154_pan_id, dst_pan.u8, 2) != 0)) {
        return false;
    }

    /* check destination address */
    if (((addr_len == IEEE802154_SHORT_ADDRESS_LEN) &&
          (memcmp(nrf802154_short_addr, dst_addr, addr_len) == 0 ||
           memcmp(ieee802154_addr_bcast, dst_addr, addr_len) == 0)) ||
        ((addr_len == IEEE802154_LONG_ADDRESS_LEN) &&
          (memcmp(nrf802154_long_addr, dst_addr, addr_len) == 0))) {
        return true;
    }

    return false;
}

static int _write(ieee802154_dev_t *dev, const iolist_t *iolist)
{
    (void)dev;

    DEBUG("[nrf802154] Send a packet\n");

    assert(iolist);

    /* copy packet data into the transmit buffer */
    unsigned int len = 0;

    /* Load packet data into FIFO. Size checks are handled by higher
     * layers */
    for (; iolist; iolist = iolist->iol_next) {
        /* Check if there is data to copy, prevents undefined behaviour with
         * memcpy when iolist->iol_base == NULL */
        if (iolist->iol_len) {
            memcpy(&txbuf[len + 1], iolist->iol_base, iolist->iol_len);
            len += iolist->iol_len;
        }
    }

    DEBUG("[nrf802154] send: putting %i bytes into the frame buffer\n", len);

    /* specify the length of the package. */
    txbuf[0] = len + IEEE802154_FCS_LEN;

    return len;
}

static int _confirm_transmit(ieee802154_dev_t *dev, ieee802154_tx_info_t *info)
{
    (void) dev;

    if (_state != STATE_IDLE
        && _state != STATE_CCA_BUSY && NRF_RADIO->STATE != RADIO_STATE_STATE_Disabled) {
        return -EAGAIN;
    }

    if (info) {
        info->status = (_state == STATE_CCA_BUSY) ? TX_STATUS_MEDIUM_BUSY : TX_STATUS_SUCCESS;
    }

    _state = STATE_IDLE;
    NRF_RADIO->SHORTS = DEFAULT_SHORTS;
    DEBUG("[nrf802154] TX Finished\n");

    return 0;
}

static int _request_transmit(ieee802154_dev_t *dev)
{
    (void) dev;
    if (cfg.ifs) {
        return -EBUSY;
    }

    _state = STATE_TX;
    if (cfg.cca_send) {
        DEBUG("[nrf802154] Transmit a frame using CCA\n");
        NRF_RADIO->SHORTS = CCA_SHORTS;
        NRF_RADIO->TASKS_RXEN = 1;
    }
    else {
        DEBUG("[nrf802154] Transmit a frame using Direct Transmission\n");
        NRF_RADIO->TASKS_TXEN = 1;
    }

    return 0;
}

/**
 * @brief   Convert from the internal representation to dBm, when the
 *          radio operates as a IEEE802.15.4 transceiver.
 */
static inline int8_t _hwval_to_ieee802154_dbm(uint8_t hwval)
{
    return (ED_RSSISCALE * hwval) + ED_RSSIOFFS;
}

static int _read(ieee802154_dev_t *dev, void *buf, size_t max_size,
                          ieee802154_rx_info_t *info)
{
    (void) dev;
    size_t pktlen = (size_t)rxbuf[0] - IEEE802154_FCS_LEN;
    int res = -ENOBUFS;

    if (max_size < pktlen) {
        DEBUG("[nrf802154] recv: buffer is to small\n");
        return res;
    }
    else {
        DEBUG("[nrf802154] recv: reading packet of length %i\n", pktlen);
        if (info != NULL) {
            ieee802154_rx_info_t *radio_info = info;
            /* Hardware link quality indicator */
            uint8_t hwlqi = rxbuf[pktlen + 1];
            /* Convert to 802.15.4 LQI (page 319 of product spec v1.1) */
            radio_info->lqi = (uint8_t)(hwlqi > UINT8_MAX/ED_RSSISCALE
                                       ? UINT8_MAX
                                       : hwlqi * ED_RSSISCALE);
            /* We calculate RSSI from LQI, since it's already 8-bit
               saturated (see page 321 of product spec v1.1) */
            radio_info->rssi = _hwval_to_ieee802154_dbm(radio_info->lqi)
                               + IEEE802154_RADIO_RSSI_OFFSET;
        }
        memcpy(buf, &rxbuf[1], pktlen);
    }

    return pktlen;
}

static int _confirm_cca(ieee802154_dev_t *dev)
{
    (void) dev;
    int res;

    switch (_state) {
    case STATE_CCA_CLEAR:
        DEBUG("[nrf802154] Channel is clear\n");
        res = true;
        break;
    case STATE_CCA_BUSY:
        DEBUG("[nrf802154] Channel is busy\n");
        res = false;
        break;
    default:
        res = -EAGAIN;
    }

    _state = STATE_RX;
    return res;
}

static int _request_cca(ieee802154_dev_t *dev)
{
    (void) dev;

    if (_state != STATE_RX) {
        DEBUG("[nrf802154] CCA request fail: EBUSY\n");
        return -EBUSY;
    }

    DEBUG("[nrf802154] CCA Requested\n");
    /* Go back to RxIdle state and start CCA */
    NRF_RADIO->TASKS_STOP = 1;
    NRF_RADIO->TASKS_CCASTART = 1;
    return 0;
}

/**
 * @brief   Set CCA threshold value in internal represetion
 */
static void _set_cca_thresh(uint8_t thresh)
{
    NRF_RADIO->CCACTRL &= ~RADIO_CCACTRL_CCAEDTHRES_Msk;
    NRF_RADIO->CCACTRL |= thresh << RADIO_CCACTRL_CCAEDTHRES_Pos;
}

/**
 * @brief   Convert from dBm to the internal representation, when the
 *          radio operates as a IEEE802.15.4 transceiver.
 */
static inline uint8_t _dbm_to_ieee802154_hwval(int8_t dbm)
{
    return ((dbm - ED_RSSIOFFS) / ED_RSSISCALE);
}

static int set_cca_threshold(ieee802154_dev_t *dev, int8_t threshold)
{
    (void) dev;
    _set_cca_thresh(_dbm_to_ieee802154_hwval(threshold));
    return 0;
}

static void _set_txpower(int16_t txpower)
{
    DEBUG("[nrf802154]: Setting TX power to %i\n", txpower);
    if (txpower > (int)RADIO_TXPOWER_TXPOWER_Max) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Max;
    }
    else if (txpower > 1) {
        NRF_RADIO->TXPOWER = (uint32_t)txpower;
    }
    else if (txpower > -1) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_0dBm;
    }
    else if (txpower > -5) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Neg4dBm;
    }
    else if (txpower > -9) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Neg8dBm;
    }
    else if (txpower > -13) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Neg12dBm;
    }
    else if (txpower > -17) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Neg16dBm;
    }
    else if (txpower > -21) {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Neg20dBm;
    }
    else {
        NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Neg40dBm;
    }
}

static void _disable(void)
{
    /* set device into DISABLED state */
    if (NRF_RADIO->STATE != RADIO_STATE_STATE_Disabled) {
        NRF_RADIO->EVENTS_DISABLED = 0;
        NRF_RADIO->TASKS_DISABLE = 1;
        DEBUG("[nrf802154] Device state: DISABLED\n");
    }
}

static void _set_ifs_timer(bool lifs)
{
    uint8_t timeout;
    cfg.ifs = true;
    if (lifs) {
        timeout = IEEE802154_LIFS_SYMS;
    }
    else {
        timeout = IEEE802154_SIFS_SYMS;
    }

    timer_set(NRF802154_TIMER, MAC_TIMER_CHAN_IFS, timeout);
    timer_start(NRF802154_TIMER);
}

static int _confirm_set_trx_state(ieee802154_dev_t *dev)
{
    (void) dev;
    int radio_state = NRF_RADIO->STATE;
    if (radio_state == RADIO_STATE_STATE_TxRu || radio_state == RADIO_STATE_STATE_RxRu ||
        radio_state == RADIO_STATE_STATE_TxDisable || radio_state == RADIO_STATE_STATE_RxDisable) {
        return -EAGAIN;
    }
    DEBUG("[nrf802154]: State transition finished\n");
    return 0;
}

static int _request_set_trx_state(ieee802154_dev_t *dev, ieee802154_trx_state_t state)
{
    (void) dev;

    if (_state != STATE_IDLE && _state != STATE_RX) {
        DEBUG("[nrf802154]: set_trx_state failed: -EBUSY\n");
        return -EBUSY;
    }

    _disable();

    /* This will take in worst case 21 us */
    while (NRF_RADIO->STATE != RADIO_STATE_STATE_Disabled) {};

    switch (state) {
    case IEEE802154_TRX_STATE_TRX_OFF:
        _state = STATE_IDLE;
        DEBUG("[nrf802154]: Request state to TRX_OFF\n");
        break;
    case IEEE802154_TRX_STATE_RX_ON:
        NRF_RADIO->PACKETPTR = (uint32_t)rxbuf;
        NRF_RADIO->TASKS_RXEN = 1;
        _state = STATE_RX;
        DEBUG("[nrf802154]: Request state to RX_ON\n");
        break;
    case IEEE802154_TRX_STATE_TX_ON:
        NRF_RADIO->PACKETPTR = (uint32_t)txbuf;
        _state = STATE_IDLE;
        DEBUG("[nrf802154]: Request state to TX_ON\n");
        break;
    }

    return 0;
}

static void _timer_cb(void *arg, int chan)
{
    (void)arg;
    ieee802154_dev_t *dev = &nrf802154_hal_dev;

    if (chan == MAC_TIMER_CHAN_ACK) {
        /* Copy sqn */
        ack[3] = rxbuf[3];

        NRF_RADIO->PACKETPTR = (uint32_t) &ack;
        NRF_RADIO->TASKS_TXEN = 1;
        dev->cb(dev, IEEE802154_RADIO_INDICATION_RX_DONE);
    }
    else if (chan == MAC_TIMER_CHAN_IFS) {
        cfg.ifs = false;
    }

    timer_stop(NRF802154_TIMER);
}
/**
 * @brief   Set radio into DISABLED state
 */
int nrf802154_init(void)
{
    DEBUG("[nrf802154]: Init\n");
    /* reset buffer */
    rxbuf[0] = 0;
    txbuf[0] = 0;

    ack[0] = IEEE802154_ACK_FRAME_LEN; /* PSDU length */
    ack[1] = IEEE802154_FCF_TYPE_ACK; /* FCF */
    ack[2] = 0; /* FCF */

    int result = timer_init(NRF802154_TIMER, TIMER_FREQ, _timer_cb, NULL);
    assert(result >= 0);
    (void)result;
    timer_stop(NRF802154_TIMER);

    /* power off peripheral */
    NRF_RADIO->POWER = 0;

    return 0;
}

void isr_radio(void)
{
    ieee802154_dev_t *dev = &nrf802154_hal_dev;

    if (NRF_RADIO->EVENTS_FRAMESTART) {
        NRF_RADIO->EVENTS_FRAMESTART = 0;
        if (_state == STATE_TX) {
            dev->cb(dev, IEEE802154_RADIO_INDICATION_TX_START);
        }
        else if (_state == STATE_RX) {
            dev->cb(dev, IEEE802154_RADIO_INDICATION_RX_START);
        }
    }

    if (NRF_RADIO->EVENTS_CCAIDLE) {
        NRF_RADIO->EVENTS_CCAIDLE = 0;
        if (_state != STATE_TX) {
            _state = STATE_CCA_CLEAR;
            dev->cb(dev, IEEE802154_RADIO_CONFIRM_CCA);
        }
    }

    if (NRF_RADIO->EVENTS_CCABUSY) {
        NRF_RADIO->EVENTS_CCABUSY = 0;
        if (_state == STATE_TX) {
            _state = STATE_CCA_BUSY;
            dev->cb(dev, IEEE802154_RADIO_CONFIRM_TX_DONE);
        }
        else {
            _state = STATE_CCA_BUSY;
            dev->cb(dev, IEEE802154_RADIO_CONFIRM_CCA);
        }
    }

    if (NRF_RADIO->EVENTS_END) {
        NRF_RADIO->EVENTS_END = 0;

        switch (_state) {
        case STATE_TX:
            DEBUG("[nrf802154] TX state: %x\n", (uint8_t)NRF_RADIO->STATE);

            _set_ifs_timer(txbuf[0] > IEEE802154_SIFS_MAX_FRAME_SIZE);
            _state = STATE_IDLE;
            dev->cb(dev, IEEE802154_RADIO_CONFIRM_TX_DONE);
            break;
        case STATE_RX:
            if (NRF_RADIO->CRCSTATUS) {
                bool l2filter_passed = _l2filter(rxbuf+1);
                bool is_auto_ack_en = ack[1];
                bool is_ack = rxbuf[1] & IEEE802154_FCF_TYPE_ACK;
                bool ack_req = rxbuf[1] & IEEE802154_FCF_ACK_REQ;

                /* If radio is in promiscuos mode, indicate packet and
                 * don't event think of sending an ACK frame :) */
                if (cfg.promisc) {
                    DEBUG("[nrf802154] Promiscuous mode is enabled.\n");
                    _state = STATE_IDLE;
                    dev->cb(dev, IEEE802154_RADIO_INDICATION_RX_DONE);
                }
                /* If the L2 filter passes, device if the frame is indicated
                 * directly or if the driver should send an ACK frame before
                 * the indication */
                else if (l2filter_passed) {
                    if (ack_req && is_auto_ack_en) {
                        timer_set(NRF802154_TIMER, MAC_TIMER_CHAN_ACK, IEEE802154_SIFS_SYMS);
                        timer_start(NRF802154_TIMER);
                        _disable();
                        _state = STATE_ACK;
                    }
                    else {
                        DEBUG("[nrf802154] RX frame doesn't require ACK frame.\n");
                        _state = STATE_IDLE;
                        dev->cb(dev, IEEE802154_RADIO_INDICATION_RX_DONE);
                    }
                }
                /* In case the packet is an ACK and the ACK filter is disabled,
                 * indicate the frame reception */
                else if (is_ack && !cfg.ack_filter) {
                    DEBUG("[nrf802154] Received ACK.\n");
                    _state = STATE_IDLE;
                    dev->cb(dev, IEEE802154_RADIO_INDICATION_RX_DONE);
                }
                /* If all failed, simply drop the frame and continue listening
                 * to incoming frames */
                else {
                    DEBUG("[nrf802154] Addr filter failed or ACK filter on.\n");
                    NRF_RADIO->TASKS_START = 1;
                }
            }
            else {
                DEBUG("[nrf802154] CRC fail.\n");
                dev->cb(dev, IEEE802154_RADIO_INDICATION_CRC_ERROR);
            }
            break;
        case STATE_ACK:
            _state = STATE_IDLE;

            /* We disable the radio to avoid unwanted emmissions (see ERRATA
             * ID 204, "Switching between TX and RX causes unwanted emissions")
             */
            _disable();
            DEBUG("[nrf52840] TX ACK done.")
            _set_ifs_timer(false);
            break;
        default:
            assert(false);
        }
    }

    cortexm_isr_end();
}

static int _confirm_on(ieee802154_dev_t *dev)
{
    (void) dev;
    return 0;
}

static int _request_on(ieee802154_dev_t *dev)
{
    (void) dev;
    _state = STATE_IDLE;
    DEBUG("[nrf802154]: Request to turn on\n");
    NRF_RADIO->POWER = 1;
    /* make sure the radio is disabled/stopped */
    _disable();
    /* we configure it to run in IEEE802.15.4 mode */
    NRF_RADIO->MODE = RADIO_MODE_MODE_Ieee802154_250Kbit;
    /* and set some fitting configuration */
    NRF_RADIO->PCNF0 = ((8 << RADIO_PCNF0_LFLEN_Pos) |
                        (RADIO_PCNF0_PLEN_32bitZero << RADIO_PCNF0_PLEN_Pos) |
                        (RADIO_PCNF0_CRCINC_Include << RADIO_PCNF0_CRCINC_Pos));
    NRF_RADIO->PCNF1 = IEEE802154_FRAME_LEN_MAX;
    /* set start frame delimiter */
    NRF_RADIO->SFD = IEEE802154_SFD;
    /* set MHR filters */
    NRF_RADIO->MHRMATCHCONF = 0;              /* Search Pattern Configuration */
    NRF_RADIO->MHRMATCHMAS = 0xff0007ff;      /* Pattern mask */
    /* configure CRC conform to IEEE802154 */
    NRF_RADIO->CRCCNF = ((RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos) |
                         (RADIO_CRCCNF_SKIPADDR_Ieee802154 << RADIO_CRCCNF_SKIPADDR_Pos));
    NRF_RADIO->CRCPOLY = 0x011021;
    NRF_RADIO->CRCINIT = 0;

    /* Disable the hardware IFS handling  */
    NRF_RADIO->MODECNF0 |= RADIO_MODECNF0_RU_Msk;

    NRF_RADIO->SHORTS = DEFAULT_SHORTS;

    /* enable interrupts */
    NVIC_EnableIRQ(RADIO_IRQn);
    NRF_RADIO->INTENSET = RADIO_INTENSET_END_Msk |
                          RADIO_INTENSET_FRAMESTART_Msk |
                          RADIO_INTENSET_CCAIDLE_Msk |
                          RADIO_INTENSET_CCABUSY_Msk;

    return 0;
}

static int _config_phy(ieee802154_dev_t *dev, const ieee802154_phy_conf_t *conf)
{
    (void) dev;
    int8_t pow = conf->pow;

    if (pow < TX_POWER_MIN || pow > TX_POWER_MAX) {
        return -EINVAL;
    }

    assert(NRF_RADIO->STATE == RADIO_STATE_STATE_Disabled);

    /* The value of this register represents the frequency offset (in MHz) from
     * 2400 MHz.  Channel 11 (first 2.4 GHz band channel) starts at 2405 MHz
     * and all channels have a bandwidth of 5 MHz. Thus, we subtract 10 to the
     * channel number and multiply by 5 to calculate the offset.
     */
    NRF_RADIO->FREQUENCY = (((uint8_t) conf->channel) - 10) * 5;

    DEBUG("[nrf802154] setting channel to %i\n", conf->channel);
    DEBUG("[nrf802154] setting TX power to %i\n", conf->pow);
    _set_txpower(pow);

    return 0;
}

static int _off(ieee802154_dev_t *dev)
{
    (void) dev;
    DEBUG("[nrf802154] Turning off the radio\n");
    NRF_RADIO->POWER = 0;
    return 0;
}

static bool _get_cap(ieee802154_dev_t *dev, ieee802154_rf_caps_t cap)
{
    (void) dev;
    switch (cap) {
    case IEEE802154_CAP_24_GHZ:
    case IEEE802154_CAP_IRQ_CRC_ERROR:
    case IEEE802154_CAP_IRQ_RX_START:
    case IEEE802154_CAP_IRQ_TX_START:
    case IEEE802154_CAP_IRQ_TX_DONE:
    case IEEE802154_CAP_IRQ_CCA_DONE:
        return true;
    default:
        return false;
    }
}

int _len(ieee802154_dev_t *dev)
{
    (void) dev;
    DEBUG("[nrf802154] Length of frame is %i\n", (size_t)rxbuf[0] - IEEE802154_FCS_LEN);
    return (size_t)rxbuf[0] - IEEE802154_FCS_LEN;
}

int _set_cca_mode(ieee802154_dev_t *dev, ieee802154_cca_mode_t mode)
{
    (void) dev;

    NRF_RADIO->CCACTRL &= RADIO_CCACTRL_CCAMODE_Msk;
    uint8_t tmp = 0;

    switch (mode) {
    case IEEE802154_CCA_MODE_ED_THRESHOLD:
        DEBUG("[nrf802154]: set CCA Mode to ED Threshold\n");
        tmp = RADIO_CCACTRL_CCAMODE_EdMode;
        break;
    case IEEE802154_CCA_MODE_CARRIER_SENSING:
        tmp = RADIO_CCACTRL_CCAMODE_CarrierOrEdMode;
        DEBUG("[nrf802154]: set CCA Mode to Carrier Sensing\n");
        break;
    case IEEE802154_CCA_MODE_ED_THRESH_AND_CS:
        DEBUG("[nrf802154]: set CCA Mode to ED Threshold AND Carrier Sensing\n");
        tmp = RADIO_CCACTRL_CCAMODE_CarrierAndEdMode;
        break;
    case IEEE802154_CCA_MODE_ED_THRESH_OR_CS:
        DEBUG("[nrf802154]: set CCA Mode to ED Threshold OR Carrier Sensing\n");
        tmp = RADIO_CCACTRL_CCAMODE_CarrierOrEdMode;
        break;
    }

    NRF_RADIO->CCACTRL |= tmp;

    return 0;
}

static int _set_hw_addr_filter(ieee802154_dev_t *dev, const network_uint16_t *short_addr,
                              const eui64_t *ext_addr, const uint16_t *pan_id)
{
    (void) dev;
    if (short_addr) {
        memcpy(nrf802154_short_addr, short_addr, IEEE802154_SHORT_ADDRESS_LEN);
    }

    if (ext_addr) {
        memcpy(nrf802154_long_addr, ext_addr, IEEE802154_LONG_ADDRESS_LEN);
    }

    if (pan_id) {
        nrf802154_pan_id = *pan_id;
    }

    return 0;
}

static int _set_rx_mode(ieee802154_dev_t *dev, ieee802154_rx_mode_t mode)
{
    (void) dev;

    bool ackf = true;
    bool _promisc = false;

    switch (mode) {
    case IEEE802154_RX_AACK_DISABLED:
        ack[1] = 0;
        break;
    case IEEE802154_RX_AACK_ENABLED:
        ack[1] = IEEE802154_FCF_TYPE_ACK;
        break;
    case IEEE802154_RX_AACK_FRAME_PENDING:
        ack[1] = IEEE802154_FCF_TYPE_ACK | IEEE802154_FCF_FRAME_PEND;
        break;
    case IEEE802154_RX_PROMISC:
        _promisc = true;
        break;
    case IEEE802154_RX_WAIT_FOR_ACK:
        ackf = false;
        break;
    }

    cfg.ack_filter = ackf;
    cfg.promisc = _promisc;

    return 0;
}

static int _set_csma_params(ieee802154_dev_t *dev, const ieee802154_csma_be_t *bd,
                            int8_t retries)
{
    (void) dev;
    (void) bd;

    if (retries > 0) {
        return -ENOTSUP;
    }

    cfg.cca_send = (retries == 0);

    return 0;
}

void nrf802154_setup(nrf802154_t *dev)
{
    (void) dev;
#if IS_USED(MODULE_NETDEV_IEEE802154_SUBMAC)
    netdev_t *netdev = (netdev_t*) dev;
    netdev_register(netdev, NETDEV_NRF802154, 0);
    DEBUG("[nrf802154] init submac.\n")
    netdev_ieee802154_submac_init(&dev->netdev, &nrf802154_hal_dev);
#endif
    nrf802154_init();
}

static const ieee802154_radio_ops_t nrf802154_ops = {
    .write = _write,
    .read = _read,
    .request_transmit = _request_transmit,
    .confirm_transmit = _confirm_transmit,
    .len = _len,
    .off = _off,
    .request_on = _request_on,
    .confirm_on = _confirm_on,
    .request_set_trx_state = _request_set_trx_state,
    .confirm_set_trx_state = _confirm_set_trx_state,
    .request_cca = _request_cca,
    .confirm_cca = _confirm_cca,
    .get_cap = _get_cap,
    .set_cca_threshold = set_cca_threshold,
    .set_cca_mode = _set_cca_mode,
    .config_phy = _config_phy,
    .set_hw_addr_filter = _set_hw_addr_filter,
    .set_csma_params = _set_csma_params,
    .set_rx_mode = _set_rx_mode,
};
