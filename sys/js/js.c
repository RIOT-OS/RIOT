#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "js.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

event_queue_t *js_event_queue;
static list_node_t js_native_refs;

void js_add_external_handler(jerry_value_t object, const char *name, jerry_external_handler_t handler)
{
    /* Create a JS function object and wrap into a jerry value */
    jerry_value_t func_obj = jerry_create_external_function(handler);
    js_check(func_obj);

    /* Set the native function as a property of the empty JS object */
    jerry_value_t prop_name = jerry_create_string((const jerry_char_t *) name);
    js_check(prop_name);

    jerry_value_t set_prop = jerry_set_property(object, prop_name, func_obj);
    js_check(set_prop);

    jerry_release_value(set_prop);
    jerry_release_value(prop_name);
    jerry_release_value(func_obj);
}

void js_add_object(jerry_value_t object, jerry_value_t other, const char *name)
{
    jerry_value_t prop_name = jerry_create_string((const jerry_char_t *) name);
    js_check(prop_name);

    jerry_value_t set_prop = jerry_set_property(object, prop_name, other);
    js_check(set_prop);

    jerry_release_value(set_prop);
    jerry_release_value(prop_name);
}

jerry_value_t js_get_property(jerry_value_t object, const char *name)
{
  jerry_value_t prop_name = jerry_create_string ((const jerry_char_t *) name);
  jerry_value_t prop_value = jerry_get_property (object, prop_name);
  jerry_release_value (prop_name);
  return prop_value;
}

double js_object_get_number(jerry_value_t object, const char *name)
{
    double res;
    jerry_value_t prop_value = js_get_property(object, name);
    if (jerry_value_is_number(prop_value)) {
        res = jerry_get_number_value(prop_value);
    }
    else {
        res = 0;
    }

    jerry_release_value(prop_value);

    return res;
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

void js_call_function(jerry_value_t function)
{
    if (jerry_value_is_function(function)) {
        jerry_value_t this_val = jerry_create_undefined();
        jerry_value_t ret_val = jerry_call_function(function, this_val, NULL, 0);

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

void js_init(void)
{
    assert(js_event_queue);

    /* Initialize engine */
    jerry_init(JERRY_INIT_EMPTY);

    /* Register the print function in the global object. */
    jerryx_handler_register_global((const jerry_char_t *) "print", jerryx_handler_print);

    js_init_objects();
}

static event_t *js_shutdown_done_event;

static void js_shutdown_event_handler(event_t *event)
{
    (void)event;

    unsigned state = irq_disable();

    /* drain all js callbackevents from queue */
    while (event_get(js_event_queue));

    /* un-reference all jerryscript objects referenced by native code */
    js_native_ref_t *ref;
    while ((ref = (js_native_ref_t*) clist_lpop(&js_native_refs))) {
        jerry_release_value(ref->object);
    }

    /* cleanup jerryscript engine */
    jerry_cleanup();

    irq_restore(state);

    if (js_shutdown_done_event) {
        event_post(js_event_queue, js_shutdown_done_event);
    }
}

static event_t js_shutdown_event = { .handler = js_shutdown_event_handler };

void js_shutdown(event_t *done_event)
{
    js_shutdown_done_event = done_event;
    event_post(js_event_queue, &js_shutdown_event);
}

char *js_strdup(jerry_value_t string)
{
    size_t len = jerry_get_string_length(string);
    if (!len) {
        return NULL;
    }

    char *buf = malloc(len+1);
    if (buf) {
        buf[jerry_string_to_char_buffer(string, (uint8_t*)buf, len)] = '\0';
    }
    return buf;
}

void js_native_ref_add(js_native_ref_t *ref, jerry_value_t object)
{
    ref->object = jerry_acquire_value(object);
    clist_rpush(&js_native_refs, &ref->ref);
}

void js_native_ref_rem(js_native_ref_t *ref)
{
    /* TODO: unfortunately, this is O(n) */
    if (clist_remove(&js_native_refs, &ref->ref)) {
        jerry_release_value(ref->object);
    }
}
