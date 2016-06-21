#include <stdio.h>
#include <assert.h>
#include <platform/radio.h>
#include "ot.h"

static RadioPacket sTransmitFrame;
static RadioPacket sReceiveFrame;

static netdev2_t *_dev;

void set_netdev(netdev2_t *dev)
{
	_dev = dev;
}

void recv_pkt(netdev2_t *dev, uint8_t *buf)
{
	int len = dev->driver->recv(dev, (char*) buf, sizeof(buf), NULL);

	assert(((unsigned)len) <= UINT16_MAX);
	sReceiveFrame.mPsdu = buf;
	sReceiveFrame.mLength = len;

	otPlatRadioReceiveDone(&sReceiveFrame, kThreadError_None);
}

void send_pkt(netdev2_t *dev)
{
	netopt_enable_t en;
	_dev->driver->get(dev, NETOPT_ACK_REQ, &en, sizeof(netopt_enable_t));
	
	otPlatRadioTransmitDone(en == NETOPT_ENABLE ? true : false, kThreadError_None);
}

ThreadError otPlatRadioSetPanId(uint16_t panid)
{
	(void) panid;
	return kThreadError_None;
}

ThreadError otPlatRadioSetExtendedAddress(uint8_t *aExtendedAddress)
{
	(void) aExtendedAddress;
	return kThreadError_None;
}

ThreadError otPlatRadioSetShortAddress(uint16_t aShortAddress)
{
	(void) aShortAddress;
	return kThreadError_None;
}

ThreadError otPlatRadioEnable(void)
{
	netopt_state_t en;
	_dev->driver->get(_dev, NETOPT_STATE, &en, sizeof(netopt_state_t));
	if(en != NETOPT_STATE_OFF)
		return kThreadError_Busy;

	netopt_state_t st = NETOPT_STATE_IDLE;
	_dev->driver->set(_dev, NETOPT_STATE, &st, sizeof(netopt_state_t));
	return kThreadError_None;
}

ThreadError otPlatRadioDisable(void)
{
	netopt_state_t st = NETOPT_STATE_OFF;
	_dev->driver->set(_dev, NETOPT_STATE, &st, sizeof(netopt_state_t));
	return kThreadError_None;
}

ThreadError otPlatRadioSleep(void)
{
	netopt_state_t en;
	_dev->driver->get(_dev, NETOPT_STATE, &en, sizeof(netopt_state_t));
	if(en != NETOPT_STATE_IDLE)
		return kThreadError_Busy;

	netopt_state_t st = NETOPT_STATE_SLEEP;
	_dev->driver->set(_dev, NETOPT_STATE, &st, sizeof(netopt_state_t));
	return kThreadError_None;
}

ThreadError otPlatRadioIdle(void)
{
	netopt_state_t en;
	_dev->driver->get(_dev, NETOPT_STATE, &en, sizeof(netopt_state_t));
	
	if(en == NETOPT_STATE_RX || en == NETOPT_STATE_TX)
		return kThreadError_Busy;

	netopt_state_t st = NETOPT_STATE_IDLE;
	_dev->driver->set(_dev, NETOPT_STATE, &st, sizeof(netopt_state_t));

	return kThreadError_None;
}

ThreadError otPlatRadioReceive(uint8_t aChannel)
{
	netopt_state_t en;
	_dev->driver->get(_dev, NETOPT_STATE, &en, sizeof(netopt_state_t));
	if(en != NETOPT_STATE_IDLE)
		return kThreadError_Busy;

	uint16_t channel = aChannel;
	_dev->driver->set(_dev, NETOPT_CHANNEL, &channel, sizeof(uint16_t));
	return kThreadError_None;
}


RadioPacket *otPlatRadioGetTransmitBuffer(void)
{
	return &sTransmitFrame;
}

ThreadError otPlatRadioTransmit(void)
{
	netopt_state_t en;
	_dev->driver->get(_dev, NETOPT_STATE, &en, sizeof(netopt_state_t));
	if(en != NETOPT_STATE_IDLE)
		return kThreadError_Busy;

	struct iovec pkt;
	pkt.iov_base = sTransmitFrame.mPsdu;
	pkt.iov_len = sTransmitFrame.mLength;

	/*Set channel and power based on transmit frame */
	uint16_t channel = sTransmitFrame.mChannel;
	int16_t mPower = sTransmitFrame.mPower;
	_dev->driver->set(_dev, NETOPT_CHANNEL, &channel, sizeof(uint16_t));
	_dev->driver->set(_dev, NETOPT_TX_POWER, &mPower, sizeof(int16_t));

	/*transmit frame*/	
	_dev->driver->send(_dev, &pkt, 1);

	return kThreadError_None;
}


int8_t otPlatRadioGetNoiseFloor(void)
{
	return 0;
}

otRadioCaps otPlatRadioGetCaps(void)
{
	return kRadioCapsNone;
}

bool otPlatRadioGetPromiscuous(void)
{
	netopt_enable_t en;
	_dev->driver->get(_dev, NETOPT_PROMISCUOUSMODE, &en, sizeof(en));
	return en == NETOPT_ENABLE ? true : false;
}

void otPlatRadioSetPromiscuous(bool aEnable)
{
	netopt_enable_t en = (aEnable) ? NETOPT_ENABLE : NETOPT_DISABLE;
	_dev->driver->set(_dev, NETOPT_PROMISCUOUSMODE, &en, sizeof(en));
}

