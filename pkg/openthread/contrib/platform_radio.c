#include <platform/radio.h>
#include <stdio.h>

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

ThreadError otPlatRadioReceive(uint8_t aChannel)
{
	return kThreadError_None;
}


RadioPacket *otPlatRadioGetTransmitBuffer(void)
{
	return NULL;
}

ThreadError otPlatRadioTransmit(void)
{
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
	return false;
}

void otPlatRadioSetPromiscuous(bool aEnable)
{
}

void PlatformRadioInit(void)
{
}

int PlatformRadioProcess(void)
{
	return 0;
}
