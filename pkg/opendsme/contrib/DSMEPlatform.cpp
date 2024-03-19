/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  José I. Álamos <jose.alamos@haw-hamburg.de>
 */

#include "opendsme/opendsme.h"
#include "opendsme/DSMEPlatform.h"
#include "ztimer.h"
#include "iolist.h"
#include "event.h"
#include "event/thread.h"
#include "luid.h"
#include "dsmeAdaptionLayer/scheduling/TPS.h"
#include "dsmeAdaptionLayer/scheduling/StaticScheduling.h"
#include "board.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* Use default openDSME value for TPS scheduler alpha */
#define OPENDSME_TPS_ALPHA (0.1f)

/* Used for symbol counter calculation. Hardcoded to O-QPSK */
#define OPENDSME_TIMER_MASK   (0xF)
#define OPENDSME_TIMER_OFFSET (4U)

namespace dsme {

/* The one and only openDSME instance */
DSMEPlatform *DSMEPlatform::instance = nullptr;

/********************* C functions *********************/

static void _cca_ev_handler(event_t *ev);
static void _acktimer_ev_handler(event_t *ev);
static void _acktimer_cb(void *arg);
static void _timer_ev_handler(event_t *ev);
static void _tx_done_handler(event_t *ev);
static void _rx_done_handler(event_t *ev);
static void _handle_rx_offload(event_t *ev);
static void _start_of_cfp_handler(event_t *ev);

/* Event used for ACK Timeout */
static event_t acktimer_ev = {{}, _acktimer_ev_handler};

/* Event used for CCA Done */
static event_t cca_ev = {{}, _cca_ev_handler};

/* Event used for timer events */
static event_t timer_event = {{}, _timer_ev_handler};

/* Event used for TX Done */
static event_t tx_done_event = {{}, _tx_done_handler};

/* Event used for RX Done */
static event_t rx_done_event = {{}, _rx_done_handler};

/* Event used for offloading the receive procedure */
static event_t rx_offload_ev = {{}, _handle_rx_offload};

/* Event used for offloading the start of a CFP */
static event_t start_of_cfp_ev = {{}, _start_of_cfp_handler};

void _handle_rx_offload(event_t *ev)
{
    dsme::DSMEPlatform::instance->processRxOffload();
}

void _start_of_cfp_handler(event_t *ev)
{
    dsme::DSMEPlatform::instance->getDSME().handleStartOfCFP();
    dsme::DSMEPlatform::instance->updateVisual();
}

static void _cca_ev_handler(event_t *ev)
{
    dsme::DSMEPlatform::instance->processCCAEvent();
}

static void _acktimer_ev_handler(event_t *ev)
{
    dsme::DSMEPlatform::instance->sendNow();
}

static void _acktimer_cb(void *arg)
{
    dsme::DSMEPlatform::instance->offloadACKTimer();
}

static void _timer_ev_handler(event_t *ev)
{
    dsme::DSMEPlatform::instance->getDSME().getEventDispatcher().timerInterrupt();
}

static void _tx_done_handler(event_t *ev)
{
    dsme::DSMEPlatform::instance->processTXDoneEvent();
}

static void _rx_done_handler(event_t *ev)
{
    dsme::DSMEPlatform::instance->processRxDone();
}

static void _hal_radio_cb(ieee802154_dev_t *dev, ieee802154_trx_ev_t status)
{
    switch (status) {
    case IEEE802154_RADIO_CONFIRM_TX_DONE:
        dsme::DSMEPlatform::instance->offloadTXDoneEvent();
        break;
    case IEEE802154_RADIO_INDICATION_RX_START:
        dsme::DSMEPlatform::instance->indicateRxStart();
        break;
    case IEEE802154_RADIO_INDICATION_CRC_ERROR:
        break;
    case IEEE802154_RADIO_INDICATION_TX_START:
        break;
    case IEEE802154_RADIO_INDICATION_RX_DONE:
        dsme::DSMEPlatform::instance->offloadRXDoneEvent();
        break;
    case IEEE802154_RADIO_CONFIRM_CCA:
        dsme::DSMEPlatform::instance->offloadCCAEvent();
        break;
    default:
        DSME_ASSERT(false);
    }
}

/********************* C++ functions *********************/

void DSMEPlatform::processRxOffload()
{
    IDSMEMessage *message = this->message;

    this->message = nullptr;
    receiveFromAckLayerDelegate(message);
}

void DSMEPlatform::processCCAEvent()
{
    bool clear = ieee802154_radio_confirm_cca(this->radio);

    this->setPlatformState(DSMEPlatform::STATE_READY);
    this->getDSME().dispatchCCAResult(clear);
}

void DSMEPlatform::processTXDoneEvent()
{
    int res = ieee802154_radio_confirm_transmit(this->radio, NULL);

    this->pending_tx = false;
    DSME_ASSERT(res >= 0);

    res = ieee802154_radio_set_rx(this->radio);
    DSME_ASSERT(res == 0);

    if (this->wait_for_ack) {
        this->wait_for_ack = false;
        ieee802154_radio_set_frame_filter_mode(this->radio, IEEE802154_FILTER_ACK_ONLY);
    }
    else {
        ieee802154_radio_set_frame_filter_mode(this->radio, IEEE802154_FILTER_ACCEPT);
    }
    this->txEndCallback(true);
    this->setPlatformState(DSMEPlatform::STATE_READY);
}

void DSMEPlatform::processRxDone()
{
    if (this->state != STATE_READY) {
        assert(false);
        return;
    }

    DSMEMessage *message = getEmptyMessage();

    message->netif = this->netif;
    message->setStartOfFrameDelimiterSymbolCounter(rx_sfd);

    int res;

    res = ieee802154_radio_set_idle(this->radio, true);
    DSME_ASSERT(res == 0);
    int len = ieee802154_radio_len(this->radio);

    if (len > 127 || len < 0) {
        ieee802154_radio_read(this->radio, NULL, 127, NULL);
        res = ieee802154_radio_set_rx(this->radio);
        DSME_ASSERT(res == 0);
        return;
    }
    res = message->loadBuffer(len);
    DSME_ASSERT(res >= 0);
    ieee802154_rx_info_t info;

    res = ieee802154_radio_read(this->radio, message->getPayload(), 127, &info);
    if (res < 0) {
        message->releaseMessage();
        res = ieee802154_radio_set_rx(this->radio);
        DSME_ASSERT(res == 0);
        return;
    }

    message->messageLQI = info.lqi;
    message->messageRSSI = info.rssi;
    const uint8_t *buf = message->getPayload();

    bool success = message->getHeader().deserializeFrom(buf, len);

    if (!success) {
        message->releaseMessage();
        res = ieee802154_radio_set_rx(this->radio);
        DSME_ASSERT(res == 0);
        return;
    }

    message->dropHdr(message->getHeader().getSerializationLength());

    res = ieee802154_radio_set_rx(this->radio);
    DSME_ASSERT(res == 0);

    getDSME().getAckLayer().receive(message);
}

void DSMEPlatform::offloadCCAEvent()
{
    event_post(this->getEventQueue(), &cca_ev);
}

void DSMEPlatform::offloadTXDoneEvent()
{
    event_post(this->getEventQueue(), &tx_done_event);
}

void DSMEPlatform::indicateRxStart()
{
    this->rx_sfd = this->getSymbolCounter();
}

void DSMEPlatform::offloadRXDoneEvent()
{
    event_post(this->getEventQueue(), &rx_done_event);
}

void DSMEPlatform::offloadTimerEvent()
{
    event_post(this->getEventQueue(), &timer_event);
}

void DSMEPlatform::offloadACKTimer()
{
    event_post(this->getEventQueue(), &acktimer_ev);
}

static void _timer_cb(void *arg)
{
    dsme::DSMEPlatform::instance->offloadTimerEvent();
}

void DSMEPlatform::sendFrame(uint16_t addr, iolist_t *pkt)
{
    /* First 2 bytes are the ID */
    if (!this->mac_pib.macAssociatedPANCoord) {
        return;
    }

    DSMEMessage *message = getEmptyMessage();

    if (message->loadBuffer(pkt) < 0) {
        return;
    }

    IEEE802154MacAddress dst;

    dst.setShortAddress(addr);
    mcps_sap::DATA::request_parameters params;

    message->getHeader().setSrcAddrMode(SHORT_ADDRESS);
    message->getHeader().setDstAddrMode(SHORT_ADDRESS);
    message->getHeader().setDstAddr(dst);

    message->getHeader().setSrcPANId(this->mac_pib.macPANId);
    message->getHeader().setDstPANId(this->mac_pib.macPANId);

    this->dsmeAdaptionLayer.sendMessage(message);
}

DSMEPlatform::DSMEPlatform() :
    phy_pib(),
    mac_pib(phy_pib),

    mcps_sap(dsme),
    mlme_sap(dsme),
    dsmeAdaptionLayer(dsme),
    initialized(false),
    state(STATE_READY)
{
    instance = this;
    this->timer.callback = _timer_cb;
    this->timer.arg = this;

    this->acktimer.callback = _acktimer_cb;
    this->acktimer.arg = this;
}

DSMEPlatform::~DSMEPlatform()
{}

/**
 * Creates an IEEE802154MacAddress out of an uint16_t short address
 */
void DSMEPlatform::translateMacAddress(uint16_t& from, IEEE802154MacAddress& to)
{
    if (from == 0xFFFF) {
        to = IEEE802154MacAddress(IEEE802154MacAddress::SHORT_BROADCAST_ADDRESS);
    }
    else {
        to.setShortAddress(from);
    }
}

void DSMEPlatform::initialize(bool pan_coord)
{
    this->instance = this;
    this->dsme.setPHY_PIB(&(this->phy_pib));
    this->dsme.setMAC_PIB(&(this->mac_pib));
    this->dsme.setMCPS(&(this->mcps_sap));
    this->dsme.setMLME(&(this->mlme_sap));

    /* Use all channels of the channel page 0 (O-QPSK) */
    constexpr uint8_t MAX_CHANNELS = 16;
    uint8_t channels[MAX_CHANNELS];

    uint8_t num = MAX_CHANNELS;

    channelList_t DSSS2450_channels(num);

    for (uint8_t i = 0; i < num; i++) {
        DSSS2450_channels[i] = 11 + i;
    }

    phy_pib.setDSSS2450ChannelPage(DSSS2450_channels);

    /* Initialize Address */
    IEEE802154MacAddress address;

    uint8_t ext_addr[IEEE802154_LONG_ADDRESS_LEN];
    network_uint16_t short_addr;

    luid_base(ext_addr, sizeof(ext_addr));
    address.setA1((ext_addr[0] << 8) | ext_addr[1]);
    address.setA2((ext_addr[2] << 8) | ext_addr[3]);
    address.setA3((ext_addr[4] << 8) | ext_addr[5]);
    address.setA4((ext_addr[6] << 8) | ext_addr[7]);
    this->mac_pib.macExtendedAddress = address;

    /* TODO: UGLY HACK! To be removed when gnrc_netif<->netdev dependency is
     * not granted */
    this->radio = (ieee802154_dev_t *)this->netif->dev;

    this->radio->cb = _hal_radio_cb;

    ieee802154_radio_request_on(this->radio);
    while (ieee802154_radio_confirm_on(this->radio) == -EAGAIN) {}
    /* Disable Auto CSMA-CA */
    ieee802154_radio_set_csma_params(this->radio, NULL, -1);

    /* Accept all frames except ACK */
    ieee802154_radio_set_frame_filter_mode(this->radio, IEEE802154_FILTER_ACCEPT);

    /* Call more radio configurations here if needed... */

    this->mac_pib.macShortAddress = this->mac_pib.macExtendedAddress.getShortAddress();

    short_addr.u8[0] = this->mac_pib.macExtendedAddress.getShortAddress() >> 8;
    short_addr.u8[1] = this->mac_pib.macExtendedAddress.getShortAddress() & 0xFF;

    this->mac_pib.macIsPANCoord = pan_coord;
    this->mac_pib.macIsCoord = pan_coord;
    if (this->mac_pib.macIsPANCoord) {
        DEBUG("This node is PAN coordinator\n");
        this->mac_pib.macPANId = CONFIG_IEEE802154_DEFAULT_PANID;
    }
    ieee802154_radio_config_addr_filter(this->radio, IEEE802154_AF_PANID, &this->mac_pib.macPANId);
    ieee802154_radio_config_addr_filter(this->radio, IEEE802154_AF_SHORT_ADDR, &short_addr);
    ieee802154_radio_config_addr_filter(this->radio, IEEE802154_AF_EXT_ADDR, &ext_addr);

    this->mac_pib.macCapReduction = CONFIG_IEEE802154_DSME_CAP_REDUCTION;

    this->mac_pib.macAssociatedPANCoord = this->mac_pib.macIsPANCoord;
    this->mac_pib.macSuperframeOrder = CONFIG_IEEE802154_DSME_SUPERFRAME_ORDER;
    this->mac_pib.macMultiSuperframeOrder = CONFIG_IEEE802154_DSME_MULTISUPERFRAME_ORDER;
    this->mac_pib.macBeaconOrder = CONFIG_IEEE802154_DSME_BEACON_ORDER;

    this->mac_pib.macMinBE = CONFIG_IEEE802154_DEFAULT_CSMA_CA_MIN_BE;
    this->mac_pib.macMaxBE = CONFIG_IEEE802154_DEFAULT_CSMA_CA_MAX_BE;
    this->mac_pib.macMaxCSMABackoffs = CONFIG_IEEE802154_DEFAULT_CSMA_CA_RETRIES;
    this->mac_pib.macMaxFrameRetries = CONFIG_IEEE802154_DEFAULT_MAX_FRAME_RETRANS;

    this->mac_pib.macDSMEGTSExpirationTime = CONFIG_IEEE802154_DSME_GTS_EXPIRATION;
    this->mac_pib.macResponseWaitTime = CONFIG_IEEE802154_DSME_MAC_RESPONSE_WAIT_TIME;
    this->mac_pib.macChannelDiversityMode = Channel_Diversity_Mode::CHANNEL_HOPPING;

    this->phy_pib.phyCurrentChannel = CONFIG_IEEE802154_DEFAULT_CHANNEL;

    this->dsmeAdaptionLayer.setIndicationCallback(DELEGATE(&DSMEPlatform::
                                                           handleDataMessageFromMCPSWrapper,
                                                           *this));
    this->dsmeAdaptionLayer.setConfirmCallback(DELEGATE(&DSMEPlatform::handleConfirmFromMCPSWrapper,
                                                        *this));

    this->dsme.initialize(this);

    channelList_t scanChannels;

    scanChannels.add(CONFIG_IEEE802154_DEFAULT_CHANNEL);
    if (IS_ACTIVE(CONFIG_IEEE802154_DSME_STATIC_GTS)) {
        StaticScheduling *staticScheduling = new StaticScheduling(this->dsmeAdaptionLayer);
        staticScheduling->setNegotiateChannels(false);
        scheduling = staticScheduling;
    }
    else {
        TPS *tps = new TPS(this->dsmeAdaptionLayer);
        tps->setAlpha(OPENDSME_TPS_ALPHA);
        tps->setMinFreshness(this->mac_pib.macDSMEGTSExpirationTime);
        scheduling = tps;
    }

    this->dsmeAdaptionLayer.initialize(scanChannels, CONFIG_IEEE802154_DSME_SCAN_DURATION,
                                       scheduling);
    this->initialized = true;
}

#if IS_ACTIVE(CONFIG_IEEE802154_DSME_STATIC_GTS)
void DSMEPlatform::allocateGTS(uint8_t superframeID, uint8_t slotID, uint8_t channelID,
                               Direction direction, uint16_t address)
{
    static_cast<StaticScheduling *>(scheduling)->allocateGTS(superframeID, slotID, channelID,
                                                             direction, address);
}
#endif

void DSMEPlatform::setGTSTransmission(bool gts)
{
    this->dsmeAdaptionLayer.getMessageHelper().setGTSTransmission(gts);
}

void DSMEPlatform::setAckReq(bool ackReq)
{
    this->dsmeAdaptionLayer.getMessageHelper().setAckReq(ackReq);
}

void DSMEPlatform::start()
{
    DSME_ASSERT(this->initialized);
    this->dsme.start();
    this->dsmeAdaptionLayer.startAssociation();
}

void DSMEPlatform::getShortAddress(network_uint16_t *addr)
{
    addr->u8[0] = this->mac_pib.macExtendedAddress.getShortAddress() >> 8;
    addr->u8[1] = this->mac_pib.macExtendedAddress.getShortAddress() & 0xFF;
}

bool DSMEPlatform::isAssociated()
{
    return this->mac_pib.macAssociatedPANCoord;
}

void DSMEPlatform::handleDataMessageFromMCPSWrapper(IDSMEMessage *msg)
{
    this->handleDataMessageFromMCPS(static_cast<DSMEMessage *>(msg));
}

void DSMEPlatform::handleConfirmFromMCPSWrapper(IDSMEMessage *msg,
                                                DataStatus::Data_Status dataStatus)
{
    this->handleConfirmFromMCPS(static_cast<DSMEMessage *>(msg), dataStatus);
}

void DSMEPlatform::handleConfirmFromMCPS(DSMEMessage *msg, DataStatus::Data_Status dataStatus)
{
    if (dataStatus == DataStatus::Data_Status::SUCCESS) {
        /* TODO: Add to statistics */
    }
    IDSMEMessage *m = static_cast<IDSMEMessage *>(msg);

    releaseMessage(m);
}

void DSMEPlatform::handleDataMessageFromMCPS(DSMEMessage *msg)
{
    msg->dispatchMessage();
}

bool DSMEPlatform::isReceptionFromAckLayerPossible()
{
    return this->state == STATE_READY;
}

void DSMEPlatform::handleReceivedMessageFromAckLayer(IDSMEMessage *message)
{
    DSME_ASSERT(receiveFromAckLayerDelegate);
    DSME_ASSERT(!this->message);
    this->message = message;
    event_post(this->getEventQueue(), &rx_offload_ev);
}

DSMEMessage *DSMEPlatform::getEmptyMessage()
{
    DSMEMessage *msg = new DSMEMessage();

    DSME_ASSERT(msg);
    msg->clearMessage();
    signalNewMsg(msg);
    return msg;
}

void DSMEPlatform::signalNewMsg(DSMEMessage *msg)
{
    /* Not used */
}

void DSMEPlatform::releaseMessage(IDSMEMessage *msg)
{
    DSMEMessage *m = static_cast<DSMEMessage *>(msg);

    m->releaseMessage();
}

void DSMEPlatform::startTimer(uint32_t symbolCounterValue)
{
    uint32_t now = ztimer_now(ZTIMER_USEC);
    uint32_t offset = now & OPENDSME_TIMER_MASK;
    /* This works even if there's an overflow */
    int32_t delta = ((symbolCounterValue - getSymbolCounter()) << OPENDSME_TIMER_OFFSET)
                    - offset;

    ztimer_set(ZTIMER_USEC, &timer, (uint32_t)delta);
}

uint32_t DSMEPlatform::getSymbolCounter()
{
    return ztimer_now(ZTIMER_USEC) >> OPENDSME_TIMER_OFFSET;
}

void DSMEPlatform::scheduleStartOfCFP()
{
    event_post(this->getEventQueue(), &start_of_cfp_ev);
}

void DSMEPlatform::signalAckedTransmissionResult(bool success, uint8_t transmissionAttempts,
                                                 IEEE802154MacAddress receiver)
{
    /* Not used */
}

/*
 * Signal GTS allocation or deallocation
 */
void DSMEPlatform::signalGTSChange(bool deallocation, IEEE802154MacAddress counterpart,
                                   uint16_t superframeID, uint8_t gtSlotID, uint8_t channel,
                                   Direction direction)
{}

void DSMEPlatform::signalQueueLength(uint32_t length)
{}

/*
 * Number of packets sent per CAP
 */
void DSMEPlatform::signalPacketsPerCAP(uint32_t packets)
{}

/*
 * Number of failed packets per CAP
 */
void DSMEPlatform::signalFailedPacketsPerCAP(uint32_t packets)
{
    /* Not used */
}

void DSMEPlatform::updateVisual()
{
    /* Not used */
}

bool DSMEPlatform::setChannelNumber(uint8_t channel)
{
    ieee802154_phy_conf_t conf = {
        .phy_mode = IEEE802154_PHY_OQPSK,
        .channel = channel,
        .page = 0,
        .pow = CONFIG_IEEE802154_DEFAULT_TXPOWER,
    };
    int res;

    res = ieee802154_radio_set_idle(this->radio, true);
    DSME_ASSERT(res == 0);
    res = ieee802154_radio_config_phy(this->radio, &conf);
    DSME_ASSERT(res == 0);

    /* TODO: Find a better solution */
    ieee802154_radio_config_addr_filter(this->radio, IEEE802154_AF_PANID, &this->mac_pib.macPANId);

    res = ieee802154_radio_set_rx(this->radio);
    DSME_ASSERT(res == 0);
    return true;
}

uint8_t DSMEPlatform::getChannelNumber()
{
    /* Apparently not used by OpenDSME */
    DSME_ASSERT(false);
    return 0;
}

bool DSMEPlatform::prepareSendingCopy(IDSMEMessage *msg, Delegate<void(bool)> txEndCallback)
{
    DSMEMessage *m = (DSMEMessage *)msg;

    this->state = DSMEPlatform::STATE_SEND;
    this->txEndCallback = txEndCallback;
    uint8_t mhr[IEEE802154_MAX_HDR_LEN];
    uint8_t mhr_len = msg->getHeader().getSerializationLength();
    uint8_t *p = mhr;

    msg->getHeader().serializeTo(p);
    iolist_t iol = {
        .iol_next = (iolist_t *)m->getIolPayload(),
        .iol_base = mhr,
        .iol_len = mhr_len,
    };

    if (mhr[0] & IEEE802154_FCF_ACK_REQ) {
        this->wait_for_ack = true;
    }
    else {
        this->wait_for_ack = false;
    }

    int res = ieee802154_radio_set_idle(this->radio, true);

    DSME_ASSERT(res == 0);
    res = ieee802154_radio_write(this->radio, &iol);
    DSME_ASSERT(res == 0);

    return true;
}

bool DSMEPlatform::sendNow()
{
    int res = ieee802154_radio_request_transmit(this->radio);

    DSME_ASSERT(res == 0);
    this->pending_tx = true;
    return true;
}

void DSMEPlatform::abortPreparedTransmission()
{
    /* Nothing to do here, since the Radio HAL will drop the frame if
     * the write function is called again */
    this->setPlatformState(DSMEPlatform::STATE_READY);
}

bool DSMEPlatform::sendDelayedAck(IDSMEMessage *ackMsg, IDSMEMessage *receivedMsg,
                                  Delegate<void(bool)> txEndCallback)
{
    DSMEMessage *m = (DSMEMessage *)ackMsg;

    DSME_ASSERT(m != nullptr);

    uint8_t ack[IEEE802154_ACK_FRAME_LEN - IEEE802154_FCS_LEN];
    uint8_t mhr_len = ackMsg->getHeader().getSerializationLength();

    DSME_ASSERT(mhr_len == sizeof(ack));

    uint8_t *p = ack;

    ackMsg->getHeader().serializeTo(p);

    this->txEndCallback = txEndCallback;

    iolist_t iol = {
        .iol_next = NULL,
        .iol_base = ack,
        .iol_len = mhr_len,
    };

    int res = ieee802154_radio_set_idle(this->radio, true);

    DSME_ASSERT(res == 0);
    res = ieee802154_radio_write(this->radio, &iol);
    DSME_ASSERT(res == 0);

    /* Hardcoded to O-QPSK
     * Preamble (4) | SFD (1) | PHY Hdr (1) | MAC Payload | FCS (2)
     */
    uint32_t endOfReception = receivedMsg->getStartOfFrameDelimiterSymbolCounter()
                              + receivedMsg->getTotalSymbols()
                              - 2 * 4   /* Preamble */
                              - 2 * 1;  /* SFD */
    uint32_t ackTime = endOfReception + aTurnaroundTime;
    uint32_t now = getSymbolCounter();
    uint32_t diff = ackTime - now;

    ztimer_set(ZTIMER_USEC, &this->acktimer, diff * aSymbolDuration);
    return true;
}

void DSMEPlatform::setReceiveDelegate(receive_delegate_t receiveDelegate)
{
    this->receiveFromAckLayerDelegate = receiveDelegate;
}

bool DSMEPlatform::startCCA()
{
    if (this->pending_tx) {
        return false;
    }
    ieee802154_radio_request_cca(this->radio);
    this->state = DSMEPlatform::STATE_CCA_WAIT;
    return true;
}

void DSMEPlatform::turnTransceiverOn()
{
    int res = ieee802154_radio_request_on(this->radio);

    DSME_ASSERT(res == 0);
    res = ieee802154_radio_confirm_on(this->radio);
    DSME_ASSERT(res == 0);
    ieee802154_radio_set_cca_threshold(this->radio, CONFIG_IEEE802154_CCA_THRESH_DEFAULT);
}

void DSMEPlatform::turnTransceiverOff()
{
    int res = ieee802154_radio_off(this->radio);

    DSME_ASSERT(res == 0);
}

bool DSMEPlatform::isRxEnabledOnCap()
{
    /* TODO: This feature is experimental. Enable RX on CAP for now... */
    return true;
}

}

/** @} */
