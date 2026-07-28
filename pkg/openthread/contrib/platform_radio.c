/*
 * Copyright (C) 2017 Fundacion Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     net
 * @file
 * @brief       Implementation of OpenThread radio platform abstraction
 *
 * @author      Jose Ignacio Alamos <jialamos@uc.cl>
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "atomic_utils.h"
#include "byteorder.h"
#include "errno.h"
#include "luid.h"
#include "net/ethernet/hdr.h"
#include "net/ethertype.h"
#include "net/eui_provider.h"
#include "net/l2util.h"
#include "openthread/platform/diag.h"
#include "openthread/platform/radio.h"
#include "ot.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define RADIO_IEEE802154_FCS_LEN    (2U)

typedef struct openthread_device {
    ieee802154_dev_t *dev;
    ieee802154_phy_conf_t phy_conf;
    /* locally set, not factory set */
    eui64_t factory_eui;
    eui64_t _ext_addr;
    network_uint16_t _short_addr;
    int8_t cca_threshold;
    int8_t rssi;
} openthread_device_t;

static openthread_device_t _ot_dev;
static otRadioFrame sTransmitFrame;
static otRadioFrame sReceiveFrame;

static uint8_t *_tx_is_ack;

/* send ack frame blocking */
static bool _send_ack(uint8_t seq_num)
{
    uint8_t ack[] = { IEEE802154_FCF_TYPE_ACK, 0x00,  seq_num };
    iolist_t iolist = {
        .iol_base = ack,
        .iol_len = sizeof(ack),
        .iol_next = NULL
    };

    while (ieee802154_radio_set_idle(_ot_dev.dev, false) != 0) {}
    /* send packet though radio hal */
    int res = ieee802154_radio_write(_ot_dev.dev, &iolist);
    if (res != 0) {
        printf("COULD NOT WRITE FRAMEBUFFER CORRECTLY: %d\n", res);
        return false;
    }

    /* wait SIFS period */
    ztimer_sleep(ZTIMER_USEC, 192);

    while (ieee802154_radio_request_transmit(_ot_dev.dev) == -EBUSY) {}

    /* wait until tx_done event is handled */
    atomic_store_u8(_tx_is_ack, 1);
    while (atomic_load_u8(_tx_is_ack) == 1) {}

    while (ieee802154_radio_set_idle(_ot_dev.dev, false) != 0) {}

    return true;
}

/* set 15.4 channel */
static int _set_channel(uint16_t channel)
{
    _ot_dev.phy_conf.channel = channel;
    return ieee802154_radio_config_phy(_ot_dev.dev, &_ot_dev.phy_conf);
}

/* set transmission power */
static int _set_power(int16_t power)
{
    _ot_dev.phy_conf.pow = power;
    return ieee802154_radio_config_phy(_ot_dev.dev, &_ot_dev.phy_conf);
}

/* set IEEE802.15.4 PAN ID */
static int _set_panid(uint16_t panid)
{
    return _ot_dev.dev->driver->config_addr_filter(_ot_dev.dev, IEEE802154_AF_PANID, &panid);
}

/* set extended HW address */
static int _set_ext_addr(uint8_t *ext_addr)
{
    memcpy(&_ot_dev._ext_addr, ext_addr, IEEE802154_AF_EXT_ADDR);
    return _ot_dev.dev->driver->config_addr_filter(_ot_dev.dev, IEEE802154_AF_EXT_ADDR, ext_addr);
}

/* set short address */
static int _set_short_addr(network_uint16_t short_addr)
{
    memcpy(&_ot_dev._short_addr, &short_addr, IEEE802154_SHORT_ADDRESS_LEN);
    return _ot_dev.dev->driver->config_addr_filter(_ot_dev.dev, IEEE802154_AF_SHORT_ADDR, &short_addr);
}

/* set the state of promiscuous mode */
static int _set_promiscuous(bool enable)
{
    ieee802154_filter_mode_t filter_mode =
        enable ? IEEE802154_FILTER_PROMISC : IEEE802154_FILTER_ACCEPT;

    return ieee802154_radio_set_frame_filter_mode(_ot_dev.dev, filter_mode);
}

static int _set_cca_threshold(int8_t cca_threshhold) {
    _ot_dev.cca_threshold = cca_threshhold;
    return ieee802154_radio_set_cca_threshold(_ot_dev.dev, cca_threshhold);
}

/* init framebuffers and initial state */
int openthread_radio_init(ieee802154_dev_t *dev, uint8_t *tb, uint8_t *rb, uint8_t *tx_is_ack)
{
    int res = 0;

    _ot_dev.dev = dev;

    sTransmitFrame.mPsdu = tb;
    sTransmitFrame.mLength = 0;
    sReceiveFrame.mPsdu = rb;
    sReceiveFrame.mLength = 0;

    _tx_is_ack = tx_is_ack;

    if ((res = ieee802154_radio_request_on(_ot_dev.dev)) < 0) {
        return res;
    }

    /* get supported PHY modes */
    // uint32_t supported_phy_modes = ieee802154_radio_get_phy_modes(_dev);
    // assert(supported_phy_modes != 0);
    // uint32_t default_phy_cap = ieee802154_phy_mode_to_cap(CONFIG_IEEE802154_DEFAULT_PHY_MODE);

    /* set phy conf (TODO add more flexibility))*/
    _ot_dev.phy_conf.phy_mode = CONFIG_IEEE802154_DEFAULT_PHY_MODE;
    _ot_dev.phy_conf.channel = CONFIG_IEEE802154_DEFAULT_CHANNEL;
    _ot_dev.phy_conf.page = 0;
    _ot_dev.phy_conf.pow = CONFIG_IEEE802154_DEFAULT_TXPOWER;

    while (ieee802154_radio_confirm_on(dev) == -EAGAIN) {}

    if ((res = ieee802154_radio_config_phy(_ot_dev.dev, &_ot_dev.phy_conf)) < 0) {
        return res;
    }

    /* generate long and short address*/
    luid_base(&_ot_dev.factory_eui, IEEE802154_LONG_ADDRESS_LEN);
    eui64_set_local(&_ot_dev.factory_eui);
    eui64_clear_group(&_ot_dev.factory_eui);
    eui_short_from_eui64(&_ot_dev._ext_addr, &_ot_dev._short_addr);
    
    /* set address filter */
    _set_ext_addr((uint8_t*) &_ot_dev.factory_eui);
    _set_short_addr(_ot_dev._short_addr);
    _set_panid(CONFIG_IEEE802154_DEFAULT_PANID);

    /* set cca threashold to default */
    _set_cca_threshold(CONFIG_IEEE802154_CCA_THRESH_DEFAULT);

    assert(res >= 0);

    /* set radio to receive */
    while (ieee802154_radio_set_rx(_ot_dev.dev) < 0) {}

    return res;
}

/* Called upon IEEE802154_EVENT_RX_COMPLETE */
void recv_pkt(otInstance *aInstance)
{
    DEBUG("Openthread: Received pkt\n");

    while (ieee802154_radio_set_idle(_ot_dev.dev, false) < 0) {}

    ieee802154_rx_info_t rx_info;
    /* Read frame length from driver */
    int len = ieee802154_radio_len(_ot_dev.dev);

    /* very unlikely */
    if ((len < 0) || ((uint32_t)len > UINT16_MAX)) {
        /* flush data in frame buffer */
        ieee802154_radio_read(_ot_dev.dev, NULL, 0, NULL);
        DEBUG("Invalid len: %d\n", len);
        otPlatRadioReceiveDone(aInstance, NULL, OT_ERROR_ABORT);
        return;
    }

    /* fill OpenThread receive frame */
    /* Openthread needs a packet length with FCS included */
    sReceiveFrame.mLength = len + RADIO_IEEE802154_FCS_LEN;

    /* Read received frame */
    int res = ieee802154_radio_read(_ot_dev.dev, (char *)sReceiveFrame.mPsdu, len, &rx_info);

    /* software ack logic */
    if ((uint16_t)len > IEEE802154_ACK_FRAME_LEN) {
        /* check for hardware ack */
        if (!ieee802154_radio_has_capability(_ot_dev.dev, IEEE802154_CAP_AUTO_ACK)) {
            /* check for correct type and ack request */
            ieee802154_filter_mode_t mode;
            if ((sReceiveFrame.mPsdu[0] & IEEE802154_FCF_TYPE_MASK) == IEEE802154_FCF_TYPE_DATA &&
                (sReceiveFrame.mPsdu[0] & IEEE802154_FCF_ACK_REQ) &&
                (ieee802154_radio_get_frame_filter_mode(_ot_dev.dev, &mode) < 0 ||
                 mode == IEEE802154_FILTER_ACCEPT)) {
                /* send ack */
                if (!_send_ack(ieee802154_get_seq(sReceiveFrame.mPsdu))) {
                    DEBUG("IEEE802154 submac: Sending ACK failed\n");
                }
            }
        }
    }

    /* The Radio HAL uses the IEEE 802.15.4 definition for RSSI.
     * OpenThread expects dBm. Therefore we need a translation here */
    _ot_dev.rssi = ieee802154_rssi_to_dbm(rx_info.rssi);
    sReceiveFrame.mInfo.mRxInfo.mRssi = _ot_dev.rssi;
    sReceiveFrame.mInfo.mRxInfo.mLqi = rx_info.lqi;

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        DEBUG("Received message: len %d\n", (int)sReceiveFrame.mLength);
        for (int i = 0; i < sReceiveFrame.mLength; ++i) {
            DEBUG("%x ", sReceiveFrame.mPsdu[i]);
        }
        DEBUG("\n");
    }

    /* Tell OpenThread that receive has finished */
    otError error = OT_ERROR_NONE;
    if (res < 0) {
        error = OT_ERROR_ABORT;
        if (res == -ENOBUFS) {
            error = OT_ERROR_NO_BUFS;
        }
    }
    otPlatRadioReceiveDone(aInstance, res > 0 ? &sReceiveFrame : NULL, error);
}

/* Called upon IEEE802154_EVENT_TX_DONE, excluding ACKs */
void process_tx_done(otInstance *aInstance)
{
    ieee802154_tx_info_t tx_info;
    int res = ieee802154_radio_confirm_transmit(_ot_dev.dev, &tx_info);

    if (res == -EAGAIN) {
        return;
    }
    switch (tx_info.status) {
    case TX_STATUS_SUCCESS:
    case TX_STATUS_FRAME_PENDING:
        otPlatRadioTxDone(aInstance, &sTransmitFrame, NULL, OT_ERROR_NONE);
        break;
    case TX_STATUS_NO_ACK:
        otPlatRadioTxDone(aInstance, &sTransmitFrame, NULL, OT_ERROR_NO_ACK);
        break;
    case TX_STATUS_MEDIUM_BUSY:
        otPlatRadioTxDone(aInstance, &sTransmitFrame, NULL, OT_ERROR_CHANNEL_ACCESS_FAILURE);
        break;
    default:
        break;
    }
}

/* OpenThread will call this for getting the radio caps */
otRadioCaps otPlatRadioGetCaps(otInstance *aInstance)
{
    (void) aInstance;
    otRadioCaps caps = 0;
    printf("Caps flags %u\n",(unsigned) _ot_dev.dev->driver->caps);
    if (ieee802154_radio_has_capability(_ot_dev.dev, IEEE802154_CAP_IRQ_ACK_TIMEOUT)) {
        caps |= OT_RADIO_CAPS_ACK_TIMEOUT;
    }
    /* OT_RADIO_CAPS_ENERGY_SCAN only possible as MAC software feature in */
    if (ieee802154_radio_has_capability(_ot_dev.dev, IEEE802154_CAP_FRAME_RETRANS)) {
        caps |= OT_RADIO_CAPS_TRANSMIT_RETRIES;
    }
    if (ieee802154_radio_has_capability(_ot_dev.dev, IEEE802154_CAP_AUTO_CSMA)) {
        caps |= OT_RADIO_CAPS_CSMA_BACKOFF;
    }
    /* OT_RADIO_CAPS_SLEEP_TO_TX not possible, see precondition static int ieee802154_radio_request_transmit */
    /* OT_RADIO_CAPS_TRANSMIT_SEC experimental with IEEE 802.15.4 security module*/
    /* OT_RADIO_CAPS_TRANSMIT_TIMING + OT_RADIO_CAPS_RECEIVE_TIMING could be implemented */
    /* OT_RADIO_CAPS_RX_ON_WHEN_IDLE currently as software feature in OpenThread */
    /* OT_RADIO_CAPS_TRANSMIT_FRAME_POWER could be implemented */
    /* OT_RADIO_CAPS_ALT_SHORT_ADDR */

    DEBUG("openthread: otPlatRadioGetCaps %u\n", (uint16_t) caps);
    return caps;
}

int8_t otPlatRadioGetReceiveSensitivity(otInstance *aInstance)
{
    (void) aInstance;
    /* -100 is around the default range of most RIOT radios */
    return -100;
}

void otPlatRadioGetIeeeEui64(otInstance *aInstance, uint8_t *aIeee64Eui64)
{
    (void) aInstance;
    DEBUG("openthread: otPlatRadioGetIeeeEui64 requested factory eui64, but is locally set\n");

    /* currently returns generated, but static eui64 based on cpuid for most devices */
    memcpy(aIeee64Eui64, (uint8_t *) &_ot_dev.factory_eui, IEEE802154_LONG_ADDRESS_LEN);
}

/* OpenThread will call this for setting PAN ID */
void otPlatRadioSetPanId(otInstance *aInstance, uint16_t panid)
{
    (void)aInstance;
    DEBUG("openthread: otPlatRadioSetPanId: setting PAN ID to %04x\n", panid);
    _set_panid(panid);
}

/* OpenThread will call this for setting extended address */
void otPlatRadioSetExtendedAddress(otInstance *aInstance, const otExtAddress *aExtAddress)
{
    (void)aInstance;
    DEBUG("openthread: otPlatRadioSetExtendedAddress\n");
    
    /* OpenThread stores aExtAddress in little endian */
    char reversed_addr[IEEE802154_LONG_ADDRESS_LEN];
    for (unsigned i = 0; i < IEEE802154_LONG_ADDRESS_LEN; ++i) {
        reversed_addr[i] = aExtAddress->m8[IEEE802154_LONG_ADDRESS_LEN - i - 1];
    }
    if (IS_ACTIVE(ENABLE_DEBUG)) {
        for (unsigned i = 0; i < IEEE802154_LONG_ADDRESS_LEN; ++i) {
            DEBUG("%x ", (uint8_t)((uint8_t *)reversed_addr)[i]);
        }
        DEBUG("\n");
    }
    _set_ext_addr((uint8_t *)reversed_addr);
}

/* OpenThread will call this for setting short address */
void otPlatRadioSetShortAddress(otInstance *aInstance, uint16_t aShortAddress)
{
    (void)aInstance;
    DEBUG("openthread: otPlatRadioSetShortAddress: setting address to %04x\n", aShortAddress);
    /* change byte order from little-endian to big-endian */
    _set_short_addr(byteorder_htons(aShortAddress));
}

/* optional */
void otPlatRadioSetAlternateShortAddress(otInstance *aInstance, otShortAddress aShortAddress)
{
    DEBUG("openthread: otPlatRadioSetAlternateShortAddress is not implemented\n");
    (void)aInstance;
    (void)aShortAddress;
}

otError otPlatRadioGetTransmitPower(otInstance *aInstance, int8_t *aPower)
{
    (void)aInstance;
    if (aPower == NULL) {
        return OT_ERROR_INVALID_ARGS;
    }

    *aPower = _ot_dev.phy_conf.pow;

    return OT_ERROR_NONE;
}

otError otPlatRadioSetTransmitPower(otInstance *aInstance, int8_t aPower)
{
    (void)aInstance;
    _set_power(aPower);

    return OT_ERROR_NONE;
}

otError otPlatRadioGetCcaEnergyDetectThreshold(otInstance *aInstance, int8_t *aThreshold)
{
    (void) aInstance;
    *aThreshold = _ot_dev.cca_threshold;

    return OT_ERROR_NONE;
}

otError otPlatRadioSetCcaEnergyDetectThreshold(otInstance *aInstance, int8_t aThreshold)
{
    (void) aInstance;
    _set_cca_threshold(aThreshold);

    return OT_ERROR_NONE;
}

otError otPlatRadioGetFemLnaGain(otInstance *aInstance, int8_t *aGain)
{
    /* Support for front-end modules is currently not available in RIOT OS */
    DEBUG("openthread: otPlatRadioGetFemLnaGain is not implemented\n");
    (void)aInstance;
    (void)aGain;

    return OT_ERROR_NOT_IMPLEMENTED;
}

otError otPlatRadioSetFemLnaGain(otInstance *aInstance, int8_t aGain)
{
    /* Support for front-end modules is currently not available in RIOT OS */
    DEBUG("openthread: otPlatRadioSetFemLnaGain is not implemented\n");
    (void)aInstance;
    (void)aGain;

    return OT_ERROR_NOT_IMPLEMENTED;
}

/* OpenThread will call this for getting the state of promiscuous mode */
bool otPlatRadioGetPromiscuous(otInstance *aInstance)
{
    (void)aInstance;
    DEBUG("openthread: otPlatRadioGetPromiscuous\n");

    ieee802154_filter_mode_t filter_mode;
    ieee802154_radio_get_frame_filter_mode(_ot_dev.dev, &filter_mode);

    return filter_mode == IEEE802154_FILTER_PROMISC ? true : false;
}

/* OpenThread will call this for setting the state of promiscuous mode */
void otPlatRadioSetPromiscuous(otInstance *aInstance, bool aEnable)
{
    (void)aInstance;
    DEBUG("openthread: otPlatRadioSetPromiscuous\n");
    _set_promiscuous(aEnable);
}

void otPlatRadioSetRxOnWhenIdle(otInstance *aInstance, bool aEnable)
{
    DEBUG("openthread: otPlatRadioSetRxOnWhenIdle is not implemented\n");
    (void) aInstance;
    (void) aEnable;
}

void otPlatRadioSetMacKey(otInstance             *aInstance,
                          uint8_t aKeyIdMode,
                          uint8_t aKeyId,
                          const otMacKeyMaterial *aPrevKey,
                          const otMacKeyMaterial *aCurrKey,
                          const otMacKeyMaterial *aNextKey,
                          otRadioKeyType aKeyType)
{
    DEBUG("openthread: otPlatRadioSetMacKey is not implemented\n");

    (void)aInstance;
    (void)aKeyIdMode;
    (void)aKeyId;
    (void)aPrevKey;
    (void)aCurrKey;
    (void)aNextKey,
    (void)aKeyType;
}

void otPlatRadioSetMacFrameCounter(otInstance *aInstance, uint32_t aMacFrameCounter)
{
    DEBUG("openthread: otPlatRadioSetMacFrameCounter is not implemented\n");
    (void)aInstance;
    (void)aMacFrameCounter;
}

void otPlatRadioSetMacFrameCounterIfLarger(otInstance *aInstance, uint32_t aMacFrameCounter)
{
    DEBUG("openthread: otPlatRadioSetMacFrameCounterIfLarger is not implemented\n");
    (void)aInstance;
    (void)aMacFrameCounter;
}

uint64_t otPlatRadioGetNow(otInstance *aInstance)
{
    DEBUG("openthread: otPlatRadioGetNow is not implemented\n");
    (void)aInstance;
    return UINT64_MAX;
}

/**
 * Get the bus speed in bits/second between the host and the radio chip.
 *
 * @returns The bus speed in bits/second between the host and the radio chip.
 *          Return 0 when the MAC and above layer and Radio layer resides on the same chip.
 */
uint32_t otPlatRadioGetBusSpeed(otInstance *aInstance)
{
    DEBUG("openthread: otPlatRadioGetBusSpeed is not implemented\n");
    (void)aInstance;
    return 0;
}

uint32_t otPlatRadioGetBusLatency(otInstance *aInstance)
{
    DEBUG("openthread: otPlatRadioGetBusLatency is not implemented\n");
    (void)aInstance;
    return 0;
}

otRadioState otPlatRadioGetState(otInstance *aInstance)
{
    DEBUG("openthread: otPlatRadioGetState is not implemented\n");
    (void)aInstance;
    return OT_RADIO_STATE_DISABLED;
}

/* OpenThread will call this for enabling the radio */
otError otPlatRadioEnable(otInstance *aInstance)
{
    DEBUG("openthread: otPlatRadioEnable\n");
    (void)aInstance;

    if (!otPlatRadioIsEnabled(aInstance)) {
        ieee802154_radio_set_idle(_ot_dev.dev, false);
    }

    return OT_ERROR_NONE;
}

/* OpenThread will call this for disabling the radio */
otError otPlatRadioDisable(otInstance *aInstance)
{
    DEBUG("openthread: otPlatRadioDisable\n");
    (void)aInstance;
    int res = 0;

    if (otPlatRadioIsEnabled(aInstance)) {
        res = _ot_dev.dev->driver->off(_ot_dev.dev);
    }

    /* Invalid state doesn't really fit, but is the only valid alternative */
    return res == 0 ? OT_ERROR_NONE : OT_ERROR_INVALID_STATE;
}

bool otPlatRadioIsEnabled(otInstance *aInstance)
{
    DEBUG("otPlatRadioIsEnabled\n");
    (void)aInstance;
    return _ot_dev.dev->driver->confirm_on == 0 ? true : false;
}

/* OpenThread will call this for setting device state to SLEEP */
otError otPlatRadioSleep(otInstance *aInstance)
{
    DEBUG("otPlatRadioSleep\n");
    (void)aInstance;

    ieee802154_radio_set_idle(_ot_dev.dev, false);
    return OT_ERROR_NONE;
}

/* OpenThread will call this for waiting the reception of a packet */
otError otPlatRadioReceive(otInstance *aInstance, uint8_t aChannel)
{
    DEBUG("openthread: otPlatRadioReceive. Channel: %i\n", aChannel);
    (void)aInstance;

    ieee802154_radio_set_rx(_ot_dev.dev);
    _set_channel(aChannel);
    sReceiveFrame.mChannel = aChannel;
    return OT_ERROR_NONE;
}

otError otPlatRadioReceiveAt(otInstance *aInstance, uint8_t aChannel, uint32_t aStart,
                             uint32_t aDuration)
{
    (void)aInstance;
    (void)aChannel;
    (void)aStart;
    (void)aDuration;
    DEBUG("otPlatRadioReceiveAt\n");
    return OT_ERROR_FAILED; //not implemented
}

/* OpenThread will call this function to get the transmit buffer */
otRadioFrame *otPlatRadioGetTransmitBuffer(otInstance *aInstance)
{
    (void)aInstance;
    DEBUG("openthread: otPlatRadioGetTransmitBuffer\n");
    return &sTransmitFrame;
}

/* OpenThread will call this for transmitting a packet*/
otError otPlatRadioTransmit(otInstance *aInstance, otRadioFrame *aPacket)
{
    (void)aInstance;

    /* Populate iolist with transmit data
     * Unlike RIOT, OpenThread includes two bytes FCS (0x00 0x00) so
     * these bytes are removed
     */
    iolist_t iolist = {
        .iol_base = aPacket->mPsdu,
        .iol_len = (aPacket->mLength - RADIO_IEEE802154_FCS_LEN)
    };

    /*Set channel and power based on transmit frame */
    if (IS_ACTIVE(ENABLE_DEBUG)) {
        DEBUG("otPlatRadioTransmit->channel: %i, length %d\n",
              (int)aPacket->mChannel, (int)aPacket->mLength);
        for (size_t i = 0; i < aPacket->mLength; ++i) {
            DEBUG("%x ", aPacket->mPsdu[i]);
        }
        DEBUG("\n");
    }

    while (ieee802154_radio_set_idle(_ot_dev.dev, false) != 0) {}
    _set_channel(aPacket->mChannel);

    /* send packet though radio hal */
    int res = ieee802154_radio_write(_ot_dev.dev, &iolist);
    if (res != 0) {
        printf("COULD NOT WRITE FRAMEBUFFER CORRECTLY: %d\n", res);
        return OT_ERROR_INVALID_STATE;
    }
    while (ieee802154_radio_request_transmit(_ot_dev.dev) == -EBUSY) {}
    otPlatRadioTxStarted(aInstance, aPacket);

    return OT_ERROR_NONE;
}

int8_t otPlatRadioGetRssi(otInstance *aInstance)
{
    DEBUG("otPlatRadioGetRssi\n");
    (void)aInstance;
    return _ot_dev.rssi;
}

otError otPlatRadioEnergyScan(otInstance *aInstance, uint8_t aScanChannel, uint16_t aScanDuration)
{
    /* using openthread software implementation */
    DEBUG("openthread: otPlatRadioEnergyScan is not implemented\n");
    (void)aInstance;
    (void)aScanChannel;
    (void)aScanDuration;
    return OT_ERROR_NOT_IMPLEMENTED;
}

void otPlatRadioEnableSrcMatch(otInstance *aInstance, bool aEnable)
{
    (void) aInstance;
    if(!ieee802154_radio_has_capability(_ot_dev.dev, IEEE802154_CAP_SRC_ADDR_MATCH)) {
        return;
    }

    if (ieee802154_radio_confirm_on(_ot_dev.dev) == 0) {
        ieee802154_radio_config_src_address_match(_ot_dev.dev, IEEE802154_SRC_MATCH_EN, &aEnable);
    }
}

otError otPlatRadioAddSrcMatchShortEntry(otInstance *aInstance, const uint16_t aShortAddress)
{
    (void)aInstance;
    if(!ieee802154_radio_has_capability(_ot_dev.dev, IEEE802154_CAP_SRC_ADDR_MATCH)) {
        return OT_ERROR_NOT_CAPABLE;
    }
    if (ieee802154_radio_confirm_on(_ot_dev.dev) == 0) {
        /* TODO is aShortAddress really little endian? */
        if (ieee802154_radio_config_src_address_match(_ot_dev.dev, IEEE802154_SRC_MATCH_SHORT_ADD, &aShortAddress) == 0) {
            return OT_ERROR_NONE;
        }
        return  OT_ERROR_NO_BUFS;
    }
    return OT_ERROR_BUSY;
}

otError otPlatRadioAddSrcMatchExtEntry(otInstance *aInstance, const otExtAddress *aExtAddress)
{
    (void)aInstance;
    if(!ieee802154_radio_has_capability(_ot_dev.dev, IEEE802154_CAP_SRC_ADDR_MATCH)) {
        return OT_ERROR_NOT_CAPABLE;
    }
     if (ieee802154_radio_confirm_on(_ot_dev.dev) == 0) {
        uint8_t _ext_addr[8];
        byteorder_htobebufll((uint8_t*) &_ext_addr,byteorder_bebuftohll((uint8_t*) aExtAddress));
        if (ieee802154_radio_config_src_address_match(_ot_dev.dev, IEEE802154_SRC_MATCH_EXT_ADD, &_ext_addr) == 0) {
            return OT_ERROR_NONE;
        }
        return  OT_ERROR_NO_BUFS;
    }
    return OT_ERROR_BUSY;
}

otError otPlatRadioClearSrcMatchShortEntry(otInstance *aInstance, const uint16_t aShortAddress)
{
    (void)aInstance;
    if(!ieee802154_radio_has_capability(_ot_dev.dev, IEEE802154_CAP_SRC_ADDR_MATCH)) {
        return OT_ERROR_NOT_CAPABLE;
    }
    if (ieee802154_radio_confirm_on(_ot_dev.dev) == 0) {
        /* TODO is aShortAddress really little endian? */
        //const int16_t _short_addr = byteorder_htob(aShortAddress);
        if (ieee802154_radio_config_src_address_match(_ot_dev.dev, IEEE802154_SRC_MATCH_SHORT_CLEAR, &aShortAddress) == 0) {
            return OT_ERROR_NONE;
        }
    }
    return OT_ERROR_BUSY;
}

otError otPlatRadioClearSrcMatchExtEntry(otInstance *aInstance, const otExtAddress *aExtAddress)
{
    (void)aInstance;
    if(!ieee802154_radio_has_capability(_ot_dev.dev, IEEE802154_CAP_SRC_ADDR_MATCH)) {
        return OT_ERROR_NOT_CAPABLE;
    }
    if (ieee802154_radio_confirm_on(_ot_dev.dev) == 0) {
        uint8_t _ext_addr[8];
        byteorder_htobebufll((uint8_t*) &_ext_addr,byteorder_bebuftohll((uint8_t*) aExtAddress));
        if (ieee802154_radio_config_src_address_match(_ot_dev.dev, IEEE802154_SRC_MATCH_EXT_CLEAR, &_ext_addr) == 0) {
            return OT_ERROR_NONE;
        }
    }
    return OT_ERROR_BUSY;
}

void otPlatRadioClearSrcMatchShortEntries(otInstance *aInstance)
{
    DEBUG("openthread: otPlatRadioClearSrcMatchShortEntries is not implemented\n");
    (void)aInstance;
}

void otPlatRadioClearSrcMatchExtEntries(otInstance *aInstance)
{
    DEBUG("openthread: otPlatRadioClearSrcMatchExtEntries is not implemented\n");
    (void)aInstance;
}

uint32_t otPlatRadioGetSupportedChannelMask(otInstance *aInstance)
{
    uint32_t channel_mask;
    channel_mask = 0x07fff800;
    (void)aInstance;
    return channel_mask;
}

uint32_t otPlatRadioGetPreferredChannelMask(otInstance *aInstance)
{
    DEBUG("openthread: otPlatRadioGetPreferredChannelMask is not implemented\n");
    (void)aInstance;
    return 0;
}

otError otPlatRadioSetCoexEnabled(otInstance *aInstance, bool aEnabled)
{
    DEBUG("openthread: otPlatRadioSetCoexEnabled is not implemented\n");
    (void)aInstance;
    (void)aEnabled;
    return OT_ERROR_FAILED;
}

bool otPlatRadioIsCoexEnabled(otInstance *aInstance)
{
    DEBUG("openthread: otPlatRadioIsCoexEnabled is not implemented\n");
    (void)aInstance;
    return false;
}

otError otPlatRadioGetCoexMetrics(otInstance *aInstance, otRadioCoexMetrics *aCoexMetrics)
{
    DEBUG("openthread: otPlatRadioGetCoexMetrics is not implemented\n");
    (void)aInstance;
    (void)aCoexMetrics;
    return OT_ERROR_INVALID_ARGS;
}

otError otPlatRadioEnableCsl(otInstance         *aInstance,
                             uint32_t aCslPeriod,
                             otShortAddress aShortAddr,
                             const otExtAddress *aExtAddr)
{
    DEBUG("openthread: otPlatRadioEnableCsl is not implemented\n");

    (void)aInstance;
    (void)aCslPeriod;
    (void)aShortAddr;
    (void)aExtAddr;

    return OT_ERROR_NOT_IMPLEMENTED;
}

otError otPlatRadioResetCsl(otInstance *aInstance)
{
    DEBUG("openthread: otPlatRadioResetCsl is not implemented\n");
    (void)aInstance;
    return OT_ERROR_NOT_IMPLEMENTED;
}

void otPlatRadioUpdateCslSampleTime(otInstance *aInstance, uint32_t aCslSampleTime)
{
    DEBUG("openthread: otPlatRadioUpdateCslSampleTime is not implemented\n");
    (void)aInstance;
    (void)aCslSampleTime;
}

uint8_t otPlatRadioGetCslAccuracy(otInstance *aInstance)
{
    DEBUG("openthread: otPlatRadioGetCslAccuracy is not implemented\n");
    (void)aInstance;
    return UINT8_MAX;
}

uint8_t otPlatRadioGetCslUncertainty(otInstance *aInstance)
{
    DEBUG("openthread: otPlatRadioGetCslUncertainty is not implemented\n");
    (void)aInstance;
    return UINT8_MAX;
}

otError otPlatRadioSetChannelMaxTransmitPower(otInstance *aInstance, uint8_t aChannel,
                                              int8_t aMaxPower)
{
    DEBUG("openthread: otPlatRadioSetChannelMaxTransmitPower is not implemented\n");
    (void)aInstance;
    (void)aChannel;
    (void)aMaxPower;
    return OT_ERROR_NOT_IMPLEMENTED;
}

otError otPlatRadioSetRegion(otInstance *aInstance, uint16_t aRegionCode)
{
    DEBUG("openthread: otPlatRadioSetRegion is not implemented\n");
    (void)aInstance;
    (void)aRegionCode;
    return OT_ERROR_NOT_IMPLEMENTED;
}

otError otPlatRadioGetRegion(otInstance *aInstance, uint16_t *aRegionCode)
{
    DEBUG("openthread: otPlatRadioGetRegion is not implemented\n");
    (void)aInstance;
    (void)aRegionCode;
    return OT_ERROR_NOT_IMPLEMENTED;
}

otError otPlatRadioConfigureEnhAckProbing(otInstance         *aInstance,
                                          otLinkMetrics aLinkMetrics,
                                          otShortAddress aShortAddress,
                                          const otExtAddress *aExtAddress)
{
    DEBUG("openthread: otPlatRadioConfigureEnhAckProbing is not implemented\n");

    (void)aInstance;
    (void)aLinkMetrics;
    (void)aShortAddress;
    (void)aExtAddress;
    return OT_ERROR_NOT_IMPLEMENTED;
}

/* optional */
otError otPlatRadioAddCalibratedPower(otInstance    *aInstance,
                                      uint8_t aChannel,
                                      int16_t aActualPower,
                                      const uint8_t *aRawPowerSetting,
                                      uint16_t aRawPowerSettingLength)
{
    DEBUG("openthread: otPlatRadioAddCalibratedPower is not implemented\n");

    (void)aInstance;
    (void)aChannel;
    (void)aActualPower;
    (void)aRawPowerSetting;
    (void)aRawPowerSettingLength;
    return OT_ERROR_NOT_IMPLEMENTED;
}

/* optional */
otError otPlatRadioClearCalibratedPowers(otInstance *aInstance)
{
    DEBUG("openthread: otPlatRadioClearCalibratedPowers is not implemented\n");
    (void)aInstance;
    return OT_ERROR_NOT_IMPLEMENTED;
}

/* optional */
otError otPlatRadioSetChannelTargetPower(otInstance *aInstance, uint8_t aChannel,
                                         int16_t aTargetPower)
{
    DEBUG("openthread: otPlatRadioSetChannelTargetPower is not implemented\n");

    (void)aInstance;
    (void)aChannel;
    (void)aTargetPower;
    return OT_ERROR_NOT_IMPLEMENTED;
}
