#include "assert.h"
#include "saul_observer.h"
#include "kernel_defines.h"
#include "event/thread.h"

static int _call_observer(clist_node_t *item, void *arg)
{
    saul_observer_t *observer = container_of(item, saul_observer_t, list_node);
    saul_reg_t *dev = (saul_reg_t *) arg;
    observer->callback(dev, observer->arg);

    /* keep on iterating over the list */
    return 0;
}

static void _handle_event(event_t *event)
{
    saul_reg_t *dev = container_of(event, saul_reg_t, event);

    /* call check method if implemented */
    if (dev->driver->check != NULL) {
        if (dev->driver->check(dev->dev) == 0) {
            /* no significant change */
            return;
        }
    }

    /* call all observers */
    mutex_lock(&dev->observers.mtx);
    clist_foreach(&dev->observers.lst, _call_observer, (void *) dev);
    mutex_unlock(&dev->observers.mtx);
}

void saul_observer_queue_event(saul_reg_t *dev)
{
    dev->event.handler = _handle_event;
    event_post(EVENT_PRIO_MEDIUM, &dev->event);
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
