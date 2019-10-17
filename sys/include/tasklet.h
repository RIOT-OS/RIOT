



#include <stddef.h>

#include "event.h"


typedef struct {
    uint8_t prio;
    char *stack;
    size_t stack_size;
} tasklet_params_t;


void auto_init_tasklet(void);
event_queue_t *tasklet_get(unsigned n);

/* for syntactic sugar, we could discuss to add some (none overhead) wrappers */
static inline tasklet_schedule(unsgined n, event_t *evt)
{
    even_post(tasklet_get(n), evt);
}
// and so on...
