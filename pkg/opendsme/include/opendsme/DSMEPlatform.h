/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    pkg_opendsme_dsmeplatform DSME Platform interface implementation.
 * @ingroup     pkg_opendsme
 * @brief       Implementation of the DSME Platform interface.
 *
 * @{
 *
 * @file
 * @brief   DSME Platform interface implementation
 *
 * @author  José I. Álamos <jose.alamos@haw-hamburg.de>
 */

#ifndef OPENDSME_DSMEPLATFORM_H
#define OPENDSME_DSMEPLATFORM_H

#include <stdint.h>
#include <stdlib.h>

#include <string>
#include "random.h"

#include "byteorder.h"
#include "dsmeAdaptionLayer/DSMEAdaptionLayer.h"
#include "dsmeLayer/DSMELayer.h"
#include "helper/DSMEDelegate.h"
#include "interfaces/IDSMEPlatform.h"
#include "mac_services/dataStructures/IEEE802154MacAddress.h"
#include "mac_services/mcps_sap/MCPS_SAP.h"
#include "mac_services/mlme_sap/MLME_SAP.h"
#include "mac_services/pib/MAC_PIB.h"
#include "mac_services/pib/PHY_PIB.h"
#include "mac_services/pib/dsme_phy_constants.h"
#include "mac_services/DSME_Common.h"
#include "net/gnrc/netif.h"
#include "net/ieee802154/radio.h"
#include "opendsme/dsme_settings.h"
#include "opendsme/DSMEMessage.h"
#include "ztimer.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace dsme {

struct DSMESettings;
class DSMELayer;
class DSMEAdaptionLayer;

/**
 * @brief DSMEPlatform interface implementation for GNRC
 */
class DSMEPlatform : public IDSMEPlatform {
public:
    /**
     * @brief DSMEPlatform constructor
     */
    DSMEPlatform();
    /**
     * @brief DSMEPlatform destructor
     */
    ~DSMEPlatform();

    /**
     * @brief pointer to the DSME instance
     */
    static DSMEPlatform* instance;

    /**
     * @brief initialize MAC with a role (PAN coordinator, child)
     */
    void initialize(bool pan_coord);

    /**
     * @brief to be called by the upper layer in order to send a frame
     */
    void sendFrame(uint16_t addr, iolist_t *pkt);

    /**
     * @brief start DSME
     */
    void start();

    /**
     * @brief get the DSME layer
     */
    DSMELayer& getDSME() {
        return dsme;
    }

    /**
     * @brief check whether the node associated
     */
    bool isAssociated();

#if IS_ACTIVE(CONFIG_IEEE802154_DSME_STATIC_GTS) || DOXYGEN
    /**
     * @brief allocate a GTS slot
     */
    void allocateGTS(uint8_t superframeID, uint8_t slotID, uint8_t channelID,
                     Direction direction, uint16_t address);
#endif

    /**
     * @brief get short address
     */
    void getShortAddress(network_uint16_t *addr);

    /**
     * @brief set GTS or CAP transmission
     */
    void setGTSTransmission(bool gts);

    /**
     * @brief set ACK_REQ bit
     */
    void setAckReq(bool ackReq);

    /**
     * @brief request to offload the CCA Done event
     */
    void offloadCCAEvent();

    /**
     * @brief request to offload the TX Done event
     */
    void offloadTXDoneEvent();

    /**
     * @brief indicate the MAC layer that the reception started
     */
    void indicateRxStart();

    /**
     * @brief request to offload RX Done event
     */
    void offloadRXDoneEvent();

    /**
     * @brief request to offload ACK Timer event
     */
    void offloadACKTimer();

    /**
     * @brief request to offload Timer event
     */
    void offloadTimerEvent();

    /**
     * @brief set the platform state
     */
    void setPlatformState(uint8_t state) {
        this->state = state;
    }

    /**
     * @brief process the CCA Done event
     */
    void processCCAEvent();

    /**
     * @brief process the TX Done event
     */
    void processTXDoneEvent();

    /**
     * @brief process the RX Done event from radio
     */
    void processRxDone();

    /**
     * @brief process the offload event of received frame
     */
    void processRxOffload();

    /**
     * @brief set the GNRC netif
     */
    void setGNRCNetif(gnrc_netif_t *netif) {
        this->netif = netif;
    }

    /**
     * @brief state of the Platform layer
     */
    enum {
        STATE_READY = 0, STATE_CCA_WAIT = 1, STATE_SEND = 2,
    };

    /*********** IDSMEPlatform implementation ***********/

    /**
     * @brief get channel number
     */
    uint8_t getChannelNumber() override;

    /**
     * @brief set channel number
     */
    bool setChannelNumber(uint8_t k) override;

    /**
    * @brief Directly send packet without delay and without CSMA
    * but keep the message (the caller has to ensure that the message is eventually released)
    * This might lead to an additional memory copy in the platform
    */
    bool sendNow() override;

    /**
     * @brief prepare the next transmission
     */
    bool prepareSendingCopy(IDSMEMessage* msg, Delegate<void(bool)> txEndCallback) override;

    /**
     * @brief prepare the next transmission
     */
    bool prepareSendingCopy(DSMEMessage* msg, Delegate<void(bool)> txEndCallback);

    /**
     * @brief abort an already prepared transmission
     */
    void abortPreparedTransmission() override;

    /**
    * @brief send an ACK message, delay until aTurnaRoundTime after reception_time has expired
    */
    bool sendDelayedAck(IDSMEMessage *ackMsg, IDSMEMessage *receivedMsg,
                        Delegate<void(bool)> txEndCallback) override;
    /**
    * @brief send an ACK message, delay until aTurnaRoundTime after reception_time has expired
    */
    bool sendDelayedAck(DSMEMessage *ackMsg, DSMEMessage *receivedMsg,
                        Delegate<void(bool)> txEndCallback);

    /**
     * @brief set the receive callback
     */
    void setReceiveDelegate(receive_delegate_t receiveDelegate) override;

    /**
     * @brief check whether the ACK layer is busy
     */
    bool isReceptionFromAckLayerPossible() override;

    /**
     * @brief handle reception of frames from ACK Layer
     */
    void handleReceivedMessageFromAckLayer(IDSMEMessage* message) override;

    /**
     * @brief handle reception of frames from ACK Layer
     */
    void handleReceivedMessageFromAckLayer(DSMEMessage* message);

    /**
     * @brief get an empty message
     */
    DSMEMessage* getEmptyMessage() override;

    /**
     * @brief release a message
     */
    void releaseMessage(IDSMEMessage* msg) override;

    /**
     * @brief release a message
     */
    void releaseMessage(DSMEMessage* msg);

    /**
     * @brief start CCA procedure
     */
    bool startCCA() override;

    /**
     * @brief start timer
     */
    void startTimer(uint32_t symbolCounterValue) override;

    /**
     * @brief get elapsed number of symbols since initialization
     */
    uint32_t getSymbolCounter() override;

    /**
     * @brief get a uint16_t random number
     */
    uint16_t getRandom() override {
        return (random_uint32() % UINT16_MAX);
    }

    /**
     * @brief update visual components of openDSME
     * @note to be used in a simulation environment and therefore not used
     * in RIOT
     */
    void updateVisual() override;

    /**
     * @brief callback to offload the start of CFP
     */
    void scheduleStartOfCFP() override;

    /**
     * @brief Get the minimum LQI. Beacons with LQI lower than this will not be
     * considered when deciding for a coordinator to associate to.
     */
    uint8_t getMinCoordinatorLQI() override{
        return CONFIG_IEEE802154_DSME_MIN_COORD_LQI;
    };

    /**
     * @brief turn on transceiver
     */
    void turnTransceiverOn() override;

    /**
     * @brief turn off transceiver
     */
    void turnTransceiverOff() override;

    /**
     * @brief get extended address
     */
    IEEE802154MacAddress& getAddress() {
        return this->mac_pib.macExtendedAddress;
    }

    /**
     * @brief signal finish of ACK'd transmission
     */
    void signalAckedTransmissionResult(bool success, uint8_t transmissionAttempts,
                                       IEEE802154MacAddress receiver) override;

    /**
     * @brief signal a change in GTS status
     */
    void signalGTSChange(bool deallocation, IEEE802154MacAddress counterpart,
                         uint16_t superframeID, uint8_t gtSlotID, uint8_t channel,
                         Direction direction) override;

    /**
     * @brief signal a change in queue length
     */
    void signalQueueLength(uint32_t length) override;

    /**
     * @brief signal the number of packets transmitted during the last CAP
     */
    void signalPacketsPerCAP(uint32_t packets) override;

    /**
     * @brief signal the number of failed packets transmitted during the last CAP
     */
    void signalFailedPacketsPerCAP(uint32_t packets) override;

    /**
     * @brief callback to check where RX is on during CAP
     */
    bool isRxEnabledOnCap() override;

protected:
    /**
     * @brief Copy constructor is not allowed.
     */
    DSMEPlatform(const DSMEPlatform&);
    /**
     * @brief Assignment operator is not allowed.
     */
    DSMEPlatform& operator=(const DSMEPlatform&);

    /**
     * @brief delegate callback for TX end
     */
    Delegate<void(bool)> txEndCallback;

    /**
     * @brief signal creation of new message
     */
    void signalNewMsg(DSMEMessage* msg);

    /**
     * @brief signal release of message
     */
    virtual void signalReleasedMsg(DSMEMessage* msg) {}

    /**
     * @brief dSMEAdaptionLayer wrapper for MCPS Indication callbacks
     */
    void handleDataMessageFromMCPSWrapper(IDSMEMessage* msg);

    /**
     * @brief dSMEAdaptionLayer wrapper for MCPS Indication callbacks
     */
    void handleDataMessageFromMCPS(DSMEMessage* msg);

    /**
     * @brief dSMEAdaptionLayer wrapper for MCPS Confirm callbacks
     */
    void handleConfirmFromMCPSWrapper(IDSMEMessage* msg, DataStatus::Data_Status dataStatus);

    /**
     * @brief dSMEAdaptionLayer wrapper for MCPS Confirm callbacks
     */
    void handleConfirmFromMCPS(DSMEMessage* msg, DataStatus::Data_Status dataStatus);

    /**
     * @brief translate MAC Address representation
     */
    void translateMacAddress(uint16_t& from, IEEE802154MacAddress& to);

    /**
     * @brief get the event queue of the MAC
     */
    event_queue_t *getEventQueue() {
        return &this->netif->evq[GNRC_NETIF_EVQ_INDEX_PRIO_LOW];
    }

    /**
     * @brief holds the PHY Information Base
     */
    PHY_PIB phy_pib;

    /**
     * @brief holds the MAC Information Base
     */
    MAC_PIB mac_pib;

    /**
     * @brief descriptor of the DSME MAC
     */
    DSMELayer dsme;

    /**
     * @brief descriptor of the MCPS Service Access Point
     */
    mcps_sap::MCPS_SAP mcps_sap;

    /**
     * @brief descriptor of the MLME Service Access Point
     */
    mlme_sap::MLME_SAP mlme_sap;

    /**
     * @brief descriptor of the DSME Adaption Layer
     */
    DSMEAdaptionLayer dsmeAdaptionLayer;

    /**
     * @brief whether the MAC is initialized
     */
    bool initialized;

    /**
     * @brief whether there is a scan or sync in progress
     */
    bool scanOrSyncInProgress{false};

    /**
     * @brief whether the association is in progress
     */
    bool associationInProgress{false};

    /**
     * @brief whether the MAC is synchronized
     */
    bool syncActive{false};

    /**
     * @brief whether the MAC keeps the receiver on during CAP
     */
    bool rx_on_cap{true};

    /**
     * @brief delegate callback for passing frames to the ACK layer
     */
    receive_delegate_t receiveFromAckLayerDelegate;

    /**
     * @brief pointer to the GNRC interface
     */
    gnrc_netif_t *netif;

    /**
     * @brief timer used for the MAC
     */
    ztimer_t timer;

    /**
     * @brief used to hold an incoming message before passing it to the MAC
     */
    IDSMEMessage *message;

    /**
     * @brief pointer to the scheduler
     */
    GTSScheduling* scheduling = nullptr;

    /**
     * @brief timestamp (in number of symbols) of the last received preamble
     */
    uint32_t rx_sfd;

    /**
     * @brief timer used for ACK timeout events
     */
    ztimer_t acktimer;

    /**
     * @brief state of the platform layer
     */
    uint8_t state;

    /**
     * @brief whether the MAC expects an ACK frame
     */
    bool wait_for_ack;

    /**
     * @brief whether there is a pending TX frame
     */
    bool pending_tx;

    /**
     * @brief pointer to the IEEE 802.15.4 HAL descriptor
     */
    ieee802154_dev_t *radio;
};

}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* OPENDSME_DSMEPLATFORM_H */
