#include <stddef.h>
#include <stdlib.h>
#include <irq.h>
#include <queue.h>
#include <timex.h>
#include <hwtimer.h>

#include <thread.h>

#include <vtimer.h>

//#define ENABLE_DEBUG
#include <debug.h>

#define VTIMER_THRESHOLD 20U

#define SECONDS_PER_TICK (4096U)
#define NANOSECONDS_PER_TICK (4096U * 1000000)

void vtimer_callback(void *ptr);
void vtimer_tick(void *ptr);
static int set_longterm(vtimer_t *timer);
static int set_shortterm(vtimer_t *timer);

static queue_node_t longterm_queue_root;
static queue_node_t shortterm_queue_root;

static vtimer_t longterm_tick_timer;
static uint32_t longterm_tick_start;
static volatile int in_callback = false;

static int hwtimer_id = -1;
static uint32_t hwtimer_next_absolute;

static uint32_t seconds = 0;

static int set_longterm(vtimer_t *timer) {
    timer->queue_entry.priority = timer->absolute.seconds;
    queue_priority_add(&longterm_queue_root, (queue_node_t*)timer);
    return 0;
}

static int update_shortterm() {
    if (hwtimer_id) {
        if (hwtimer_next_absolute != shortterm_queue_root.next->priority) {
            hwtimer_remove(hwtimer_id);
        } else {
            return 0;
        }
    }

    uint32_t now = hwtimer_now();
    hwtimer_next_absolute = shortterm_queue_root.next->priority;
    
    while ((hwtimer_next_absolute + longterm_tick_start)-(hwtimer_now()+VTIMER_THRESHOLD) > NANOSECONDS_PER_TICK) {
        hwtimer_next_absolute += VTIMER_THRESHOLD;
    }

    hwtimer_id = hwtimer_set_absolute(hwtimer_next_absolute + longterm_tick_start, vtimer_callback, NULL);

    DEBUG("update_shortterm: Set hwtimer to %lu (now=%lu)\n", hwtimer_next_absolute + longterm_tick_start, hwtimer_now());
    //printf("%lu (now=%lu)\n", hwtimer_next_absolute + longterm_tick_start, hwtimer_now());

    return 0;
}

void vtimer_tick(void *ptr) {
    puts("vtimer_tick().");
    seconds += SECONDS_PER_TICK;

    longterm_tick_start = longterm_tick_timer.absolute.nanoseconds;
    longterm_tick_timer.absolute.nanoseconds = longterm_tick_timer.absolute.nanoseconds + NANOSECONDS_PER_TICK;
    set_shortterm(&longterm_tick_timer);

    while (longterm_queue_root.next) {
        vtimer_t *timer = (vtimer_t*) longterm_queue_root.next;
        if (timer->absolute.seconds == seconds) {
            timer = (vtimer_t*) queue_remove_head(&longterm_queue_root);
            set_shortterm(timer);
        } else {
            break;
        }
    }

    update_shortterm();
}

static int set_shortterm(vtimer_t *timer) {
    DEBUG("set_shortterm(): Absolute: %lu %lu\n", timer->absolute.seconds, timer->absolute.nanoseconds);
    timer->queue_entry.priority = timer->absolute.nanoseconds;
    queue_priority_add(&shortterm_queue_root, (queue_node_t*)timer);
    return 1;
}

void vtimer_callback(void *ptr) {
    vtimer_t *timer;
    in_callback = true;
    timer = (vtimer_t *)queue_remove_head(&shortterm_queue_root);

    DEBUG("vtimer_callback(): Shooting %lu.\n", timer->absolute.nanoseconds);
    /* shoot timer */
    timer->action(timer->arg);

    in_callback = false;
    update_shortterm();
}

void normalize_to_tick(timex_t *time) {
    DEBUG("Normalizing: %lu %lu\n", time->seconds, time->nanoseconds);
    uint32_t seconds_tmp = time->seconds % SECONDS_PER_TICK;
    time->seconds -= seconds_tmp;
    uint32_t nsecs_tmp = time->nanoseconds + (seconds_tmp * 1000000);
    DEBUG("Normalizin2: %lu %lu\n", time->seconds, nsecs_tmp);
    if (nsecs_tmp < time->nanoseconds) {
        nsecs_tmp -= NANOSECONDS_PER_TICK;
        time->seconds += SECONDS_PER_TICK;
    }
    if (nsecs_tmp > NANOSECONDS_PER_TICK) {
        nsecs_tmp -= NANOSECONDS_PER_TICK;
        time->seconds += SECONDS_PER_TICK;
    }
    time->nanoseconds = nsecs_tmp;
    DEBUG("     Result: %lu %lu\n", time->seconds, time->nanoseconds);
}

int vtimer_set(vtimer_t *timer, timex_t *offset) {
    DEBUG("vtimer_set(): New timer. Offset: %lu %lu\n", offset->seconds, offset->nanoseconds);


//    timex_t vnow = vtimer_now();
//    DEBUG("Now = %lu %lu\n", vnow.seconds, vnow.nanoseconds);

    timer->absolute = timex_add(vtimer_now(), *offset);
    normalize_to_tick(&(timer->absolute));

    DEBUG("vtimer_set(): Absolute: %lu %lu\n", timer->absolute.seconds, timer->absolute.nanoseconds);

    int result = 0;

    int state = disableIRQ();
    if (timer->absolute.seconds != seconds ) {
        /* we're long-term */
        DEBUG("vtimer_set(): setting long_term\n");
        result = set_longterm(timer);
    } else {
        DEBUG("vtimer_set(): setting short_term\n");
        if (set_shortterm(timer)) {

            /* delay update of next shortterm timer if we 
             * are called from within vtimer_callback.
             */
            if (!in_callback) {
                result = update_shortterm();
            }
        }
    }

    restoreIRQ(state);

    return result;
}

timex_t vtimer_now() {
    timex_t t = timex_set(seconds, hwtimer_now()-longterm_tick_start);
    return t;
}

int vtimer_init() {
    DEBUG("vtimer_init().\n");
    seconds = 0;

    longterm_tick_timer.action = vtimer_tick;
    longterm_tick_timer.arg = NULL;
   
    longterm_tick_timer.absolute.seconds = 0;
    longterm_tick_timer.absolute.nanoseconds = NANOSECONDS_PER_TICK;

    DEBUG("vtimer_init(): Setting longterm tick to %lu\n", longterm_tick_timer.absolute.nanoseconds);

    set_shortterm(&longterm_tick_timer);
    update_shortterm();

    return 0;
}

int vtimer_usleep(uint32_t usecs) {
    vtimer_t t;
    timex_t offset = timex_set(0, usecs);
    t.action = thread_wakeup;
    t.arg = (void*) thread_getpid();
    vtimer_set(&t, &offset);
    thread_sleep();
    return 0;
}

