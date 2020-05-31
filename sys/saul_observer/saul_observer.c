#include "thread.h"
#include "msg.h"
#include "saul_observer.h"

#if ENABLE_DEBUG
static char _stack[SAUL_OBSERVER_STACK_SIZE + THREAD_EXTRA_STACKSIZE_PRINTF];
#else
static char _stack[SAUL_OBSERVER_STACK_SIZE];
#endif

kernel_pid_t saul_observer_pid = KERNEL_PID_UNDEF;

msg_bus_t saul_observer_bus;

static void *_event_loop(void *args)
{
    msg_t msg, msg_q[CONFIG_SAUL_OBSERVER_MSG_QUEUE_SIZE];

    (void)args;
    msg_init_queue(msg_q, CONFIG_SAUL_OBSERVER_MSG_QUEUE_SIZE);
    msg_bus_init(&saul_observer_bus);

    while (1) {
        msg_receive(&msg);
        if (msg.type != SAUL_OBSERVER_MSG_TYPE_CHECK_DEV) {
            continue;
        }

        saul_reg_t *dev = msg.content.ptr;
        if (dev->driver->check == NULL || dev->driver->check(dev->dev) <= 0) {
            continue;
        }

        msg_bus_post(&saul_observer_bus, dev->id, dev);
    }

    return NULL;
}

void saul_observer_init(void)
{
    if (saul_observer_pid == KERNEL_PID_UNDEF) {
        saul_observer_pid = thread_create(_stack, sizeof(_stack), SAUL_OBSERVER_PRIO,
                                      THREAD_CREATE_STACKTEST,
                                      _event_loop, NULL, "saul");
    }
}

void saul_observer_queue_check(saul_reg_t *dev)
{
    /* check if the device is observable */
    if (dev->driver->check == NULL) {
        return;
    }

    if (saul_observer_pid == KERNEL_PID_UNDEF) {
        return;
    }

    msg_t m = {.type = SAUL_OBSERVER_MSG_TYPE_CHECK_DEV,
               .content.ptr = (void *)dev};
    msg_try_send(&m, saul_observer_pid);
}
