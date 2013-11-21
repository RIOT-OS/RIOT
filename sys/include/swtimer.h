/** \addtogroup system
 * @{ */

/**
 * \defgroup swtimer Software Timer library
 *
 * The swtimer library provides functions for setting, resetting and restarting
 * software timers, and for checking if a swtimer has expired.
 *
 * @{
 */

/**
 * \file
 * Timer library header file.
 */

#ifndef __SWTIMER_H__
#define __SWTIMER_H__

#include <stdint.h>

#define MSG_TIMER 12345

#define SWTIMER_WAKEUP 0
#define SWTIMER_MSG 1

#undef wakeup

typedef uint32_t swtime_t;

/**
 * A swtimer.
 *
 * This structure is used for declaring a swtimer. The swtimer must be set
 * with swtimer_set() before it can be used.
 *
 * \hideinitializer
 */
typedef struct swtimer_t {
    swtime_t start;
    swtime_t interval;

    struct swtimer_t *next;

    int action_type;
    union {
        struct {
            int pid;
        } wakeup;
        struct {
            unsigned int value;
            int target_pid;
        } msg;
    } action;
} swtimer_t;

/**
 * @brief   Current system time
 * @return  Time in ticks since system boot
 */
swtime_t swtimer_now(void);

/**
 * @brief   Initializes swtimer
 * @return  always 0
 */
int swtimer_init(void);

/**
 * @brief   set swtimer interval and activate 
 * @param[in]   t           pointer to preinitialised swtimer_t
 * @param[in]   interval    swtimer interval
 * @return  always 0
 */
int swtimer_set(swtimer_t *t, swtime_t interval);

/**
 * @brief   reset swtimer
 * @param[in]   t           pointer to preinitialised swtimer_t 
 */
void swtimer_reset(swtimer_t *t);

/**
 * @brief   restart swtimer
 * @param[in]   t           pointer to preinitialised swtimer_t 
 */
void swtimer_restart(swtimer_t *t);

/**
 * @brief   check if swtimer is expired
 * @param[in]   t           pointer to preinitialised swtimer_t
 * @return  
 */
int swtimer_expired(swtimer_t *t);

/**
 * @brief   will cause the calling thread to be suspended from excecution until the number of microseconds has elapsed
 * @param[in]   us          number of microseconds 
 * @return      0 on success, < 0 on error
 */
int swtimer_usleep(swtime_t us);

/**
 * @brief   set a swtimer with msg event handler
 * @param[in]   t           pointer to preinitialised swtimer_t
 * @param[in]   interval    swtimer interval
 * @param[in]   pid         process id
 * @param[in]   ptr         message value
 * @return      0 on success, < 0 on error
 */
int swtimer_set_msg(swtimer_t *t, swtime_t interval, int pid, void *ptr);

/**
 * @brief   set a swtimer with wakeup event
 * @param[in]   t           pointer to preinitialised swtimer_t
 * @param[in]   pid         process id
 * @return      0 on success, < 0 on error
 */
int swtimer_set_wakeup(swtimer_t *t, swtime_t interval, int pid);

/**
 * @brief   remove a swtimer
 * @param[in]   t           pointer to preinitialised swtimer_t
 * @return      0 on success, < 0 on error
 */
int swtimer_remove(swtimer_t *t);

#endif /* __SWTIMER_H__ */

/** @} */
/** @} */
