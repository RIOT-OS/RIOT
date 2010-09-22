#include <stdio.h>
#include <thread.h>

int main(void)
{
    int pid = thread_getpid();
    printf("PID: %i\n", pid);
}
