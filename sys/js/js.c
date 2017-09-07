#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "js.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

event_queue_t *js_event_queue;

void js_add_external_handler(jerry_value_t object, const char *name, jerry_external_handler_t handler)
{
    /* Create a JS function object and wrap into a jerry value */
    jerry_value_t func_obj = jerry_create_external_function(handler);

    /* Set the native function as a property of the empty JS object */
    jerry_value_t prop_name = jerry_create_string((const jerry_char_t *) name);

    jerry_release_value(jerry_set_property(object, prop_name, func_obj));
    jerry_release_value(prop_name);
    jerry_release_value(func_obj);
}

void js_add_object(jerry_value_t object, jerry_value_t other, const char *name)
{
    jerry_value_t prop_name = jerry_create_string((const jerry_char_t *) name);

    jerry_release_value(jerry_set_property(object, prop_name, other));
    jerry_release_value(prop_name);
}

jerry_value_t js_object_native_create(size_t size, const jerry_object_native_info_t *native_obj_type_info)
{
    jerry_value_t object = jerry_create_object();

    if (size) {
        void *native_obj = malloc(size);
        if (!native_obj) {
            printf("%s(): malloc failed\n", __func__);
        }
        jerry_set_object_native_pointer(object, native_obj, native_obj_type_info);
    }

    return object;
}

void *js_get_object_native_pointer(jerry_value_t object, const jerry_object_native_info_t *type)
{
    void *result;
    const jerry_object_native_info_t *type_tmp;

    bool has_p = jerry_get_object_native_pointer(object, &result, &type_tmp);

    return (has_p && (type_tmp == type)) ? result : NULL;
}

jerry_value_t js_object_create_with_methods(const js_native_method_t *methods, unsigned num_methods)
{
    jerry_value_t object = jerry_create_object();

    for (unsigned n = 0; n < num_methods; n++) {
        const js_native_method_t *method = methods++;
        js_add_external_handler(object, method->name, method->handler);
    }

    return object;
}

void js_run_callback(jerry_value_t callback)
{
    if (jerry_value_is_function(callback)) {
        jerry_value_t this_val = jerry_create_undefined();
        jerry_value_t ret_val = jerry_call_function(callback, this_val, NULL, 0);

        if (!jerry_value_has_error_flag(ret_val)) {
            DEBUG("%s():l%u %s\n", __FILE__, __LINE__, __func__);
        }
        else {
            printf("%s():l%u %s\n", __FILE__, __LINE__, __func__);
        }

        jerry_release_value(ret_val);
        jerry_release_value(this_val);
    }
    else {
        printf("%s():l%u %s\n", __FILE__, __LINE__, __func__);
    }
    jerry_release_value(callback);
}

void js_callback(void *arg)
{
    js_callback_t *js_callback = (js_callback_t *)arg;
    event_post(js_event_queue, &js_callback->event);
}

void js_event_callback(event_t *event)
{
    js_callback_t *js_callback = (js_callback_t *) event;
    js_run_callback(js_callback->callback);
    jerry_release_value(js_callback->object);
}

int js_run(const jerry_char_t *script, size_t script_size)
{
    jerry_value_t ret_value;

    /* Setup Global scope code */
    ret_value = jerry_parse(script, script_size, false);

    if (!jerry_value_has_error_flag(ret_value)) {
        /* Execute the parsed source code in the Global scope */
        ret_value = jerry_run(ret_value);
    }

    int res = 0;

    if (jerry_value_has_error_flag(ret_value)) {
        puts("js_run(): Script Error!");
        res = -1;
    }

    jerry_release_value(ret_value);

    return res;
}

void js_init(event_queue_t *queue)
{
    js_event_queue = queue;

    /* Initialize engine */
    jerry_init(JERRY_INIT_EMPTY);

    /* Register the print function in the global object. */
    jerryx_handler_register_global((const jerry_char_t *) "print", jerryx_handler_print);

    js_init_objects();
}
