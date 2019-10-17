
#include "thread.h"
#include "event.h"
#include "tasklet.h"
#include "tasklet_params.h"


static event_queue_t _eqs[ARRAY_SIZE(tasklet_params)];

static void *_runner(void *arg)
{
    event_queue_init((event_queue_t *)arg);
    event_loop((event_queue_t *)arg);
    return NULL;
}

void auto_init_tasklet(void)
{
    for (unsigned i = 0; i < ARRAY_SIZE(tasklet_params); i++) {
        thread_create(tasklet_params.stack, ... , _runner, &_eqs[i]);
    }
}

void tasklet_get(unsigned n)
{
    assert(n <= ARRAY_SIZE(tasklet_params));
    return &_eqs[n];
}
