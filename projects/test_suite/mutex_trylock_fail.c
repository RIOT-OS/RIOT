#include <stdio.h>
#include <mutex.h>

#include <thread.h>
#include <flags.h>
#include <kernel.h>

mutex_t mutex;

static void second_thread(void) {
    puts(" 2nd: trying to lock mutex...");
    mutex_trylock(&mutex);
    puts(" 2nd: done.");
}

static tcb second_tcb;
static char second_stack[KERNEL_CONF_STACKSIZE_MAIN];

void mutex_trylock_fail(char* cmdline)
{
    puts("main: locking mutex...");
    mutex_lock(&mutex);

    puts("main: creating thread...");
    thread_create(&second_tcb, second_stack, KERNEL_CONF_STACKSIZE_MAIN, PRIORITY_MAIN-1, CREATE_STACKTEST, second_thread, "nr2");
    
    puts("main: thread created. Unlocking mutex...");
    mutex_unlock(&mutex, true);

    puts("main: mutex unlocked.");
}
