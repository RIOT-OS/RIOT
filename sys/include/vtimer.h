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

int vtimer_set(vtimer_t *timer, timex_t *offset);
timex_t vtimer_now();
int vtimer_usleep(uint32_t usecs); 

#endif /* __VTIMER_H */
