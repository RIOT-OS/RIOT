#include "thread.h"
#include <stdio.h>


char rcv_thread_stack[THREAD_STACKSIZE_MAIN];


void *rcv_thread(void *arg)
{
	msg_t m;
	while(1) {
		msg_receive(&m);
		printf("got msg from %"PRIkernel_pid "\n", m.sender_pid);
	}
	return NULL;
}

int main(void)
{
	thread_create(rcv_thread_stack, sizeof(rcv_thread_stack),
			THREAD_PRIORITY_MAIN -1, THREAD_CREATE_STACKTEST,
			rcv_thread, NULL, "rcv_thread");
	return 0;
}
