#include <stdio.h>
#include <thread.h>
#include <msg.h>
#include <kernel.h>

void second_thread(void) {
    printf("second_thread starting.\n");
    msg m;
    int i = 1;
    while(1) {
        msg_receive(&m);
//        printf("2nd: got msg from %i\n", m.sender_pid);
        m.content.value++;
        msg_reply(&m, &m);
    }
}

int main(void)
{
    printf("Hello world!\n");

    msg m;

    int pid = thread_create(8192, PRIORITY_MAIN-1, CREATE_WOUT_YIELD | CREATE_STACKTEST, second_thread, "pong");
    
    m.content.value = 1;

    while(1) {
                msg_send_receive(&m, &m, pid);
                printf("Got msg with content %i\n", m.content.value);
    }
}
