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

/**
 * A vtimer object.
 *
 * This structure is used for declaring a vtimer. This should not be used by programmers, use the vtimer_set_*-functions instead.
 *
 * \hideinitializer
 */
typedef struct vtimer_t {
    priority_queue_node_t priority_queue_entry;
    timex_t absolute;
    void (*action)(struct vtimer_t *timer);
    void *arg;
    kernel_pid_t pid;
} vtimer_t;

/**
 * @brief   Current system time
 * @return  Time as timex_t since system boot
 */
void vtimer_now(timex_t *out);

/**
 * @brief Get the current time in seconds and microseconds since system start
 * @param[in] tp    Uptime will be stored in the timeval structure pointed to by tp
 */
void vtimer_gettimeofday(struct timeval *tp);

/**
 * @brief   Returns the current time in broken down format
 * @param[out]  localt      Pointer to structure to receive time
 */
void vtimer_get_localtime(struct tm *localt);

/**
 * @brief   Initializes the vtimer subsystem. To be called once at system initialization. Will be initialized by auto_init.
 *
 * @return  always 0
 */
int vtimer_init(void);

/**
 * @brief   will cause the calling thread to be suspended from excecution until the number of microseconds has elapsed
 * @param[in]   us          number of microseconds
 * @return      0 on success, < 0 on error
 */
int vtimer_usleep(uint32_t us);

/**
 * @brief   will cause the calling thread to be suspended from excecution until the time specified by time has elapsed
 * @param[in]   time    timex_t with time to suspend execution
 * @return      0 on success, < 0 on error
 */
int vtimer_sleep(timex_t time);

/**
 * @brief   set a vtimer with msg event handler
 * @param[in]   t           pointer to preinitialised vtimer_t
 * @param[in]   interval    vtimer timex_t interval
 * @param[in]   pid         process id
 * @param[in]   ptr         message value
 * @return      0 on success, < 0 on error
 */
int vtimer_set_msg(vtimer_t *t, timex_t interval, kernel_pid_t pid, void *ptr);

/**
 * @brief   set a vtimer with wakeup event
 * @param[in]   t           pointer to preinitialised vtimer_t
 * @param[in]   pid         process id
 * @return      0 on success, < 0 on error
 */
int vtimer_set_wakeup(vtimer_t *t, timex_t interval, kernel_pid_t pid);

/**
 * @brief   remove a vtimer
 * @param[in]   t           pointer to preinitialised vtimer_t
 * @return      0 on success, < 0 on error
 */
int vtimer_remove(vtimer_t *t);


/**
 * @brief   receive a message but return in case of timeout time is passed by without a new message
 * @param[out]   m           pointer to a msg_pulse_t which will be filled in case of no timeout
 * @param[in]    timeout     timex_t containing the relative time to fire the timeout
 * @return       < 0 on error, other value otherwise
 */
int vtimer_msg_receive_timeout(msg_pulse_t *m, timex_t timeout);

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
