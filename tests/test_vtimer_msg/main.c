#include <stdio.h>
#include <time.h>

#include "vtimer.h"
#include "thread.h"
#include "msg.h"

char timer_stack[KERNEL_CONF_STACKSIZE_PRINTF];
char timer_stack_local[KERNEL_CONF_STACKSIZE_PRINTF];

struct timer_msg {
    vtimer_t timer;
    timex_t interval;
    char *msg;
};

timex_t now;

struct timer_msg msg_a = { .timer = {0}, .interval = { .seconds = 2, .microseconds = 0}, .msg = "Hello World" };
struct timer_msg msg_b = { .timer = {0}, .interval = { .seconds = 5, .microseconds = 0}, .msg = "This is a Test" };

void timer_thread(void)
{
    printf("This is thread %d\n", thread_getpid());

    /* we need a queue if the second message arrives while the first is still processed */
    /* without a queue, the message would get lost */
    /* because of the way this timer works, there can be max 1 queued message */
    msg_t msgq[1];
    msg_init_queue(msgq, sizeof msgq);

    while (1) {
        msg_t m;
        msg_receive(&m);
        struct timer_msg *tmsg = (struct timer_msg *) m.content.ptr;
        vtimer_now(&now);
        printf("now=%" PRIu32 ":%" PRIu32 " -> every %u.%us: %s\n",
               now.seconds,
               now.microseconds,
               tmsg->interval.seconds,
               tmsg->interval.microseconds,
               tmsg->msg);

        if (vtimer_set_msg(&tmsg->timer, tmsg->interval, thread_getpid(), tmsg) != 0) {
            puts("something went wrong");
        }
        else {
            puts("timer_thread: set timer successfully");
        }
    }
}

void timer_thread_local(void)
{
    printf("This is thread %d\n", thread_getpid());

    while (1) {
        msg_t m;
        msg_receive(&m);

        struct tm t;
        vtimer_get_localtime(&t);
        printf("sec=%d min=%d hour=%d\n", t.tm_sec, t.tm_min, t.tm_hour);
    }
}

int main(void)
{
    msg_t m;
    int pid = thread_create(
                  timer_stack,
                  sizeof timer_stack,
                  PRIORITY_MAIN - 1,
                  CREATE_STACKTEST,
                  timer_thread,
                  "timer");

    puts("sending 1st msg");
    m.content.ptr = (char *) &msg_a;
    msg_send(&m, pid, false);

    puts("sending 2nd msg");
    m.content.ptr = (char *) &msg_b;
    msg_send(&m, pid, false);

    int pid2 = thread_create(
                   timer_stack_local,
                   sizeof timer_stack_local,
                   PRIORITY_MAIN - 1,
                   CREATE_STACKTEST,
                   timer_thread_local,
                   "timer local");

    timex_t sleep = timex_set(1, 0);

    while (1) {
        vtimer_sleep(sleep);
        msg_send(&m, pid2, 0);
    }
}
