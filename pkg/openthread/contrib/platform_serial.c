#include <stdint.h>
#include <stdio.h>

#include <platform/serial.h>

bool writing;
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
	printf("%.*s", aBufLength, aBuf);
	otPlatSerialSendDone();
	return kThreadError_None;
}


void PlatformSerialInit(void)
{
}
void PlatformSerialProcess(void)
{
}
