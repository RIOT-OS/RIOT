#include <stdio.h>
#include <thread.h>
#include <flags.h>
#include <kernel.h>

void second_thread(void) {
    puts("second thread\n");
}

int main(void)
{
    int pid = thread_create(8192, PRIORITY_MAIN-1, CREATE_WOUT_YIELD | CREATE_STACKTEST, second_thread, "nr2");
    puts("first thread\n");
}
