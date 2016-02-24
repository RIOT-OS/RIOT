#include "bitarithm.h"
#include "thread_flags.h"
#include "irq.h"
#include "thread.h"
#include "tcb.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#ifdef MODULE_THREAD_FLAGS
static thread_flags_t _thread_flags_clear_atomic(tcb_t *tcb, thread_flags_t mask)
{
    unsigned state = disableIRQ();
    mask &= tcb->flags;
    tcb->flags &= ~mask;
    restoreIRQ(state);
    return mask;
}

void thread_flags_set_atomic(tcb_t *tcb, thread_flags_t mask)
{
    unsigned state = disableIRQ();
    tcb->flags |= mask;
    restoreIRQ(state);
}

static void _thread_flags_wait(thread_flags_t mask, tcb_t *tcb, unsigned threadstate, unsigned irqstate)
{
    DEBUG("_thread_flags_wait: me->flags=0x%08x me->mask=0x%08x. going blocked.\n",
            (unsigned)tcb->flags, (unsigned)mask);

    tcb->wait_data = (void *)(unsigned)mask;
    sched_set_status(tcb, threadstate);
    restoreIRQ(irqstate);
    thread_yield_higher();
}

thread_flags_t thread_flags_clear(thread_flags_t mask)
{
    tcb_t *me = (tcb_t*) sched_active_thread;
    mask = _thread_flags_clear_atomic(me, mask);
    DEBUG("thread_flags_clear(): pid %"PRIkernel_pid" clearing 0x%08x\n", thread_getpid(), mask);
    return mask;
}

static void _thread_flags_wait_any(thread_flags_t mask)
{
    tcb_t *me = (tcb_t*) sched_active_thread;
    unsigned state = disableIRQ();
    if (!(me->flags & mask)) {
        _thread_flags_wait(mask, me, STATUS_FLAG_BLOCKED_ANY, state);
    }
    else {
        restoreIRQ(state);
    }
}

thread_flags_t thread_flags_wait_any(thread_flags_t mask)
{
    tcb_t *me = (tcb_t*) sched_active_thread;
    _thread_flags_wait_any(mask);
    return _thread_flags_clear_atomic(me, mask);
}

thread_flags_t thread_flags_wait_one(thread_flags_t mask)
{
    _thread_flags_wait_any(mask);
    tcb_t *me = (tcb_t*) sched_active_thread;
    unsigned tmp = me->flags & mask;
    return _thread_flags_clear_atomic(me, thread_flags_clear(1 << bitarithm_lsb(tmp)));
}

thread_flags_t thread_flags_wait_all(thread_flags_t mask)
{
    unsigned state = disableIRQ();
    tcb_t *me = (tcb_t*) sched_active_thread;
    if (!((me->flags & mask) == mask)) {
        DEBUG("thread_flags_wait_all(): pid %"PRIkernel_pid" waiting for %08x\n", thread_getpid(), (unsigned)mask);
        _thread_flags_wait(mask, me, STATUS_FLAG_BLOCKED_ALL, state);
    }
    else {
        restoreIRQ(state);
    }

    return _thread_flags_clear_atomic(me, mask);
}

inline int __attribute__((always_inline)) thread_flags_wake(tcb_t *tcb)
{
    unsigned wakeup = 0;
    thread_flags_t mask = (uint16_t)(unsigned)tcb->wait_data;
    switch(tcb->status) {
        case STATUS_FLAG_BLOCKED_ANY:
            wakeup = (tcb->flags & mask);
            break;
        case STATUS_FLAG_BLOCKED_ALL:
            wakeup = ((tcb->flags & mask) == mask);
            break;
    }

    if (wakeup) {
        DEBUG("_thread_flags_wake(): wakeing up pid %"PRIkernel_pid"\n", tcb->pid);
        sched_set_status(tcb, STATUS_RUNNING);
    }

    return wakeup;
}

void thread_flags_set(tcb_t *tcb, thread_flags_t mask)
{
    DEBUG("thread_flags_set(): setting 0x%08x for pid %"PRIkernel_pid"\n", mask, tcb->pid);
    unsigned state = disableIRQ();
    tcb->flags |= mask;
    if (thread_flags_wake(tcb)) {
        restoreIRQ(state);
        thread_yield_higher();
    }
    else {
        restoreIRQ(state);
    }
}
#endif /* MODULE_THREAD_FLAGS */
