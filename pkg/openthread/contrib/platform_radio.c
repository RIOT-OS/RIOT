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

int get_state(void)
{
	netopt_state_t en;
	_dev->driver->get(_dev, NETOPT_STATE, &en, sizeof(netopt_state_t));
	return en;
}

void set_state(netopt_state_t state)
{
	_dev->driver->set(_dev, NETOPT_STATE, &state, sizeof(netopt_state_t));
}

bool is_off(void)
{
	return get_state() == NETOPT_STATE_OFF;
}

void off(void)
{
	set_state(NETOPT_STATE_OFF);
}

bool dev_is_sleep(void)
{
	return get_state() == NETOPT_STATE_SLEEP;
}

void sleep(void)
{
	set_state(NETOPT_STATE_SLEEP);
}

bool dev_is_idle(void)
{
	return get_state() == NETOPT_STATE_IDLE_NO_RX;
}

void idle(void)
{
	set_state(NETOPT_STATE_IDLE_NO_RX);
}

bool dev_is_listening(void)
{
	return get_state() == NETOPT_STATE_IDLE;
}

void listen(void)
{
	set_state(NETOPT_STATE_IDLE);
}

bool is_rx(void)
{
	return get_state() == NETOPT_STATE_RX;
}

void rx(void)
{
	set_state(NETOPT_STATE_RX);
}

bool is_tx(void)
{
	return get_state() == NETOPT_STATE_TX;
}

void tx(void)
{
	set_state(NETOPT_STATE_TX);
}

bool dev_is_tx_or_rx(void)
{
	int res = get_state();
	return res == NETOPT_STATE_RX || res == NETOPT_STATE_TX;
}

void openthread_radio_init(netdev2_t *dev, uint8_t *tb, uint8_t *rb)
{
	sTransmitFrame.mPsdu = tb;
	sTransmitFrame.mLength = 0;
	sReceiveFrame.mPsdu = rb;
	sReceiveFrame.mLength = 0;
	_dev = dev;
}

void recv_pkt(netdev2_t *dev)
{
	/* Read data from driver */
	int len = dev->driver->recv(dev, NULL, 0, NULL);
	assert(len <= (unsigned) UINT16_MAX);
	int res = dev->driver->recv(dev, (char*) sReceiveFrame.mPsdu, len, NULL);

	/* Fill OT receive frame */
	sReceiveFrame.mLength = len;
	sReceiveFrame.mPower = get_power();

	/* Turn off rx */
	idle();

	/* Tell OpenThread that receive has finished */
	otPlatRadioReceiveDone(res > 0 ? &sReceiveFrame : NULL, kThreadError_None);
}

void send_pkt(netdev2_t *dev, netdev2_event_t event)
{
	/* Turn off rx */
	idle();

	switch(event)
	{
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
	}
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
	if(!is_off())
	{
		DEBUG("openthread: otPlatRadioEnable: Radio was enabled\n");
		return kThreadError_Busy;
	}

	sleep();
	return kThreadError_None;
}

ThreadError otPlatRadioDisable(void)
{
	DEBUG("openthread: otPlatRadioDisable\n");
	off();
	return kThreadError_None;
}

ThreadError otPlatRadioSleep(void)
{
	DEBUG("openthread: otPlatRadioSleep\n");
	if(!dev_is_idle() || !dev_is_listening())
	{
		DEBUG("openthread: otPlatRadioSleep: Couldn't sleep\n");
		return kThreadError_Busy;
	}

	sleep();
	return kThreadError_None;
}

ThreadError otPlatRadioIdle(void)
{
	DEBUG("openthread: otPlatRadioIdle\n");
	
	if(dev_is_idle())
	{
		DEBUG("openthread: device was idle\n");
		return kThreadError_None;
	}

	if(is_rx() || is_off())
	{
		DEBUG("openthread: OtPlatRadioIdle: Busy\n");
		return kThreadError_Busy;
	}

	sleep();
	idle();

	return kThreadError_None;
}

ThreadError otPlatRadioReceive(uint8_t aChannel)
{
	DEBUG("openthread: otPlatRadioReceive\n");
	if(!dev_is_idle())
	{
		DEBUG("openthread: otPlatRadioReceive: Device not ready\n");
		return kThreadError_Busy;
	}

	set_channel(aChannel);
	sReceiveFrame.mChannel = aChannel;
	/*turn on rx*/
	listen();

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

	if(!dev_is_idle())
	{
		DEBUG("openthread: otPlatRadioTransmit: Device not ready. (Assert triggered in OpenThread)\n");
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

