#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "event.h"
#include "js.h"
#include "xtimer.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

typedef struct {
    js_callback_t callback;
    jerry_value_t object;
    xtimer_t xtimer;
} js_xtimer_t;

static void _js_xtimer_freecb(void *native_p)
{
    DEBUG("%s:l%u:%s()\n", __FILE__, __LINE__, __func__);

    js_xtimer_t *js_xtimer = (js_xtimer_t *) native_p;
    xtimer_remove(&js_xtimer->xtimer);
    free(js_xtimer);
}

static const jerry_object_native_info_t js_xtimer_object_native_info =
{
    .free_cb = _js_xtimer_freecb
};

static jerry_value_t js_xtimer_create(jerry_value_t callback, uint32_t timeout)
{
    jerry_value_t object = js_object_native_create(sizeof(js_xtimer_t), &js_xtimer_object_native_info);
    js_xtimer_t *js_xtimer = js_get_object_native_pointer(object, &js_xtimer_object_native_info);

    if (!js_xtimer) {
        DEBUG("%s():l%u\n", __FILE__, __LINE__);
    }

    /* add the xtimer object as property to the callback so
     * it stays referenced */
    js_add_object(callback, object, "_obj");

    memset(js_xtimer, '\0', sizeof(*js_xtimer));
    js_callback_init(&js_xtimer->callback, callback);
    js_xtimer->xtimer.callback = js_callback_isr;
    js_xtimer->xtimer.arg = js_xtimer;
    xtimer_set(&js_xtimer->xtimer, timeout);

    return object;
}

static JS_EXTERNAL_HANDLER(timer_setTimeout) {
    (void)func_value;
    (void)this_value;

    if (args_cnt < 2) {
        puts("timer.setTimeout(): not enough arguments");
        return 0;
    }

    if (!jerry_value_is_function(args_p[0])) {
        puts("timer.setTimeout(): arg 0 not a function");
        return 0;
    }
    if (!jerry_value_is_number(args_p[1])) {
        puts("timer.setTimeout(): arg 1 not a number");
        return 0;
    }

    return js_xtimer_create(args_p[0], jerry_get_number_value(args_p[1]));
}

static JS_EXTERNAL_HANDLER(timer_now) {
    (void)func_value;
    (void)this_value;
    (void)args_p;
    (void)args_cnt;

    return jerry_create_number(xtimer_now_usec());
}

const js_native_method_t timer_methods[] = {
    { "setTimeout", js_external_handler_timer_setTimeout },
    { "now", js_external_handler_timer_now }
};

const unsigned timer_methods_len = sizeof(timer_methods) / sizeof(timer_methods[0]);
