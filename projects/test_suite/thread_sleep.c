#include <stdio.h>
#include <thread.h>
#include <kernel.h>

static int integer = 0;
static int i = 0;

static void second_thread(void) {
    while(1) {
        integer++;
        printf("sleeper: running. integer=%i, i=%i.\n", integer, i);
        if (integer % 2 == 0) { 
            printf("Going to sleep.\n");
            thread_sleep();
            printf("Woke up!\n");
        }
    }
}

static char second_thread_stack[KERNEL_CONF_STACKSIZE_DEFAULT*2];
static tcb second_thread_tcb;

void test_thread_sleep(char* line)
{
    int pid = thread_create(&second_thread_tcb, second_thread_stack, sizeof(second_thread_stack), PRIORITY_MAIN-1, CREATE_STACKTEST | CREATE_SLEEPING | CREATE_WOUT_YIELD, second_thread, "sleeper");

    if (pid < 0) {
        puts("Error creating second_thread! Stopping test.");
        while(1);
    }

    while(i < 10) {
        i++;
        printf("   main: running. integer=%i, i=%i.\n", integer, i);
        if (i % 2 == 0) { 
            printf("Waking up sleeper.\n");
            thread_wakeup(pid);
        }
    }
}
