/*
 * Copyright (C) 2016 José Ignacio Alamos
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  José Ignacio Alamos <jialamos@uc.cl>
 */

#include <stdio.h>
#include <assert.h>
#include <platform/radio.h>
#include "ot.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#include "errno.h"
#include "net/ethernet/hdr.h"
#include "net/ethertype.h"
#include "byteorder.h"
#include <string.h>



static RadioPacket sTransmitFrame;
static RadioPacket sReceiveFrame;

static netdev2_t *_dev;

/* asks the driver the current 15.4 channel */
uint16_t get_channel(void)
{
    uint16_t channel;

    _dev->driver->get(_dev, NETOPT_CHANNEL, &channel, sizeof(uint16_t));
    return channel;
}

/* set 15.4 channel */
int set_channel(uint16_t channel)
{
    return _dev->driver->set(_dev, NETOPT_CHANNEL, &channel, sizeof(uint16_t));
}

/*get transmission power from driver */
int16_t get_power(void)
{
    int16_t power;

    _dev->driver->get(_dev, NETOPT_TX_POWER, &power, sizeof(int16_t));
    return power;
}

/* set transmission power */
int set_power(int16_t power)
{
    return _dev->driver->set(_dev, NETOPT_TX_POWER, &power, sizeof(int16_t));
}

/* set IEEE802.15.4 PAN ID */
int set_panid(uint16_t panid)
{
    return _dev->driver->set(_dev, NETOPT_NID, &panid, sizeof(uint16_t));
}

/* set extended HW address */
int set_long_addr(uint8_t *ext_addr)
{
    return _dev->driver->set(_dev, NETOPT_ADDRESS_LONG, ext_addr, IEEE802154_LONG_ADDRESS_LEN);
}

/* set short address */
int set_addr(uint16_t addr)
{
    return _dev->driver->set(_dev, NETOPT_ADDRESS, &addr, sizeof(uint16_t));
}

/* check the state of promiscuous mode */
netopt_enable_t is_promiscuous(void)
{
    netopt_enable_t en;

    _dev->driver->get(_dev, NETOPT_PROMISCUOUSMODE, &en, sizeof(en));
    return en == NETOPT_ENABLE ? true : false;;
}

/* set the state of promiscuous mode */
int set_promiscuous(netopt_enable_t enable)
{
    return _dev->driver->set(_dev, NETOPT_PROMISCUOUSMODE, &enable, sizeof(enable));
}

/* wrapper for getting device state */
int get_state(void)
{
    netopt_state_t en;

    _dev->driver->get(_dev, NETOPT_STATE, &en, sizeof(netopt_state_t));
    return en;
}

/* wrapper for setting device state */
void set_state(netopt_state_t state)
{
    _dev->driver->set(_dev, NETOPT_STATE, &state, sizeof(netopt_state_t));
}

/* checks if the device is off */
bool is_off(void)
{
    return get_state() == NETOPT_STATE_OFF;
}

/* sets device state to OFF */
void ot_off(void)
{
    set_state(NETOPT_STATE_OFF);
}

/* sets device state to SLEEP */
void ot_sleep(void)
{
    set_state(NETOPT_STATE_SLEEP);
}

/* check if device state is IDLE */
bool is_idle(void)
{
    return get_state() == NETOPT_STATE_IDLE;
}

/* set device state to IDLE */
void ot_idle(void)
{
    set_state(NETOPT_STATE_IDLE);
}

/* check if device is receiving a packet */
bool is_rx(void)
{
    return get_state() == NETOPT_STATE_RX;
}

/* turn on packet reception */
void enable_rx(void)
{
    netopt_enable_t enable = true;

    _dev->driver->set(_dev, NETOPT_RX_LISTENING, &enable, sizeof(enable));
}

/* turn off packet reception */
void disable_rx(void)
{
    netopt_enable_t enable = true;

    _dev->driver->set(_dev, NETOPT_RX_LISTENING, &enable, sizeof(enable));
}

/* init framebuffers and initial state */
void openthread_radio_init(netdev2_t *dev, uint8_t *tb, uint8_t *rb)
{
    sTransmitFrame.mPsdu = tb;
    sTransmitFrame.mLength = 0;
    sReceiveFrame.mPsdu = rb;
    sReceiveFrame.mLength = 0;
    _dev = dev;
}

/* Called upon NETDEV2_EVENT_RX_COMPLETE event */
void recv_pkt(netdev2_t *dev)
{
    /* Read frame length from driver */
    int len = dev->driver->recv(dev, NULL, 0, NULL);

    /* Since OpenThread does the synchronization of rx/tx, it's necessary to turn off the receiver now */
    ot_idle();
    disable_rx();

    /* very unlikely */
    if ((len > (unsigned) UINT16_MAX)) {
        otPlatRadioReceiveDone(NULL, kThreadError_Abort);
        return;
    }

    /* Fill OpenThread receive frame */
    sReceiveFrame.mLength = len;
    sReceiveFrame.mPower = get_power();


    /* Read received frame */
    int res = dev->driver->recv(dev, (char *) sReceiveFrame.mPsdu, len, NULL);

    /* Tell OpenThread that receive has finished */
    otPlatRadioReceiveDone(res > 0 ? &sReceiveFrame : NULL, res > 0 ? kThreadError_None : kThreadError_Abort);
}

/* Called upon TX event */
void send_pkt(netdev2_t *dev, netdev2_event_t event)
{
    /* Tell OpenThread transmission is done depending on the NETDEV2 event */
    switch (event) {
        case NETDEV2_EVENT_TX_COMPLETE:
            DEBUG("openthread: NETDEV2_EVENT_TX_COMPLETE\n");
            otPlatRadioTransmitDone(false, kThreadError_None);
            break;
        case NETDEV2_EVENT_TX_COMPLETE_DATA_PENDING:
            DEBUG("openthread: NETDEV2_EVENT_TX_COMPLETE_DATA_PENDING\n");
            otPlatRadioTransmitDone(true, kThreadError_None);
            break;
        case NETDEV2_EVENT_TX_NOACK:
            DEBUG("openthread: NETDEV2_EVENT_TX_NOACK\n");
            otPlatRadioTransmitDone(false, kThreadError_NoAck);
            break;
        case NETDEV2_EVENT_TX_MEDIUM_BUSY:
            DEBUG("openthread: NETDEV2_EVENT_TX_MEDIUM_BUSY\n");
            otPlatRadioTransmitDone(false, kThreadError_ChannelAccessFailure);
            break;
        default:
            break;
    }

    /* Since the transmission is finished, turn off reception */
    disable_rx();
    ot_idle();

}

/* OpenThread will call this for setting PAN ID */
ThreadError otPlatRadioSetPanId(uint16_t panid)
{
    DEBUG("openthread: otPlatRadioSetPanId: setting PAN ID to %04x\n", panid);
    set_panid(((panid & 0xff) << 8) | ((panid >> 8) & 0xff));
    return kThreadError_None;
}

/* OpenThread will call this for setting extended address */
ThreadError otPlatRadioSetExtendedAddress(uint8_t *aExtendedAddress)
{
    DEBUG("openthread: otPlatRadioSetExtendedAddress\n");
    uint8_t reversed_addr[IEEE802154_LONG_ADDRESS_LEN];
    for (int i = 0; i < IEEE802154_LONG_ADDRESS_LEN; i++) {
        reversed_addr[i] = aExtendedAddress[IEEE802154_LONG_ADDRESS_LEN - 1 - i];
    }
    set_long_addr(reversed_addr);
    return kThreadError_None;
}

/* OpenThread will call this for setting short address */
ThreadError otPlatRadioSetShortAddress(uint16_t aShortAddress)
{
    DEBUG("openthread: otPlatRadioSetShortAddress: setting address to %04x\n", aShortAddress);
    set_addr(((aShortAddress & 0xff) << 8) | ((aShortAddress >> 8) & 0xff));
    return kThreadError_None;
}

/* OpenThread will call this for enabling the radio */
ThreadError otPlatRadioEnable(void)
{
    DEBUG("openthread: otPlatRadioEnable\n");
    if (!is_off()) {
        DEBUG("openthread: otPlatRadioEnable: Radio was enabled\n");
        return kThreadError_Busy;
    }

    ot_sleep();
    return kThreadError_None;
}

/* OpenThread will call this for disabling the radio */
ThreadError otPlatRadioDisable(void)
{
    DEBUG("openthread: otPlatRadioDisable\n");
    ot_off();
    return kThreadError_None;
}

/* OpenThread will call this for setting device state to SLEEP */
ThreadError otPlatRadioSleep(void)
{
    DEBUG("openthread: otPlatRadioSleep\n");
    if (!is_idle()) {
        DEBUG("openthread: otPlatRadioSleep: Couldn't sleep\n");
        return kThreadError_Busy;
    }

    ot_sleep();
    return kThreadError_None;
}

/* OpenThread will call this for setting the device state to IDLE */
ThreadError otPlatRadioIdle(void)
{
    DEBUG("openthread: otPlatRadioIdle\n");

    if (is_rx() || is_off()) {
        DEBUG("openthread: OtPlatRadioIdle: Busy\n");
        return kThreadError_Busy;
    }

    /* OpenThread will call this before calling otPlatRadioTransmit.
     * If a packet is received between this function and otPlatRadioTransmit OpenThread will fail! */
    disable_rx();
    ot_idle();

    return kThreadError_None;
}

/*OpenThread will call this for waiting the reception of a packet */
ThreadError otPlatRadioReceive(uint8_t aChannel)
{
    DEBUG("openthread: otPlatRadioReceive\n");
    if (!is_idle()) {
        DEBUG("openthread: otPlatRadioReceive: Device not ready\n");
        return kThreadError_Busy;
    }

    set_channel(aChannel);
    sReceiveFrame.mChannel = aChannel;

    /* enable the reception of packets */
    enable_rx();

    return kThreadError_None;
}


/* OpenThread will call this function to get the transmit buffer */
RadioPacket *otPlatRadioGetTransmitBuffer(void)
{
    DEBUG("openthread: otPlatRadioGetTransmitBuffer\n");
    return &sTransmitFrame;
}

/* OpenThread will call this for transmitting a packet*/
ThreadError otPlatRadioTransmit(void)
{
    DEBUG("openthread: otPlatRadioTransmit\n");

    if (!is_idle()) {
        DEBUG("openthread: otPlatRadioTransmit: Device not ready.\n");

        /* OpenThread will assert(false) if this function returns kThreadError_None.
         * These asserts don't throw core_panic, so it's better to assert here.*/
        assert(false);
        return kThreadError_Busy;
    }

    struct iovec pkt;

    /* Populate iovec with transmit data */
    pkt.iov_base = sTransmitFrame.mPsdu;
    pkt.iov_len = sTransmitFrame.mLength;

    /*Set channel and power based on transmit frame */
    set_channel(sTransmitFrame.mChannel);
    set_power(sTransmitFrame.mPower);

    /* send packet though netdev2 */
    _dev->driver->send(_dev, &pkt, 1);

    /* need for waiting ACK */
    enable_rx();

    return kThreadError_None;
}


/* OpenThread will call this for getting the Noise Floor */
int8_t otPlatRadioGetNoiseFloor(void)
{
    DEBUG("openthread: otPlatRadioGetNoiseFloor\n");
    return 0;
}

/* OpenThread will call this for getting the radio caps */
otRadioCaps otPlatRadioGetCaps(void)
{
    DEBUG("openthread: otPlatRadioGetCaps\n");
    /* all drivers should handle ACK, including call of NETDEV2_EVENT_TX_NOACK */
    return kRadioCapsAckTimeout;
}

/* OpenThread will call this for getting the state of promiscuous mode */
bool otPlatRadioGetPromiscuous(void)
{
    DEBUG("openthread: otPlatRadioGetPromiscuous\n");
    return is_promiscuous();
}

/* OpenThread will call this for setting the state of promiscuous mode */
void otPlatRadioSetPromiscuous(bool aEnable)
{
    DEBUG("openthread: otPlatRadioSetPromiscuous\n");
    set_promiscuous((aEnable) ? NETOPT_ENABLE : NETOPT_DISABLE);
}

/** @} */
