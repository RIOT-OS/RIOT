
#include "tasklet.h"

#ifdef MODULE_TASKLET_HIGHPRIO
static char _stack_t1[SOME_STACK_SIZE];
#endif

#ifdef MODULE_TASKLET_LOWPRIO
static char _stack_t2[SOME_OTHER_STACK_SIZE];

/* syntactic sugar: we could give some names - however, we need to find a better
 * way to do this non statically depending on the actual selected modules... */
#define TASKLET_LOWPRIO         (1u) /* position in params array */
#endif



static tasklet_params_t tasklet_params[] = {
#ifdef MODULE_TASKLET_HIGHPRIO
    {
        .prio = (THREAD_PRIORITY_HIGHEST + 1),
        .stack = _stack_t1,
        .stack_size = sizeof(stack_t1),

    },
#endif
#ifdef MODULE_TASKLET_LOWPRIO
    {
        .prio = (THREAD_PRIORITY_IDLE - 1),
        .stack = _stack_t2,
        .stack_size = sizeof(stack_t2),

    },
#endif
}
