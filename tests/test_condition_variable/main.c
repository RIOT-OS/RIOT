#include <stdio.h>
#include "condition_variable.h"
#include "thread.h"
#include "mutex.h"

mutex_t mutex;
struct pthread_cond_t cv;
int count;
char stack[KERNEL_CONF_STACKSIZE_MAIN];

/**
 * @brief   This thread tries to lock the mutex to enter the critical section.
 *          Then it sets cv true (==1), signals one waiting thread to check cv state and sleep 
 */
void second_thread(void) {
    while(1) {
        mutex_lock(&mutex);	
        cv.val = 1;
        pthread_cond_signal(&cv);
        thread_sleep();
        mutex_unlock(&mutex);
	}
}

int main(void) {
    count = 0;
    mutex_init(&mutex);
    pthread_cond_init(&cv, NULL);

    int pid = thread_create(stack,
                            KERNEL_CONF_STACKSIZE_MAIN,
                            PRIORITY_MAIN-1,
                            CREATE_WOUT_YIELD | CREATE_STACKTEST,
                            second_thread,
                            "second_thread");

    while(1) {
        mutex_lock(&mutex);
        thread_wakeup(pid);
        count++;

        if ((count%100000) == 0) {
            printf("Still alive alternated [count: %dk] times.\n", count/1000);
        }

        pthread_cond_wait(&cv, &mutex);
        cv.val = 0;
        mutex_unlock(&mutex);
    }
}
