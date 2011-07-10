#include <stdio.h>
#include <hwtimer.h>
#include <swtimer.h>
#include <thread.h>
#include <kernel.h>
#include <msg.h>

void callback(void* ptr){
    puts((char*)ptr);
}

void wakeup_thread(void){
        puts("wakeup");
}

void msg_thread(void){
        msg_t m;
        msg_receive(&m);
        printf("%s\n",(char*)m.content.ptr);
}

int main(void)
{
    hwtimer_init();
    swtimer_init();

    int pid1 = thread_create(8192, PRIORITY_MAIN-1,
                CREATE_WOUT_YIELD | CREATE_SLEEPING | CREATE_STACKTEST, wakeup_thread, "nr1");

    int pid2 = thread_create(8192, PRIORITY_MAIN-1,
                /*CREATE_WOUT_YIELD | CREATE_SLEEPING |*/ CREATE_STACKTEST, msg_thread, "nr2");


    swtimer_t cbt;
    swtimer_t wut;
    swtimer_t mst;
    /* test callback */
    swtimer_set_cb(&cbt,1000L,callback,"callback");
    /* test wake-up */
    swtimer_set_wakeup(&wut, 1000L, pid1);
    /* test message */
    swtimer_set_msg(&mst,1000L, pid2, "message");
         
    while(1);
}
