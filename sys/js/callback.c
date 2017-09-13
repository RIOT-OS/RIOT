#include "js.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

void js_callback_isr(void *arg)
{
    js_callback_t *js_callback = (js_callback_t *)arg;
    DEBUG("trigger %p\n", js_callback);
    event_post(js_event_queue, &js_callback->event);
}

static void js_event_callback(event_t *event)
{
    js_callback_t *js_callback = (js_callback_t *) event;
    js_callback_run(js_callback);
}

void js_callback_init(js_callback_t *js_callback, jerry_value_t callback)
{
    DEBUG("init %p\n", js_callback);
    js_callback->event.callback = js_event_callback;
    js_callback->callback.object = jerry_acquire_value(callback);
    DEBUG("acquire obj %lu\n", js_callback->callback.object);
    clist_rpush(&js_native_refs, &js_callback->callback.ref);
}

static void js_callback_cleanup(js_callback_t *js_callback)
{
    DEBUG("cleanup %p\n", js_callback);
    jerry_value_t callback = js_callback->callback.object;
    jerry_release_value(callback);
    clist_remove(&js_native_refs, &js_callback->callback.ref);
    DEBUG("release %lu\n", callback);
}

void js_callback_run(js_callback_t *js_callback)
{
    DEBUG("run %p\n", js_callback);
    jerry_value_t callback = js_callback->callback.object;
    js_call_function(callback);
    js_callback_cleanup(js_callback);
}

void js_callback_cancel(js_callback_t *js_callback)
{
    DEBUG("cancel %p\n", js_callback);
    event_cancel(js_event_queue, &js_callback->event);
    js_callback_cleanup(js_callback);
}

