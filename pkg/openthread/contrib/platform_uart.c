#include <stdint.h>
#include <stdio.h>

#include <platform/uart.h>

/* OpenThread will call this for enabling UART (required for OpenThread's CLI)*/
ThreadError otPlatUartEnable(void)
{
	return kThreadError_None;
}

/* OpenThread will call this for disabling UART */
ThreadError otPlatUartDisable(void)
{
	return kThreadError_None;
}

/* OpenThread will call this for sending data through UART */
ThreadError otPlatUartSend(const uint8_t *aBuf, uint16_t aBufLength)
{
	/* print UART data on screen */
	printf("%.*s", aBufLength, aBuf);

	/* Tell OpenThread the sending of UART is done */
	otPlatUartSendDone();

	return kThreadError_None;
}

