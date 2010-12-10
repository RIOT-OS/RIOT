#ifndef __VTIMER_H
#define __VTIMER_H 

#include <queue.h>
#include <timex.h>

typedef struct vtimer_t {
    queue_node_t queue_entry;
    timex_t absolute;
    void(*action)(void*);
    void* arg;
} vtimer_t;

int vtimer_init();

timex_t vtimer_now();

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
int vtimer_set_msg(vtimer_t *t, timex_t interval, int pid, void *ptr);

/**
 * @brief   set a vtimer with wakeup event
 * @param[in]   t           pointer to preinitialised vtimer_t
 * @param[in]   pid         process id
 * @return      0 on success, < 0 on error
 */
int vtimer_set_wakeup(vtimer_t *t, timex_t interval, int pid);

/**
 * @brief   set a vtimer with callback function event handler 
 * @param[in]   t           pointer to preinitialised vtimer_t
 * @param[in]   interval    vtimer interval
 * @param[in]   f_ptr       pointer to callback function
 * @return      0 on success, < 0 on error
 */
int vtimer_set_cb(vtimer_t *t, timex_t interval, void (*f_ptr)(void *), void *ptr);

/**
 * @brief   remove a vtimer
 * @param[in]   t           pointer to preinitialised vtimer_t
 * @return      0 on success, < 0 on error
 */
int vtimer_remove(vtimer_t *t);

#endif /* __VTIMER_H */
