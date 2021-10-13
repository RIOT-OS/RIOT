/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_nimble
 * @{
 *
 * @file
 * @brief       Mynewt-Nimble Porting layer wrappers
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#ifndef NIMBLE_NIMBLE_NPL_OS_H
#define NIMBLE_NIMBLE_NPL_OS_H

#include <stdint.h>
#include <stdbool.h>
#include "os/os.h"
#include "mcu/mcu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    BLE NPL layer macros
 * @{
 */
#define BLE_NPL_OS_ALIGNMENT    (OS_ALIGNMENT)
#define BLE_NPL_TIME_FOREVER    (OS_WAIT_FOREVER)
/** @} */

/**
 * @brief time type
 */
typedef uint32_t ble_npl_time_t;
/**
 * @brief time type
 */
typedef int32_t ble_npl_stime_t;

/**
 * @brief ble_npl event wrapper
 */
struct ble_npl_event {
    struct os_event ev;     /**< the event */
};

/**
 * @brief ble_npl event queue wrapper
 */
struct ble_npl_eventq {
    struct os_eventq evq;   /**< the event queue */
};

/**
 * @brief ble_npl callout wrapper
 */
struct ble_npl_callout {
    uint32_t ticks;         /**< the callout set timeout */
    struct os_callout co;   /**< the callout */
};

/**
 * @brief ble_npl mutex wrapper
 */
struct ble_npl_mutex {
    struct os_mutex mu;     /**< mutex */
};

/**
 * @brief ble_npl semaphore wrapper
 */
struct ble_npl_sem {
    struct os_sem sem;      /**< semaphore */
};

/**
 * @brief   Not used in RIOT
 *
 * @return  Always true
 */
static inline bool ble_npl_os_started(void)
{
    return true;
}

/**
 * @brief   Init a event
 *
 * @param[in]   ev      pointer to event to set
 * @param[in]   fn      event callback function
 * @param[in]   arg     event argument
 */
static inline void ble_npl_event_init(struct ble_npl_event *ev, ble_npl_event_fn *fn,
                                      void *arg)
{
    os_event_init(&ev->ev, (os_event_fn *)fn, arg);
}

/**
 * @brief   Check if event is in queue
 *
 * @param[in]   ev      event to check
 *
 * @return  true if event is queues, false otherwise
 */
static inline bool ble_npl_event_is_queued(struct ble_npl_event *ev)
{
    return os_event_is_queued(&ev->ev);
}

/**
 * @brief   Runs an event
 *
 * @param[in]   ev      event to run
 */
static inline void *ble_npl_event_get_arg(struct ble_npl_event *ev)
{
    return os_event_get_arg(&ev->ev);
}

/**
 * @brief   Set the vent arg
 *
 * @param[in]   ev      event
 * @param[in]   arg     arg to set event
 */
static inline void ble_npl_event_set_arg(struct ble_npl_event *ev, void *arg)
{
    os_event_set_arg(&ev->ev, arg);
}

/**
 * @brief   Runs an event
 *
 * @param[in]   ev      event to run
 */
static inline void ble_npl_event_run(struct ble_npl_event *ev)
{
    os_event_run(&ev->ev);
}

/**
 * @brief   Initialize the event queue
 *
 * @param[in]   evq     The event queue to initialize
 */
static inline void ble_npl_eventq_init(struct ble_npl_eventq *evq)
{
    os_eventq_init(&evq->evq);
}

/**
 * @brief   Check whether the event queue is initialized.
 *
 * @param[in]   evq     the event queue to check
 */
static inline int ble_npl_eventq_inited(struct ble_npl_eventq *evq)
{
    return os_eventq_inited(&evq->evq);
}

/**
 * @brief   Deinitialize an event queue
 *
 * @note    Not supported in RIOT
 *
 * @param[in]   evq     the event queue to deinit
 */
static inline void ble_npl_eventq_deinit(struct ble_npl_eventq *evq)
{
    (void)evq;
    /* Can't deinit an eventq in RIOT */
}

/**
 * @brief   Get next event from event queue, blocking.
 *
 * @param[in]   evq     the event queue to pull an event from
 * @param[in]   tmo     timeout, NPL_TIME_FOREVER to block, 0 to return immediately
 *
 * @return  the event from the queue
 */
static inline struct ble_npl_event *ble_npl_eventq_get(struct ble_npl_eventq *evq,
                                                       ble_npl_time_t tmo)
{
    return (struct ble_npl_event *)os_eventq_get(&evq->evq, tmo);
}

/**
 * @brief   Get next event from event queue, non-blocking
 *
 * @param[in]   evq     the event queue to pull an event from
 *
 * @return  event from the queue, or NULL if none available.
 */
static inline struct ble_npl_event *ble_npl_eventq_get_no_wait(struct ble_npl_eventq *evq)
{
    return (struct ble_npl_event *)os_eventq_get_no_wait(&evq->evq);
}

/**
 * @brief   Put an event on the event queue.
 *
 * @param[in]   evq     event queue
 * @param[in]   ev      event to put in queue
 */
static inline void ble_npl_eventq_put(struct ble_npl_eventq *evq, struct ble_npl_event *ev)
{
    os_eventq_put(&evq->evq, &ev->ev);
}

/**
 * @brief   Remove an event from the queue.
 *
 * @param[in]   evq     event queue to remove the event from
 * @param[in]   ev      event to remove from the queue
 */
static inline void ble_npl_eventq_remove(struct ble_npl_eventq *evq, struct ble_npl_event *ev)
{
    os_eventq_remove(&evq->evq, &ev->ev);
}

/**
 * @brief   Gets and runs an event from the queue callback.
 *
 * @param[in]   evq     The event queue to pull the item off.
 */
static inline void ble_npl_eventq_run(struct ble_npl_eventq *evq)
{
    os_eventq_run(&evq->evq);
}

/**
 * @brief   Check if queue is empty
 *
 * @param[in]   evq     the event queue to check
 *
 * @return      true    if empty, false otherwise
 */
static inline bool ble_npl_eventq_is_empty(struct ble_npl_eventq *evq)
{
    return os_eventq_is_empty(&evq->evq);
}

/**
 * @brief Initializes a mutex object.
 *
 * @param[out]  mu  pre-allocated mutex structure, must not be NULL.
 */
static inline ble_npl_error_t ble_npl_mutex_init(struct ble_npl_mutex *mu)
{
    return (ble_npl_error_t)os_mutex_init(&mu->mu);
}

/**
 * @brief Pend (wait) for a mutex.
 *
 * @param[in]   mu      Pointer to mutex.
 * @param[in]   timeout Timeout, in os ticks.
 *                A timeout of 0 means do not wait if not available.
 *                A timeout of OS_TIMEOUT_NEVER means wait forever.
 *
 * @return ble_npl_error_t
 *      BLE_NPL_INVALID_PARM    mutex passed in was NULL
 *      BLE_NPL_OK              no error
 */
static inline ble_npl_error_t ble_npl_mutex_pend(struct ble_npl_mutex *mu, ble_npl_time_t timeout)
{
    return (ble_npl_error_t)os_mutex_pend(&mu->mu, timeout);
}

/**
 *
 * @brief Release a mutex.
 *
 * @return ble_npl_error_t
 *      BLE_NPL_INVALID_PARM    mutex was NULL
 *      BLE_NPL_OK              no error
 */
static inline ble_npl_error_t ble_npl_mutex_release(struct ble_npl_mutex *mu)
{
    return (ble_npl_error_t)os_mutex_release(&mu->mu);
}

/**
 * @brief Initialize a semaphore
 *
 * @param[in]   sem     pointer to semaphore
 * @param[in]   tokens  # of tokens the semaphore should contain initially.
 *
 * @return ble_npl_error_t
 *         BLE_NPL_INVALID_PARM     Semaphore passed in was NULL.
 *         BLE_NPL_OK               no error.
 */
static inline ble_npl_error_t ble_npl_sem_init(struct ble_npl_sem *sem, uint16_t tokens)
{
    return (ble_npl_error_t)os_sem_init(&sem->sem, tokens);
}

/**
 * @brief Pend (wait) for a semaphore.
 *
 * @param[in]   sem     pointer to semaphore.
 * @param[in]   timeout timeout, in os ticks.
 *                A timeout of 0 means do not wait if not available.
 *                A timeout of BLE_NPL_TIMEOUT_NEVER means wait forever.
 *
 *
 * @return ble_npl_error_t
 *      BLE_NPL_INVALID_PARM     semaphore passed in was NULL.
 *      BLE_NPL_TIMEOUT          semaphore was owned by another task and timeout=0
 *      BLE_NPL_OK               no error
 */
static inline ble_npl_error_t ble_npl_sem_pend(struct ble_npl_sem *sem, ble_npl_time_t timeout)
{
    return (ble_npl_error_t)os_sem_pend(&sem->sem, timeout);
}

/**
 * @brief Release a semaphore.
 *
 * @param[in]   sem     pointer to the semaphore to be released
 *
 * @return ble_npl_error_t
 *      BLE_NPL_INVALID_PARM    semaphore passed in was NULL.
 *      BLE_NPL_OK              no error
 */
static inline ble_npl_error_t ble_npl_sem_release(struct ble_npl_sem *sem)
{
    return (ble_npl_error_t)os_sem_release(&sem->sem);
}

/**
 * @brief Get current semaphore's count
 */
static inline uint16_t ble_npl_sem_get_count(struct ble_npl_sem *sem)
{
    return os_sem_get_count(&sem->sem);
}

/**
 * @brief   Initialize a callout.
 *
 * Callouts are used to schedule events in the future onto an event
 * queue. Callout timers are scheduled using the ble_npl_callout_reset()
 * function.  When the timer expires, an event is posted to the event
 * queue specified in ble_npl_callout_init(). The event argument given here
 * is posted in the ev_arg field of that event.
 *
 * @param[out]  c       callout to initialize
 * @param[in]   q       event queue to queue event in
 * @param[in]   e_cb    callback function
 * @param[in]   e_arg   callback function argument
 */
static inline void ble_npl_callout_init(struct ble_npl_callout *c, struct ble_npl_eventq *q,
                                        ble_npl_event_fn *e_cb, void *e_arg)
{
    os_callout_init(&c->co, &q->evq, (os_event_fn *)e_cb, e_arg);
}

/**
 * @brief   Reset the callout to fire off in 'ticks' ticks.
 *
 * @param[in]   c       callout to reset
 * @param[in]   ticks   number of ticks to wait before posting an event
 *
 * @return 0 on success, non-zero on failure
 */
static inline ble_npl_error_t ble_npl_callout_reset(struct ble_npl_callout *c, ble_npl_time_t ticks)
{
    uint32_t state = os_hw_enter_critical();

    c->ticks = ztimer_now(ZTIMER_MSEC) + ticks;
    os_callout_reset(&c->co, ticks);
    os_hw_exit_critical(state);
    return BLE_NPL_OK;
}

/**
 * @brief   Stops the callout from firing.
 *
 * @param[in]   c   the callout to stop
 */
static inline void ble_npl_callout_stop(struct ble_npl_callout *c)
{
    os_callout_stop(&c->co);
}

/**
 * @brief   Check if callout is active
 *
 * @param[in]   c   the callout to check
 *
 * @return true if active, false otherwise
 */
static inline bool ble_npl_callout_is_active(struct ble_npl_callout *c)
{
    return ztimer_is_set(ZTIMER_MSEC, &c->co.timer);
}

/**
 * @brief   Get the callout set ticks
 *
 * @param[in]   co   the callout to check
 */
static inline ble_npl_time_t ble_npl_callout_get_ticks(struct ble_npl_callout *co)
{
    return co->ticks;
}

/**
 * @brief   Get the remaining ticks for callout expire
 *
 * @param[in]   co      the callout to check
 * @param[in]   time    ignored
 *
 * @return      remaining ticks
 */
static inline ble_npl_time_t ble_npl_callout_remaining_ticks(struct ble_npl_callout *co,
                                                             ble_npl_time_t time)
{
    (void)time;
    ztimer_now_t now = ztimer_now(ZTIMER_MSEC);
    return (ble_npl_time_t)(co->ticks - now);
}

/**
 * @brief   Set the callout event argument
 *
 * @param[in]   co      the callout
 * @param[in]   arg   callback function argument
 */
static inline void ble_npl_callout_set_arg(struct ble_npl_callout *co, void *arg)
{
    co->co.c_ev.arg = arg;
}

/**
 * @brief Returns the low 32 bits of cputime.
 *
 * @return uint32_t The lower 32 bits of cputime
 */
static inline ble_npl_time_t ble_npl_time_get(void)
{
    return os_time_get();
}

/**
 * @brief Converts the given number of milliseconds into cputime ticks.
 *
 * @param[in]   ms          The number of milliseconds to convert to ticks
 * @param[out]  out_ticks   The number of ticks corresponding to 'ms'
 *
 * @return ble_npl_error_t  BLE_NPL_OK - no error
 */
static inline ble_npl_error_t ble_npl_time_ms_to_ticks(uint32_t ms, ble_npl_time_t *out_ticks)
{
    return (ble_npl_error_t)os_time_ms_to_ticks(ms, out_ticks);
}

/**
 * @brief Convert the given number of ticks into milliseconds.
 *
 * @param[in]   ticks   The number of ticks to convert to milliseconds.
 * @param[out]  out_ms  The converted milliseconds from 'ticks'
 *
 * @return ble_npl_error_t  BLE_NPL_OK - no error
 */
static inline ble_npl_error_t  ble_npl_time_ticks_to_ms(ble_npl_time_t ticks, uint32_t *out_ms)
{
    return (ble_npl_error_t)os_time_ticks_to_ms(ticks, out_ms);
}

/**
 * @brief   Converts the given number of milliseconds into cputime ticks.
 *
 * @param[in]   ms  The number of milliseconds to convert to ticks
 *
 * @return  uint32_t    The number of ticks corresponding to 'ms'
 */
static inline ble_npl_time_t ble_npl_time_ms_to_ticks32(uint32_t ms)
{
    return os_time_ms_to_ticks32(ms);
}

/**
 * @brief   Convert the given number of ticks into milliseconds.
 *
 * @param[in]   ticks   The number of ticks to convert to milliseconds.
 *
 * @return  uint32_t    The number of milliseconds corresponding to 'ticks'
 */
static inline ble_npl_time_t ble_npl_time_ticks_to_ms32(ble_npl_time_t ticks)
{
    return os_time_ticks_to_ms32(ticks);
}
/**
 * @brief   Wait until the number of ticks has elapsed, BLOICKING.
 *
 * @param[in]   ticks   The number of ticks to wait.
 */
static inline void ble_npl_time_delay(ble_npl_time_t ticks)
{
    return os_time_delay(ticks);
}

/**
 * @brief   Disable ISRs
 *
 * @return  current isr context
 */
static inline uint32_t ble_npl_hw_enter_critical(void)
{
    return os_hw_enter_critical();
}

/**
 * @brief   Restores ISR context
 *
 * @param[in]   ctx      ISR context to restore.
 */
static inline void ble_npl_hw_exit_critical(uint32_t ctx)
{
    os_hw_exit_critical(ctx);
}

/**
 * @brief Check if is in critical section
 *
 * @return true, if in critical section, false otherwise
 */
static inline bool ble_npl_hw_is_in_critical(void)
{
    return os_hw_is_in_critical();
}

/**
 * @brief Return current thread PID
 *
 * @return PID
 */
static inline void *ble_npl_get_current_task_id(void)
{
    return (void *)(uint32_t)thread_getpid();
}

/**
 * @brief   Set nrf5x radio ISR callback
 *
 * @param[in]   irqn    IRQ number
 * @param[in]   addr    the ISR callback
 */
static inline void ble_npl_hw_set_isr(int irqn, void (*addr)(void))
{
    nrf5x_hw_set_isr(irqn, addr);
}

#ifdef __cplusplus
}
#endif

#endif /* NIMBLE_NIMBLE_NPL_OS_H */
