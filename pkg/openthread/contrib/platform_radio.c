#include <stdio.h>
#include <assert.h>
#include <platform/radio.h>
#include "ot.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#include "errno.h"


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

int get_state(void)
{
	netopt_state_t en;
	int res =_dev->driver->get(_dev, NETOPT_STATE, &en, sizeof(netopt_state_t));
	if(res == -ENOTSUP)
		return -1;
	return en;
}

bool dev_is_off(void)
{
	int res = get_state();
	return res == NETOPT_STATE_OFF;
}
bool dev_is_sleep(void)
{
	int res = get_state();
	return res == NETOPT_STATE_SLEEP;
}

bool dev_is_ready(void)
{
	int res = get_state();
	return res < 0 || res == NETOPT_STATE_IDLE;
}

bool dev_is_tx_or_rx(void)
{
	int res = get_state();
	return res == NETOPT_STATE_RX || res == NETOPT_STATE_TX;
}

void radio_init(uint8_t *tb)
{
	sTransmitFrame.mPsdu = tb;
	sTransmitFrame.mLength = 0;
}
ThreadError otPlatRadioSetPanId(uint16_t panid)
{
	DEBUG("openthread: otPlatRadioSetPanId\n");
	(void) panid;
	return kThreadError_None;
}

ThreadError otPlatRadioSetExtendedAddress(uint8_t *aExtendedAddress)
{
	DEBUG("openthread: otPlatRadioSetExtendedAddress\n");
	(void) aExtendedAddress;
	return kThreadError_None;
}

ThreadError otPlatRadioSetShortAddress(uint16_t aShortAddress)
{
	DEBUG("openthread: otPlatRadioSetShortAddress\n");
	(void) aShortAddress;
	return kThreadError_None;
}

ThreadError otPlatRadioEnable(void)
{
	DEBUG("openthread: otPlatRadioEnable\n");
	if(dev_is_off())
	{
		DEBUG("openthread: otPlatRadioEnable: Radio was enabled\n");
		return kThreadError_Busy;
	}

	netopt_state_t st = NETOPT_STATE_IDLE;
	_dev->driver->set(_dev, NETOPT_STATE, &st, sizeof(netopt_state_t));
	return kThreadError_None;
}

ThreadError otPlatRadioDisable(void)
{
	DEBUG("openthread: otPlatRadioDisable\n");
	netopt_state_t st = NETOPT_STATE_OFF;
	_dev->driver->set(_dev, NETOPT_STATE, &st, sizeof(netopt_state_t));
	return kThreadError_None;
}

ThreadError otPlatRadioSleep(void)
{
	DEBUG("openthread: otPlatRadioSleep\n");
	if(dev_is_ready())
		return kThreadError_Busy;

	netopt_state_t st = NETOPT_STATE_SLEEP;
	_dev->driver->set(_dev, NETOPT_STATE, &st, sizeof(netopt_state_t));
	return kThreadError_None;
}

ThreadError otPlatRadioIdle(void)
{
	DEBUG("openthread: otPlatRadioIdle\n");
	
	if(dev_is_tx_or_rx())
		return kThreadError_Busy;

	netopt_state_t st = NETOPT_STATE_IDLE;
	_dev->driver->set(_dev, NETOPT_STATE, &st, sizeof(netopt_state_t));

	return kThreadError_None;
}

ThreadError otPlatRadioReceive(uint8_t aChannel)
{
	DEBUG("openthread: otPlatRadioReceive\n");
	if(!dev_is_ready())
		return kThreadError_Busy;

	uint16_t channel = aChannel;
	_dev->driver->set(_dev, NETOPT_CHANNEL, &channel, sizeof(uint16_t));
	return kThreadError_None;
}


RadioPacket *otPlatRadioGetTransmitBuffer(void)
{
	DEBUG("openthread: otPlatRadioGetTransmitBuffer\n");
	return &sTransmitFrame;
}

ThreadError otPlatRadioTransmit(void)
{
	DEBUG("openthread: otPlatRadioTransmit\n");

	if(!dev_is_ready())
	{
		DEBUG("I'm exiting\n");
		return kThreadError_Busy;
	}

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
	DEBUG("openthread: otPlatRadioGetNoiseFloor\n");
	return 0;
}

otRadioCaps otPlatRadioGetCaps(void)
{
	DEBUG("openthread: otPlatRadioGetCaps\n");
	return kRadioCapsNone;
}

bool otPlatRadioGetPromiscuous(void)
{
	DEBUG("openthread: otPlatRadioGetPromiscuous\n");
	netopt_enable_t en;
	_dev->driver->get(_dev, NETOPT_PROMISCUOUSMODE, &en, sizeof(en));
	return en == NETOPT_ENABLE ? true : false;
}

void otPlatRadioSetPromiscuous(bool aEnable)
{
	DEBUG("openthread: otPlatRadioSetPromiscuous\n");
	netopt_enable_t en = (aEnable) ? NETOPT_ENABLE : NETOPT_DISABLE;
	_dev->driver->set(_dev, NETOPT_PROMISCUOUSMODE, &en, sizeof(en));
}

