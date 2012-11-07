#include <stdio.h>
#include <thread.h>
#include <flags.h>
#include <kernel.h>

#define STACK_SIZE  (8192)

char t2_stack[STACK_SIZE];

void second_thread(void) {
    puts("second thread\n");
}

int main(void)
{
    (void) thread_create(t2_stack, STACK_SIZE, PRIORITY_MAIN-1, CREATE_WOUT_YIELD | CREATE_STACKTEST, second_thread, "nr2");
    puts("first thread\n");
}
