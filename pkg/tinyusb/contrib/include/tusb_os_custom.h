/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_tinyusb
 * @{
 *
 * @file
 * @brief       TinyUSB OS Abstraction Layer for RIOT
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef TUSB_OS_CUSTOM_H
#define TUSB_OS_CUSTOM_H

#include "mutex.h"
#include "sema.h"
#include "ztimer.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !DOXYGEN

/* set to 1 to use cancelable mutex in osal_mutex and osal_queue */
#ifndef TINYUSB_OSAL_MUTEX_CANCELABLE
#define TINYUSB_OSAL_MUTEX_CANCELABLE 0
#endif

/**
 * Task API
 */

TU_ATTR_ALWAYS_INLINE static inline void osal_task_delay(uint32_t msec)
{
    ztimer_sleep(ZTIMER_MSEC, msec);
}

/**
 * Semaphore API
 *
 * This API is only used by RNDIS-CDC.
 */

typedef sema_t  osal_semaphore_def_t;    /* semaphore */
typedef sema_t *osal_semaphore_t;        /* semaphore handle */

TU_ATTR_ALWAYS_INLINE
static inline osal_semaphore_t osal_semaphore_create(osal_semaphore_def_t* semdef)
{
    sema_create(semdef, 0);
    return (osal_semaphore_t)semdef;
}

TU_ATTR_ALWAYS_INLINE
static inline bool osal_semaphore_post(osal_semaphore_t sem_hdl, bool in_isr)
{
    (void)in_isr; /* hasn't to be considered since RIOT doesn't block in sema_post */
    return sema_post(sem_hdl) == 0;
}

TU_ATTR_ALWAYS_INLINE
static inline bool osal_semaphore_wait(osal_semaphore_t sem_hdl, uint32_t msec)
{
    assert(0);
    return sema_wait_timed_ztimer(sem_hdl, ZTIMER_MSEC, msec) == 0;
}

TU_ATTR_ALWAYS_INLINE static inline void osal_semaphore_reset(osal_semaphore_t const sem_hdl)
{
    (void)sem_hdl;
    /* TODO, function seems to be removed anyway */
}

/** Mutex API (priority inheritance) */
typedef mutex_t  osal_mutex_def_t;  /* RIOT mutex */
typedef mutex_t *osal_mutex_t;      /* RIOT mutex handle */

TU_ATTR_ALWAYS_INLINE
static inline osal_mutex_t osal_mutex_create(osal_mutex_def_t* mdef)
{
    assert(mdef != NULL);
    mutex_init((mutex_t *)mdef);
    return (osal_mutex_t)mdef;
}

#if TINYUSB_OSAL_MUTEX_CANCELABLE
static void _osal_mutex_lock_timeout(void *arg)
{
    mutex_cancel(arg);
}
#endif

TU_ATTR_ALWAYS_INLINE
static inline bool osal_mutex_lock(osal_mutex_t mutex_hdl, uint32_t msec)
{
    assert(mutex_hdl);
#if TINYUSB_OSAL_MUTEX_CANCELABLE
    mutex_cancel_t _mc = mutex_cancel_init(mutex_hdl);

    ztimer_t _timer = { .callback = _osal_mutex_lock_timeout, .arg = &_mc };
    ztimer_set(ZTIMER_MSEC, &_timer, msec);

    return mutex_lock_cancelable(&_mc) == 0;
#else
    (void)msec;
    assert(msec == OSAL_TIMEOUT_WAIT_FOREVER);
    mutex_lock(mutex_hdl);
    return true;
#endif
}

TU_ATTR_ALWAYS_INLINE
static inline bool osal_mutex_unlock(osal_mutex_t mutex_hdl)
{
    assert(mutex_hdl);
    mutex_unlock(mutex_hdl);
    return true;
}

/**
 * Queue API
 */

#define OSAL_QUEUE_DEF(_int_set, _name, _depth, _type) \
    /* _int_set is not used in RTOS */ \
    static _type _name##_##q_items[_depth];  /* queue item data storage */ \
    osal_queue_def_t _name = { \
                                .buffer = _name##_##q_items, \
                                .size = sizeof(_type), \
                                .depth = _depth, \
                             };

typedef struct {
    list_node_t node;
    void *data;
} osal_queue_entry;

typedef struct {
    void *buffer;       /* buffer used for queue item data */
    uint16_t size;      /* queue item size */
    uint16_t depth;     /* maximum number of queue items */
    uint16_t front;
    uint16_t tail;
    sema_t smphr;       /* semaphore value represents the queue fill level */
} osal_queue_def_t;

typedef osal_queue_def_t *osal_queue_t;

TU_ATTR_ALWAYS_INLINE static inline osal_queue_t osal_queue_create(osal_queue_def_t* qdef)
{
    assert(qdef != NULL);

    qdef->front = 0;
    qdef->tail = 0;

    sema_create(&qdef->smphr, 0);

    return (osal_queue_t)qdef;
}

TU_ATTR_ALWAYS_INLINE
static inline bool osal_queue_send(osal_queue_t qhdl, void const * data, bool in_isr)
{
    assert(qhdl != NULL);
    assert(data != NULL);

    if (sema_get_value(&qhdl->smphr) == qhdl->depth) {
        /* queue is full */
        if (in_isr) {
            /* return in case of ISR */
            return false;
        }
        /* We do not block the sending thread when `osal_queue_send` is called
         * from a thread context and the queue is full. The call of function
         * `osal_queue_send` is usually interrupt-driven and must not block
         * anyway. There is only one exception in `src/class/msc/msc_device.c`
         * where it is called in thread context. However, since the call of
         * `osal_queue_send` would then be made in the same thread context as
         * the call of the unlocking function `osal_queue_receive`, using a
         * mutex to block the sending thread would not work here anyway.
         * Therefore, we return with false in all cases when `NDEBUG` is
         * defined, as most other implementations of `osal_queue_send` do,
         * or point out the problem with assertion. */
#ifdef NDEBUG
        return false;
#else
        assert(0);
#endif
    }

    /* copy the data to the queue item data */
    memcpy(qhdl->buffer + (qhdl->tail * (qhdl->size)), data, qhdl->size);
    /* update write pointer */
    qhdl->tail = (qhdl->tail + 1) % qhdl->depth;

    /* unlock a possibly waiting receiving thread */
    sema_post(&qhdl->smphr);

    return true;
}

#if TINYUSB_OSAL_MUTEX_CANCELABLE
static void _osal_queue_lock_timeout(void *arg)
{
    mutex_cancel(arg);
}
#endif

TU_ATTR_ALWAYS_INLINE
static inline bool osal_queue_receive(osal_queue_t qhdl, void* data, uint32_t msec)
{
    assert(qhdl != NULL);
    assert(data != NULL);

    (void)msec;
    /* In RIOT we use only `tusd_task` and `tush_task` functions which call
     * `osal_queue_receive` with `OSAL_TIMEOUT_WAIT_FOREVER` (`UINT32_MAX`).
     * Therefore we do not use `msec` and just call `sema_wait` without timeout
     * handling here. */
    if (sema_wait(&qhdl->smphr) != 0) {
        /* timeout error or any other semaphore error on receiving */
        return false;
    }

    /* at least one item should be in the queue now */
    assert(qhdl->front != qhdl->tail);

    /* copy data from queue item data */
    memcpy(data, qhdl->buffer + (qhdl->front * (qhdl->size)), qhdl->size);
    /* update read pointer */
    qhdl->front = (qhdl->front + 1) % qhdl->depth;

    return true;
}

TU_ATTR_ALWAYS_INLINE static inline bool osal_queue_empty(osal_queue_t qhdl)
{
    assert(qhdl != NULL);
    return sema_get_value(&qhdl->smphr) == 0;
}

#ifdef __cplusplus
}
#endif

#endif /* !DOXYGEN */
#endif /* TUSB_OS_CUSTOM_H */
/** @} */
