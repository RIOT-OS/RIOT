#ifndef JS_H
#define JS_H

#include "clist.h"
#include "event.h"
#include "jerryscript.h"
#include "jerryscript-ext/handler.h"

extern event_queue_t *js_event_queue;

typedef struct {
    const char *name;
    jerry_external_handler_t handler;
} js_native_method_t;

typedef struct {
    const char *name;
    const js_native_method_t *methods;
} js_native_objects_t;

typedef struct {
    list_node_t ref;
    jerry_value_t object;
} js_native_ref_t;

typedef struct {
    event_t event;
    js_native_ref_t callback;
} js_callback_t;

extern list_node_t js_native_refs;

void js_init(event_queue_t *queue);
int js_run(const jerry_char_t *script, size_t script_size);

/* internal */
void js_init_objects(void);
void js_add_object(jerry_value_t object, jerry_value_t other, const char *name);
void *js_get_object_native_pointer(jerry_value_t object, const jerry_object_native_info_t *type);
jerry_value_t js_object_create_with_methods(const js_native_method_t *methods, unsigned num_methods);
jerry_value_t js_object_native_create(size_t size, const jerry_object_native_info_t *native_obj_type_info);
void js_callback_isr(void *arg);
void js_call_function(jerry_value_t callback);
void js_callback_init(js_callback_t *js_callback, jerry_value_t callback);
void js_callback_run(js_callback_t *js_callback);
void js_callback_cancel(js_callback_t *callback);
void js_shutdown(event_t *shutdown_done_event);

#define JS_EXTERNAL_HANDLER(name) \
    jerry_value_t \
    js_external_handler_ ## name (const jerry_value_t func_value, \
                                const jerry_value_t this_value, \
                                const jerry_value_t * args_p, \
                                const jerry_length_t args_cnt)

#endif /* JS_H */
