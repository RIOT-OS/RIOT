#include <stdio.h>
#include <thread.h>
#include <kernel.h>
#include <hwtimer.h>
#include <ps.h>

int integer = 0;
int i = 0;

void second_thread(void) {
    printf("Woke up!\n");
    while(1) {
        integer++;
        printf("sleeper: running. integer=%i, i=%i.\n", integer, i);
        if (integer % 1 == 0) { 
            printf("Going to sleep.\n");
            thread_sleep();
        }
    }
}

char second_thread_stack[KERNEL_CONF_STACKSIZE_DEFAULT*2];

int main(void)
{
    hwtimer_init();

    int pid = thread_create(second_thread_stack, sizeof(second_thread_stack), PRIORITY_MAIN-1, CREATE_STACKTEST | CREATE_SLEEPING | CREATE_WOUT_YIELD, second_thread, "sleeper");

    if (pid < 0) {
        puts("Error creating second_thread! Stopping test.");
        while(1);
    }

    while(1) {
        i++;
        printf("   main: running. integer=%i, i=%i.\n", integer, i);
        if (i % 1 == 0) { 
            thread_print_all();
            printf("Waking up sleeper.\n");
            thread_wakeup(pid);
            thread_print_all();
            thread_yield();
        }
    }
}
