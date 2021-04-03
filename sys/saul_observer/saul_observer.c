#include "thread.h"
#include "mutex.h"
#include "assert.h"
#include "saul_observer.h"
#include "kernel_defines.h"

static char _stack[SAUL_OBSERVER_STACK_SIZE];
static kernel_pid_t _pid = KERNEL_PID_UNDEF;
static clist_node_t _event_queue;
static mutex_t _event_queue_mtx = MUTEX_INIT_LOCKED;

static inline saul_reg_t * _event_get(void)
{
    saul_reg_t *dev;

    while (1) {
        /* Try to get next event from FIFO */
        unsigned state = irq_disable();
        saul_observer_event_t *event = clist_lpop(&_event_queue);
        irq_restore(state);

        /* Check if we got an event from the queue */
        if (event) {
            dev = container_of(event, saul_reg_t, event);
            break;
        }

        /* Wait until an event shows up on the queue */
        mutex_lock(&_event_queue_mtx);
    }

    return dev;
}

static int _call_observer(clist_node_t *item, void *arg)
{
    saul_observer_t *observer = container_of(item, saul_observer_t, list_node);
    saul_reg_t *dev = (saul_reg_t *) arg;
    observer->callback(dev, observer->arg);

    /* keep on iterating over the list */
    return 0;
}

static void *_event_loop(void *args)
{
    (void)args;

    while (1) {
        /* Fetch next event from queue */
        saul_reg_t *dev = _event_get();

        /* call check method if implemented */
        if (dev->driver->check != NULL) {
            if (dev->driver->check(dev->dev) == 0) {
                /* no significant change */
                continue;
            }
        }

        /* call all observers */
        mutex_lock(&dev->observers.mtx);
        clist_foreach(&dev->observers.lst, _call_observer, (void *) dev);
        mutex_unlock(&dev->observers.mtx);
    }

    return NULL;
}

void saul_observer_init(void)
{
    if (_pid == KERNEL_PID_UNDEF) {
        _pid = thread_create(_stack, sizeof(_stack), SAUL_OBSERVER_PRIO,
                             THREAD_CREATE_STACKTEST,
                             _event_loop, NULL, "saul");
    }
}

void saul_observer_queue_event(saul_reg_t *dev)
{
    unsigned state = irq_disable();

    /* schedule check of saul device if its check isn't pending, yet */
    if (clist_find(&_event_queue, &dev->event) == NULL) {
        clist_rpush(&_event_queue, &dev->event);

        /* Unlock the mutex to indicate an event has been pushed in the queue */
        mutex_unlock(&_event_queue_mtx);
    }

    irq_restore(state);
}

void saul_observer_add(saul_reg_t *dev, saul_observer_t *observer)
{
    mutex_lock(&dev->observers.mtx);
    clist_rpush(&dev->observers.lst, &observer->list_node);
    mutex_unlock(&dev->observers.mtx);
}

void saul_observer_rm(saul_reg_t *dev, saul_observer_t *observer)
{
    mutex_lock(&dev->observers.mtx);
    clist_remove(&dev->observers.lst, &observer->list_node);
    mutex_unlock(&dev->observers.mtx);
}
