#include "js.h"
#include "periph/pm.h"

static JS_EXTERNAL_HANDLER(reboot) {
    (void)func_value;
    (void)this_value;
    (void)args_p;
    (void)args_cnt;

    pm_reboot();

    return 0;
}

const js_native_method_t riot_methods[] = {
    { "reboot", js_external_handler_reboot }
};

const unsigned riot_methods_len = sizeof(riot_methods)/sizeof(riot_methods[0]);
