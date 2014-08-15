/**
 * @defgroup    sys_vtimer Virtual (Software) Timer library
 * @ingroup  sys
 * @brief       Virtual software timer for general timer functionality in applications
 * @{
 *
 * @file        vtimer.h
 * @brief       VTimer API
 *
 * The vtimer library provides functions for setting, resetting and restarting
 * software timers, and for checking if a vtimer has expired.
 *
 * (As of now, not resetting, restarting, removing and checking are not implemented)
 *
 * @author      unknown
 */

#ifndef __VTIMER_H
#define __VTIMER_H

#include <time.h>
#include <sys/time.h>

#include "priority_queue.h"
#include "timex.h"
#include "msg.h"

#define MSG_TIMER 12345

struct vtimer_t;

/**
 * @brief           Prototype of a vtimer callback function.
 * @param[in]       The calling vtimer.
 */
typedef void (*vtimer_action_t)(struct vtimer_t *timer);

/**
 * @brief           A vtimer token.  The structure is used for setting a vtimer.
 * @details         This should not be used by programmers directly, use the vtimer_set_*-functions instead.
 * @note            The token remains the callers responsibility until the event fired.
 *                  If you declare a vtimer token on the stack, then you need to call vtimer_remove(),
 *                  if the event did not fire yet.
 * @hideinitializer
 */
typedef struct vtimer_t {
    priority_queue_node_t priority_queue_entry;
    timex_t absolute;
    vtimer_action_t action;
    void *arg;
    kernel_pid_t pid;
} vtimer_t;

/**
 * @brief           Values to tell if a `timex_t timestamp` is a relative or absolute timestamp.
 */
typedef enum vtimer_absolute {
    VTIMER_RELATIVE, /**< The timestamp is relative to the current wallclock time. */
    VTIMER_ABSOLUTE, /**< The timestamp is an absolute value. Epoch is vtimer_now(). */
} vtimer_absolute_t;

/**
 * @brief           The epoch of the vtimer.
 * @param[out]      out   Time since system boot / vtimer initialization.
 */
void vtimer_now(timex_t *out);

/**
 * @brief Get the current time in seconds and microseconds since system start
 * @param[in] tp    Uptime will be stored in the timeval structure pointed to by tp
 */
void vtimer_gettimeofday(struct timeval *tp);

/**
 * @brief           Returns the current time in broken down format
 * @details         This function only fills the fields `tm_sec`, `tm_min` and `tm_hour`.
 *                  Use tm_fill_derived_values() to fill the other fields.
 * @param[out]      localt   Pointer to structure to receive time
 */
void vtimer_get_localtime(struct tm *localt);

/**
 * @brief           Initializes the vtimer subsystem.
 * @details         To be called once at system initialization. Will be initialized by auto_init.
 * @return          always 0
 */
int vtimer_init(void);

/**
 * @brief           Let the current thread sleep for a given amount of microseconds.
 * @note            You must not call this function in a ISR.
 * @note            You must call this function with interrupts enabled.
 * @param[in]       us   Number of microseconds to sleep.
 * @returns         @arg `= 0`, if the sleeping was successful.
 *                  @arg `> 0`, if the thread was woken up spuriously.
 *                  @arg `< 0`, if the vtimer refused to handle this request.
 */
int vtimer_usleep(uint32_t us);

/**
 * @brief           Let the current thread sleep until a given timestamp.
 * @note            You must not call this function in a ISR.
 *                  You must call this function with interrupts enabled.
 * @param[in]       timestamp   Relative or absolute timestamp when to wake up.
 * @param[in]       absolute    Whether the timestamp is a relative or an absolute value.
 * @returns         @arg `= 0`, if the sleeping was successful.
 *                  @arg `> 0`, if the thread was woken up spuriously.
 *                  @arg `< 0`, if the vtimer refused to handle this request.
 */
int vtimer_sleep(timex_t timestamp, vtimer_absolute_t absolute);

/**
 * @brief           Schedule a vtimer callback.
 * @note            The callback will happen inside the ISR.
 * @details         This function should be strictly avoided in application code.
 *                  This function is used by `vtimer_set_…()`, call use functions instead.
 * @param[in,out]   t           vtimer token, keeping it remains the callers responsibility.
 * @param[in]       timestamp   Relative or absolute timestamp when to send the message.
 * @param[in]       absolute    Whether the timestamp is a relative or an absolute value.
 * @param[in]       pid         New value for `t->pid`.
 * @param[in]       ptr         New value for `t->arg`.
 * @param[in]       callback    Function to call once the interval has elapsed.
 * @returns         @arg `= 0`, if the timer was scheduled successfully.
 *                  @arg `!= 0`, otherwise.
 */
int __vtimer_set_callback(vtimer_t *t, timex_t timestamp, vtimer_absolute_t absolute,
                          kernel_pid_t pid, void *ptr, vtimer_action_t callback);

/**
 * @brief           Send a message to a thread after a given timestamp.
 * @details         You may call this function in an ISR and/or with interrupts disabled.
 * @details         The target thread will receive a message with `msg.type = MSG_TIMER` and `msg.content.ptr = arg`.
 * @param[in,out]   t           vtimer token, keeping it remains the callers responsibility.
 * @param[in]       timestamp   Relative or absolute timestamp when to send the message.
 * @param[in]       absolute    Whether the timestamp is a relative or an absolute value.
 * @param[in]       pid         Target thread for the message.
 * @param[in]       ptr         The value for `msg.content.ptr`.
 * @returns         `0` on success, `!= 0` on error.
 */
static inline int vtimer_set_msg(vtimer_t *t, timex_t timestamp, vtimer_absolute_t absolute, kernel_pid_t pid, void *ptr)
{
    extern void __vtimer_callback_msg(vtimer_t *timer);
    return __vtimer_set_callback(t, timestamp, absolute, pid, ptr, __vtimer_callback_msg);
}

/**
 * @brief           Wake up a thread after a given timestamp.
 * @details         You may call this function in an ISR and/or with interrupts disabled.
 * @param[in,out]   t           vtimer token, keeping it remains the callers responsibility.
 * @param[in]       timestamp   Relative or absolute timestamp when to send the message.
 * @param[in]       absolute    Whether the timestamp is a relative or an absolute value.
 * @param[in]       pid         Target thread to wake up, e.g. the current thread is `sched_active_pid`.
 * @returns         `0` on success, `!= 0` on error.
 */
static inline int vtimer_set_wakeup(vtimer_t *t, timex_t timestamp, vtimer_absolute_t absolute, kernel_pid_t pid, int *spurious)
{
    extern void __vtimer_callback_wakeup(vtimer_t *timer);
    return __vtimer_set_callback(t, timestamp, absolute, pid, spurious, __vtimer_callback_wakeup);
}

/**
 * @brief           Unschedule a vtimer event.
 * @param[in,out]   t   Pointer to the vtimer token that was previously supplied to e.g. vtimer_set_msg().
 */
void vtimer_remove(vtimer_t *t);

/**
 * @brief           Try to receive a message until a given timestamp.
 * @note            You must not call this function in a ISR.
 *                  You must call this function with interrupts enabled.
 * @details         See msg_receive() for further information about messaging.
 * @param[out]      The received message. The result is random if no message was received in time.
 * @param[in]       timestamp   Relative or absolute timestamp when to send the message.
 * @param[in]       absolute    Whether the timestamp is a relative or an absolute value.
 * @return          @arg `= 0` if a message was received.
 *                  @arg `> 0` on timeout.
 *                  @arg `< 0` on error.
 */
int vtimer_msg_receive_timeout(msg_t *m, timex_t timestamp, vtimer_absolute_t absolute);

#if ENABLE_DEBUG

/**
 * @brief Prints a vtimer_t
 */
void vtimer_print(vtimer_t *t);

/**
 * @brief Prints the vtimer shortterm queue (use for debug purposes)
 */
void vtimer_print_short_queue(void);

/**
 * @brief Prints the vtimer longterm queue (use for debug purposes)
 */
void vtimer_print_long_queue(void);

#endif

/** @} */
#endif /* __VTIMER_H */
