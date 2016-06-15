#include <platform/radio.h>

ThreadError otPlatRadioSetPanId(uint16_t panid)
{
	return kThreadError_None;
}

ThreadError otPlatRadioSetExtendedAddress(uint8_t *aExtendedAddress)
{
	return kThreadError_None;
}

ThreadError otPlatRadioSetShortAddress(uint16_t aShortAddress)
{
	return kThreadError_None;
}

void otPlatRadioInit(void)
{
}

ThreadError otPlatRadioEnable(void)
{
	return kThreadError_None;
}

ThreadError otPlatRadioDisable(void)
{
	return kThreadError_None;
}

ThreadError otPlatRadioSleep(void)
{
	return kThreadError_None;
}

ThreadError otPlatRadioIdle(void)
{
	return kThreadError_None;
}

ThreadError otPlatRadioReceive(RadioPacket *aPacket)
{
	return kThreadError_None;
}

void otPlatRadioSignalReceiveDone(void)
{
}

ThreadError otPlatRadioHandleReceiveDone(void)
{
	return kThreadError_None;
}

ThreadError otPlatRadioTransmit(RadioPacket *aPacket)
{
	return kThreadError_None;
}

void otPlatRadioSignalTransmitDone(void)
{
}

ThreadError otPlatRadioHandleTransmitDone(bool *aFramePending)
{
	return kThreadError_None;
}

int8_t otPlatRadioGetNoiseFloor()
{
	return 0;
}
