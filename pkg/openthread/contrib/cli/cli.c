#include "msg.h"
#include <stdio.h>
#include "ot.h"
#include "xtimer.h"
#include "msg.h"
#include "string.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static char buf[100];

void openthread_cli_run(void)
{
	msg_t msg;
	serial_msg_t sm;
	sm.len = sizeof(buf)+1;
	msg.type = OPENTHREAD_SERIAL_MSG_TYPE_EVENT;
	msg.content.ptr = &sm;

    char *p=buf;
	sm.buf = (uint8_t*) buf;
    sm.len = 0;

	while(1)
	{
        *p = getchar();
        if(*p == '\r' || *p == '\n')
        {
            sm.len = p-buf+1; 
            msg_send(&msg, openthread_get_pid());
            p=buf;
        }
        else
        {
            p++;
        }
	}
}
