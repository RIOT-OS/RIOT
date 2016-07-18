#include <stdint.h>
#include <stdio.h>

#include <platform/uart.h>

ThreadError otPlatUartEnable(void)
{
	return kThreadError_None;
}

ThreadError otPlatUartDisable(void)
{
	return kThreadError_None;
}

ThreadError otPlatUartSend(const uint8_t *aBuf, uint16_t aBufLength)
{
	printf("%.*s", aBufLength, aBuf);
	otPlatUartSendDone();
	return kThreadError_None;
}

