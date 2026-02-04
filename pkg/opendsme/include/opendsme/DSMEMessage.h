/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    pkg_opendsme_dsmemessage DSME Message interface implementation.
 * @ingroup     pkg_opendsme
 * @brief       Implementation of the DSME Message interface for GNRC.
 *
 * @{
 *
 * @file
 * @brief   DSME Message interface implementation for GNRC.
 *
 * @author  José I. Álamos <jose.alamos@haw-hamburg.de>
 */

#include <stdint.h>
#include <string.h>

#include "dsmeLayer/messages/IEEE802154eMACHeader.h"
#include "interfaces/IDSMEMessage.h"
#include "iolist.h"
#include "mac_services/dataStructures/DSMEMessageElement.h"
#include "net/gnrc/pktbuf.h"
#include "opendsme/dsme_settings.h"
#include "opendsme/dsme_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace dsme {

/**
 * @brief Forward declaration of DSMEPlatform class
 */
class DSMEPlatform;

/**
 * @brief DSME Message interface implementation for GNRC
 */
class DSMEMessage : public IDSMEMessage {
public:
    /************* IDSMEMessage implementation *************/

    /**
     * @brief prepend a header to current message
     */
    void prependFrom(DSMEMessageElement *msg) override;

    /**
     * @brief decapsulate header to a message
     */
    void decapsulateTo(DSMEMessageElement *msg) override;

    /**
     * @brief copy payload to DSME Message Element
     */
    void copyTo(DSMEMessageElement *msg);

    /**
     * @brief check whether the message has payload
     */
    bool hasPayload() override
    {
        return this->pkt != NULL && this->pkt->size > 0;
    }

    /**
     * @brief get the symbol counter at the end of the SFD
     */
    uint32_t getStartOfFrameDelimiterSymbolCounter() override
    {
        return startOfFrameDelimiterSymbolCounter;
    }

    /**
     * @brief set the symbol counter at the end of the SFD
     */
    void setStartOfFrameDelimiterSymbolCounter(uint32_t symbolCounter) override
    {
        startOfFrameDelimiterSymbolCounter = symbolCounter;
    }

    /**
     * @brief get the total number of symbols in current frame
     */
    uint16_t getTotalSymbols() override
    {
        DSME_ASSERT(pkt);
        /* Hardcoded to O-QPSK */
        uint16_t bytes = macHdr.getSerializationLength()
                         + pkt->size
                         + 2        /* FCS */
                         + 4        /* Preamble */
                         + 1        /* SFD */
                         + 1;       /* PHY Header */

        return bytes * 2;           /* 4 bit per symbol */
    }

    /**
     * @brief get number of MPDU Symbols
     * @note not used by openDSME
     */
    uint8_t getMPDUSymbols() override;

    /**
     * @brief get IEEE 802.15.4 header
     */
    IEEE802154eMACHeader& getHeader() override
    {
        return macHdr;
    }

    /**
     * @brief get LQI of the message
     */
    uint8_t getLQI() override
    {
        return messageLQI;
    }

    /**
     * @brief check whether the message was received via MCPS
     */
    bool getReceivedViaMCPS() override
    {
        return receivedViaMCPS;
    }

    /**
     * @brief indicated that message was received via MCPS
     */
    void setReceivedViaMCPS(bool receivedViaMCPS) override
    {
        this->receivedViaMCPS = receivedViaMCPS;
    }

    /**
     * @brief whether the message is being sent
     */
    bool getCurrentlySending() override
    {
        return currentlySending;
    }

    /**
     * @brief indicate that the message is being sent
     */
    void setCurrentlySending(bool currentlySending) override
    {
        this->currentlySending = currentlySending;
    }

    /**
     * @brief increase retry counter for current message
     */
    void increaseRetryCounter() override
    {
        this->retryCounter++;
    }

    /**
     * @brief get retry counter
     */
    uint8_t getRetryCounter() override
    {
        return this->retryCounter;
    }

    /**
     * @brief get payload length
     */
    uint8_t getPayloadLength()
    {
        DSME_ASSERT(pkt);
        return pkt->size;
    }

    /**
     * @brief get RSSI of frame
     */
    int8_t getRSSI() override
    {
        return this->messageRSSI;
    }

    /**
     * @brief preallocate buffer with a given length
     */
    int loadBuffer(size_t len);

    /**
     * @brief load a GNRC packet into the internal openDSME message representation
     */
    int loadBuffer(iolist_t *pkt);

    /**
     * @brief get buffer associated to the current payload
     */
    uint8_t *getPayload()
    {
        DSME_ASSERT(pkt);
        return (uint8_t *)pkt->data;
    }

    /**
     * @brief drop a number of bytes from the header
     */
    int dropHdr(size_t len);

    /**
     * @brief release the message
     */
    void releaseMessage();

    /**
     * @brief dispatch the message to upper layers
     */
    void dispatchMessage();

    /**
     * @brief get the IOLIST representation of the message
     */
    iolist_t *getIolPayload();

    /**
     * @brief clear the message
     */
    void clearMessage();

    /**
     * @brief whether the message is being sent on the first try
     */
    bool firstTry;

    /**
     * @brief whether the message is free
     */
    bool free;
private:
    /**
     * @brief DSMEMessage constructor
     */
    DSMEMessage() :
        messageRSSI(0),
        messageLQI(0),
        receivedViaMCPS(false),
        currentlySending(false),
        retryCounter(0)
    {}

    /**
     * @brief DSMEMessage destructor
     */
    ~DSMEMessage()
    {}

    /**
     * @brief prepare message
     */
    void prepare()
    {
        currentlySending = false;
        firstTry = true;
        receivedViaMCPS = false;
        retryCounter = 0;

        macHdr.reset();
    }

    /**
     * @brief descriptor of the MAC header
     */
    IEEE802154eMACHeader macHdr;

    /**
     * @brief stores the RSSI of the received frame
     */
    uint8_t messageRSSI;

    /**
     * @brief stores the LQI of the received frame
     */
    uint8_t messageLQI;

    /**
     * @brief stores a pointer to the GNRC Pktbuf representation
     */
    gnrc_pktsnip_t *pkt;

    /**
     * @brief stores the timestamp of the preamble
     */
    uint32_t startOfFrameDelimiterSymbolCounter;

    /**
     * @brief stores whether the message was received via MCPS
     */
    bool receivedViaMCPS;

    /**
     * @brief stores whether the frame is currently being sent
     */
    bool currentlySending;

    /**
     * @brief number of retransmission attempts
     */
    uint8_t retryCounter;

    /**
     * @brief pointer to the GNRC network interface
     */
    gnrc_netif_t *netif;

/* declare related classes as friends */
friend class DSMEPlatform;
friend class DSMEMessageElement;
friend class DSMEPlatformBase;
friend class DSMEMessageBuffer;
};

}

#ifdef __cplusplus
}
#endif

/** @} */
