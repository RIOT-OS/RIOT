#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include <irq.h>
#include <queue.h>
#include <timex.h>
#include <hwtimer.h>
#include <msg.h>
#include <thread.h>
#include <queue.h>

#include <vtimer.h>

#define ENABLE_DEBUG    (0)
#include <debug.h>

#define VTIMER_THRESHOLD 20UL
#define VTIMER_BACKOFF 10UL

#define SECONDS_PER_TICK (4096U)
#define MICROSECONDS_PER_TICK (4096UL * 1000000)

void vtimer_callback(void *ptr);
void vtimer_tick(void *ptr);
static int vtimer_set(vtimer_t *timer);
static int set_longterm(vtimer_t *timer);
static int set_shortterm(vtimer_t *timer);

#if ENABLE_DEBUG
void vtimer_print(vtimer_t *t);
#endif

static queue_node_t longterm_queue_root;
static queue_node_t shortterm_queue_root;

static vtimer_t longterm_tick_timer;
static uint32_t longterm_tick_start;
static volatile int in_callback = false;

static int hwtimer_id = -1;
static uint32_t hwtimer_next_absolute;

static uint32_t seconds = 0;

static int set_longterm(vtimer_t *timer)
{
    timer->queue_entry.priority = timer->absolute.seconds;
    queue_priority_add(&longterm_queue_root, (queue_node_t *)timer);
    return 0;
}

static int update_shortterm(void)
{
    if (hwtimer_id != -1) {
        if (hwtimer_next_absolute != shortterm_queue_root.next->priority) {
            hwtimer_remove(hwtimer_id);
        }
        else {
            return 0;
        }
    }

    hwtimer_next_absolute = shortterm_queue_root.next->priority;

    uint32_t next = hwtimer_next_absolute + longterm_tick_start;
    uint32_t now = HWTIMER_TICKS_TO_US(hwtimer_now());

    if((next -  HWTIMER_TICKS_TO_US(VTIMER_THRESHOLD) - now) > MICROSECONDS_PER_TICK ) {
        next = now +  HWTIMER_TICKS_TO_US(VTIMER_BACKOFF);
    }
    
    hwtimer_id = hwtimer_set_absolute(HWTIMER_TICKS(next), vtimer_callback, NULL);

    DEBUG("update_shortterm: Set hwtimer to %" PRIu32 " (now=%lu)\n", next, HWTIMER_TICKS_TO_US(hwtimer_now()));
    return 0;
}

void vtimer_tick(void *ptr)
{
    (void) ptr;
    DEBUG("vtimer_tick().");
    seconds += SECONDS_PER_TICK;

    longterm_tick_start = longterm_tick_timer.absolute.microseconds;
    longterm_tick_timer.absolute.microseconds = longterm_tick_timer.absolute.microseconds + MICROSECONDS_PER_TICK;
    set_shortterm(&longterm_tick_timer);

    while (longterm_queue_root.next) {
        vtimer_t *timer = (vtimer_t *) longterm_queue_root.next;

        if (timer->absolute.seconds == seconds) {
            timer = (vtimer_t *) queue_remove_head(&longterm_queue_root);
            set_shortterm(timer);
        }
        else {
            break;
        }
    }

    update_shortterm();
}

static int set_shortterm(vtimer_t *timer)
{
    DEBUG("set_shortterm(): Absolute: %" PRIu32 " %" PRIu32 "\n", timer->absolute.seconds, timer->absolute.microseconds);
    timer->queue_entry.priority = timer->absolute.microseconds;
    queue_priority_add(&shortterm_queue_root, (queue_node_t *)timer);
    return 1;
}

void vtimer_callback(void *ptr)
{
    (void) ptr;
    vtimer_t *timer;
    in_callback = true;
    hwtimer_id = -1;

    timer = (vtimer_t *)queue_remove_head(&shortterm_queue_root);

#if ENABLE_DEBUG
    vtimer_print(timer);
#endif
    DEBUG("vtimer_callback(): Shooting %" PRIu32 ".\n", timer->absolute.microseconds);

    /* shoot timer */
    if (timer->action == (void (*)(void *)) msg_send_int) {
        msg_t msg;
        msg.type = MSG_TIMER;
        msg.content.value = (unsigned int) timer->arg;
        msg_send_int(&msg, timer->pid);
    }
    else if (timer->action == (void (*)(void *)) thread_wakeup){
        timer->action(timer->arg);
    }
    else {
        DEBUG("Timer was poisoned.\n");
    }

    in_callback = false;
    update_shortterm();
}

void normalize_to_tick(timex_t *time)
{
    DEBUG("Normalizing: %" PRIu32 " %" PRIu32 "\n", time->seconds, time->microseconds);
    uint32_t seconds_tmp = time->seconds % SECONDS_PER_TICK;
    time->seconds -= seconds_tmp;
    uint32_t usecs_tmp = time->microseconds + (seconds_tmp * 1000000);
    DEBUG("Normalizin2: %" PRIu32 " %" PRIu32 "\n", time->seconds, usecs_tmp);

    if (usecs_tmp < time->microseconds) {
        usecs_tmp -= MICROSECONDS_PER_TICK;
        time->seconds += SECONDS_PER_TICK;
    }

    if (usecs_tmp > MICROSECONDS_PER_TICK) {
        usecs_tmp -= MICROSECONDS_PER_TICK;
        time->seconds += SECONDS_PER_TICK;
    }

    time->microseconds = usecs_tmp;
    DEBUG("     Result: %" PRIu32 " %" PRIu32 "\n", time->seconds, time->microseconds);
}

static int vtimer_set(vtimer_t *timer)
{
    DEBUG("vtimer_set(): New timer. Offset: %" PRIu32 " %" PRIu32 "\n", timer->absolute.seconds, timer->absolute.microseconds);

    timex_t now;
    vtimer_now(&now);
    timer->absolute = timex_add(now, timer->absolute);
    normalize_to_tick(&(timer->absolute));

    DEBUG("vtimer_set(): Absolute: %" PRIu32 " %" PRIu32 "\n", timer->absolute.seconds, timer->absolute.microseconds);
    DEBUG("vtimer_set(): NOW: %" PRIu32 " %" PRIu32 "\n", now.seconds, now.microseconds);

    int result = 0;

    if (timer->absolute.seconds == 0) {
        if (timer->absolute.microseconds > 10) {
            timer->absolute.microseconds -= 10;
        }
    }

    int state = disableIRQ();

    if (timer->absolute.seconds != seconds) {
        /* we're long-term */
        DEBUG("vtimer_set(): setting long_term\n");
        result = set_longterm(timer);
    }
    else {
        DEBUG("vtimer_set(): setting short_term\n");

        if (set_shortterm(timer)) {

            /* delay update of next shortterm timer if we
            * are called from within vtimer_callback. */

            if (!in_callback) {
                result = update_shortterm();
            }
        }
    }


    restoreIRQ(state);

    return result;
}

void vtimer_now(timex_t *out)
{
    timex_t t = timex_set(seconds, hwtimer_now() - longterm_tick_start);
    memcpy(out, &t, sizeof(timex_t));
}

int vtimer_init()
{
    DEBUG("vtimer_init().\n");
    int state = disableIRQ();
    seconds = 0;

    longterm_tick_start = 0;

    longterm_tick_timer.action = vtimer_tick;
    longterm_tick_timer.arg = NULL;

    longterm_tick_timer.absolute.seconds = 0;
    longterm_tick_timer.absolute.microseconds = MICROSECONDS_PER_TICK;

    DEBUG("vtimer_init(): Setting longterm tick to %" PRIu32 "\n", longterm_tick_timer.absolute.microseconds);

    set_shortterm(&longterm_tick_timer);
    update_shortterm();

    restoreIRQ(state);
    return 0;
}

int vtimer_set_wakeup(vtimer_t *t, timex_t interval, int pid)
{
    int ret;
    t->action = (void *) thread_wakeup;
    t->arg = (void *) pid;
    t->absolute = interval;
    t->pid = 0;
    ret = vtimer_set(t);
    return ret;
}

int vtimer_usleep(uint32_t usecs)
{
    timex_t offset = timex_set(0, usecs);
    return vtimer_sleep(offset);
}

int vtimer_sleep(timex_t time)
{
    int ret;
    vtimer_t t;
    ret = vtimer_set_wakeup(&t, time, thread_getpid());
    thread_sleep();
    return ret;
}

int vtimer_remove(vtimer_t *t)
{
    queue_remove(&shortterm_queue_root, (queue_node_t *)t);
    queue_remove(&longterm_queue_root, (queue_node_t *)t);

    update_shortterm();

    if (!inISR()) {
        eINT();
    }

    return 0;
}

int vtimer_set_msg(vtimer_t *t, timex_t interval, unsigned int pid, void *ptr)
{
    t->action = (void *) msg_send_int;
    t->arg = ptr;
    t->absolute = interval;
    t->pid = pid;
    vtimer_set(t);
    return 0;
}

#if ENABLE_DEBUG

void vtimer_print_short_queue(){
    queue_print(&shortterm_queue_root);
}

void vtimer_print_long_queue(){
    queue_print(&longterm_queue_root);
}

void vtimer_print(vtimer_t *t)
{
    printf("Seconds: %"PRIu32" - Microseconds: %"PRIu32"\n \
            action: %p\n \
            arg: %p\n \
            pid: %u\n",
           t->absolute.seconds, t->absolute.microseconds,
           t->action,
           t->arg,
           t->pid);
}

#endif
