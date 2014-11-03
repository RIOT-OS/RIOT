#include <stdio.h>
#include "thread.h"

char snd_thread_stack[KERNEL_CONF_STACKSIZE_MAIN];

void *snd_thread(void *unused)
{
    (void) unused;
    puts("snd_thread running");
    return NULL;
}

int main(void)
{
    puts("Starting Sched testing");
    thread_create(snd_thread_stack, sizeof(snd_thread_stack), PRIORITY_MAIN,
                  CREATE_WOUT_YIELD, snd_thread, NULL, "snd");

    puts("yield 1");
    thread_yield();
    puts("yield 2");
    thread_yield();
    puts("terminating main thread");

    return 0;
}
