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

uint16_t get_channel(void)
{
	uint16_t channel;
	_dev->driver->get(_dev, NETOPT_CHANNEL, &channel, sizeof(uint16_t));
	return channel;
}

int set_channel(uint16_t channel)
{
	return _dev->driver->set(_dev, NETOPT_CHANNEL, &channel, sizeof(uint16_t));
}

int16_t get_power(void)
{
	int16_t power;
	_dev->driver->get(_dev, NETOPT_TX_POWER, &power, sizeof(int16_t));
	return power;
}

int set_power(int16_t power)
{
	return _dev->driver->set(_dev, NETOPT_TX_POWER, &power, sizeof(int16_t));
}

int set_panid(uint16_t panid)
{
	return _dev->driver->set(_dev, NETOPT_NID, &panid, sizeof(uint16_t));	
}

int set_long_addr(uint8_t *ext_addr)
{
	return _dev->driver->set(_dev, NETOPT_ADDRESS_LONG, ext_addr, IEEE802154_LONG_ADDRESS_LEN);	
}

int set_addr(uint16_t addr)
{
	return _dev->driver->set(_dev, NETOPT_ADDRESS, &addr, sizeof(uint16_t));	
}

netopt_enable_t is_promiscuous(void)
{
	netopt_enable_t en;
	_dev->driver->get(_dev, NETOPT_PROMISCUOUSMODE, &en, sizeof(en));
	return en == NETOPT_ENABLE ? true : false;;
}

int set_promiscuous(netopt_enable_t enable)
{
	return _dev->driver->set(_dev, NETOPT_PROMISCUOUSMODE, &enable, sizeof(enable));
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

	//get_channel(dev);
	sReceiveFrame.mPower = get_power();

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

void set_state(netopt_state_t state)
{
	_dev->driver->set(_dev, NETOPT_STATE, &state, sizeof(netopt_state_t));
}

bool dev_is_off(void)
{
	return get_state() == NETOPT_STATE_OFF;
}
bool dev_is_sleep(void)
{
	return get_state() == NETOPT_STATE_SLEEP;
}

bool dev_is_ready(void)
{
	return get_state() == NETOPT_STATE_IDLE;
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
	set_panid(((panid & 0xff) << 8) | ((panid >> 8) & 0xff));
	return kThreadError_None;
}

ThreadError otPlatRadioSetExtendedAddress(uint8_t *aExtendedAddress)
{
	DEBUG("openthread: otPlatRadioSetExtendedAddress\n");

	uint8_t reversed_addr[IEEE802154_LONG_ADDRESS_LEN];
	for(int i=0;i<IEEE802154_LONG_ADDRESS_LEN;i++)
	{
		reversed_addr[i] = aExtendedAddress[IEEE802154_LONG_ADDRESS_LEN-1-i];
		DEBUG("%02x ", reversed_addr[i]);
	}
	DEBUG("\n");
	set_long_addr(reversed_addr);
	return kThreadError_None;
}

ThreadError otPlatRadioSetShortAddress(uint16_t aShortAddress)
{
	DEBUG("openthread: otPlatRadioSetShortAddress: setting address to %04x\n", aShortAddress);

	set_addr(((aShortAddress & 0xff) << 8) | ((aShortAddress >> 8) & 0xff));
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

	set_state(NETOPT_STATE_SLEEP);
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

	set_state(NETOPT_STATE_IDLE);

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
	set_channel(sTransmitFrame.mChannel);
	set_power(sTransmitFrame.mPower);

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
	return is_promiscuous();
}

void otPlatRadioSetPromiscuous(bool aEnable)
{
	DEBUG("openthread: otPlatRadioSetPromiscuous\n");
	 set_promiscuous((aEnable) ? NETOPT_ENABLE : NETOPT_DISABLE);
}

