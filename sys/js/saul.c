#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "event.h"
#include "js.h"
#include "saul_reg.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

static void _js_saul_reg_freecb(void *native_p)
{
    (void)native_p;
    DEBUG("%s:l%u:%s()\n", __FILE__, __LINE__, __func__);
}

static const jerry_object_native_info_t js_saul_reg_object_native_info =
{
    .free_cb = _js_saul_reg_freecb
};

static JS_EXTERNAL_HANDLER(saul_reg_read)
{
    (void)func_value;
//    (void)this_value;
    (void)args_p;
    (void)args_cnt;

    phydat_t phydat;
    saul_reg_t *saul_reg = js_get_object_native_pointer(this_value, &js_saul_reg_object_native_info);
    if (!saul_reg) {
        puts("nosaulreg");
        goto error;
    }
    int res = saul_reg_read(saul_reg, &phydat);
    if (res < 1) {
        puts("saulnothingread");
        goto error;
    }
    return jerry_create_number(phydat.val[0]);

error:
    return jerry_create_undefined();
}

static JS_EXTERNAL_HANDLER(saul_reg_write)
{
    (void)func_value;
//    (void)this_value;
    (void)args_p;
    (void)args_cnt;

    saul_reg_t *saul_reg = js_get_object_native_pointer(this_value, &js_saul_reg_object_native_info);
    if (!saul_reg) {
        puts("nosaulreg");
        goto error;
    }
    if (args_cnt < 1) {
        puts("saul_write(): not enough arguments");
        goto error;
    }

    phydat_t phydat;
    memset(&phydat, '\0', sizeof(phydat));

    for (unsigned i = 0; i < args_cnt; i++) {
        if (jerry_value_is_number(args_p[i])) {
            phydat.val[i] = jerry_get_number_value(args_p[i]);
        } else {
            printf("saul_write(): arg %i is not a number!", i);
            goto error;
        }
    }

    saul_reg_write(saul_reg, &phydat);

error:
    return jerry_create_undefined();
}

const js_native_method_t saul_reg_methods[] = {
    { "read", js_external_handler_saul_reg_read },
    { "write", js_external_handler_saul_reg_write },
};

const unsigned saul_reg_methods_len = sizeof(saul_reg_methods) / sizeof(saul_reg_methods[0]);

static jerry_value_t js_saul_reg_create(saul_reg_t *saul_reg)
{
    jerry_value_t object = js_object_create_with_methods(saul_reg_methods, saul_reg_methods_len);
    jerry_set_object_native_pointer(object, saul_reg, &js_saul_reg_object_native_info);
    return object;
}

static JS_EXTERNAL_HANDLER(saul_get_one) {
    (void)func_value;
    (void)this_value;

    if (args_cnt < 1) {
        puts("saul.get_one(): not enough arguments");
        return 0;
    }

    if (!jerry_value_is_number(args_p[0])) {
        puts("saul.get_one(): arg 0 not a saul type");
        return 0;
    }

/*    int pos = 0;
    if ((args_cnt) > 1 && jerry_value_is_number(args_p[1])) {
        pos = jerry_get_number_value(args_p[1]);
    }
*/

    saul_reg_t *entry = saul_reg_find_type(jerry_get_number_value(args_p[0]));
    if (entry) {
        return js_saul_reg_create(entry);
    } else {
        return jerry_create_undefined();
    }
}

static JS_EXTERNAL_HANDLER(saul_find_name) {
    (void)func_value;
    (void)this_value;

    if (args_cnt < 1) {
        puts("saul.get_by_name(): not enough arguments");
        return 0;
    }

    if (!jerry_value_is_string(args_p[0])) {
        puts("saul.get_by_name(): arg 0 not a string");
        return 0;
    }

    char name[32];
    memset(name, '\0', sizeof(name));
    jerry_string_to_char_buffer(args_p[0], (jerry_char_t*)name, sizeof(name));
    saul_reg_t *entry = saul_reg_find_name(name);
    if (entry) {
        DEBUG("saul_find_name(): found entry for name \"%s\"\n", name);
        return js_saul_reg_create(entry);
    } else {
        DEBUG("saul_find_name(): no entry found for name \"%s\"\n", name);
        return jerry_create_undefined();
    }
}
const js_native_method_t saul_methods[] = {
    { "_get_one", js_external_handler_saul_get_one },
    { "_find_name", js_external_handler_saul_find_name }
};

const unsigned saul_methods_len = sizeof(saul_methods) / sizeof(saul_methods[0]);
