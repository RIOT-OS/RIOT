/*
 * SPDX-FileCopyrightText: 2024-2026 Carl Seifert
 * SPDX-FileCopyrightText: 2024-2026 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @ingroup net_unicoap
 * @brief   Core functionality
 * @author  Carl Seifert <carl.seifert@tu-dresden.de>
 */

#include <errno.h>

#include "event.h"
#include "mutex.h"
#include "random.h"
#include "sched.h"
#include "thread.h"

#define ENABLE_DEBUG CONFIG_UNICOAP_DEBUG_LOGGING
#include "debug.h"
#include "private.h"

UNICOAP_DECL_RECEIVER_STORAGE;

kernel_pid_t _unicoap_pid = KERNEL_PID_UNDEF;
static event_queue_t _queue;

/* MARK: - Event Scheduling on Internal Queue */

static void _scheduled_event_callback(void* scheduled_event)
{
    if (IS_ACTIVE(DEVELHELP)) {
        _STATE_EVENT_DEBUG("%s fired, exec queued\n", unicoap_scheduled_event_name(
                                                          unicoap_scheduled_event_of_event((event_t*)scheduled_event)));
    }
    event_post(&_queue, (event_t*)scheduled_event);
}

void unicoap_event_schedule(unicoap_scheduled_event_t* event, unicoap_event_callback_t callback,
                            uint32_t duration, const char* name)
{
    (void)name;
#if DEVELHELP
    assert(name);
    event->name = name;
#endif
    if (IS_ACTIVE(DEVELHELP)) {
        _STATE_EVENT_DEBUG("%s in %" PRIu32 "ms\n", unicoap_scheduled_event_name(event), duration);
    }
    event->ztimer.callback = _scheduled_event_callback;
    event->ztimer.arg = (void*)event;
    /* This cast is fine because (a) the return types are identical and the function argument is
     * a pointer to a struct, too. */
    event->super = (event_t){ .handler = (event_handler_t)callback };
    ztimer_set(UNICOAP_CLOCK, &event->ztimer, duration);
}

void unicoap_event_reschedule(unicoap_scheduled_event_t* event, uint32_t duration)
{
    if (IS_ACTIVE(DEVELHELP)) {
        _STATE_EVENT_DEBUG("%s in %" PRIu32 "ms (rescheduled)\n",
                           unicoap_scheduled_event_name(event), duration);
    }
    ztimer_set(UNICOAP_CLOCK, &event->ztimer, duration);
}

void unicoap_event_cancel(unicoap_scheduled_event_t* event)
{
    if (!event->super.handler) {
        return;
    }
    if (IS_ACTIVE(DEVELHELP) && unicoap_scheduled_event_name(event)) {
        _STATE_EVENT_DEBUG("%s cancelled\n", unicoap_scheduled_event_name(event));
    }
    bool triggered = !ztimer_remove(UNICOAP_CLOCK, &event->ztimer);
    if (triggered) {
        /* event cancel runs in O(n), so we really only want to call
         * event_cancel if the event has already been posted */
        event_cancel(&_queue, &event->super);
    }
    event->super.handler = NULL;
#if DEVELHELP
    event->name = NULL;
#endif
}

/* MARK: - Lifecycle */

static inline int _init_drivers(event_queue_t* queue)
{
    (void)queue;
#if IS_USED(MODULE_UNICOAP_DRIVER_RFC7252_COMMON)
    if (unicoap_init_rfc7252_common(queue) < 0) {
        return -1;
    }
#endif
#if IS_USED(MODULE_UNICOAP_DRIVER_UDP)
    if (unicoap_init_udp(queue) < 0) {
        return -1;
    }
#endif
#if IS_USED(MODULE_UNICOAP_DRIVER_DTLS)
    if (unicoap_init_dtls(queue) < 0) {
        return -1;
    }
#endif
#if IS_USED(MODULE_UNICOAP_DRIVER_SLIPMUX)
    if (unicoap_init_slipmux(queue) < 0) {
        return -1;
    }
#endif
    /* MARK: unicoap_driver_extension_point */
    return 0;
}

static inline int _deinit_drivers(event_queue_t* queue)
{
    (void)queue;
    int res = 0;
#if IS_USED(MODULE_UNICOAP_DRIVER_RFC7252_COMMON)
    res += unicoap_deinit_rfc7252_common(queue);
#endif
#if IS_USED(MODULE_UNICOAP_DRIVER_UDP)
    res += unicoap_deinit_udp(queue);
#endif
#if IS_USED(MODULE_UNICOAP_DRIVER_DTLS)
    res += unicoap_deinit_dtls(queue);
#endif
#if IS_USED(MODULE_UNICOAP_DRIVER_SLIPMUX)
    res += unicoap_deinit_slipmux(queue);
#endif
    /* MARK: unicoap_driver_extension_point */
    return res;
}

static void* _unicoap_loop_run(void* arg)
{
    (void)arg;
    /* Now we set the already-initialized queue's waiter thread. See unicoap_init below. */
    /* _queue.waiter == NULL is asserted by event_queue_claim */
    event_queue_claim(&_queue);
    _unicoap_pid = thread_getpid();
    event_loop(&_queue);
    return &_queue;
}

kernel_pid_t unicoap_init(void)
{
    if (_unicoap_pid != KERNEL_PID_UNDEF) {
        return -EEXIST;
    }

    event_queue_init_detached(&_queue);

    if (_init_drivers(&_queue) < 0) {
        UNICOAP_DEBUG("driver initialization failed\n");
        return -1;
    }

#if IS_USED(MODULE_UNICOAP_SERVER)
    unicoap_init_server(&_queue);
#endif
#if IS_USED(MODULE_UNICOAP_CLIENT)
    unicoap_init_client(&_queue);
#endif

#if IS_ACTIVE(CONFIG_UNICOAP_CREATE_THREAD)
    static char _unicoap_thread_stack[UNICOAP_STACK_SIZE];
    _unicoap_pid = thread_create(_unicoap_thread_stack, sizeof(_unicoap_thread_stack),
                                 THREAD_PRIORITY_MAIN - 1, 0, _unicoap_loop_run,
                                 NULL, UNICOAP_THREAD_IDENTIFIER);
#endif

    return _unicoap_pid;
}

int unicoap_deinit(void)
{
    if (_unicoap_pid == KERNEL_PID_UNDEF) {
        return -ENOENT;
    }

    _deinit_drivers(&_queue);
    _unicoap_pid = KERNEL_PID_UNDEF;
    memset(&_queue, 0, sizeof(_queue));

    return 0;
}

int unicoap_loop_enqueue(unicoap_job_t* job)
{
    event_post(&_queue, &job->super);
    return 0;
}

int unicoap_loop_enqueue_and_block(unicoap_blocking_job_t* job)
{
    if (thread_getpid() == _unicoap_pid) {
        /* Call function directly if on unicoap thread. */
        job->super.super.handler(&job->super.super);
        return job->res;
    }
    event_post(&_queue, &job->super.super);
    /* Block until callback calls unlock. */
    mutex_lock(&job->roadblock);
    return job->res;
}
