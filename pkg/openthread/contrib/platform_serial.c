#include <platform/serial.h>
#include <stdint.h>
#include <stdio.h>

ThreadError otPlatSerialEnable(void)
{
	return kThreadError_None;
}

ThreadError otPlatSerialDisable(void)
{
	return kThreadError_None;
}

ThreadError otPlatSerialSend(const uint8_t *aBuf, uint16_t aBufLength)
{
	return kThreadError_None;
}

void PlatformSerialProcess(void)
{
}
