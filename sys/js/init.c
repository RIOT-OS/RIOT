#include "event.h"
#include "js.h"

extern const js_native_method_t riot_methods[];
extern const unsigned riot_methods_len;

extern const js_native_method_t timer_methods[];
extern const unsigned timer_methods_len;

extern const js_native_method_t saul_methods[];
extern const unsigned saul_methods_len;

void js_init_objects(void)
{
    jerry_value_t global_object = jerry_get_global_object();

    jerry_value_t riot_object = js_object_create_with_methods(riot_methods, riot_methods_len);
    js_add_object(global_object, riot_object, "riot");
    jerry_release_value(riot_object);

    jerry_value_t timer_object = js_object_create_with_methods(timer_methods, timer_methods_len);
    js_add_object(global_object, timer_object, "timer");
    jerry_release_value(timer_object);

    jerry_value_t saul_object = js_object_create_with_methods(saul_methods, saul_methods_len);
    js_add_object(global_object, saul_object, "saul");
    jerry_release_value(saul_object);

    jerry_release_value(global_object);
}
