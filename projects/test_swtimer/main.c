#include <stdio.h>
#include <stdint.h>
#include <thread.h>
#include <kernel.h>
#include <hwtimer.h>

#include <swtimer.h>

#include "primes.h"

#define TIMERS 20
swtimer_t timers[TIMERS];
int timer_hits[TIMERS];
swtime_t timer_durations[TIMERS];
swtime_t timer_planned_durations[TIMERS];

extern swtime_t swtimer_next_alarm_absolute;

void simple_callback(void* ptr) {
    int timernum = (int) ptr;
    swtimer_t *t = &timers[timernum];
    timer_durations[timernum] = swtimer_now() - t->start;
    timer_hits[timernum] += 1;
    swtimer_set(t, t->interval);
}

#define TIMER_FACTOR 1000L

int main(void)
{
    hwtimer_init();
    swtimer_init();

    printf("swtimer stress test.\n");

    int prime = 10;
    for (int i = 0; i < TIMERS; i++) {
        timer_hits[i] = 0;
        timers[i].action_type = SWTIMER_CALLBACK;
        timers[i].action.callback.f = simple_callback;
        timers[i].action.callback.ptr = (void*)i;

        swtimer_set(&timers[i], TIMER_FACTOR*primes[(prime)-1]);
        timer_planned_durations[i] = TIMER_FACTOR*primes[(prime)-1];
        prime+=5;
        if (prime >= 999) prime = 0;
    }
   
    swtime_t last = 0;
    unsigned int total_hits = 0;
    while(1) {
        swtimer_usleep(1000000L);
        printf("Duration: %lu\n", swtimer_now() - last);
        last = swtimer_now();

        for (int t = 0; t < TIMERS; t++) {
            total_hits += timer_hits[t];
            printf("timernum: %i \thits=%i \tduration: %8lu \tdiff=%lu\n", t, timer_hits[t], timer_durations[t], timer_durations[t] - timer_planned_durations[t]);
        }
        printf("-----\nTotal Hits: %u\n", total_hits);
        puts("");
    }
}
