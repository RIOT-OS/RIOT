#include <stdio.h>
#include "thread.h"
#include "mutex.h"

mutex_t mutex;
int indicator, count;

char stack[KERNEL_CONF_STACKSIZE_MAIN];
void second_thread(void) {
    while(1) {
        mutex_lock(&mutex);	
        thread_wakeup(1);
        indicator--;
        mutex_unlock_and_sleep(&mutex);
	}
}

int main(void) {
    indicator = 0;
    count = 0;
    mutex_init(&mutex);

    thread_create(stack,
                    KERNEL_CONF_STACKSIZE_MAIN,
                    PRIORITY_MAIN-1,
                    CREATE_WOUT_YIELD | CREATE_STACKTEST,
                    second_thread,
                    "second_thread");

    while(1) {
        mutex_lock(&mutex);
        thread_wakeup(2);
        indicator++;
        count++;

        if (indicator > 1 || indicator < -1) {
            printf("Error, threads did not sleep properly. [indicator: %d]\n", indicator);
            return -1;
        }
        if ((count%100000) == 0) {
            printf("Still alive alternated [count: %dk] times.\n", count/1000);
        }

        mutex_unlock_and_sleep(&mutex);
    }
}
