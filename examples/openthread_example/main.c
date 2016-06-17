#include "msg.h"
#include <stdio.h>
#include <openthread.h>
#include <cli/cli-serial.h>
#include "platform.h"
#include "platform/alarm.h"
#include "ot.h"

#define OPENTHREAD_QUEUE_LEN      (8)
//static msg_t _queue[OPENTHREAD_QUEUE_LEN];
//static uint8_t buf[100];

int main(void)
{
    puts("Hello World!");

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

    PlatformInit();
    printf("Platform Init completed\n");
    otInit();
	puts("Hello");
	//msg_init_queue(_queue, OPENTHREAD_QUEUE_LEN);
	//openthread_init();
	//uint8_t *b = buf;
	while(1)
	{
		/*msg_t msg;
		msg_receive(&msg);
		switch(msg.type)
		{
			case OPENTHREAD_XTIMER_MSG_TYPE_EVENT:
				otPlatAlarmFired();
				break;
			case OPENTHREAD_NETDEV2_MSG_TYPE_EVENT:
				break;
		}*/
		//printf("%i\n", getchar());
	}

    return 0;
}
