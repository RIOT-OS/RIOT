#include <stdio.h>
#include <thread.h>
#include <msg.h>
#include <kernel.h>

void second_thread(void) {
    printf("second_thread starting.\n");
    msg_t m;
    
    while(1) {
        msg_receive(&m);
//        printf("2nd: got msg from %i\n", m.sender_pid);
        m.content.value++;
        msg_send(&m, m.sender_pid, true);
    }
}

char second_thread_stack[KERNEL_CONF_STACKSIZE_MAIN];

int main(void)
{
    printf("Hello world!\n");

    msg_t m;

    int pid = thread_create(second_thread_stack, sizeof(second_thread_stack), PRIORITY_MAIN-1, CREATE_WOUT_YIELD | CREATE_STACKTEST, second_thread, "pong");
    
    m.content.value = 1;

    while(1) {
                msg_send(&m, pid, true);
                msg_receive(&m);
                printf("Got msg with content %i\n", m.content.value);
    }
}
