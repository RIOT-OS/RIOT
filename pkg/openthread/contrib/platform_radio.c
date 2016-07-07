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

void set_netdev(netdev2_t *dev)
{
	_dev = dev;
}

static inline void _addr_set_broadcast(uint8_t *dst)
{
	memset(dst, 0xff, ETHERNET_ADDR_LEN);
}

void recv_pkt(netdev2_t *dev)
{
	/* Read data from driver */
	int len = dev->driver->recv(dev, NULL, 0, NULL);

	assert(len <= (unsigned) UINT16_MAX);
	int res = dev->driver->recv(dev, (char*) sReceiveFrame.mPsdu, len, NULL);


#if 0
	DEBUG("Len is: %i\n", len);
	DEBUG("After receive: ");
	for(int i=0;i<len;i++)
	{
		DEBUG("%02x ", sReceiveFrame.mPsdu[i]);
	}
	printf("\n");
#endif
	/* Fill OT receive frame */
	sReceiveFrame.mLength = len;

	uint16_t channel;
	int16_t mPower;

	_dev->driver->get(_dev, NETOPT_CHANNEL, &channel, sizeof(uint16_t));
	_dev->driver->get(_dev, NETOPT_TX_POWER, &mPower, sizeof(int16_t));

	sReceiveFrame.mPower = mPower;

	/* Tell OpenThread that receive has finished */
	DEBUG("recv_pkt res: %i\n", res);
	otPlatRadioReceiveDone(res > 0 ? &sReceiveFrame : NULL, kThreadError_None);
}

void send_pkt(netdev2_t *dev)
{
#if 1
	DEBUG("Sent:");
	for(int i=0;i<sTransmitFrame.mLength;i++)
	{
		DEBUG("%02x ", sTransmitFrame.mPsdu[i]);
	}
	DEBUG("\n");
#endif

	/*Tell OpenThread that transmission has finished.*/
	otPlatRadioTransmitDone(false, kThreadError_None);
}

int get_state(void)
{
	netopt_state_t en;
	int res =_dev->driver->get(_dev, NETOPT_STATE, &en, sizeof(netopt_state_t));

	if(res == -ENOTSUP)
	{
		DEBUG("openthread: Get state: No such option\n");
		return -1;
	}

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
	return res == NETOPT_STATE_IDLE;
}

bool dev_is_tx_or_rx(void)
{
	int res = get_state();
	return res == NETOPT_STATE_RX || res == NETOPT_STATE_TX;
}

void radio_init(uint8_t *tb, uint8_t *rb)
{
	sTransmitFrame.mPsdu = tb;
	sTransmitFrame.mLength = 0;
	sReceiveFrame.mPsdu = rb;
	sReceiveFrame.mLength = 0;
}
ThreadError otPlatRadioSetPanId(uint16_t panid)
{
	DEBUG("openthread: otPlatRadioSetPanId: setting PAN ID to %04x\n", panid);
	uint16_t reverse = ((panid & 0xff) << 8) | ((panid >> 8) & 0xff);
	_dev->driver->set(_dev, NETOPT_NID, &reverse, sizeof(uint16_t));	
	return kThreadError_None;
}

ThreadError otPlatRadioSetExtendedAddress(uint8_t *aExtendedAddress)
{
	DEBUG("openthread: otPlatRadioSetExtendedAddress\n");
	uint8_t reverse[IEEE802154_LONG_ADDRESS_LEN];
	for(int i=0;i<IEEE802154_LONG_ADDRESS_LEN;i++)
	{
		reverse[i] = aExtendedAddress[IEEE802154_LONG_ADDRESS_LEN-1-i];
		DEBUG("%02x ", reverse[i]);
	}
	DEBUG("\n");
	(void) aExtendedAddress;
	_dev->driver->set(_dev, NETOPT_ADDRESS_LONG, &reverse, IEEE802154_LONG_ADDRESS_LEN);	
	return kThreadError_None;
}

ThreadError otPlatRadioSetShortAddress(uint16_t aShortAddress)
{
	DEBUG("openthread: otPlatRadioSetShortAddress: setting address to %04x\n", aShortAddress);
	uint16_t reverse = ((aShortAddress & 0xff) << 8) | ((aShortAddress >> 8) & 0xff);
	_dev->driver->set(_dev, NETOPT_ADDRESS, &reverse, sizeof(uint16_t));	
	return kThreadError_None;
}

ThreadError otPlatRadioEnable(void)
{
	DEBUG("openthread: otPlatRadioEnable\n");
	if(!dev_is_off())
	{
		DEBUG("openthread: otPlatRadioEnable: Radio was enabled\n");
		return kThreadError_Busy;
	}

	netopt_state_t st = NETOPT_STATE_SLEEP;
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
	if(!dev_is_ready())
	{
		DEBUG("openthread: otPlatRadioSleep: Couldn't sleep\n");
		return kThreadError_Busy;
	}

	netopt_state_t st = NETOPT_STATE_SLEEP;
	_dev->driver->set(_dev, NETOPT_STATE, &st, sizeof(netopt_state_t));
	return kThreadError_None;
}

ThreadError otPlatRadioIdle(void)
{
	DEBUG("openthread: otPlatRadioIdle\n");
	
	int res = get_state();
	if(res == NETOPT_STATE_IDLE)
		return kThreadError_None;

	if(res == NETOPT_STATE_RX || res == NETOPT_STATE_OFF)
	{
		DEBUG("openthread: OtPlatRadioIdle: Busy\n");
		return kThreadError_Busy;
	}

	netopt_state_t st = NETOPT_STATE_IDLE;
	_dev->driver->set(_dev, NETOPT_STATE, &st, sizeof(netopt_state_t));

	return kThreadError_None;
}

ThreadError otPlatRadioReceive(uint8_t aChannel)
{
	DEBUG("openthread: otPlatRadioReceive\n");
	if(!dev_is_ready())
	{
		DEBUG("openthread: otPlatRadioReceive: Device not ready\n");
		return kThreadError_Busy;
	}

	sReceiveFrame.mChannel = aChannel;
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
		DEBUG("openthread: otPlatRadioTransmit: Device not ready\n");
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
	DEBUG("otPlatRadioGetPromiscuous: %i\n", en);
	return en == NETOPT_ENABLE ? true : false;
}

void otPlatRadioSetPromiscuous(bool aEnable)
{
	DEBUG("openthread: otPlatRadioSetPromiscuous\n");
	netopt_enable_t en = (aEnable) ? NETOPT_ENABLE : NETOPT_DISABLE;
	_dev->driver->set(_dev, NETOPT_PROMISCUOUSMODE, &en, sizeof(en));
}

