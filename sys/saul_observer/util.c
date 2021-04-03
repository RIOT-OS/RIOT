#include "saul_observer.h"
#include "msg.h"
#include "thread.h"

#if IS_ACTIVE(MODULE_CORE_MSG)
static void _callback_msg(saul_reg_t *dev, void *arg)
{
    msg_t *msg = (msg_t *)arg;
    kernel_pid_t target_pid = msg->sender_pid;
    msg->content.ptr = (void *)dev;
    msg_try_send(msg, target_pid);
    /* restore the sender_pid - it has been modified by msg_try_send */
    msg->sender_pid = target_pid;
}

void saul_observer_msg(saul_reg_t *dev, saul_observer_t *observer, msg_t *msg, kernel_pid_t target)
{
    observer->callback = _callback_msg;
    observer->arg = (void *)msg;

    /* use sender_pid field to get target_pid into callback function */
    msg->sender_pid = target;

    saul_observer_add(dev, observer);
}
#endif

static void _callback_wakeup(saul_reg_t *dev, void *arg)
{
    (void) dev;
    thread_wakeup((kernel_pid_t)((intptr_t)arg));
}

void saul_observer_wakeup(saul_reg_t *dev, saul_observer_t *observer, kernel_pid_t pid)
{
    observer->callback = _callback_wakeup;
    observer->arg = (void *)((intptr_t)pid);

    saul_observer_add(dev, observer);
}
