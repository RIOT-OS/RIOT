/** \addtogroup system
 * @{ */

/**
 * \defgroup vtimer Virtual (Software) Timer library
 *
 * The vtimer library provides functions for setting, resetting and restarting
 * software timers, and for checking if a vtimer has expired.
 *
 * (As of now, not resetting, restarting, removing and checking are not implemented)
 *
 * @{
 */

/**
 * \file
 * Timer library header file.
 */
#ifndef __VTIMER_H
#define __VTIMER_H

#include "queue.h"
#include "timex.h"

#define MSG_TIMER 12345

/**
 * A vtimer object.
 *
 * This structure is used for declaring a vtimer. This should not be used by programmers, use the vtimer_set_*-functions instead.
 *
 * \hideinitializer
 */
typedef struct vtimer_t {
    queue_node_t queue_entry;
    timex_t absolute;
    void(*action)(void *);
    void *arg;
    unsigned int pid;
} vtimer_t;

/**
 * @brief   Current system time
 * @return  Time as timex_t since system boot
 */
void vtimer_now(timex_t *out);

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
int vtimer_set_msg(vtimer_t *t, timex_t interval, unsigned int pid, void *ptr);

/**
 * @brief   set a vtimer with wakeup event
 * @param[in]   t           pointer to preinitialised vtimer_t
 * @param[in]   pid         process id
 * @return      0 on success, < 0 on error
 */
int vtimer_set_wakeup(vtimer_t *t, timex_t interval, int pid);

/**
 * @brief   remove a vtimer
 * @param[in]   t           pointer to preinitialised vtimer_t
 * @return      0 on success, < 0 on error
 */
int vtimer_remove(vtimer_t *t);

/**
 * @brief Prints a vtimer_t
 */
void vtimer_print(vtimer_t *t);

#endif /* __VTIMER_H */
