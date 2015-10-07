#include <stdio.h>

#include "unistd.h"
#include "xtimer.h"
#include "thread.h"


#define STACKSIZE_TIMER     (1024)

char stack_timer1[STACKSIZE_TIMER];
char stack_timer2[STACKSIZE_TIMER];

long unsigned int count = 0;

void* timer_func1(void* arg)
{
    while(1)
    {
        xtimer_usleep(1000);
        puts("+");
    }
}

void* timer_func2(void* arg)
{
    while(1)
    {
        xtimer_usleep(1100);
        puts("-");
    }
}

int main(void)
{

    puts("xTimer hangup\n");

    thread_create(stack_timer1,
                  STACKSIZE_TIMER,
                  2,
                  CREATE_STACKTEST,
                  timer_func1,
                  NULL,
                  "timer1");

    thread_create(stack_timer2,
                  STACKSIZE_TIMER,
                  3,
                  CREATE_STACKTEST,
                  timer_func2,
                  NULL,
                  "timer2");

    while(1)
    {
        printf("Ping %lu\n", count);
        count++;
        xtimer_usleep(100 * 1000);
        printf("Pong %lu\n", count);
        count++;
        xtimer_usleep(100 * 1000);
    }
    return 0;
}
