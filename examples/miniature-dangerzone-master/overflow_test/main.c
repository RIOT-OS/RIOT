#include <stdio.h>
#include "thread.h"

#define STACKSIZE   (52)

char stack[STACKSIZE];

void my_thread(void)
{
    while(1);
}

int main(void)
{
    extern void profiling_init(void);
    profiling_init();
    puts("overflow_tester");

    thread_create(stack, STACKSIZE, PRIORITY_MAIN-1, CREATE_STACKTEST, my_thread, "test");
    while(1);
    return 0;
}
