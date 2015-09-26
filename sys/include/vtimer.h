/*
 * Copyright (C) 2009, 2014, 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2013, 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup  sys_vtimer Virtual Timer
 * @ingroup   sys
 * @brief     Provides a high level abstraction timer module to register
 *            timers, get current system time, and let a thread sleep for a certain amount
 *            of time. It does not give any timing guarantees.
 *
 * @deprecated
 *
 * @{
 * @file
 * @brief     vtimer interface definitions
 * @author Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef VTIMER_H
#define VTIMER_H

#include "msg.h"
#include "timex.h"
#include "xtimer.h"

#include <time.h>
#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief IPC message type for vtimer msg callback
 */
#define MSG_TIMER 12345

/**
 * @brief A vtimer object.
 *
 * This structure is used for declaring a vtimer. This should not be used by
 * programmers, use the vtimer_set_*-functions instead.
 *
 * \hideinitializer
 * @deprecated
 */
typedef struct vtimer {
    xtimer_t timer;
    msg_t msg;
} vtimer_t;

/**
 * @brief   Current system time
 * @return  Time as timex_t since system boot
 * @deprecated
 */
void vtimer_now(timex_t *out);

/**
 * @brief   will cause the calling thread to be suspended from excecution until the time specified by time has elapsed
 * @param[in]   time    timex_t with time to suspend execution
 * @return      0 on success, < 0 on error
 * @deprecated
 */
int vtimer_sleep(timex_t time);

/**
 * @brief   receive a message but return in case of timeout time is passed by without a new message
 * @param[out]   m           pointer to a msg_t which will be filled in case of no timeout
 * @param[in]    timeout     timex_t containing the relative time to fire the timeout
 * @return       < 0 on error, other value otherwise
 * @deprecated
 */
int vtimer_msg_receive_timeout(msg_t *m, timex_t timeout);

/**
 * @brief   Returns the current time in broken down format
 * @param[out]  localt      Pointer to structure to receive time
 * @deprecated
 */
void vtimer_get_localtime(struct tm *localt);

/**
 * @brief   set a vtimer with msg event handler of the specified type
 * @param[in]   t           pointer to preinitialised vtimer_t
 * @param[in]   interval    vtimer timex_t interval
 * @param[in]   pid         process id
 * @param[in]   type        value for the msg_t type
 * @param[in]   ptr         message value
 * @deprecated
 */
void vtimer_set_msg(vtimer_t *t, timex_t interval, kernel_pid_t pid, uint16_t type, void *ptr);

/**
 * @brief   set a vtimer with wakeup event
 * @param[in]   t           pointer to preinitialised vtimer_t
 * @param[in]   interval    the interval after which the timer shall fire
 * @param[in]   pid         process id
 * @return      0 on success, < 0 on error
 */
int vtimer_set_wakeup(vtimer_t *t, timex_t interval, kernel_pid_t pid);

/**
 * @brief Get the current time in seconds and microseconds since system start
 * @param[in] tp    Uptime will be stored in the timeval structure pointed to by tp
 */
void vtimer_gettimeofday(struct timeval *tp);

/**
 * @brief   remove a vtimer
 * @param[in]   t           pointer to preinitialised vtimer_t
 * @deprecated
 */
void vtimer_remove(vtimer_t *t);

/**
 * @brief   will cause the calling thread to be suspended from excecution until the number of microseconds has elapsed
 * @param[in]   us          number of microseconds
 * @return      0 on success, < 0 on error
 */
static inline int vtimer_usleep(uint32_t us)
{
    xtimer_usleep(us);
    return 0;
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* VTIMER_H */
